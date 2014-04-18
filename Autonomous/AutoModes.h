//==================================================
// Copyright (C) 2014 Team 1538 / The Holy Cows
//==================================================

#ifndef __AUTO_MODES_H__
#define __AUTO_MODES_H__

#include <map>
#include <deque>
#include "../Controllers/AutoModeController.h"

class AutoModes
{
private:
	static AutoModes* m_SingletonInstance;
	std::map<char*, std::deque<RobotCommand> > m_Modes;
	std::map<char*, std::deque<RobotCommand> >::iterator m_Iterator;
	
	AutoModes();
public:
	static AutoModes* GetInstance();
	std::deque<RobotCommand> GetCommandList();
	char* GetName();
	void NextMode();
};

#endif
