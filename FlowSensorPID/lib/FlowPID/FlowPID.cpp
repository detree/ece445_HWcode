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
#define COMPILE_XCODE 1
#ifdef COMPILE_XCODE
    #define NULL nullptr
    void noInterrupts(){;}
    void interrupts(){;}
#endif

/*Constructor (...)*********************************************************
 *    The parameters specified here are those for for which we can't set up
 *    reliable defaults, so we need to have the user set them.
 ***************************************************************************/
FlowPID::FlowPID(double* Output, double timeoutin,
         double Kp, double Ki, double Kd, double* defRatein,
                 QueueList<unsigned long>* FlowRecordin)
{
    
    myOutput = Output;
    timeout = timeoutin;
    FlowRecord = FlowRecordin;
    if(defRatein != NULL)
        defRate = *defRatein;
    else
        defRate = 1.0/3.0;
    
    lastT = 0;
    lastdT = 0;
    inteErr = 0.0;
    FlowPID::SetOutputLimits(0, 255);	//default output limit corresponds to the arduino pwm limits
    
    FlowPID::SetTunings(Kp, Ki, Kd);
}



double FlowPID::Compute(){
    double ret = 0;
    unsigned long currT = 0;
    while(1){
        noInterrupts();
        if(!FlowRecord->isEmpty()){
            currT = FlowRecord->pop();
        }
        interrupts();
        unsigned long dT = currT - lastT;
        ret = ret + kp*dT*defRate + kd*double(dT-lastdT)*defRate;
        lastT = currT;
        lastdT = dT;
    }
    if(ret < outMin)
        ret = outMin;
    else if(ret > outMax)
        ret = outMax;
    return ret;
}

void FlowPID::ResetVal(){
    lastdT = 0;
    lastT = 0;
    inteErr = 0;
}

void FlowPID::SetOutputLimits(double floor, double ceil){
    outMin = floor;
    outMax = ceil;
}

void FlowPID::SetTunings(double Kp, double Ki, double Kd){
    kp = Kp;
    kd = Kd;
    ki = Ki;
}

void FlowPID::SetTimeout(unsigned long in){
    timeout = in;
}

double FlowPID::GetKp(){
    return kp;
}
double FlowPID::GetKi(){
    return ki;
}
double FlowPID::GetKd(){
    return kd;
}


















