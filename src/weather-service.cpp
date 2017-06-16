#include "weather-service.h"

#include "lib/SparkWeatherShield/SparkFun_Photon_Weather_Shield_Library.h" // Include the SparkFun MPL3115A2 library
#include "lib/SparkJson/SparkJson.h"

// ctor()
WeatherService::WeatherService(){}

void WeatherService::init(bool debugMode) {
    _debugMode = debugMode;

    if (debugMode) {
        Serial.begin(9600);   // open serial over USB at 9600 baud

        // Make sure your Serial Terminal app is closed before powering your device
        // Now open your Serial Terminal, and hit any key to continue!
        Serial.println("Press any key to begin");

        //This line pauses the Serial port until a key is pressed
        while(!Serial.available()) Spark.process();
    }

    //Initialize the I2C sensors and ping them
    _sensor.begin();

    /*You can only receive acurate barrometric readings or acurate altitiude
    readings at a given time, not both at the same time. The following two lines
    tell the sensor what mode to use. You could easily write a function that
    takes a reading in one made and then switches to the other mode to grab that
    reading, resulting in data that contains both acurate altitude and barrometric
    readings. For this example, we will only be using the barometer mode. Be sure
    to only uncomment one line at a time. */
    _sensor.setModeBarometer();//Set to Barometer Mode
    //sensor.setModeAltimeter();//Set to altimeter Mode

    //These are additional MPL3115A2 functions the MUST be called for the sensor to work.
    _sensor.setOversampleRate(7); // Set Oversample rate
    //Call with a rate from 0 to 7. See page 33 for table of ratios.
    //Sets the over sample rate. Datasheet calls for 128 but you can set it
    //from 1 to 128 samples. The higher the oversample rate the greater
    //the time between data samples.

    _sensor.enableEventFlags(); //Necessary register calls to enble temp, baro and alt
}

char* WeatherService::getWeatherData() {

    // Measure Relative Humidity from the HTU21D or Si7021
    float humidity = _sensor.getRH();

    // Measure Temperature from the HTU21D or Si7021
    float tempf = _sensor.getTempF();

    //Measure the Barometer temperature in F from the MPL3115A2
    // float baroTemp = _sensor.readBaroTempF();

    //Measure Pressure from the MPL3115A2 in Pascals
    // 1 Pa = 0.000295299830714 inHg (inches of Mercury)
    float pascals = _sensor.readPressure();
    float mmHg = pascals * 0.000295299830714;

    //If in altitude mode, you can get a reading in feet with this line:
    //altf = _sensor.readAltitudeFt();

    // get fuel guage and bundle with temperature data
    FuelGauge fuel;

    StaticJsonBuffer<200> jsonBuffer;

    JsonObject& root = jsonBuffer.createObject();
    root["h"] = humidity;
    root["t"] = tempf;
    root["p"] = mmHg;
    root["v"] = fuel.getVCell(); // voltage
    root["c"] = fuel.getSoC(); // state of charge in %

    static char buffer[100];
    root.printTo(buffer, sizeof(buffer));
    return buffer;
}
