const byte ledPin = 13;
const byte interruptPin = 2;
volatile byte state = LOW;
int intcnt = 0;
void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);
}
long lastsec = 0;
long time = 0;
void loop() {
  
  time = millis();
  if(time/1000 > lastsec){
    lastsec = time/1000;
    Serial.print(time);
    Serial.print(" ");
    Serial.print(intcnt);
    Serial.println(" pulses");
    intcnt = 0;
  }
}

void blink() {
  intcnt++;
}
//const byte ledPin = 13;
//const byte interruptPin = 2;
//volatile byte state = LOW;
//
//void setup() {
//  pinMode(ledPin, OUTPUT);
//  pinMode(interruptPin, INPUT_PULLUP);
//  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);
//}
//
//void loop() {
//  digitalWrite(ledPin, state);
//}
//
//void blink() {
//  state = !state;
//}
