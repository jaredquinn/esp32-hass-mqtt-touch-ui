
#ifndef UI_SENSOR_DALLAS
#define UI_SENSOR_DALLAS

#define MAX_SENSORS 8

#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>


class UI_Sensor_Dallas {
  public:
    UI_Sensor_Dallas(uint8_t pinOneWire, char *bt);

    void setup(PubSubClient * ps);
    void loop(PubSubClient * ps);

    int deviceCount = 0;
    
  private:

    void _sendHAconfig(PubSubClient * ps);

    char baseTopic[72];
    
    long _configurationSent;
    
    OneWire * _ow;
    DallasTemperature * _sensor;    
    DeviceAddress _address[MAX_SENSORS]; 

    long _lastSent[MAX_SENSORS] = {};
    float _lastValue[MAX_SENSORS] = {};

    long _requestedTemp = 0;
    long _lastUpdate = 0;

    long _lastReset = 0;
    long _lastConfig = 0;
    long _polls = 0;
    long _resetFrequency = 10000;
    long _lastValid = 0;
    
};


#endif

