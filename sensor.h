
#ifndef UI_SENSOR_DALLAS
#define UI_SENSOR_DALLAS

#define MAX_SENSORS 8

#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>

#include "UI.h"

class UI_Sensor_Dallas {
  public:
    UI_Sensor_Dallas(uint8_t pinOneWire, UI * ui);

    void setup(PubSubClient * ps);
    void loop(PubSubClient * ps);

    int deviceCount = 0;
    
  private:
    
    UI * _ui;
    OneWire * _ow;
    DallasTemperature * _sensor;    
    DeviceAddress _address[MAX_SENSORS]; 

    long _lastSent[MAX_SENSORS] = {};
    long _lastValue[MAX_SENSORS] = {};

    long _requestedTemp = 0;
    long _lastUpdate = 0;
};


#endif

/*
#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();
*/

