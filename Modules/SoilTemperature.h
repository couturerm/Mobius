
#if !defined(SOIL_TEMPERATURE_MODEL_H)

static void
AddSoilTemperatureModel(mobius_model *Model)
{
	auto WattsPerMetrePerDegreeCelsius				= RegisterUnit(Model, "W/m/°C");
	auto MegaJoulesPerCubicMetrePerDegreeCelsius	= RegisterUnit(Model, "MJ/m3/°C");
	auto Dimensionless 								= RegisterUnit(Model);
	auto PerCm										= RegisterUnit(Model, "/cm");
	auto Metres										= RegisterUnit(Model, "m");
	auto Cm											= RegisterUnit(Model, "cm");
	auto Seconds									= RegisterUnit(Model, "s");
	auto DegreesCelsius								= RegisterUnit(Model, "°C");
	
	auto Land = GetParameterGroupHandle(Model, "Landscape units");    //NOTE: This should be declared by the main model
	
	auto ThermalConductivitySoil 			= RegisterParameterDouble(Model, Land, "Thermal conductivity of soil", 				WattsPerMetrePerDegreeCelsius,			 0.7);
	auto SpecificHeatCapacityFreezeThaw	    = RegisterParameterDouble(Model, Land, "Specific heat capacity due to freeze/thaw", MegaJoulesPerCubicMetrePerDegreeCelsius, 6.6, 1.0, 200.0, "Controls the energy released when water is frozen and energy consumed under melting");
	auto WaterEquivalentFactor	            = RegisterParameterDouble(Model, Land, "Water equivalent factor", 	   Dimensionless, 	  0.3,  0.01,   1.0, "1mm of snow would produce this amount of water when melted");
	auto SnowDepthSoilTemperatureFactor	    = RegisterParameterDouble(Model, Land, "Snow depth / soil temperature factor", 		PerCm, 								    -0.02, -0.03, -0.001, "Defines an empirical relationship between snow pack depth and its insulating effect on soils");
	auto SpecificHeatCapacitySoil			= RegisterParameterDouble(Model, Land, "Specific heat capacity of soil",			MegaJoulesPerCubicMetrePerDegreeCelsius, 1.1);
	auto SoilDepth							= RegisterParameterDouble(Model, Land, "Soil depth",								Metres,									 0.2);
	auto InitialSoilTemperature		    	= RegisterParameterDouble(Model, Land, "Initial soil temperature",					DegreesCelsius,							20.0);
	
	auto AirTemperature = GetInputHandle(Model, "Air temperature");  //NOTE: This should be declared by the main model
	
	auto Da                  = RegisterEquation(Model, "Da",                    Dimensionless);
	auto COUPSoilTemperature = RegisterEquation(Model, "COUP soil temperature", DegreesCelsius);
	SetInitialValue(Model, COUPSoilTemperature, InitialSoilTemperature);
	auto SoilTemperature     = RegisterEquation(Model, "Soil temperature",      DegreesCelsius);
	auto SnowDepth = RegisterEquation(Model, "Snow depth", Cm);
	
	auto SnowAsWaterEquivalent = GetEquationHandle(Model, "Snow depth as water equivalent");
	
	EQUATION(Model, Da,
		if ( LAST_RESULT(COUPSoilTemperature) > 0.0)
		{
			return PARAMETER(ThermalConductivitySoil)
					/ ( 1000000.0 * PARAMETER(SpecificHeatCapacitySoil) );
		}

		return PARAMETER(ThermalConductivitySoil)
				/ ( 1000000.0 * ( PARAMETER(SpecificHeatCapacitySoil) + PARAMETER(SpecificHeatCapacityFreezeThaw) ) );
	)
	
	EQUATION(Model, COUPSoilTemperature,
		return LAST_RESULT(COUPSoilTemperature)
			+ 86400.0
				* RESULT(Da) / Square( 2.0 * PARAMETER(SoilDepth))
				* ( INPUT(AirTemperature) - LAST_RESULT(COUPSoilTemperature) );
	)
	
	EQUATION(Model, SoilTemperature,
		return RESULT(COUPSoilTemperature)
			* std::exp(PARAMETER(SnowDepthSoilTemperatureFactor) * RESULT(SnowDepth));
	)
	
	EQUATION(Model, SnowDepth,
		return RESULT(SnowAsWaterEquivalent)
					/ 10.0 / PARAMETER(WaterEquivalentFactor);
	)
}

#define SOIL_TEMPERATURE_MODEL_H
#endif