#include "CowRobot.h"
#include "CowBase.h"

// TODO: a lot of the older functions in this file are clunky.  clean them up.

//TODO: initializer list
/// Constructor for CowRobot
CowRobot::CowRobot()
{	
	m_DSUpdateCount = 0;
		
	m_Controller = NULL;
	// Set up drive motors
	m_RightDrive = new Talon(PWM_RIGHT_DRIVE);
	m_LeftDrive = new Talon(PWM_LEFT_DRIVE);
	
	m_Compressor = new Relay(RELAY_COMPRESSOR);
	m_PressureSwitch = new DigitalInput(DIGITAL_PRESSURE_SWITCH);
	
	m_FrontIntake = new Intake(PWM_FRONT_INTAKE_A, PWM_FRONT_INTAKE_B, SOLENOID_FRONT_INTAKE_A, SOLENOID_FRONT_INTAKE_B);
	m_RearIntake = new Intake(PWM_REAR_INTAKE_A, PWM_REAR_INTAKE_B, SOLENOID_REAR_INTAKE_A, SOLENOID_REAR_INTAKE_B);
	m_Winch = new Winch(PWM_WINCH_A, PWM_WINCH_B, SOLENOID_WINCH_LOCK, SOLENOID_WINCH_FIRE, ANALOG_WINCH_POT);
	
	m_Gyro = new CowLib::CowGyro(ANALOG_GYRO);
	m_Gyro->SetSensitivity(GYRO_SENSITIVITY);
	m_Gyro->Reset();
	
	m_DriveEncoder = new Encoder(DIGITAL_ENCODER_A, DIGITAL_ENCODER_B, true, Encoder::k1X);
	m_DriveEncoder->SetDistancePerPulse(0.03490658503); // 4*pi/360
	m_DriveEncoder->Start();
	
	m_FrontIR = new AnalogChannel(ANALOG_FRONT_IR);
	m_RearIR = new AnalogChannel(ANALOG_REAR_IR);
	
	m_KinectLeftArm = new KinectStick(1);
	m_KinectRightArm = new KinectStick(2);
	
	m_LeftDriveValue = 0;
	m_RightDriveValue = 0;
	
	m_FrontIntakeSpeed = 0;
	m_FrontIntakeState = false;
	m_RearIntakeSpeed = 0;
	m_RearIntakeState = false;	
	
	m_BallAdjustTime = 0;
	m_FiringState = IDLE;
	m_SettlingState = SETTLE_IDLE;
	m_BallSettledTime = 0;
	
	m_PreviousGyroError = 0;
	m_PreviousDriveError = 0;
}

void CowRobot::Reset()
{
	m_Gyro->Reset();
	m_DriveEncoder->Reset();
	m_PreviousGyroError = 0;
	m_PreviousDriveError = 0;
	if(m_Winch->GetState() == Winch::DESPRINGING)
	{
		m_FiringState = IDLE;
	}
	if(m_FiringState != IDLE)
	{
		m_FiringState = WAITING_FOR_RELOAD;
	}
	m_FrontIntakeState = false;
	m_RearIntakeState = false;

	m_FrontIntake->SetState(m_FrontIntakeState);
	m_RearIntake->SetState(m_RearIntakeState);
	m_SettlingState = SETTLE_IDLE;
}

double CowRobot::GetFrontIR()
{
	return m_FrontIR->GetVoltage();
}

double CowRobot::GetRearIR()
{
	return m_RearIR->GetVoltage();
}

void CowRobot::GyroHandleCalibration()
{
	m_Gyro->HandleCalibration();
}

void CowRobot::GyroFinalizeCalibration()
{
	m_Gyro->FinalizeCalibration();
}

void CowRobot::SetController(GenericController* controller)
{
	m_Controller = controller;
}

void CowRobot::PrintToDS()
{
	if(m_DSUpdateCount % 10 == 0)
	{
		m_DSUpdateCount = 0;
		char* kinectArms = ".0. (No arms up)";
		if(KinectLeftRight() < -0.5)
		{
			kinectArms = "\\0. (Left arm up)";
		}
		else if(KinectLeftRight() > 0.5)
		{
			kinectArms = ".0/ (Right arm up)";
		}
		CowLib::PrintToLCD("K:%s\nA:%s\nP:%f\nG:%f\nE:%f\nI:F%f_R%f",
						   kinectArms,
						   AutoModes::GetInstance()->GetName(),
						   m_Winch->GetAverageVoltage(),
						   m_Gyro->GetAngle(),
						   m_DriveEncoder->GetDistance(),
						   m_FrontIR->GetVoltage(), m_RearIR->GetVoltage());
	}
	m_DSUpdateCount++;
}

