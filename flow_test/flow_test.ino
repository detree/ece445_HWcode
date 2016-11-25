#include "FlowPID.h"
#include <QueueList.h>
#include <LiquidCrystal.h>
//=============hardware setup================
LiquidCrystal* lcd;
const byte flowSensor1Pin = 2;
const byte flowSensor2Pin = 3;
volatile bool updateFlag = false;

//===========debugging variables==============
int pulsecnt = 0, pulsecnt2 = 0;
byte LEDstat = HIGH;
int LEDpin = 6;
double kdimpact = 0.0, kpimpact = 0.0, defimpact = 0.0;
double kdimpact2 = 0.0, kpimpact2 = 0.0, defimpact2 = 0.0;

//==========onward direction flow sensor==========
double accumWater = 0;
long lastT = 0, lastdT = 0;
const int timeout = 180;
const double defRate = 0.04, kp = 0.2, kd = 0.25, ki = 0;

//==========backward direction flow sensor==========
double backWater = 0;
long lastT2 = 0, lastdT2 = 0;
const int timeout2 = 60;
const double defRate2 = 0.15, kp2 = 0.003, kd2 = 0.0, ki2 = 0;

void setup() {
  //debugging info
  Serial.begin(9600);
  lcd = new LiquidCrystal(7,8,9,10,11,12);
  pinMode(LEDpin, OUTPUT);//LED to test whether program stuck
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  lcd->begin(16, 2);
  lcd->print(0);
  lcd->setCursor(8,0);
  lcd->print("mL");
  accumWater = 0;
  backWater = 0;
  lastT = 0;
  lastdT = 0;
  kdimpact = 0.0;
  kpimpact = 0.0;
  pinMode(flowSensor1Pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(flowSensor1Pin), flow1ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(flowSensor2Pin), flow2ISR, CHANGE);
}

// the loop function runs over and over again forever
unsigned long lastsec = 0;
unsigned long time;
const long timesplit = 1000;

void loop() {
  //accumWater += flow1->Compute();
  time = millis();
//  Serial.println(time);
  if(time / timesplit > lastsec){
    noInterrupts();
    lcd->setCursor(0,0);
    lcd->print("        ");
    lcd->setCursor(0,0);
    lcd->print(int(accumWater-backWater));
    interrupts();
    digitalWrite(LEDpin, LEDstat);
    LEDstat = !LEDstat;
    lastsec = time/timesplit;

    Serial.print(time);
    Serial.print(" ");
    Serial.print(accumWater-backWater);
    Serial.print(" ml ");
    Serial.print(pulsecnt2);
    Serial.print("pulses, kpimpact=");
    Serial.print(kpimpact2);
    Serial.print(" kdimpact=");
    Serial.print(kdimpact2);
    Serial.print(" defimpact=");
    Serial.println(defimpact2);
  }

}

void flow1ISR(){
  unsigned long currT = millis();
//  Serial.println("in flow1");
  if(currT-lastT>timeout){
    //Serial.println("timeout");
    lastT = millis();
    lastdT = 0;
    return;
  }
  pulsecnt++;
  long dT = currT - lastT;
  //Serial.println(dT);
  accumWater = accumWater + defRate; 
  if(dT>=120){
    accumWater+=( 0.4+kp*tan( (double(dT+7.0-120.0)/65) ) );
    defimpact+=( 0.4+kp*tan( (double(dT+7.0-120.0)/65) ) );
  }
  else if(dT>=60){
    accumWater+=( 0.2+kp*tan( (double(dT+7.0-60.0)/65) ) );
    defimpact+=( 0.2+kp*tan( (double(dT+7.0-60.0)/65) ) );
  }
  else{
    accumWater += ( kp*tan( (double(dT+7.0)/65) ) );
    kpimpact += (kp*tan( (double(dT+7.0)/65) ) );
  }
  if(dT-lastdT!=0){
    accumWater += (kd/double(dT-lastdT));
    kdimpact += (kd/double(dT-lastdT));
  }
  lastT = currT;
  lastdT = dT;
  updateFlag = true;
}

void flow2ISR(){
  unsigned long currT = millis();
  //Serial.println(currT);
  if(currT-lastT<130) return;
  if(currT-lastT2>timeout2){
    lastT2 = millis();
    lastdT2 = 0;
    return;
  }
  pulsecnt2++;
  long dT = currT - lastT2;
  backWater = backWater + defRate2;
  double change=0;
  change += (kp2*pow(dT, 0.6));
  backWater+=change;
  kpimpact2+=change;//debug
//  if(dT-lastdT!=0){
//    backWater += (kd/double(dT-lastdT));
//    kdimpact += (kd/double(dT-lastdT));//debug
//  }
  lastT2 = currT;
  lastdT2 = dT;
  updateFlag = true;
}

