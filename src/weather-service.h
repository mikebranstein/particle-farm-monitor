#include "application.h"

#include "lib/SparkWeatherShield/SparkFun_Photon_Weather_Shield_Library.h" // Include the SparkFun MPL3115A2 library

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
};

#endif