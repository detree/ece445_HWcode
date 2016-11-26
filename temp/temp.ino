void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}
boolean output = HIGH;
void loop() {
  long time = millis();
  Serial.println(time);
  delay(500);
}
