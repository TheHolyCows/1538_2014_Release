//==================================================
// Copyright (C) 2014 Team 1538 / The Holy Cows
//==================================================

#ifndef __COW_BASE_H__
#define __COW_BASE_H__

#include <hostLib.h>
#include <WPILib.h>
#include "CowControlBoard.h"
#include "Controllers/OperatorController.h"
#include "Controllers/AutoModeController.h"
#include "Autonomous/AutoModes.h"
#include "CowLib/CowLib.h"
#include "CowConstants.h"

class CowBase : public IterativeRobot
{
private:
	CowRobot* m_Bot;
	CowControlBoard* m_ControlBoard;
	OperatorController* m_OpController;
	AutoModeController* m_AutoController;
	CowConstants* m_Constants;
	Winch::e_WinchState m_WinchState;
public:
	CowBase();
	void RobotInit();
	void DisabledInit();
	void AutonomousInit();
	void TeleopInit();
	void DisabledContinuous();
	void AutonomousContinuous();
	void TeleopContinuous();
	void DisabledPeriodic();
	void AutonomousPeriodic();
	void TeleopPeriodic();
};

#endif

