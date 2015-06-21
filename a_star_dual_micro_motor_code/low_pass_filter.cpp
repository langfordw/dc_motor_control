/*
 *  low_pass_filter.cpp
 *  
 *
 *  Created by William Langford on 3/29/11.
 *  Copyright 2011 Tufts Robotics Club. All rights reserved.
 *
 */

// #include "WProgram.h"
#include "low_pass_filter.h"

LPFilter::LPFilter()
{	
}

void LPFilter::init(float fbreak, float y_init)
{
	time = 0;
	dt = 0;
	y = y_init;
	_w = 2*3.1415926*fbreak;
}

float LPFilter::step(float u)
{
	dt = millis()-time;
	time = millis();
	y = (_w*u*dt/1000+y)/(1+_w*dt/1000);
}
