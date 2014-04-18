#include "AutoModeController.h"

AutoModeController::AutoModeController()
	:
	m_Timer(new Timer()),
	m_CurrentCommand(RobotCommand()),
	m_Fired(false),
	m_SmartLoaded(false),
	m_ChangedSetpoint(false)
{
	m_Timer->Start();
	reset();
}

void AutoModeController::SetCommandList(deque<RobotCommand> list)
{
	m_CommandList = list;
}

void AutoModeController::reset()
{
	//CowConstants * rc = CowConstants::GetInstance();
	//bot->GetGyro()->Reset();
	//bot->GetEncoder()->Reset();

	//m_CommandList.clear();
	//m_CurrentCommand = RobotCommand();
}


void AutoModeController::handle(CowRobot* bot)
{
	bool result = false;
	
	// Run the command
	switch(m_CurrentCommand.m_Command)
	{
		case CMD_NULL:
		{
			doNothing(bot);
			
			result = true;
			break;
		}
		case CMD_WAIT:
		{
			bot->FrontIntake(m_CurrentCommand.m_FrontIntakeSpeed, m_CurrentCommand.m_FrontIntakeState);
			bot->RearIntake(m_CurrentCommand.m_RearIntakeSpeed, m_CurrentCommand.m_RearIntakeState);
			bot->DriveWithHeading(m_CurrentCommand.m_Heading, 0);
			doNothing(bot);
			break;
		}
		case CMD_DESPRING:
		{
			doNothing(bot);
			bot->WinchDespring();
			break;
		}
		case CMD_DETECT_HOT:
		{
			doNothing(bot);
			if(bot->KinectLeftRight() < -0.5)
			{
				m_CommandList = *(m_CurrentCommand.m_HotGoalLeftCommandList);
				printf("Detected kinect left!\n");
				result = true;
			}
			else if(bot->KinectLeftRight() > 0.5)
			{
				m_CommandList = *(m_CurrentCommand.m_HotGoalRightCommandList);
				printf("Detected kinect right!\n");
				result = true;
			}
			break;
		}
		case CMD_CHANGE_SETPOINT:
		{
			bot->FrontIntake(m_CurrentCommand.m_FrontIntakeSpeed, m_CurrentCommand.m_FrontIntakeState);
			bot->RearIntake(m_CurrentCommand.m_RearIntakeSpeed, m_CurrentCommand.m_RearIntakeState);
			bot->DriveWithHeading(m_CurrentCommand.m_Heading, 0);
			if(!m_ChangedSetpoint)
			{
				bot->ChangeWinchSetpoint(m_CurrentCommand.m_WinchSetpoint);
				m_ChangedSetpoint = true;
			}
			else
			{
				if(bot->ReadyToFire())
				{
					m_ChangedSetpoint = false;
					result = true;
				}
			}
			break;
		}
		case CMD_FORCE_FIRE:
		{
			bot->DriveWithHeading(m_CurrentCommand.m_Heading, 0);
			bot->FrontIntake(m_CurrentCommand.m_FrontIntakeSpeed, m_CurrentCommand.m_FrontIntakeState);
			bot->RearIntake(m_CurrentCommand.m_RearIntakeSpeed, m_CurrentCommand.m_RearIntakeState);
			if(!m_Fired)
			{
				bot->ForceFire();
				m_Fired = true;
			}
			else if(bot->ReadyToFire())
			{
				result = true;
				m_Fired = false;
			}
			break;
		}
		case CMD_FIRE:
		{
			bot->DriveDistanceWithHeading(m_CurrentCommand.m_Heading, m_CurrentCommand.m_EncoderCount);
			bot->FrontIntake(m_CurrentCommand.m_FrontIntakeSpeed, m_CurrentCommand.m_FrontIntakeState);
			bot->RearIntake(m_CurrentCommand.m_RearIntakeSpeed, m_CurrentCommand.m_RearIntakeState);
			if(!m_Fired)
			{
				if(bot->ReadyToFire())
				{
					bot->AskForFire();
					m_Fired = true;
				}
			}
			else
			{
				// if we have fired
				if(!m_ChangedSetpoint)
				{
					m_ChangedSetpoint = bot->ChangeWinchSetpoint(m_CurrentCommand.m_WinchSetpoint);
				}
				if(bot->Reloading())
				{
					result = true;
					m_Fired = false;
				}
			}
			break;
		}
		case CMD_FIRE_HEADING_ONLY:
		{
			bot->DriveWithHeading(m_CurrentCommand.m_Heading, 0);
			//bot->DriveDistanceWithHeading(m_CurrentCommand.m_Heading, m_CurrentCommand.m_EncoderCount);
			bot->FrontIntake(m_CurrentCommand.m_FrontIntakeSpeed, m_CurrentCommand.m_FrontIntakeState);
			bot->RearIntake(m_CurrentCommand.m_RearIntakeSpeed, m_CurrentCommand.m_RearIntakeState);
			if(!m_Fired)
			{
				if(bot->ReadyToFire())
				{
					bot->AskForFire();
					m_Fired = true;
				}
			}
			else
			{
				// if we have fired
				if(!m_ChangedSetpoint)
				{
					m_ChangedSetpoint = bot->ChangeWinchSetpoint(m_CurrentCommand.m_WinchSetpoint);
				}
				if(bot->Reloading())
				{
					result = true;
					m_Fired = false;
				}
			}
			break;
		}
		case CMD_FIRE_INSTANT:
		{
			bot->DriveWithHeading(m_CurrentCommand.m_Heading, 0);
			bot->FrontIntake(m_CurrentCommand.m_FrontIntakeSpeed, m_CurrentCommand.m_FrontIntakeState);
			bot->RearIntake(m_CurrentCommand.m_RearIntakeSpeed, m_CurrentCommand.m_RearIntakeState);
			if(bot->ReadyToFire())
			{
				bot->AskForFire();
				result = true;
			}
			break;
		}
		case CMD_FIRE_INTAKE:
		{
			bot->DriveWithHeading(m_CurrentCommand.m_Heading, 0);
			bot->RearIntake(m_CurrentCommand.m_RearIntakeSpeed, m_CurrentCommand.m_RearIntakeState);
			if(bot->ReadyToFire())
			{
				bot->FrontIntake(0, m_CurrentCommand.m_FrontIntakeState);
				if(!m_Fired)
				{
					bot->AskForFire();
					m_Fired = true;
				}
				else
				{
					result = true;
					m_Fired = false;
				}
			}
			else if(m_Fired)
			{
				bot->FrontIntake(m_CurrentCommand.m_FrontIntakeSpeed, m_CurrentCommand.m_FrontIntakeState);
			}
			else
			{
				bot->FrontIntake(0, m_CurrentCommand.m_FrontIntakeState);
			}
			break;
		}
		case CMD_DRIVE_FIRE_SKIP:
		{
			bot->DriveDistanceWithHeading(m_CurrentCommand.m_Heading, m_CurrentCommand.m_EncoderCount);
			bot->FrontIntake(m_CurrentCommand.m_FrontIntakeSpeed, m_CurrentCommand.m_FrontIntakeState);
			bot->RearIntake(m_CurrentCommand.m_RearIntakeSpeed, m_CurrentCommand.m_RearIntakeState);
			if(bot->GetDriveDistance() > m_CurrentCommand.m_FirePoint)
			{
				if(!m_Fired)
				{
					if(bot->ReadyToFire())
					{
						bot->AskForFire();
						m_Fired = true;
					}
				}
				else
				{
					// if we have fired
					if(!m_ChangedSetpoint)
					{
						m_ChangedSetpoint = bot->ChangeWinchSetpoint(m_CurrentCommand.m_WinchSetpoint);
					}
					if(bot->Reloading())
					{
						result = true;
						m_Fired = false;
					}
				}
			}
			break;
		}
		case CMD_DRIVE_FIRE:
		{
			bool drive = bot->DriveDistanceWithHeading(m_CurrentCommand.m_Heading, m_CurrentCommand.m_EncoderCount);
			bot->FrontIntake(m_CurrentCommand.m_FrontIntakeSpeed, m_CurrentCommand.m_FrontIntakeState);
			bot->RearIntake(m_CurrentCommand.m_RearIntakeSpeed, m_CurrentCommand.m_RearIntakeState);
			if(bot->ReadyToFire() && bot->GetDriveDistance() > m_CurrentCommand.m_FirePoint)
			{
				if(!m_Fired)
				{
					bot->AskForFire();
					m_Fired = true;
				}
				else
				{
					// if we have fired
					if(!m_ChangedSetpoint)
					{
						m_ChangedSetpoint = bot->ChangeWinchSetpoint(m_CurrentCommand.m_WinchSetpoint);
					}
					if(drive)
					{
						result = true;
						m_Fired = false;
					}
				}
			}
			break;
		}
		case CMD_INTAKES:
		{
			bot->DriveWithHeading(m_CurrentCommand.m_Heading, 0);
			bot->FrontIntake(m_CurrentCommand.m_FrontIntakeSpeed, m_CurrentCommand.m_FrontIntakeState);
			bot->RearIntake(m_CurrentCommand.m_RearIntakeSpeed, m_CurrentCommand.m_RearIntakeState);
			break;
		}
		case CMD_INTAKES_FRONT:
		{
			//Figure out if we actually sucked in the ball
			if(bot->GetRearIR() > 1)
			{
				result = true;
			}
			bot->DriveWithHeading(m_CurrentCommand.m_Heading, 0);
			bot->FrontIntake(m_CurrentCommand.m_FrontIntakeSpeed, m_CurrentCommand.m_FrontIntakeState);
			bot->RearIntake(m_CurrentCommand.m_RearIntakeSpeed, m_CurrentCommand.m_RearIntakeState);
			break;
		}
		case CMD_TURN:
		{
			bot->FrontIntake(m_CurrentCommand.m_FrontIntakeSpeed, m_CurrentCommand.m_FrontIntakeState);
			bot->RearIntake(m_CurrentCommand.m_RearIntakeSpeed, m_CurrentCommand.m_RearIntakeState);
			result = bot->DriveWithHeading(m_CurrentCommand.m_Heading, 0);
			break;
		}
		case CMD_DRIVE_DISTANCE:
		{
			bot->FrontIntake(m_CurrentCommand.m_FrontIntakeSpeed, m_CurrentCommand.m_FrontIntakeState);
			bot->RearIntake(m_CurrentCommand.m_RearIntakeSpeed, m_CurrentCommand.m_RearIntakeState);
			result = bot->DriveDistanceWithHeading(m_CurrentCommand.m_Heading, m_CurrentCommand.m_EncoderCount);
			break;
		}
		case CMD_SETTLE_BALL:
		{
			bot->FrontIntake(0, m_CurrentCommand.m_FrontIntakeState);
			bot->RearIntake(0, m_CurrentCommand.m_RearIntakeState);
//			bot->SetFrontIntakeState(m_CurrentCommand.m_FrontIntakeState);
//			bot->SetRearIntakeState(m_CurrentCommand.m_RearIntakeState);
			bot->AutoSettle();
			result = (bot->GetSettledState() == CowRobot::SETTLE_FINISHED);
			break;
		}
		default:
		{
			doNothing(bot);
			result = true;
			break;
		}
	}
	
	// Check if this command is done
	if(result == true || m_CurrentCommand.m_Command == CMD_NULL || m_Timer->Get() > m_CurrentCommand.m_Timeout)
	{
		if(m_CurrentCommand.m_Command == CMD_DETECT_HOT && m_Timer->Get() > m_CurrentCommand.m_Timeout)
		{
			m_CommandList = *(m_CurrentCommand.m_HotGoalLeftCommandList);
		}
		// This command is done, go get the next one
		if(m_CommandList.size() > 0 )
		{			
			m_CurrentCommand = m_CommandList.front();
			m_CommandList.pop_front();
			
			if(!m_CurrentCommand.m_Command == CMD_NULL)
				printf("Time elapsed: %f\n", m_Timer->Get());
			
			m_Timer->Reset();
			
			m_Fired = false;
			m_SmartLoaded = false;
			m_ChangedSetpoint = false;
		}
		else
		{
			//we're done clean up
			m_CurrentCommand = RobotCommand();
		}
	}
}

// Drive Functions
void AutoModeController::doNothing(CowRobot* bot)
{
	bot->DriveLeftRight(0, 0);
	bot->FrontIntake(0, true);
	bot->RearIntake(0, true);
//	bot->DriveLeftRight(0,0);
//	bot->GetShooter()->SetRaw(0);
//	bot->GetFeeder()->SetRaw(0);
//	bot->GetIntake()->SetRaw(0);
	//bot->getArm()->SetMotor(0);
	//bot->getArm()->
}
