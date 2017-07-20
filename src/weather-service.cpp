#include "weather-service.h"

#include "lib/SparkWeatherShield/SparkFun_Photon_Weather_Shield_Library.h" // Include the SparkFun MPL3115A2 library
#include "lib/SparkJson/SparkJson.h"

#include "OneWire.h"

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

    // initialize soil moisture sensor
    //
    // Leaving the soil moisture sensor powered all the time leads to corrosion 
    // of the probes. To account for this, this port breaks out Digital Pin D5 
    // as the power pin for the sensor, allowing the Photon to power the sensor, 
    // take a reading, and then disable power, giving the probes a longer lifespan. 
    // The moisture level can be read on Analog Pin A1.
    //
    // D5 is the power pin
    // A1 is the signal pin
    _soilMoistureSignalPin = A1;
    _soilMoisturePowerPin = D5;

    // set the soil moisture range reading levels, this should be calibrated based 
    // on the type of soil. These values re converted to a percentage of 0-100%.
    // The range values also depend on the voltage applied by the photon
    _soilMoistureRangeLow = 0; // no water
    _soilMoistureRangeHigh = 3350; // pure water

    pinMode(_soilMoisturePowerPin, OUTPUT);
    digitalWrite(_soilMoisturePowerPin, LOW);
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
    root["st"] = getSoilTemp(); // soil temp in degF
    root["m"] = getSoilMoisture(); // soil moisture level
    root["v"] = fuel.getVCell(); // voltage
    root["c"] = fuel.getSoC(); // state of charge in %

    static char buffer[100];
    root.printTo(buffer, sizeof(buffer));
    return buffer;
}

float WeatherService::getSoilTemp() {
    OneWire ds = OneWire(D4);  // 1-wire signal on pin D4

    byte i;
    byte present = 0;
    byte type_s;
    byte data[12];
    byte addr[8];
    float celsius, fahrenheit;

    while (ds.search(addr)) {

        // first the returned address is printed
        serialPrint("ROM =");
        for( i = 0; i < 8; i++) {
            serialWrite(' ');
            serialPrint(addr[i], HEX);
        }

        // second the CRC is checked, on fail,
        // print error and just return to try again
        if (OneWire::crc8(addr, 7) != addr[7]) {
            serialPrintln("CRC is not valid!");
            return 0;
        }
        serialPrintln();

        // we have a good address at this point
        // what kind of chip do we have?
        // we will set a type_s value for known types or just return

        // the first ROM byte indicates which chip
        switch (addr[0]) {
            case 0x10:
            serialPrintln("  Chip = DS1820/DS18S20");
            type_s = 1;
            break;
            case 0x28:
            serialPrintln("  Chip = DS18B20");
            type_s = 0;
            break;
            case 0x22:
            serialPrintln("  Chip = DS1822");
            type_s = 0;
            break;
            case 0x26:
            serialPrintln("  Chip = DS2438");
            type_s = 2;
            break;
            default:
            serialPrintln("Unknown device type.");
            return 0;
        }

        // this device has temp so let's read it

        ds.reset();               // first clear the 1-wire bus
        ds.select(addr);          // now select the device we just found
        // ds.write(0x44, 1);     // tell it to start a conversion, with parasite power on at the end
        ds.write(0x44, 0);        // or start conversion in powered mode (bus finishes low)

        // just wait a second while the conversion takes place
        // different chips have different conversion times, check the specs, 1 sec is worse case + 250ms
        // you could also communicate with other devices if you like but you would need
        // to already know their address to select them.

        delay(1000);     // maybe 750ms is enough, maybe not, wait 1 sec for conversion
        
        // we might do a ds.depower() (parasite) here, but the reset will take care of it.

        // first make sure current values are in the scratch pad

        present = ds.reset();
        ds.select(addr);
        ds.write(0xB8,0);         // Recall Memory 0
        ds.write(0x00,0);         // Recall Memory 0

        // now read the scratch pad

        present = ds.reset();
        ds.select(addr);
        ds.write(0xBE,0);         // Read Scratchpad
        if (type_s == 2) {
            ds.write(0x00,0);       // The DS2438 needs a page# to read
        }

        // transfer and print the values

        serialPrint("  Data = ");
        serialPrint(present, HEX);
        serialPrint(" ");
        for ( i = 0; i < 9; i++) {           // we need 9 bytes
            data[i] = ds.read();
            serialPrint(data[i], HEX);
            serialPrint(" ");
        }
        serialPrint(" CRC=");
        serialPrint(OneWire::crc8(data, 8), HEX);
        serialPrintln();

        // Convert the data to actual temperature
        // because the result is a 16 bit signed integer, it should
        // be stored to an "int16_t" type, which is always 16 bits
        // even when compiled on a 32 bit processor.
        int16_t raw = (data[1] << 8) | data[0];
        if (type_s == 2) raw = (data[2] << 8) | data[1];
        byte cfg = (data[4] & 0x60);

        switch (type_s) {
            case 1:
            raw = raw << 3; // 9 bit resolution default
            if (data[7] == 0x10) {
                // "count remain" gives full 12 bit resolution
                raw = (raw & 0xFFF0) + 12 - data[6];
            }
            celsius = (float)raw * 0.0625;
            break;
            case 0:
            // at lower res, the low bits are undefined, so let's zero them
            if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
            if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
            if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
            // default is 12 bit resolution, 750 ms conversion time
            celsius = (float)raw * 0.0625;
            break;

            case 2:
            data[1] = (data[1] >> 3) & 0x1f;
            if (data[2] > 127) {
                celsius = (float)data[2] - ((float)data[1] * .03125);
            }else{
                celsius = (float)data[2] + ((float)data[1] * .03125);
            }
        }

        fahrenheit = celsius * 1.8 + 32.0;
        serialPrint("  Temperature = ");
        serialPrint(celsius);
        serialPrint(" Celsius, ");
        serialPrint(fahrenheit);
        serialPrintln(" Fahrenheit");
    }

    serialPrintln("No more addresses.");
    serialPrintln();
    ds.reset_search();

    return fahrenheit;
}

