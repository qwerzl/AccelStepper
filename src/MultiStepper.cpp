// MultiStepper.cpp
//
// Copyright (C) 2015 Mike McCauley
// $Id: MultiStepper.cpp,v 1.3 2020/04/20 00:15:03 mikem Exp mikem $

#include "MultiStepper.h"
#include "AccelStepper.h"

MultiStepper::MultiStepper()
    : _num_steppers(0)
{
}

boolean MultiStepper::addStepper(AccelStepper& stepper)
{
    if (_num_steppers >= MULTISTEPPER_MAX_STEPPERS)
	return false; // No room for more
    _steppers[_num_steppers++] = &stepper;
    return true;
}

void MultiStepper::moveTo(long absolute[])
{
    uint8_t i;
	// Now work out a new max speed for each stepper so they will all 
	// arrived at the same time of longestTime
	for (i = 0; i < _num_steppers; i++)
	{
	    _steppers[i]->moveTo(absolute[i]); // New target position (resets speed)
        if (_steppers[i]->currentPosition() < absolute[i]) _steppers[i]->setSpeed(_steppers[i]->maxSpeed());
        else _steppers[i]->setSpeed(-_steppers[i]->maxSpeed());
	}
}

// Returns true if any motor is still running to the target position.
boolean MultiStepper::run()
{
    uint8_t i;
    boolean ret = false;
    for (i = 0; i < _num_steppers; i++)
    {
	if ( _steppers[i]->distanceToGo() != 0)
	{
	    _steppers[i]->runSpeed();
	    ret = true;
	}
	// Caution: it has een reported that if any motor is used with acceleration outside of
	// MultiStepper, this code is necessary, you get 
	// strange results where it moves in the wrong direction for a while then 
	// slams back the correct way.
#if 0
	else
	{
	    // Need to call this to clear _stepInterval, _speed and _n 
	    otherwise future calls will fail.
		_steppers[i]->setCurrentPosition(_steppers[i]->currentPosition());
	}
#endif
	
    }
    return ret;
}

// Blocks until all steppers reach their target position and are stopped
void    MultiStepper::runSpeedToPosition()
{ 
    while (run())
	;
}

