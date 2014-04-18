#include "Utility.h"
#include <sysSymTbl.h>

double CowLib::AnalogInScale(double oldx, double center)
{
	const double min = 0 - center;
	const double max = 3.25 - center;
	const double deadband = .05;
	
	double x = oldx - center;

	if ((x <= (center*deadband)) && (x >= (-1*center*deadband)))
	{
		x = 0;
	}
	else if (x > (max - center*deadband))
	{
		x = 1;
	}
	else if (x < (min + center*deadband))
	{
		x = -1;
	}
	else if (x > 0)
	{
		x = ((x - (center*deadband))/((max - center*deadband) - (center
				*deadband)));
	}
	else if (x < 0)
	{
		x = ((x + (center*deadband))/(-1*((min + center*deadband) + (center
				*deadband))));
	}
	return x;
}

void CowLib::PrintToLCD(char* format, ...)
{
	DriverStationLCD* ds = DriverStationLCD::GetInstance();
	char output[256] = {0};
	va_list args;
	va_start(args, format);
	vsnprintf((char*)&output, 256, format, args);
	va_end(args);
	std::istringstream buffer(output);
	for(int i = 0; i < 6; i++)
	{
		char line[22] = {0};
		buffer.getline((char*)&line, 21);
		ds->PrintfLine((DriverStationLCD::Line)i, line);
	}
	ds->UpdateLCD();
}

double CowLib::LimitMix(double value, double maxValue)
{
	double limited_value = 0;
	
	if(value < -maxValue)
	{
		limited_value = -maxValue;
	}
	else if(value > maxValue)
	{
		limited_value = maxValue;
	}
	else
	{
		limited_value = value;
	}
	
	return limited_value;
}

double CowLib::LimitMix(double value)
{
	double limited_value = 0;
	
	if(value < -1.0)
	{
		limited_value = -1.0;
	}
	else if(value > 1.0)
	{
		limited_value = 1.0;
	}
	else
	{
		limited_value = value;
	}
	
	return limited_value;
}

double CowLib::UnitsPerSecond(double value)
{
	return value * ROBOT_HZ;
}
