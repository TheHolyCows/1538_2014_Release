#include "Intake.h"

Intake::Intake(int motorPortA, int motorPortB, int solenoidPortA, int solenoidPortB)
	:
	m_MotorA(new Talon(motorPortA)),
	m_MotorB(new Talon(motorPortB)),
	m_SolenoidA(new Solenoid(solenoidPortA)),
	m_SolenoidB(new Solenoid(solenoidPortB)),
	m_LastDropTime(0)
{
	m_SolenoidA->Set(false);
	m_SolenoidB->Set(false);
}

void Intake::SetSpeed(double speed)
{
	m_MotorA->Set(speed);
	m_MotorB->Set(speed);
}

void Intake::SetState(bool state)
{
	if(!m_SolenoidA->Get() && state)
	{
		// if we're up and we want to go down, reset the timer
		m_LastDropTime = Timer::GetFPGATimestamp();
	}
	m_SolenoidA->Set(state);
	m_SolenoidB->Set(state);
}

bool Intake::GetState()
{
	return m_SolenoidA->Get();
}

bool Intake::IsPhysicallyDropped()
{
	return m_SolenoidA->Get() && TimeSinceLastDrop() > CONSTANT("IntakeDropTime");
}

double Intake::TimeSinceLastDrop()
{
	return (Timer::GetFPGATimestamp() - m_LastDropTime);
}
