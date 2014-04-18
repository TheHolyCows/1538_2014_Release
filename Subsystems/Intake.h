//==================================================
// Copyright (C) 2014 Team 1538 / The Holy Cows
//==================================================

#ifndef __INTAKE_H__
#define __INTAKE_H__

#include <WPILib.h>
#include "../CowConstants.h"

class Intake
{
private:
	Intake();
	Talon* m_MotorA;
	Talon* m_MotorB;
	Solenoid* m_SolenoidA;
	Solenoid* m_SolenoidB;
	double m_LastDropTime;
public:
	Intake(int motorPortA, int motorPortB, int solenoidPortA, int solenoidPortB);
	void SetSpeed(double speed);
	void SetState(bool state);
	bool GetState();
	bool IsPhysicallyDropped();
	double TimeSinceLastDrop();
};

#endif