/// Used to handle the recurring logic funtions inside the robot.
/// Please call this once per update cycle.
void CowRobot::handle()
{	
	if(m_Controller == NULL)
	{
		printf("No controller for CowRobot!!\n");
		return;
	}
	//printf("Handling...\n");
	m_Controller->handle(this);
	
	// Default drive
	float tmpLeftMotor = m_LeftDriveValue;
	float tmpRightMotor = m_RightDriveValue;
	
	switch(m_FiringState)
	{
		case IDLE:
		{
			m_FrontIntake->SetSpeed(m_FrontIntakeSpeed);
			m_RearIntake->SetSpeed(m_RearIntakeSpeed);
			
			break;
		}
		case ADJUSTING_BALL:
		{
			if((Timer::GetFPGATimestamp() - m_BallAdjustTime) < CONSTANT("BallAdjustTime") && !m_RearIntake->IsPhysicallyDropped())
			{
				m_FrontIntake->SetSpeed(0);
				m_RearIntake->SetSpeed(-1);
			}
			else
			{
				m_FiringState = DROPPING_INTAKES;
			}
			break;
		}
		case DROPPING_INTAKES:
		{
			m_RearIntakeState = true;
			m_FrontIntakeState = true;
			
			if(m_FrontIntake->IsPhysicallyDropped() && m_RearIntake->IsPhysicallyDropped())
			{
				m_Winch->Fire();
				m_FiringState = WAITING_FOR_RELOAD;
				//printf("CowRobot: WAITING_FOR_RELOAD\n");
			}
			else
			{
				if(!m_FrontIntake->IsPhysicallyDropped())
				{
					m_FrontIntake->SetSpeed(-CONSTANT("FRONT_INTAKE_DROP_SPEED"));
				}
				if(!m_RearIntake->IsPhysicallyDropped())
				{
					m_RearIntake->SetSpeed(CONSTANT("REAR_INTAKE_DROP_SPEED"));
				}
			}
			break;
		}
		case WAITING_FOR_RELOAD:
		{
			if(m_Winch->GetState() != Winch::READY_TO_FIRE)
			{
				m_FrontIntake->SetSpeed(m_FrontIntakeSpeed);
				m_RearIntake->SetSpeed(m_RearIntakeSpeed);
				
				if(m_Winch->GetState() == Winch::RELOADING || m_Winch->GetState() == Winch::LOCKING || m_Winch->GetState() == Winch::HOLDING && m_Winch->GetAverageVoltage() < CONSTANT("IntakeSafeSetpoint"))
				{
					//printf("catapult not locked but returning op control\n");
				}
				else
				{
					m_FrontIntakeState = true;
					m_RearIntakeState = true;
				}
			} 
			else 
			{
				//printf("CowRobot: IDLE\n");
				m_FiringState = IDLE;
			}
			break;
		}
		case AUTO_SETTLING:
		{
			double rearIR = m_RearIR->GetVoltage();
			double frontIR = m_FrontIR->GetVoltage();
			
			//if(m_SettlingState == SETTLE_IDLE && rearIR > 1.1 && frontIR > 1.1)
			if(m_SettlingState == SETTLE_IDLE || m_SettlingState == SETTLE_FINISHED)
			{
				m_SettlingState = SETTLE_EXHAUST_BOTH;
				m_BallSettledTime = Timer::GetFPGATimestamp();
			}
			if((m_SettlingState == SETTLE_EXHAUST_BOTH && rearIR > 2.3) ||
				(m_SettlingState == SETTLE_EXHAUST_BOTH && (Timer::GetFPGATimestamp() - m_BallSettledTime) > 5))
			{
				m_SettlingState = SETTLE_EXHAUST_REAR;
				m_BallSettledTime = Timer::GetFPGATimestamp();
			}
			if((m_SettlingState == SETTLE_EXHAUST_REAR && (frontIR > 2.7 && frontIR < 2.85 && rearIR < 2.85 && rearIR > 2.6)) ||
				(m_SettlingState == SETTLE_EXHAUST_REAR && (Timer::GetFPGATimestamp() - m_BallSettledTime) > 0.5))
			{
				m_SettlingState = SETTLE_EXHAUST_REAR_PULSE;
				m_BallSettledTime = Timer::GetFPGATimestamp();
			}
			if(m_SettlingState == SETTLE_EXHAUST_REAR_PULSE  && (Timer::GetFPGATimestamp() - m_BallSettledTime) > 0.75)
			{
				m_SettlingState = SETTLE_EXHAUST_FRONT;
				m_BallSettledTime = Timer::GetFPGATimestamp();
			}
			if(m_SettlingState == SETTLE_EXHAUST_FRONT && (Timer::GetFPGATimestamp() - m_BallSettledTime) > 0.25)
			{
				m_SettlingState = SETTLE_FINISHED;
				m_FiringState = IDLE;
				printf("Done with autosettle\n");
			}
			
			if(m_FrontIntakeSpeed != 0 || m_RearIntakeSpeed != 0)
			{
				m_SettlingState = SETTLE_FINISHED;
				m_FiringState = IDLE;
			}
			
			switch(m_SettlingState)
			{
				case SETTLE_IDLE:
				{
					m_RearIntake->SetSpeed(0); 
					m_FrontIntake->SetSpeed(0);
					break;
				}
				case SETTLE_FINISHED:
				{
					m_RearIntake->SetSpeed(0); 
					m_FrontIntake->SetSpeed(0);
					break;
				}
				case SETTLE_EXHAUST_REAR:
				{
					m_RearIntake->SetSpeed(-1);
					m_FrontIntake->SetSpeed(0);
					break;
				}
				case SETTLE_EXHAUST_REAR_PULSE:
				{
					if(sin(Timer::GetFPGATimestamp()*48) > 0)
					{
						m_RearIntake->SetSpeed(-1);
					}
					else
					{
						m_RearIntake->SetSpeed(0);
					}
					m_FrontIntake->SetSpeed(0);
					break;
				}
				case SETTLE_EXHAUST_FRONT:
				{
					m_RearIntake->SetSpeed(-0.2); 
					m_FrontIntake->SetSpeed(1);
					break;
				}
				case SETTLE_PULL_DOWN:
				{
					m_RearIntake->SetSpeed(-1); 
					m_FrontIntake->SetSpeed(-0.5);
					break;
				}
				case SETTLE_EXHAUST_BOTH:
				{
					m_RearIntake->SetSpeed(-1); 
					m_FrontIntake->SetSpeed(1);
					break;
				}
			}
			break;
		}
	}
	
	if(m_RearIntakeState && !m_RearIntake->GetState() &&
	   m_FrontIntakeState && !m_FrontIntake->IsPhysicallyDropped())
	{
		m_RearIntakeState = false;
	}
	
	m_FrontIntake->SetState(m_FrontIntakeState);
	m_RearIntake->SetState(m_RearIntakeState);
	
	m_Winch->handle();
	SetLeftMotors(tmpLeftMotor);
	SetRightMotors(tmpRightMotor);
	//printf("%f, %f\r\n",  m_Encoder->GetDistance(), m_Gyro->GetAngle());
	//printf("%f\n", m_Gyro->GetAngle());
	
	if(m_PressureSwitch->Get())
	{
		m_Compressor->Set(Relay::kOff);
	} else
	{
		m_Compressor->Set(Relay::kForward);
	}
}

