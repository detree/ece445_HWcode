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


#include "FlowPID.h"

/*Constructor (...)*********************************************************
 *    The parameters specified here are those for for which we can't set up
 *    reliable defaults, so we need to have the user set them.
 ***************************************************************************/
FlowPID::FlowPID(double* Input, double* Output, double* Setpoint,
         double Kp, double Ki, double Kd, QueueList<unsigned long>* FlowRecordin)
{
    
    myOutput = Output;
    myInput = Input;
    mySetpoint = Setpoint;
    FlowRecord = FlowRecordin;
    
    FlowPID::SetOutputLimits(0, 255);	//default output limit corresponds to the arduino pwm limits
    
    FlowPID::SetTunings(Kp, Ki, Kd);
}

double FlowPID::Compute(){
    double ret = 0;
    if(ret < outMin)
        ret = outMin;
    else if(ret > outMax)
        ret = outMax;
    return ret;
}

void FlowPID::SetOutputLimits(double floor, double ceil){
    outMin = floor;
    outMax = ceil;
}
