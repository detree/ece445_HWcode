//
//  FlowPID.h
//  FlowPID
//
//  Created by Shuchen on 10/18/16.
//  Copyright © 2016 Shuchen. All rights reserved.
//
/**********************************************************************************************
 * Flow Sensor PID Library - Version 0.1.0
 * changed from Arduino PID Library - Version 1.1.1 by Brett Beauregard
 *
 * This Library is licensed under a GPLv3 License
 **********************************************************************************************/

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <FlowPID.h>

/*Constructor (...)*********************************************************
 *    The parameters specified here are those for for which we can't set up
 *    reliable defaults, so we need to have the user set them.
 ***************************************************************************/
PID::PID(double* Input, double* Output, double* Setpoint,
         double Kp, double Ki, double Kd, int ControllerDirection)
{
    
    myOutput = Output;
    myInput = Input;
    mySetpoint = Setpoint;
    inAuto = false;
    
    PID::SetOutputLimits(0, 255);				//default output limit corresponds to
    //the arduino pwm limits
    
    SampleTime = 100;							//default Controller Sample Time is 0.1 seconds
    
    PID::SetControllerDirection(ControllerDirection);
    PID::SetTunings(Kp, Ki, Kd);
    
    lastTime = millis()-SampleTime;
}