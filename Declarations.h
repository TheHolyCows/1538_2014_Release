//==================================================
// Copyright (C) 2014 Team 1538 / The Holy Cows
//==================================================

#ifndef __DECLARATIONS_H__
#define __DECLARATIONS_H__

#define ROBOT_HZ						400

#define COWCONSTANTS_DEFAULT_FILE		"constants.ini"

// Number of accumulation periods to be summed
#define COWGYRO_RING_SIZE				5
// Length of an accumulation period in seconds
#define COWGYRO_ACCUMULATION_PERIOD		1.0

#define GYRO_SENSITIVITY				0.007

#define EXHAUST_REAR					4
#define EXHAUST_FRONT					1
#define INTAKE_FRONT					3
#define INTAKE_REAR						2
#define FAR								5

// PWM ports
#define PWM_LEFT_DRIVE					1
#define PWM_FRONT_INTAKE_A				2
#define PWM_FRONT_INTAKE_B				3
#define PWM_REAR_INTAKE_A				4
#define PWM_REAR_INTAKE_B				5
#define PWM_WINCH_A						6
#define PWM_WINCH_B						7
#define PWM_RIGHT_DRIVE					8

// Solenoid outputs
#define SOLENOID_FRONT_INTAKE_A			6
#define SOLENOID_FRONT_INTAKE_B			3
#define SOLENOID_REAR_INTAKE_A			5
#define SOLENOID_REAR_INTAKE_B			4
#define SOLENOID_WINCH_LOCK				2
#define SOLENOID_WINCH_FIRE				1

// Analog inputs
#define ANALOG_WINCH_POT				1
#define ANALOG_GYRO						2
#define ANALOG_FRONT_IR					3
#define ANALOG_REAR_IR					4

// Digital outputs
#define RELAY_COMPRESSOR				1

// Digital inputs
#define DIGITAL_PRESSURE_SWITCH			1
#define DIGITAL_ENCODER_A				2
#define DIGITAL_ENCODER_B				3

#endif
