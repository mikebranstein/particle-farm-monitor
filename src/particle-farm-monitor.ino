#include "weather-service.h"
#define MINUTES_TO_SLEEP (15*60)

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

  System.sleep(weatherService.getRainGaugeSignalPin(), FALLING, MINUTES_TO_SLEEP);
}