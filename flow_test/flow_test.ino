#include "FlowPID.h"
#include <QueueList.h>
double flowOut = 0;
const byte flowSensor1Pin = 2;
const byte flowSensor2Pin = 3;
//QueueList<unsigned long> *flowQ1;
//QueueList<unsigned long> *flowQ2;
int pulsecnt = 0;
FlowPID *flow1;
double accumWater = 0;
byte LEDstat = HIGH;
long lastT = 0;
long lastdT = 0;
double inteErr = 0.0;
const int timeout = 100;
const double defRate = 3.0/20.0, kp = -0.4, kd = 0.007, ki = 0;
void setup() {
  //debugging info
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);//LED to test whether program stuck

  //Flow sensor related data structures
//  flowQ1 = new QueueList<unsigned long>();
//  flow1 = new FlowPID(&flowOut, 80,
//                      -0.001, 0.007, 0, NULL,
//                      flowQ1);
  accumWater = 0;
  lastT = 0;
  lastdT = 0;
  pinMode(flowSensor1Pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(flowSensor1Pin), flow1ISR, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(flowSensor2Pin), flow2ISR, CHANGE);
}

// the loop function runs over and over again forever
unsigned long lastsec = 0;
unsigned long time;
const long timesplit = 500;

void loop() {
  //accumWater += flow1->Compute();
  time = millis();
//  Serial.println(time);
  if(time / timesplit > lastsec){
    digitalWrite(LED_BUILTIN, LEDstat);
    LEDstat = !LEDstat;
    lastsec = time/timesplit;
    Serial.print(time);
    Serial.print(" ");
    Serial.print(accumWater);
    Serial.print(" ml ");
    Serial.print(pulsecnt);
    Serial.println("pulses,");
  }

}

void flow1ISR(){
//  noInterrupts();
//flowQ1->push(millis());
  unsigned long currT = millis();
  if(currT-lastT>timeout){
    Serial.println("timeout");
    lastT = millis();
    lastdT = 0;
  }
  pulsecnt++;
  long dT = currT - lastT;
  //Serial.println(dT);
  accumWater = accumWater + defRate + kd*double(dT-lastdT);
  if(dT>0)
    accumWater += (kp/double(dT));
  lastT = currT;
  lastdT = dT;
}

void flow2ISR(){
//  noInterrupts();
//  flowQ1->push(millis());
//  interrupts();
}

