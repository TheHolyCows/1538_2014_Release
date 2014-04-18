//==================================================
// Copyright (C) 2014 Team 1538 / The Holy Cows
//==================================================

#ifndef __COW_CONTROL_BOARD_H__
#define __COW_CONTROL_BOARD_H__

#include <WPILib.h>

#define SHIFTER_BUTTON			4
#define AUTON_BUTTON			1
#define PID_BUTTON				3

#define LEFT_GAMEPAD_X			1
#define LEFT_GAMEPAD_Y			2
#define RIGHT_GAMEPAD_X			3
#define RIGHT_GAMEPAD_Y			4
#define STEERING_X				1

#define FAST_TURN				6 

/// This class offers access to the 2010 specific Cow Control Board
///
class CowControlBoard
{
private:	
	Joystick* m_DriveStick;
	Joystick* m_DriveWheel;
	Joystick* m_OperatorPanel;
	
	bool m_PreviousAuto;
public:
	CowControlBoard();
	
	bool GetQuickTurn();
	bool GetAutoSelectButton();
	
	float GetDriveAxis(unsigned int axis);
	
	float GetDriveStickY();
	float GetSteeringX();
	
	bool GetDriveButton(int button);
	bool GetSteeringButton(int button);
	bool GetOperatorButton(int button);
};

#endif
