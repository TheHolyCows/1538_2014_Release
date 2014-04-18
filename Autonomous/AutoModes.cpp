#include "AutoModes.h"

AutoModes*  AutoModes::m_SingletonInstance = NULL;

AutoModes* AutoModes::GetInstance()
{
	if(m_SingletonInstance == NULL)
	{
		m_SingletonInstance = new AutoModes();
	}
	return m_SingletonInstance;
}

AutoModes::AutoModes()
{	
	//
	// WARNING: NEVER REUSE CMD_DRIVE_FIRE, CMD_SMART_REAR, CMD_FIRE_INTAKE, CMD_DRIVE_FIRE_SKIP UNLESS YOU KNOW WHAT YOU ARE DOING.  ROBOT DAMAGE WILL OCCUR IF IMPROPER PARAMETERS ARE SPECIFIED.
	//
	
	//											 COMMAND			ENCODER	FIREAT	HEADING	FRONT	REAR	FSPEED	RSPEED	REARIR		WINCHSV		TIMEOUT
	
	// Set up our selection order
	m_Modes["Hot Goal Kinect"];
	m_Modes["Una hot left"];
	m_Modes["Una hot right"];
	m_Modes["Una pelota"];

	
	m_Modes["Hot Goal Kinect"].push_back(RobotCommand(CMD_DETECT_HOT, &(m_Modes["Hot Goal Left"]), &(m_Modes["Hot Goal Right"]), 2));

	m_Modes["Una hot left"].push_back(RobotCommand(CMD_DETECT_HOT, &(m_Modes["Una pelota"]), &(m_Modes["Una delay"]), 2));
	m_Modes["Una hot right"].push_back(RobotCommand(CMD_DETECT_HOT, &(m_Modes["Una delay"]), &(m_Modes["Una pelota"]), 2));
	
	m_Modes["Una pelota"].push_back(RobotCommand(CMD_INTAKES,		100,		0,		0,		true,	true,	0,		0,		false,		0, 			0.5));
	m_Modes["Una pelota"].push_back(RobotCommand(CMD_DRIVE_DISTANCE,100,		0,		0,		true,	true,	0,		0,		false,		0,			5));
	m_Modes["Una pelota"].push_back(RobotCommand(CMD_FIRE,			100,		0,		0,		true,	true,	0,		0,		false,		0,			5));
	
	m_Modes["Una delay"].push_back(RobotCommand(CMD_INTAKES,		100,		0,		0,		true,	true,	0,		0,		false,		0, 			0.5));
	m_Modes["Una delay"].push_back(RobotCommand(CMD_DRIVE_DISTANCE,100,		0,		0,		true,	true,	0,		0,		false,		0,			5));
	m_Modes["Una delay"].push_back(RobotCommand(CMD_WAIT,		100,		0,		0,		true,	true,	0,		0,		false,		0,			3));
	m_Modes["Una delay"].push_back(RobotCommand(CMD_FIRE,		100,		0,		0,		true,	true,	0,		0,		false,		0,			5));
	
	m_Modes["Dos pelotas"].push_back(RobotCommand(CMD_INTAKES,		100,		0,		0,		true,	true,	0,		0,		false,		0, 			0.5));
	m_Modes["Dos pelotas"].push_back(RobotCommand(CMD_DRIVE_DISTANCE,100,		0,		0,		true,	true,	0,		0,		false,		0,			5));
	m_Modes["Dos pelotas"].push_back(RobotCommand(CMD_FIRE,			100,		0,		0,		true,	true,	0,		0,		false,		0,			5));
	m_Modes["Dos pelotas"].push_back(RobotCommand(CMD_INTAKES,		100,		0,		0,		true,	false,	0.8,	0,		false,		0, 			1));
	m_Modes["Dos pelotas"].push_back(RobotCommand(CMD_INTAKES,		100,		0,		0,		false,	false,	-0.15,	-0.4,	false,		0, 			1));
	m_Modes["Dos pelotas"].push_back(RobotCommand(CMD_FIRE,			100,		0,		0,		true,	true,	0,		0,		false,		0, 			5));
	
//	m_Modes["Ballz deep"].push_back(RobotCommand(CMD_INTAKES,		96,	0,		0,		true,	true,	0,		0,		true,		0, 			0.5));
//	m_Modes["Ballz deep"].push_back(RobotCommand(CMD_DRIVE_FIRE,	96,	80,		0,		true,	true,	0,		0,		true,		CONSTANT("HalfBackSetpoint"),			5));
//	m_Modes["Ballz deep"].push_back(RobotCommand(CMD_SMART_REAR,	96,	0,		0,		true,	true,	0,		0,		false,		0, 			1.75));
//	m_Modes["Ballz deep"].push_back(RobotCommand(CMD_FIRE_INTAKE,	96,	0,		0,		false,	true,	0.8,	0,		false,		0, 			5));
//	m_Modes["Ballz deep"].push_back(RobotCommand(CMD_INTAKES,		96,	0,		0,		false,	false,	-0.15,	-0.4,	false,		0, 			1));
//	m_Modes["Ballz deep"].push_back(RobotCommand(CMD_FIRE,			96,	0,		0,		true,	true,	0,		0,		false,		0, 			5));
		
	m_Modes["Hot Goal Left"].push_back(RobotCommand(CMD_DRIVE_DISTANCE,	24,			0,		0,		true,	true,	0,		0,		false,		0,			0.125));	
	m_Modes["Hot Goal Left"].push_back(RobotCommand(CMD_INTAKES,		103,		0,		0,		true,	true,	0,		0,		false,		0, 			0.5));
	m_Modes["Hot Goal Left"].push_back(RobotCommand(CMD_DRIVE_FIRE_SKIP,103,		85,		-15,	true,	true,	0.07,	0,		false,		0,			3.8));	
	m_Modes["Hot Goal Left"].push_back(RobotCommand(CMD_INTAKES_FRONT,	103,		0,		-25,	true,	false,	1,		0,		false,		0, 			10));
	m_Modes["Hot Goal Left"].push_back(RobotCommand(CMD_INTAKES,		103,		0,		-25,	false,	false,	1,		0,		false,		0, 			0.75));
	m_Modes["Hot Goal Left"].push_back(RobotCommand(CMD_SETTLE_BALL,	103,		0,		-25,	false,	false,	0,		0,		false,		0, 			1.5));
	m_Modes["Hot Goal Left"].push_back(RobotCommand(CMD_INTAKES,		103,		0,		-25,	false,	false,	0,		0,		false,		0, 			0.75));
	m_Modes["Hot Goal Left"].push_back(RobotCommand(CMD_TURN,			103,		103,	30,		true,	true,	0,		0,		false,		0,			1));	
	m_Modes["Hot Goal Left"].push_back(RobotCommand(CMD_FIRE_HEADING_ONLY,103,		103,	30,		true,	true,	0,		0,		false,		0,			3.8));	
	m_Modes["Hot Goal Left"].push_back(RobotCommand(CMD_DESPRING,		103,		103,	30,		true,	true,	0,		0,		false,		0,			3));	

	
	m_Modes["Hot Goal Right"].push_back(RobotCommand(CMD_DRIVE_DISTANCE,24,			0,		0,		true,	true,	0,		0,		false,		0,			0.125));	
	m_Modes["Hot Goal Right"].push_back(RobotCommand(CMD_INTAKES,		103,		0,		0,		true,	true,	0,		0,		false,		0, 			0.5));
	m_Modes["Hot Goal Right"].push_back(RobotCommand(CMD_DRIVE_FIRE_SKIP,103,		85,		15,		true,	true,	0.07,	0,		false,		0,			3.8));	
	m_Modes["Hot Goal Right"].push_back(RobotCommand(CMD_INTAKES_FRONT,	103,		0,		25,		true,	false,	1,		0,		false,		0, 			10));
	m_Modes["Hot Goal Right"].push_back(RobotCommand(CMD_INTAKES,		103,		0,		25,		false,	false,	1,		0,		false,		0, 			0.75));
	m_Modes["Hot Goal Right"].push_back(RobotCommand(CMD_SETTLE_BALL,	103,		0,		25,		false,	false,	0,		0,		false,		0, 			1.5));
	m_Modes["Hot Goal Right"].push_back(RobotCommand(CMD_INTAKES,		103,		0,		25,		false,	false,	0,		0,		false,		0, 			0.75));
	m_Modes["Hot Goal Right"].push_back(RobotCommand(CMD_TURN,			103,		103,	-30,	true,	true,	0,		0,		false,		0,			1));	
	m_Modes["Hot Goal Right"].push_back(RobotCommand(CMD_FIRE_HEADING_ONLY,103,		103,	-30,	true,	true,	0,		0,		false,		0,			3.8));	
	m_Modes["Hot Goal Right"].push_back(RobotCommand(CMD_DESPRING,		103,		103,	-30,	true,	true,	0,		0,		false,		0,			3));	

	m_Iterator = m_Modes.begin();
}

std::deque<RobotCommand> AutoModes::GetCommandList()
{
	return m_Iterator->second;
}

char* AutoModes::GetName()
{
	return m_Iterator->first;
}

void AutoModes::NextMode()
{
	m_Iterator++;
	if(m_Iterator == m_Modes.end())
	{
		m_Iterator = m_Modes.begin();
	}
}
