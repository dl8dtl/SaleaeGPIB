#ifndef GPIB_ANALYZER_SETTINGS
#define GPIB_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class GPIBAnalyzerSettings : public AnalyzerSettings
{
public:
	static constexpr int nWires = 16; // 8 data lines, 3 handshake signals, 5
			       // control signals
	const char *wireNames[nWires] = {
		"DIO1", "DIO2", "DIO3", "DIO4", "DIO5", "DIO6", "DIO7", "DIO8",
		"REN", "EOI", "DAV", "NRFD", "NDAC", "IFC", "SRQ", "ATN"
	};
public:
	GPIBAnalyzerSettings();
	virtual ~GPIBAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	Channel mChannels[nWires];
    U32 mGlitchTime; // in µs

protected:
	AnalyzerSettingInterfaceChannel	mInterfaceChannels[nWires];
    AnalyzerSettingInterfaceInteger mInterfaceGlitchTime;
};

#endif //GPIB_ANALYZER_SETTINGS
 
