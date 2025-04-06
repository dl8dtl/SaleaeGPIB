#include "GPIBAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "GPIBAnalyzer.h"
#include "GPIBAnalyzerSettings.h"
#include <iostream>
#include <fstream>

GPIBAnalyzerResults::GPIBAnalyzerResults( GPIBAnalyzer* analyzer, GPIBAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

GPIBAnalyzerResults::~GPIBAnalyzerResults()
{
}

void GPIBAnalyzerResults::convert_data(U64 data, bool is_bubble, DisplayBase base, char *result, U32 max_length)
{
    U8 databyte = data & DATA_MASK;
    if (data & ATN_MASK)
    {
        // convert number to command if possible
        databyte &= 0x7F; // 8th bit not used in cmd
        if (databyte < 0x20)
            switch (databyte)
            {
                case 0x01:
                    strcpy(result, "GTL");
                    if (is_bubble)
                        strcat(result, " Go to local");
                    break;

                case 0x04:
                    strcpy(result, "SDC");
                    if (is_bubble)
                        strcat(result, " Selected device clear");
                    break;

                case 0x05:
                    strcpy(result, "PPC");
                    if (is_bubble)
                        strcat(result, " Parallel poll configure");
                    break;

                case 0x08:
                    strcpy(result, "GET");
                    if (is_bubble)
                        strcat(result, " ");
                    break;

                case 0x09:
                    strcpy(result, "TCT");
                    if (is_bubble)
                        strcat(result, " ");
                    break;

                case 0x11:
                    strcpy(result, "LLO");
                    if (is_bubble)
                        strcat(result, " Local lockout");
                    break;

                case 0x14:
                    strcpy(result, "DCL");
                    if (is_bubble)
                        strcat(result, " Device clear");
                    break;

                case 0x15:
                    strcpy(result, "PPU");
                    if (is_bubble)
                        strcat(result, " Parallel poll unconfigure");
                    break;

                case 0x18:
                    strcpy(result, "SPE");
                    if (is_bubble)
                        strcat(result, " Serial poll enable");
                    break;

                case 0x19:
                    strcpy(result, "SPD");
                    if (is_bubble)
                        strcat(result, " Serial poll disable");
                    break;

                default:
                    snprintf(result, max_length, "0x%02x", databyte);
                    if (is_bubble)
                        strcat(result, " (unknown command)");
                    break;
            }
        else if (databyte & 0x20)
        {
            if (databyte == 0x3F)
            {
                strcpy(result, "UNL");
                if (is_bubble)
                    strcat(result, " Unlisten");
            }
            else
            {
                snprintf(result, max_length, "LAD %d", databyte & 0x1F);
                if (is_bubble)
                    strcat(result, " Listener address");
            }
        }
        else if (databyte & 0x40)
        {
            if (databyte == 0x5F)
            {
                strcpy(result, "UNT");
                if (is_bubble)
                    strcat(result, " Untalk");
            }
            else
            {
                snprintf(result, max_length, "TAD %d", databyte & 0x1F);
                if (is_bubble)
                    strcat(result, " Talker address");
            }
        }
        else if ((databyte & 0x70) == 0x60)
        {
            snprintf(result, max_length, "PPE 0x%02x", databyte & 0x0F);
            if (is_bubble)
                strcat(result, " Parallel poll enable");
        }
        else // if ((databyte & 0x70) == 0x70)
        {
            snprintf(result, max_length, "PPD 0x%02x", databyte & 0x0F);
            if (is_bubble)
                strcat(result, " Parallel poll disable");
        }
    }
    else // normal data, not command
    {
        AnalyzerHelpers::GetNumberString(databyte, base, 8, result, max_length);
        if (data & EOI_MASK)
            strcat(result, "+EOI");
    }
}

void GPIBAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );

	char number_str[128];
    convert_data(frame.mData1, true, display_base, number_str, 128);
	AddResultString( number_str );
}

void GPIBAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
	std::ofstream file_stream( file, std::ios::out );

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	file_stream << "Time [s],Value" << std::endl;

	U64 num_frames = GetNumFrames();
	for( U32 i=0; i < num_frames; i++ )
	{
		Frame frame = GetFrame( i );
		
		char time_str[128];
		AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

        char number_str[128];
        convert_data(frame.mData1, false, display_base, number_str, 128);

		file_stream << time_str << "," << number_str << std::endl;

		if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
		{
			file_stream.close();
			return;
		}
	}

	file_stream.close();
}

void GPIBAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
#ifdef SUPPORTS_PROTOCOL_SEARCH
	Frame frame = GetFrame( frame_index );
	ClearTabularText();

	char number_str[128];
    convert_data(frame.mData1, false, display_base, number_str, 128);
	AddTabularText( number_str );
#endif
}

void GPIBAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	//not supported

}

void GPIBAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	//not supported
}
