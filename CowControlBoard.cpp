#include "CowControlBoard.h"

/// Constructor for Cow Control Board
CowControlBoard::CowControlBoard()
	:
	m_DriveStick(new Joystick(1)),
	m_DriveWheel(new Joystick(2)),
	m_OperatorPanel(new Joystick(3)),
	m_PreviousAuto(false)
{
}

/// Returns state of shifter switch
float CowControlBoard::GetDriveAxis(unsigned int axis)
{
	return m_DriveStick->GetRawAxis(axis);
}

/// Returns state of autonomous select button
bool CowControlBoard::GetAutoSelectButton()
{
	if(GetDriveButton(AUTON_BUTTON) && !m_PreviousAuto)
	{
		m_PreviousAuto = GetDriveButton(AUTON_BUTTON);
		return true;
	}
	m_PreviousAuto = GetDriveButton(AUTON_BUTTON);
	return false;
}


/// Returns the state of the quick turn
bool CowControlBoard::GetQuickTurn()
{
	return !GetDriveButton(8);
}

float CowControlBoard::GetDriveStickY()
{
	return m_DriveStick->GetRawAxis(LEFT_GAMEPAD_Y);
}

float CowControlBoard::GetSteeringX()
{
	return m_DriveWheel->GetRawAxis(STEERING_X);
}

bool CowControlBoard::GetDriveButton(const int button)
{
	return m_DriveStick->GetRawButton(button);
}

bool CowControlBoard::GetOperatorButton(const int button)
{
	return m_OperatorPanel->GetRawButton(button);
}

bool CowControlBoard::GetSteeringButton(const int button)
{
	return m_DriveWheel->GetRawButton(button);
}
