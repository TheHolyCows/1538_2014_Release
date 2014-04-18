//==================================================
// Copyright (C) 2014 Team 1538 / The Holy Cows
//==================================================

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <WPILib.h>
#include "../Declarations.h"
#include "Timer.h"
#include <algorithm>
#include <stdarg.h>
#include <sstream>

#define HZ(x) 1.0/(double)x

namespace CowLib
{
	double LimitMix(double value, double maxValue);
	double LimitMix(double value);
	double AnalogInScale(double oldx, double center);
	
	inline bool compareParticleAnalysisReport(const ParticleAnalysisReport& a, const ParticleAnalysisReport& b)
	{
		//Want the taller one as the last element
		return a.center_mass_y_normalized > b.center_mass_y_normalized;
	}
	
	template <class IT>
	void QSortParticleAnalysisReport(IT begin, IT end) 
	{
	  std::sort(begin, end, compareParticleAnalysisReport);
	}  
	
	void PrintToLCD(char* format, ...);
	
	double UnitsPerSecond(double value); // converts units per robot period to units per second
}


#endif
