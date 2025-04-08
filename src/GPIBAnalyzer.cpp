#include "GPIBAnalyzer.h"
#include "GPIBAnalyzerSettings.h"
#include "GPIBAnalyzerResults.h"
#include <AnalyzerChannelData.h>

GPIBAnalyzer::GPIBAnalyzer()
:	Analyzer2(),  
	mSettings(),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( &mSettings );
    UseFrameV2();
}

GPIBAnalyzer::~GPIBAnalyzer()
{
	KillThread();
}

void GPIBAnalyzer::SetupResults()
{
	// SetupResults is called each time the analyzer is run. Because the same instance can be used for multiple runs, we need to clear the results each time.
	mResults.reset(new GPIBAnalyzerResults( this, &mSettings ));
	SetAnalyzerResults( mResults.get() );
	//mResults->AddChannelBubblesWillAppearOn( mSettings.mInputChannel );
}

void GPIBAnalyzer::WorkerThread()
{
    constexpr U32 minimum_width = 20; // samples

    for (int i = 0; i < GPIBAnalyzerSettings::nWires; i++)
        mChannels[i] = GetAnalyzerChannelData(mSettings.mChannels[i]);

    U64 sample_rate = GetSampleRate() / 1E6; // in MHz

    for (;;)
    {

        if (mChannels[REN]->GetBitState() == BIT_HIGH)
            // not in remote mode
            mChannels[REN]->AdvanceToNextEdge();

        U64 sample_number = mChannels[REN]->GetSampleNumber();
        mChannels[DAV]->AdvanceToAbsPosition(sample_number);
        if (mChannels[DAV]->GetBitState() == BIT_HIGH)
            // no data available
            mChannels[DAV]->AdvanceToNextEdge();

        sample_number = mChannels[DAV]->GetSampleNumber();
        mChannels[ATN]->AdvanceToAbsPosition(sample_number);
        BitState atn = mChannels[ATN]->GetBitState();
        mResults->AddMarker(sample_number, AnalyzerResults::DownArrow, mSettings.mChannels[DAV]);

        U64 data = 0;
        U8 mask = 1;
        for (int i = 0; i < 8; i++, mask <<= 1)
        {
            mChannels[i]->AdvanceToAbsPosition(sample_number);
            if (mChannels[i]->GetBitState() == BIT_LOW)
                data |= mask;
        }
        // add EOI and ATN so we can remember them for result display
        mChannels[EOI]->AdvanceToAbsPosition(sample_number);
        if (mChannels[EOI]->GetBitState() == BIT_LOW)
            data |= EOI_MASK;
        mChannels[ATN]->AdvanceToAbsPosition(sample_number);
        if (mChannels[ATN]->GetBitState() == BIT_LOW)
            data |= ATN_MASK;

        U64 starting_sample = sample_number;
        U64 ending_sample = starting_sample;
        // find rising edge of NDAC
        mChannels[NDAC]->AdvanceToAbsPosition(sample_number);
        if (mChannels[NDAC]->GetBitState() == BIT_LOW) {
            mChannels[NDAC]->AdvanceToNextEdge();
            ending_sample =mChannels[NDAC]->GetSampleNumber();
        }
        mResults->AddMarker(ending_sample, AnalyzerResults::UpArrow, mSettings.mChannels[NDAC]);
        if (ending_sample - starting_sample < minimum_width)
            ending_sample = starting_sample + minimum_width;

        //we have a byte to save.
        Frame frame;
        frame.mData1 = data;
        frame.mFlags = 0;
        frame.mStartingSampleInclusive = starting_sample;
        frame.mEndingSampleInclusive = ending_sample;
        mResults->AddFrame( frame );

        FrameV2 frame_v2;
        const char *type = "data";
        if (data & ATN_MASK) {
            // decode command
            char result[128];
            GPIBAnalyzerResults::convert_data(data, false, DisplayBase::Decimal, result, 128);
            frame_v2.AddString("cmd/data", result);
            type = "cmd";
        } else {
            frame_v2.AddByte("cmd/data", data & DATA_MASK );
            if (data & EOI_MASK)
                frame_v2.AddBoolean("eoi", data & EOI_MASK);
        }

        mResults->AddFrameV2(frame_v2, type, starting_sample, ending_sample);
        mResults->CommitResults();
        ReportProgress( ending_sample );

        mChannels[DAV]->AdvanceToNextEdge();
        sample_number = mChannels[DAV]->GetSampleNumber();

        // skip glitch time
        U64 skip = sample_rate * mSettings.mGlitchTime;
        sample_number += skip;
        mChannels[DAV]->AdvanceToAbsPosition(sample_number);
        mChannels[REN]->AdvanceToAbsPosition(sample_number);
    }
}

bool GPIBAnalyzer::NeedsRerun()
{
	return false;
}

U32 GPIBAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), &mSettings );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 GPIBAnalyzer::GetMinimumSampleRateHz()
{
	return 10000000;
}

const char* GPIBAnalyzer::GetAnalyzerName() const
{
	return "GPIB Bus analyzer";
}

const char* GetAnalyzerName()
{
	return "GPIB Bus analyzer";
}

Analyzer* CreateAnalyzer()
{
	return new GPIBAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}
