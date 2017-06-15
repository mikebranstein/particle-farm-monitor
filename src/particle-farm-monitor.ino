/*
 * Project particle-farm-monitor
 * Description:
 * Author:
 * Date:
 */

int led = D7;

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.

  pinMode(led, OUTPUT);

  Particle.publish("w", "{\"test\":\"testValue\"}", PRIVATE);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  digitalWrite(led, HIGH);

  delay(500);

  digitalWrite(led, LOW);

  delay(500);

}