#include "FlowPID.h"
#include <QueueList.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(7,8,9,10,11,12);
double flowOut = 0;
const byte flowSensor1Pin = 3;
const byte flowSensor2Pin = 4;
volatile bool updateFlag = false;
//QueueList<unsigned long> *flowQ1;
//QueueList<unsigned long> *flowQ2;
int pulsecnt = 0;
FlowPID *flow1;
double accumWater = 0;
byte LEDstat = HIGH;
long lastT = 0, lastdT = 0;
double inteErr = 0.0;
double kdimpact = 0.0, kpimpact = 0.0, defimpact = 0.0;
const int timeout = 180;
const double defRate = 0.04, kp = 0.2, kd = 0.25, ki = 0;
void setup() {
  //debugging info
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);//LED to test whether program stuck
  lcd.begin(16, 2);
  lcd.print(0);
  lcd.setCursor(8,0);
  lcd.print("mL");
  lcd.setCursor(0,0);
  accumWater = 0;
  lastT = 0;
  lastdT = 0;
  kdimpact = 0.0;
  kpimpact = 0.0;
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
    noInterrupts();
    lcd.setCursor(0,0);
    lcd.print(int(accumWater));
    interrupts();
    digitalWrite(LED_BUILTIN, LEDstat);
    LEDstat = !LEDstat;
    lastsec = time/timesplit;

    Serial.print(time);
    Serial.print(" ");
    Serial.print(accumWater);
    Serial.print(" ml ");
    Serial.print(pulsecnt);
    Serial.print("pulses, kpimpact=");
    Serial.print(kpimpact);
    Serial.print(" kdimpact=");
    Serial.print(kdimpact);
    Serial.print(" defimpact=");
    Serial.println(defimpact);
  }

}

void flow1ISR(){
  unsigned long currT = millis();
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
//  noInterrupts();
//  flowQ1->push(millis());
//  interrupts();
}

