#include "Winch.h"

Winch::Winch(int motorPortA, int motorPortB, int lockPort, int firePort, int potPort)
	:
	m_MotorA(new Talon(motorPortA)),
	m_MotorB(new Talon(motorPortB)),
	m_Lock(new Solenoid(lockPort)),
	m_Fire(new Solenoid(firePort)),
	m_Pot(new AnalogChannel(potPort)),
	m_State(UNLOCKING),
	m_NextState(RELOADING),
	m_Setpoint(CONSTANT("DefaultSetpoint")),
	m_PreviousVoltage(0),
	m_LastUnlockVoltage(0),
	m_HoldVoltage(0),
	m_LastFireTime(0),
	m_LastFireStopTime(0),
	m_LastReloadStopTime(0),
	m_InitTime(0)
{
	m_Pot->SetAverageBits(6);
}

void Winch::handle()
{
	// TODO: improve readability
	switch(m_State)
	{
		case READY_TO_FIRE:
		{
			//printf("LOCKED (READY_TO_FIRE)\n");
			m_Lock->Set(false);
			m_Fire->Set(false);
			SetMotors(0);
			break;
		}
		case PREFIRING:
		{
			m_Lock->Set(true);
			double error = m_Pot->GetAverageVoltage() - m_Setpoint;
			double change = error - (m_PreviousVoltage - m_Setpoint);
			const double PID_P = CONSTANT("PID_P");
			const double PID_D = 3*CONSTANT("PID_D");
			double output = PID_P*error + PID_D*change;
			if(fabs(error) < CONSTANT("LockBand"))
			{
				//printf("Pulling freespin\n");
				m_Fire->Set(true);
			}
			if(CowLib::UnitsPerSecond(fabs(change)) > 10)
			{
				printf("Freespinning detected\n");
				SetMotors(0);
				m_State = FIRING;
				break;
			}
			if(output < 0)
			{
				output = CowLib::LimitMix(output, 0.225); 
			}
			//printf("PREFIRING: %f\t%f\n", error, output);
			SetMotors(output);
			break;
		}
		case FIRING:
		{
			//printf("FIRING\n");
			m_Lock->Set(true);
			m_Fire->Set(true);
			SetMotors(0);
			if(m_Pot->GetAverageVoltage() > CONSTANT("FiredSetpoint")  && TimeSinceLastFireStop() > 2)
			{
				m_LastFireStopTime = Timer::GetFPGATimestamp();
			}
			if(TimeSinceLastFireStop() > 0.2 && m_Pot->GetAverageVoltage() > CONSTANT("FiredSetpoint"))
			{
				m_Setpoint += 0.01;
				m_State = RELOADING;
			}
			break;
		}
		case RELOADING:
		{
			//printf("RELOADING\n");
			m_Lock->Set(true);
			m_Fire->Set(false);
			double error = m_Pot->GetAverageVoltage() - m_Setpoint;
			double change = error - (m_PreviousVoltage - m_Setpoint);
			const double PID_P = CONSTANT("PID_P");
			const double PID_D = CONSTANT("PID_D");
			double output = PID_P*error + PID_D*change;
			if(fabs(error) < CONSTANT("LockBand"))
			{
				printf("Locking\n");
				m_LastReloadStopTime = Timer::GetFPGATimestamp();
				m_State = LOCKING;
				break;
			}
			if(output < 0)
			{
				output = CowLib::LimitMix(output, 0.225); 
			}
			static unsigned int printCount = 0;
			if(printCount % 10 == 0)
			{
				//printf("Winch: %f; Setpoint: %f; Output: %f\n", m_Pot->GetAverageVoltage(), m_Setpoint, output);
				printCount = 0;
			}
			printCount++;
			SetMotors(output);
			break;
		}
		case LOCKING:
		{
			//printf("LOCKING\n");
			m_Fire->Set(false);
			double error = m_Pot->GetAverageVoltage() - m_Setpoint;
			double change = error - (m_PreviousVoltage - m_Setpoint);
			const double PID_P = CONSTANT("PID_P");
			const double PID_D = CONSTANT("PID_D");
			double output = PID_P*error + PID_D*change;
			if(TimeSinceLastReloadStop() > 0.125)
			{
				//printf("Locking\n");
				m_Lock->Set(false);
			}
			else
			{
				m_Lock->Set(true);
			}
			if(TimeSinceLastReloadStop() > 0.8)
			{
				printf("Ending PID\n");
				SetMotors(0);
				m_State = READY_TO_FIRE;
				break;
			}
			if(output < 0)
			{
				output = CowLib::LimitMix(output, 0.225); 
			}
			SetMotors(output);			
			break;
		}
		case UNLOCKING:
		{
			//printf("UNLOCKING\n");
			m_Fire->Set(false);
			m_Lock->Set(true);
			SetMotors(0);
			//printf("Starting unlock\n");
			if(fabs(m_Pot->GetAverageVoltage() - m_LastUnlockVoltage) > 0.005)
			{
				printf("Unlock complete\n");
				m_State = m_NextState;
			}
			break;
		}
		case HOLDING:
		{
			//printf("HOLDING\n");
			m_Lock->Set(true);
			m_Fire->Set(false);
			double error = m_Pot->GetAverageVoltage() - m_HoldVoltage;
			double change = error - (m_PreviousVoltage - m_HoldVoltage);
			const double PID_P = CONSTANT("PID_P");
			const double PID_D = CONSTANT("PID_D");
			double output = PID_P*error + PID_D*change;
			if(output < 0)
			{
				output = CowLib::LimitMix(output, 0.225); 
			}
			SetMotors(output);
			m_State = RELOADING;
			break;
		}
		case DESPRINGING:
		{
			//printf("DESPRINGING\n");
			m_Lock->Set(true);
			m_Fire->Set(false);
			SetMotors(0);
			break;
		}
	}
	m_PreviousVoltage = m_Pot->GetAverageVoltage();
}

