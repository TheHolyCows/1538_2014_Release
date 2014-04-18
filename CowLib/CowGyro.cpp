#include "CowGyro.h"

namespace CowLib
{
	CowGyro::CowGyro(uint32_t channel)
		:
		m_RingBuffer(rngCreate(sizeof(st_Accumulation) * COWGYRO_RING_SIZE)),
		m_Analog(new AnalogChannel(channel)),
		m_VoltsPerDegreePerSecond(Gyro::kDefaultVoltsPerDegreePerSecond),
		m_Center(0),
		m_Offset(0),
		m_LastSaveTime(0),
		m_RecalQueued(true)
	{
		if(m_RingBuffer == NULL)
		{
			wpi_setWPIErrorWithContext(NoAvailableResources,
					"could not allocate ring buffer");
			return;
		}
				
		if(!m_Analog->IsAccumulatorChannel())
		{
			wpi_setWPIErrorWithContext(ParameterOutOfRange,
					"moduleNumber and/or channel (must be accumulator channel)");
			rngDelete(m_RingBuffer);
			delete m_Analog;
			m_Analog = NULL;
			return;
		}
		
		// This is just a copy of part of WPI's InitGyro routine
		m_Analog->SetAverageBits(Gyro::kAverageBits);
		m_Analog->SetOversampleBits(Gyro::kOversampleBits);
		double sampleRate = Gyro::kSamplesPerSecond * (1 << (Gyro::kAverageBits + Gyro::kOversampleBits));
		m_Analog->GetModule()->SetSampleRate(sampleRate);
		Wait(1.0); // Not sure why we do this, but WPILib does it
		
		m_Analog->InitAccumulator();
		nUsageReporting::report(nUsageReporting::kResourceType_Gyro, m_Analog->GetChannel(), m_Analog->GetModuleNumber() - 1);
	}
	
	CowGyro::~CowGyro()
	{
		delete m_Analog;
		rngDelete(m_RingBuffer);
	}
	
	double CowGyro::TimeSinceLastSave()
	{
		return Timer::GetFPGATimestamp() - m_LastSaveTime;
	}
	
	double CowGyro::GetAngle()
	{
		// Dumb way to return obviously bad value until first calibration is done
		if(m_Center == 0 && m_Offset == 0)
		{
			return -999;
		}
		
		INT64 rawValue;
		uint32_t count;
		m_Analog->GetAccumulatorOutput(&rawValue, &count);

		INT64 value = rawValue - (INT64)((float)count * m_Offset);
		
		return value * 1e-9 * (double)m_Analog->GetLSBWeight() * (double)(1 << m_Analog->GetAverageBits()) /
			(m_Analog->GetModule()->GetSampleRate() * m_VoltsPerDegreePerSecond);
	}
	
	void CowGyro::Reset()
	{
		m_Analog->ResetAccumulator();
	}
	
	void CowGyro::HandleCalibration()
	{
		if(TimeSinceLastSave() > COWGYRO_ACCUMULATION_PERIOD)
		{
			st_Accumulation acc;
			m_Analog->GetAccumulatorOutput(&acc.value, &acc.count);
			
			if(m_RecalQueued && rngNBytes(m_RingBuffer) == sizeof(st_Accumulation)*(COWGYRO_RING_SIZE - 1))
			{
				rngBufPut(m_RingBuffer, (char*)&acc, sizeof(st_Accumulation));
				FinalizeCalibration();
			}
			else
			{
				rngBufPut(m_RingBuffer, (char*)&acc, sizeof(st_Accumulation));
			}
			
			m_Analog->ResetAccumulator();
			
			m_LastSaveTime = Timer::GetFPGATimestamp();
		}
	}
	
	void CowGyro::FinalizeCalibration()
	{
		if((unsigned int)rngNBytes(m_RingBuffer) < sizeof(st_Accumulation) * COWGYRO_RING_SIZE)
		{
			printf("Not enough data to recalibrate with, recal queued\n");
			m_RecalQueued = true;
			return;
		}
		m_RecalQueued = false;
		
		INT64 totalValue = 0;
		uint32_t totalCount = 0;
		
		st_Accumulation acc[COWGYRO_RING_SIZE] = {0};
		rngBufGet(m_RingBuffer, (char*)&acc, sizeof(st_Accumulation) * COWGYRO_RING_SIZE);
		for(int i = 0; i < COWGYRO_RING_SIZE; i++)
		{
			totalValue += acc[i].value;
			totalCount += acc[i].count;
		}
		
		double average = ((double)totalValue / (double)totalCount) + (double)m_Center - m_Offset;
		
		m_Center = (INT64)(average + .5);
		
		m_Offset = average - m_Center;
		
		printf("Done with CowGyro calibration!\nValue:\t%lld\tCount:%d\nCenter:\t%lld\tOffset:\t%f\n", totalValue, totalCount, m_Center, m_Offset);
		
		m_Analog->SetAccumulatorCenter(m_Center);
		m_Analog->SetAccumulatorDeadband(0);
		m_Analog->ResetAccumulator();
	}
	
	void CowGyro::SetSensitivity(double voltsPerDegreePersecond)
	{
		m_VoltsPerDegreePerSecond = voltsPerDegreePersecond;
	}
}
