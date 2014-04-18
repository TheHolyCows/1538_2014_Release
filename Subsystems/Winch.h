//==================================================
// Copyright (C) 2014 Team 1538 / The Holy Cows
//==================================================

#ifndef __WINCH_H__
#define __WINCH_H__

#include <WPILib.h>
#include "../CowLib/CowLib.h"
#include "../CowConstants.h"
#include <math.h>

class Winch
{
public:
	typedef enum
	{
		RELOADING,
		LOCKING,
		READY_TO_FIRE,
		PREFIRING,
		FIRING,
		UNLOCKING,
		HOLDING,
		DESPRINGING
	} e_WinchState;
	
private:	
	Winch();
	Talon* m_MotorA;
	Talon* m_MotorB;
	Solenoid* m_Lock;
	Solenoid* m_Fire;
	AnalogChannel* m_Pot;
	e_WinchState m_State;
	e_WinchState m_NextState;
	double m_Setpoint;
	double m_PreviousVoltage;
	double m_LastUnlockVoltage;
	double m_HoldVoltage;
	double m_LastFireTime;
	double m_LastFireStopTime;
	double m_LastReloadStopTime;
	double m_InitTime;
	void SetMotors(double speed);
public:
	Winch(int motorPortA, int motorPortB, int lockPort, int firePort, int potPort);
	void handle();
	void SetSetpoint(double setpoint);
	double GetAverageVoltage();
	double GetVoltage();
	double GetSetpoint();
	void Fire();
	bool ReadyToFire();
	double TimeSinceLastFire();
	double TimeSinceLastFireStop();
	double TimeSinceLastReloadStop();
	void SetState(e_WinchState state);
	e_WinchState GetState();
	void HoldVoltage(double setpoint);
	void Despring();
};

#endif
