#include "CowBase.h"

CowBase::CowBase()
	:
	m_Bot(new CowRobot()),
	m_ControlBoard(new CowControlBoard()),
	m_OpController(new OperatorController(m_ControlBoard)),
	m_AutoController(new AutoModeController()),
	m_Constants(CowConstants::GetInstance()),
	m_WinchState(Winch::UNLOCKING)
{	
	SetPeriod(HZ(ROBOT_HZ));
	GetWatchdog().SetEnabled(false);
	printf("Done constructing CowBase!\n");
}

void CowBase::RobotInit()
{
}

void CowBase::DisabledInit()
{
	m_Constants->RestoreData();
}

void CowBase::AutonomousInit()
{
	m_Bot->GyroFinalizeCalibration();
	
	m_AutoController->SetCommandList(AutoModes::GetInstance()->GetCommandList());
	m_Bot->SetController(m_AutoController);
	m_Bot->Reset();
	
	m_Bot->SetWinchState(Winch::READY_TO_FIRE);
	//m_WinchState = Winch::READY_TO_FIRE;
}
void CowBase::TeleopInit()
{
	m_Bot->SetController(m_OpController);
	m_Bot->Reset();
	
	//m_Bot->SetWinchState(m_WinchState);
	//m_WinchState = Winch::READY_TO_FIRE;
}

void CowBase::DisabledContinuous()
{
	taskDelay(WAIT_FOREVER);
}

void CowBase::AutonomousContinuous()
{
	taskDelay(WAIT_FOREVER);
}

void CowBase::TeleopContinuous()
{
	taskDelay(WAIT_FOREVER);
}

void CowBase::DisabledPeriodic()
{
	m_Bot->GyroHandleCalibration();
	
	if(m_ControlBoard->GetAutoSelectButton())
	{
		if(m_ControlBoard->GetDriveButton(3))
		{
			m_Bot->Reset();
			m_Constants->RestoreData();
			AutoModes::GetInstance()->NextMode();
		}
		else
		{ 
			m_Bot->GyroFinalizeCalibration();		
		}
	}
	
	m_Bot->PrintToDS();
}
void CowBase::AutonomousPeriodic()
{
	m_Bot->PrintToDS();
	m_Bot->handle();
}
void CowBase::TeleopPeriodic()
{
	if(m_ControlBoard->GetAutoSelectButton())
	{
		m_Bot->Reset();
		m_Constants->RestoreData();
	}
	
	m_Bot->PrintToDS();
	m_Bot->handle();
}

START_ROBOT_CLASS(CowBase);
