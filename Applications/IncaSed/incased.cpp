#define MOBIUS_TIMESTEP_VERBOSITY 0
//NOTE: the g++ compiler flag ffast-math will make it so that isnan does not work correctly, so don't use that flag.
#define MOBIUS_TEST_FOR_NAN 0
#define MOBIUS_EQUATION_PROFILING 0
#define MOBIUS_PRINT_TIMING_INFO 0

#include "../../mobius.h"

#include "../../Modules/Persist.h"
#include "../../Modules/INCA-Sed.h"

#include "../../incaview_compatibility.h"

int main(int argc, char **argv)
{
	incaview_commandline_arguments Args;
	ParseIncaviewCommandline(argc, argv, &Args);
	
	mobius_model *Model = BeginModelDefinition("INCA-Sed", "0.0");
	
	auto Days 	      = RegisterUnit(Model, "days");
	auto System       = RegisterParameterGroup(Model, "System");
	RegisterParameterUInt(Model, System, "Timesteps", Days, 100);
	RegisterParameterDate(Model, System, "Start date", "1999-1-1");
	
	AddPersistModel(Model);
	AddINCASedModel(Model);
	
	EnsureModelComplianceWithIncaviewCommandline(Model, &Args);
	
	EndModelDefinition(Model);
	
	mobius_data_set *DataSet = GenerateDataSet(Model);
	
	RunDatasetAsSpecifiedByIncaviewCommandline(DataSet, &Args);
}