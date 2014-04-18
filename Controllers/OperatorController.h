//==================================================
// Copyright (C) 2014 Team 1538 / The Holy Cows
//==================================================

#ifndef __OPERATOR_CONTROLLER_H__
#define __OPERATOR_CONTROLLER_H__

#include <math.h>
#include <WPILib.h>
#include "../CowRobot.h"
#include "../CowControlBoard.h"
#include "../CowLib/CowLib.h"
#include "../Declarations.h"
#include "../CowConstants.h"

class OperatorController : public GenericController
{	
private:
	OperatorController();
	CowControlBoard* m_CB;
	bool m_PreviousFire;
	bool m_PreviousFullBack;
	bool m_PreviousHalfBack;
public:
	OperatorController(CowControlBoard* controlboard);
	void handle(CowRobot* bot);
};

#endif
