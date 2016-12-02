#include "FlowPID.h"
#include <QueueList.h>
#include <LiquidCrystal.h>
//=============hardware setup================
LiquidCrystal* lcd;
const byte flowSensor1Pin = 2;
const byte flowSensor2Pin = 3;
const byte buttonPin = 13;
const byte FSR1pin = A1;
const byte FSR2pin = A0; ///好的

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

//============weight calculate and its button==============
double weight = 0;
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button
bool weightResendNeed = false;
bool weightCalcNeed = false;

//===========BT and to Android communication===========
volatile bool updateFlag = false;
const char SentSep = ',';
const char PartSep = ':';
const char updateW = 'W';
const char updateV = 'V';
const char recalcW = 'R';
const char userInputW = 'E';
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete


void setup() {
  //========hardware init===============
  Serial.begin(9600);
  lcd = new LiquidCrystal(7,8,9,10,11,12);
  pinMode(5, OUTPUT);  //for LCD digit display power
  digitalWrite(5, HIGH);
  pinMode(buttonPin, INPUT);  //weight calc button
  //==========the initial words on LED=========
  lcd->begin(16, 2);
  lcd->print(0);
  lcd->setCursor(6,0);
  lcd->print("mL");
  lcd->setCursor(6,1);
  lcd->print("mL left");
  //==========flow sensor variables==========
  accumWater = 0;
  backWater = 0;
  lastT = 0;
  lastdT = 0;
  //==========FSR variables==========
  weight = 0;
  buttonState = 0;  
  lastButtonState = 0;
  weightResendNeed = false;
  weightCalcNeed = false;
  //========debugging info=========
  pinMode(LEDpin, OUTPUT);//LED to test whether program stuck
  kdimpact = 0.0;
  kpimpact = 0.0;
//  pinMode(flowSensor1Pin, INPUT_PULLUP); ???????????can be commented?
  //==============interrupt info==================
  attachInterrupt(digitalPinToInterrupt(flowSensor1Pin), flow1ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(flowSensor2Pin), flow2ISR, CHANGE);
}

//============variables used in the loop()===============
unsigned long lastsec = 0;
unsigned long currtime;
const long timesplit = 1000;
int FSR1val=0, FSR2val=0;

void loop() {
  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState && buttonState==LOW) weightCalcNeed = true;
  lastButtonState = buttonState;
  if(weightCalcNeed){
    FSR1val = analogRead(FSR1pin);
    FSR2val = 0;
    for(int i=0;i<10;i++)
      FSR2val += analogRead(FSR2pin);
    FSR2val/=10;
    //Serial.println(FSR2val);
    weight = 6.5*double(FSR2val-337)+88.24;
    accumWater = 0;
    backWater = 0;
    weightResendNeed = true;
    weightCalcNeed = false;
  }
  //Serial.println(analogRead(FSR2pin));
  currtime = millis();
  if(currtime / timesplit > lastsec){
    noInterrupts();
    lcd->setCursor(0,0);
    lcd->print("      ");
    lcd->setCursor(0,0);
    lcd->print(int(accumWater-backWater));
    lcd->setCursor(0,1);
    lcd->print("      ");
    lcd->setCursor(0,1);
    lcd->print(int(weight-accumWater+backWater));
    interrupts();
    digitalWrite(LEDpin, LEDstat);
    LEDstat = !LEDstat;
    lastsec = currtime/timesplit;
    //=======BT to Android communication=========
    String msg = "";
    if(weightResendNeed){
      msg = msg+updateW+PartSep+String((int)weight)+SentSep;
      weightResendNeed = false;
    }
    msg = msg+String(updateV)+PartSep+String(int(accumWater-backWater))+SentSep;
    Serial.println(msg);
    
//    USBdebug(currtime);
  }

  //==============read through the BT input messages=============
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n' || inChar==',') {
      stringComplete = true;
    }
  }
  if (stringComplete) {
//    Serial.println(inputString[0]);
    if(inputString[0]==recalcW){
      weightCalcNeed = true;
    }else if(inputString[0]==userInputW){
//      Serial.println("in here");
      weight=0; int i=2;
      while(inputString[i]!=SentSep){
        weight*=10;weight+=inputString[i]-'0';
        i++;
      }
      weightResendNeed = true;
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

void USBdebug(int currtime){
  Serial.print(currtime);
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
  unsigned long currT = millis();
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