void Winch::SetSetpoint(double setpoint)
{
	if(setpoint < CONSTANT("MaxSetpoint"))
	{
		return;
	}
	m_Setpoint = setpoint;

	if(m_State == READY_TO_FIRE)
	{
		m_LastUnlockVoltage = m_Pot->GetAverageVoltage();
		m_NextState = RELOADING;
		m_State = UNLOCKING;
	}
	if(m_State == PREFIRING)
	{
		m_State = RELOADING;
	}
	if(m_State == DESPRINGING)
	{
		m_State = RELOADING;
	}
}

double Winch::GetVoltage()
{
	return m_Pot->GetVoltage();
}

double Winch::GetAverageVoltage()
{
	return m_Pot->GetAverageVoltage();
}

double Winch::GetSetpoint()
{
	return m_Setpoint;
}

void Winch::Fire()
{
	m_Setpoint -= 0.01;
	m_LastUnlockVoltage = m_Pot->GetAverageVoltage();
	m_NextState = PREFIRING;
	m_State = UNLOCKING;
	m_LastFireTime = Timer::GetFPGATimestamp();
}

void Winch::SetState(e_WinchState state)
{
	m_State = state;
}

Winch::e_WinchState Winch::GetState()
{
	return m_State;
}

double Winch::TimeSinceLastFire()
{
	return (Timer::GetFPGATimestamp() - m_LastFireTime);
}

double Winch::TimeSinceLastFireStop()
{
	return (Timer::GetFPGATimestamp() - m_LastFireStopTime);
}

double Winch::TimeSinceLastReloadStop()
{
	return (Timer::GetFPGATimestamp() - m_LastReloadStopTime);
}


void Winch::HoldVoltage(double setpoint)
{
	if(setpoint < CONSTANT("MaxSetpoint"))
	{
		return;
	}
	
	m_HoldVoltage = setpoint;
	
	if(m_State == RELOADING)
	{
		m_State = HOLDING;
	}
	if(m_State == READY_TO_FIRE)
	{
		m_LastUnlockVoltage = m_Pot->GetAverageVoltage();
		m_NextState = HOLDING;
		m_State = UNLOCKING;
	}
}

void Winch::Despring()
{	
	if(m_State == RELOADING)
	{
		m_State = DESPRINGING;
	}
	if(m_State == READY_TO_FIRE)
	{
		m_LastUnlockVoltage = m_Pot->GetAverageVoltage();
		m_NextState = DESPRINGING;
		m_State = UNLOCKING;
	}
}

void Winch::SetMotors(double speed)
{
	m_MotorA->Set(speed);
	m_MotorB->Set(speed);
}
