#ifndef GPIB_SIMULATION_DATA_GENERATOR
#define GPIB_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class GPIBAnalyzerSettings;

class GPIBSimulationDataGenerator
{
public:
	GPIBSimulationDataGenerator();
	~GPIBSimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, GPIBAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	GPIBAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected:
	void CreateSerialByte();
	std::string mSerialText;
	U32 mStringIndex;

	SimulationChannelDescriptor mSerialSimulationData;

};
#endif //GPIB_SIMULATION_DATA_GENERATOR