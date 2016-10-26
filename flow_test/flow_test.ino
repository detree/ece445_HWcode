#include "FlowPID.h"
#include <QueueList.h>
double flowOut = 0;
const byte flowSensor1Pin = 2;
const byte flowSensor2Pin = 3;
QueueList<unsigned long> *flowQ1;
QueueList<unsigned long> *flowQ2;
FlowPID *flow1;
int accumWater = 0;
void setup() {
  Serial.begin(9600);
  flowQ1 = new QueueList<unsigned long>();
  flow1 = new FlowPID(&flowOut, 100,
                      0.01, 0.02, 0, NULL,
                      flowQ1);
  pinMode(flowSensor1Pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(flowSensor1Pin), flow1ISR, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(flowSensor2Pin), flow2ISR, CHANGE);
}

// the loop function runs over and over again forever
unsigned long lastsec = 0;
unsigned long time;

void loop() {
  accumWater += flow1->Compute();
  time = millis();
//  Serial.println(time);
  if(time / 1000 > lastsec){
    lastsec = time/1000;
    Serial.print(time);
    Serial.print(" ");
    Serial.print(accumWater);
    Serial.println(" ml");
  }

}

void flow1ISR(){
//  noInterrupts();
  flowQ1->push(millis());
//  interrupts();
}

void flow2ISR(){
//  noInterrupts();
//  flowQ1->push(millis());
//  interrupts();
}