void WeatherService::serialPrint(char s[]) {
    if (_debugMode) {
        Serial.print(s);
    }
}

void WeatherService::serialPrint(double value, int digits) {
    if (_debugMode) {
        Serial.print(value, digits);
    }
}

void WeatherService::serialPrint(long value) {
    if (_debugMode) {
        Serial.print(value);
    }
}

void WeatherService::serialPrintln(char s[]) {
    if (_debugMode) {
        Serial.println(s);
    }
}

void WeatherService::serialPrintln() {
    serialPrintln("");
}

void WeatherService::serialWrite(char c) {
    if (_debugMode) {
        Serial.write(c);
    }
}

int WeatherService::getSoilMoisture()
{
    int soilMoistureRaw;
    int soilMoistureAdjusted;

    digitalWrite(_soilMoisturePowerPin, HIGH); // power soil moisture probe
    delay(10); // wait 10 milliseconds 

    soilMoistureRaw = analogRead(_soilMoistureSignalPin); // Read the SIG value form sensor 
    
    digitalWrite(_soilMoisturePowerPin, LOW); //turn "Off" soil moisture probe
    
    serialPrint("Soil Moisture (Raw): ");    
    serialPrint((long)soilMoistureRaw);
    serialPrintln();

    soilMoistureAdjusted = map(soilMoistureRaw, _soilMoistureRangeLow, _soilMoistureRangeHigh, 0, 100);

    serialPrint("Soil Moisture (Adjusted): ");    
    serialPrint((long)soilMoistureAdjusted);
    serialPrintln();

    // cap soil moisture to 100
    if (soilMoistureAdjusted > 100) {
        soilMoistureAdjusted = 100;
    }

    return soilMoistureAdjusted;
}