void CowRobot::AutoSettle()
{
	m_FiringState = AUTO_SETTLING;
}

CowRobot::e_SettlingState CowRobot::GetSettledState()
{
	return m_SettlingState;
}

double CowRobot::GetDriveDistance()
{
	return m_DriveEncoder->GetDistance();
}

bool CowRobot::DriveDistanceWithHeading(double heading, double distance)
{
	double PID_P = CONSTANT("DRIVE_P");
	double PID_D = CONSTANT("DRIVE_D");
	double error = distance - m_DriveEncoder->GetDistance();
	double dError = error - m_PreviousDriveError;
	double output = PID_P*error + PID_D*dError;
	
	bool headingResult = DriveWithHeading(heading, CowLib::LimitMix(output, CONSTANT("DRIVE_MAX_SPEED")));
	
	m_PreviousDriveError = error;
	
	return (fabs(error) < 4 && CowLib::UnitsPerSecond(fabs(dError)) < 1 && headingResult);
}

bool CowRobot::DriveWithHeading(double heading, double speed)
{
	speed *= -1;
	double PID_P = CONSTANT("TURN_P");
	double PID_D = CONSTANT("TURN_D");
	double error = heading - m_Gyro->GetAngle();
	double dError = error - m_PreviousGyroError;
	double output = PID_P*error + PID_D*dError;
				
	DriveLeftRight(speed-output, speed+output);
	
	m_PreviousGyroError = error;
	
	return (fabs(error) < 1 && CowLib::UnitsPerSecond(fabs(dError)) < 0.5);
}

