#include "weather-service.h"

WeatherService weatherService;

// setup() runs once, when the device is first turned on.
void setup() {

  // initialize the weather service, true = debug, false = production
  weatherService.init(false);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  char* json = weatherService.getWeatherData();
  Particle.publish("w", json, PRIVATE);

  int minutesToDeepSleep = 5;
  System.sleep(SLEEP_MODE_DEEP, minutesToDeepSleep * 60);
}