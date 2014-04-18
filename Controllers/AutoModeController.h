//==================================================
// Copyright (C) 2014 Team 1538 / The Holy Cows
//==================================================

#ifndef __AUTO_MODE_CONTROLLER_H__
#define __AUTO_MODE_CONTROLLER_H__

#include <deque>
#include "../CowConstants.h"

typedef enum
{
	CMD_NULL = 0,
	CMD_CHANGE_SETPOINT,
	CMD_FIRE,
	CMD_FORCE_FIRE,
	CMD_FIRE_HEADING_ONLY,
	CMD_FIRE_INSTANT,
	CMD_FIRE_INTAKE,
	CMD_DRIVE_FIRE,
	CMD_DRIVE_FIRE_SKIP,
	CMD_INTAKES,
	CMD_INTAKES_FRONT,
	CMD_TURN,
	CMD_DRIVE_DISTANCE,
	CMD_SETTLE_BALL,
	CMD_DETECT_HOT,
	CMD_WAIT,
	CMD_DESPRING
} e_RobotCommand;

class RobotCommand
{
public:
	
	e_RobotCommand m_Command;
	double m_EncoderCount;
	double m_FirePoint;
	double m_Heading;
	bool m_FrontIntakeState;
	bool m_RearIntakeState;
	double m_FrontIntakeSpeed;
	double m_RearIntakeSpeed;
	bool m_RearIRHold;
	double m_WinchSetpoint;
	double m_Timeout;
	deque<RobotCommand>* m_HotGoalLeftCommandList;
	deque<RobotCommand>* m_HotGoalRightCommandList;
	
	RobotCommand() :
		m_Command(CMD_NULL),
		m_EncoderCount(0),
		m_FirePoint(0),
		m_Heading(0),
		m_FrontIntakeState(false),
		m_RearIntakeState(false),
		m_FrontIntakeSpeed(0),
		m_RearIntakeSpeed(0),
		m_RearIRHold(false),
		m_WinchSetpoint(CONSTANT("FullBackSetpoint")),
		m_Timeout(0)
	{
	}
	
	RobotCommand(e_RobotCommand cmd, double encoder, double firepoint, double heading, bool frontstate, bool rearstate, double frontspeed, double rearspeed, bool rearhold, double setpoint, double timeout) :
		m_Command(cmd),
		m_EncoderCount(encoder),
		m_FirePoint(firepoint),
		m_Heading(heading),
		m_FrontIntakeState(frontstate),
		m_RearIntakeState(rearstate),
		m_FrontIntakeSpeed(frontspeed),
		m_RearIntakeSpeed(rearspeed),
		m_RearIRHold(rearhold),
		m_WinchSetpoint(setpoint),
		m_Timeout(timeout)
	{
	}
	
	RobotCommand(e_RobotCommand cmd, deque<RobotCommand>* left, deque<RobotCommand>* right, double timeout) :
		m_Command(cmd),
		m_EncoderCount(0),
		m_FirePoint(0),
		m_Heading(0),
		m_FrontIntakeState(false),
		m_RearIntakeState(false),
		m_FrontIntakeSpeed(0),
		m_RearIntakeSpeed(0),
		m_RearIRHold(false),
		m_WinchSetpoint(CONSTANT("FullBackSetpoint")),
		m_Timeout(timeout),
		m_HotGoalLeftCommandList(left),
		m_HotGoalRightCommandList(right)
	{
	}
};

#include "../CowRobot.h"
#include "../CowLib/CowLib.h"

class AutoModeController : public GenericController
{
private:
	Timer* m_Timer; //TODO: standardize timing
	deque<RobotCommand> m_CommandList;
	RobotCommand m_CurrentCommand;
	bool m_Fired;
	bool m_SmartLoaded;
	bool m_ChangedSetpoint;
	
	// do nothing
	void doNothing(CowRobot* bot);
public:
	AutoModeController();
	void SetCommandList(deque<RobotCommand> list);

	void handle(CowRobot* bot);
	void reset();
};


#endif

