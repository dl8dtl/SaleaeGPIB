#include "GPIBAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


GPIBAnalyzerSettings::GPIBAnalyzerSettings()
: mGlitchTime(5)
{
    for (int i = 0; i < nWires; i++) {
        mInterfaceChannels[i].SetTitleAndTooltip( wireNames[i], wireNames[i] );
        mInterfaceChannels[i].SetChannel( mChannels[i] );
        AddChannel( mChannels[i], wireNames[i], false );
        AddInterface( mInterfaceChannels + i );
    }
    mInterfaceGlitchTime.SetTitleAndTooltip("DAV glitch time in µs", "Time to skip after DAV went inactive");
    mInterfaceGlitchTime.SetInteger(mGlitchTime);
    AddInterface(&mInterfaceGlitchTime);

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
}

GPIBAnalyzerSettings::~GPIBAnalyzerSettings()
{
}

bool GPIBAnalyzerSettings::SetSettingsFromInterfaces()
{
    for (int i = 0; i < nWires; i++) {
        mChannels[i] = mInterfaceChannels[i].GetChannel();
        AddChannel( mChannels[i], wireNames[i], true );
    }
    mGlitchTime = mInterfaceGlitchTime.GetInteger();
	ClearChannels();

	return true;
}

void GPIBAnalyzerSettings::UpdateInterfacesFromSettings()
{
    for (int i = 0; i < nWires; i++)
        mInterfaceChannels[i].SetChannel( mChannels[i] );
    mInterfaceGlitchTime.SetInteger(mGlitchTime);
}

void GPIBAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

    for (int i = 0; i < nWires; i++)
        text_archive >> mChannels[i];
    text_archive >> mGlitchTime;

	ClearChannels();
    for (int i = 0; i < nWires; i++)
        AddChannel( mChannels[i], wireNames[i], true );

	UpdateInterfacesFromSettings();
}

const char* GPIBAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

    for (int i = 0; i < nWires; i++)
        text_archive << mChannels[i];
    text_archive << mGlitchTime;

	return SetReturnString( text_archive.GetString() );
}
