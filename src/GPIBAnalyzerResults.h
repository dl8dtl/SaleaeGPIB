#ifndef GPIB_ANALYZER_RESULTS
#define GPIB_ANALYZER_RESULTS

#include <AnalyzerResults.h>

class GPIBAnalyzer;
class GPIBAnalyzerSettings;

enum wireIdx {
    DIO1,
    DIO2,
    DIO3,
    DIO4,
    DIO5,
    DIO6,
    DIO7,
    DIO8,
    REN,
    EOI,
    DAV,
    NRFD,
    NDAC,
    IFC,
    SRQ,
    ATN,
    DATA_MASK = 0xFF,
    // bit values used in result
    EOI_MASK = 0x100,
    ATN_MASK = 0x200,
};

class GPIBAnalyzerResults : public AnalyzerResults
{
public:
    static void convert_data(U64 data, bool is_bubble, DisplayBase base, char *result, U32 max_length);
	GPIBAnalyzerResults( GPIBAnalyzer* analyzer, GPIBAnalyzerSettings* settings );
	virtual ~GPIBAnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );

protected: //functions

protected:  //vars
	GPIBAnalyzerSettings* mSettings;
	GPIBAnalyzer* mAnalyzer;
};

#endif //GPIB_ANALYZER_RESULTS
