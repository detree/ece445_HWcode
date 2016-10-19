//
//  FlowPID.h
//  FlowPID
//
//  Created by Shuchen on 10/18/16.
//  Copyright © 2016 Shuchen. All rights reserved.
//

#ifndef FlowPID_h
#define FlowPID_h
#define LIBRARY_VERSION	1.1.1

#include <QueueList.h>

class FlowPID
{
    
    
public:
    
    //commonly used functions **************************************************************************
    FlowPID(double*, double*, double*,        // * constructor.  links the PID to the Input, Output, and
        double, double, double, QueueList<unsigned long>*);     //   Setpoint.  Initial tuning parameters are also set here
    
    double Compute();                       // * performs the PID calculation.  it should be
    //   called every time loop() cycles. ON/OFF and
    //   calculation frequency can be set using SetMode
    //   SetSampleTime respectively
    
    void SetOutputLimits(double, double); //clamps the output to a specific range. 0-255 by default, but
    //it's likely the user will want to change this depending on
    //the application
    
    
    
    //available but not commonly used functions ********************************************************
    void SetTunings(double, double,       // * While most users will set the tunings once in the
                    double);         	  //   constructor, this function gives the user the option
    //   of changing tunings during runtime for Adaptive control
    void SetTimeout(int);              // * sets the timeout time, default is 2000ms
    
    
    
    //Display functions ****************************************************************
    double GetKp();						  // These functions query the pid for interal values.
    double GetKi();						  //  they were created mainly for the pid front-end,
    double GetKd();						  // where it's important to know what is actually
    int GetMode();						  //  inside the PID.
    int GetDirection();					  //
    
private:
    void Initialize();
    
    double kp;                  // * (P)roportional Tuning Parameter
    double ki;                  // * (I)ntegral Tuning Parameter
    double kd;                  // * (D)erivative Tuning Parameter
    
    double *myInput;              // * Pointers to the Input, Output, and Setpoint variables
    double *myOutput;             //   This creates a hard link between the variables and the
    double *mySetpoint;           //   PID, freeing the user from having to constantly tell us
    //   what these values are.  with pointers we'll just know.
    
    unsigned long lastTime;
    unsigned long timeout;
    double ITerm, lastInput;
    QueueList<unsigned long>* FlowRecord;
    
    double outMin, outMax;
};


#endif /* FlowPID_h */
