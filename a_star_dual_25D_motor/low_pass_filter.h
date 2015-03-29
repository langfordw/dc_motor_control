/*
 *  low_pass_filter.h
 *  
 *
 *  Created by William Langford on 3/29/11.
 *  Copyright 2011 Tufts Robotics Club. All rights reserved.
 *
 */
#ifndef low_pass_filter_h
#define low_pass_filter_h

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include <avr/io.h>
 #include "WProgram.h"
#endif

class LPFilter
{
	public:
		LPFilter();
		void init(float fbreak, float y_init=0);
		float step(float u);
		float y;
		unsigned long dt;
		unsigned long time;
	private:
		float _w;
};

#endif