void CowRobot::SetFrontIntakeState(bool state)
{
	m_FrontIntakeState = state;
}

void CowRobot::SetRearIntakeState(bool state)
{
	m_RearIntakeState = state;
}

void CowRobot::FrontIntake(double speed, bool state)
{
	m_FrontIntakeSpeed = -speed;
	m_FrontIntakeState = state;
}

void CowRobot::RearIntake(double speed, bool state)
{
	m_RearIntakeSpeed = speed;
	m_RearIntakeState = state;
}

void CowRobot::SetWinchState(Winch::e_WinchState state)
{
	m_Winch->SetState(state);
}

void CowRobot::AskForFire()
{
	if(ReadyToFire())
	{
		m_BallAdjustTime = Timer::GetFPGATimestamp();
		m_FiringState = ADJUSTING_BALL;
	}
}

void CowRobot::ForceFire()
{
	if(m_FiringState == IDLE || m_FiringState == WAITING_FOR_RELOAD)
	{
		m_BallAdjustTime = Timer::GetFPGATimestamp();
		m_FiringState = ADJUSTING_BALL;
	}
}

bool CowRobot::ReadyToFire()
{
	return m_FiringState == IDLE && m_Winch->GetState() == Winch::READY_TO_FIRE;
}

bool CowRobot::Reloading()
{
	return m_Winch->GetState() == Winch::RELOADING;
}

bool CowRobot::ChangeWinchSetpoint(double setpoint)
{
	if(m_FiringState == IDLE || m_FiringState == WAITING_FOR_RELOAD)
	{
		m_Winch->SetSetpoint(setpoint);
		return true;
	}
	return false;
}

void CowRobot::WinchHoldVoltage(double setpoint)
{
	if(m_FiringState == IDLE)
	{
		if(fabs(m_Winch->GetSetpoint() - setpoint) > 0.01)
		{
			m_Winch->HoldVoltage(setpoint);
		}
	}
}

void CowRobot::WinchDespring()
{
	if(m_FiringState == IDLE || m_FiringState == WAITING_FOR_RELOAD)
	{
		m_Winch->Despring();
	}
}

double CowRobot::KinectLeftRight()
{
	return m_KinectLeftArm->GetY() - m_KinectRightArm->GetY();
}

/// Allows skid steer robot to be driven using tank drive style inputs
/// @param leftDriveValue
/// @param rightDriveValue
///
void CowRobot::DriveLeftRight(float leftDriveValue, float rightDriveValue)
{
	m_LeftDriveValue = leftDriveValue;
	m_RightDriveValue = rightDriveValue;
}

void CowRobot::DriveSpeedTurn(float speed, float turn, bool quickTurn)
{
	//Linear degredation of steeering based off of velocity
	//	velocity *= 0.003;
	float temp_vel = speed;
	float sensitivity = 0;
	float unscaled_turn = 0;

	if (temp_vel < 0)
		temp_vel = -temp_vel;

	//printf("Velocity: %f, stick: %f\r\n", velocity, temp_vel);
	
	if(speed < 0.10 && speed > -0.10)
		speed = 0;
	if (turn < 0.10 && turn > -0.10 || (speed == 0 && !quickTurn))
		turn = 0;

	unscaled_turn = turn;

	if(quickTurn)
		sensitivity = 1;
	else
		sensitivity = 0.4;

	turn *= sensitivity;
	turn = -turn;
	

	float left_power = CowLib::LimitMix(speed + turn);
	float right_power = CowLib::LimitMix(speed - turn);

	DriveLeftRight(left_power, right_power);
}

/// Allows robot to spin in place
/// @param turnRate
///
void CowRobot::QuickTurn(float turnRate)
{
	//when provided with + turn, quick turn right

	float left = -1 * turnRate;
	float right = turnRate;

	DriveLeftRight(left, right);
}

/// Returns the value of the drive's left side encoder
//Encoder * CowRobot::GetEncoder()
//{
//	return m_Encoder;
//}
//
//Gyro * CowRobot::GetGyro()
//{
//	return m_Gyro;
//}

/// sets the left side motors
void CowRobot::SetLeftMotors(float val)
{
	if (val > 1.0)
		val = 1.0;
	if (val < -1.0)
		val = -1.0;

	m_LeftDrive->SetSpeed(-val);
}

/// sets the left side motors
void CowRobot::SetRightMotors(float val)
{
	if (val > 1.0)
		val = 1.0;
	if (val < -1.0)
		val = -1.0;

	m_RightDrive->SetSpeed(val);
}
