//==================================================
// Copyright (C) 2014 Team 1538 / The Holy Cows
//==================================================

#ifndef __COW_GYRO_H__
#define __COW_GYRO_H__

#include <vxWorks.h>
#include <rngLib.h>
#include <WPIErrors.h>
#include <NetworkCommunication/UsageReporting.h>
#include <AnalogModule.h>
#include <AnalogChannel.h>
#include <Gyro.h>
#include <Timer.h>
#include "../Declarations.h"

namespace CowLib
{
	class CowGyro : public SensorBase
	{
	private:
		typedef struct
		{
			INT64 value;
			uint32_t count;
		} st_Accumulation;
		
		RING_ID m_RingBuffer;
		AnalogChannel* m_Analog;
		double m_VoltsPerDegreePerSecond;
		INT64 m_Center;
		double m_Offset;
		double m_LastSaveTime;
		bool m_RecalQueued;
		double TimeSinceLastSave();
		CowGyro();
	public:
		CowGyro(uint32_t channel);
		~CowGyro();
		void SetSensitivity(double voltsPerDegreePerSecond);
		double GetAngle();
		void Reset();
		void HandleCalibration();
		void FinalizeCalibration();
	};
}

#endif
