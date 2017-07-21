#include "application.h"

#include "lib/SparkWeatherShield/SparkFun_Photon_Weather_Shield_Library.h" // Include the SparkFun MPL3115A2 library
#include "OneWire.h"

#ifndef WeatherService_h
#define WeatherService_h

class WeatherService {
    public:
        WeatherService();
        
        void init(bool debugMode);
        char* getWeatherData();
    private:
        Weather _sensor;
        bool _debugMode;
        
        int _soilMoistureSignalPin;
        int _soilMoisturePowerPin;
        int _soilMoistureRangeLow;
        int _soilMoistureRangeHigh;
        
        int _anemometerSignalPin;
        float _anemometerScaleMPH;
        unsigned int _lastAnemoneterEvent;
        volatile unsigned int _gustPeriod;
        volatile unsigned int _anemoneterPeriodTotal;
        volatile unsigned int _anemoneterPeriodReadingCount;

        int _windVaneSignalPin;
        float _windVaneCosTotal;
        float _windVaneSinTotal;
        unsigned int _windVaneReadingCount;

        float getSoilTemp();
        void serialPrint(char s[]);
        void serialPrint(long value);
        void serialPrint(double value, int digits);
        void serialWrite(char c);
        void serialPrintln(char s[]);
        void serialPrintln();
        int getSoilMoisture();
        
        void handleAnemometerEvent();
        float getAnemometerMPH(float * gustMPH);

        void captureWindVane();
        float lookupRadiansFromRaw(unsigned int analogRaw);
        float getWindVaneDegrees();
};

#endif