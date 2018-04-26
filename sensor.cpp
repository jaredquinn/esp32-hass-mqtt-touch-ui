
#include <PubSubClient.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#include "sensor.h"

UI_Sensor_Dallas::UI_Sensor_Dallas(uint8_t pinOneWire, UI * ui) {
  _ui = ui;
  _ow = new OneWire(pinOneWire);
  _sensor = new DallasTemperature(_ow);
  
};

void UI_Sensor_Dallas::setup(PubSubClient * ps) {
  (*_ui).updateStatus("Setting up OneWire", (*_ui).screen->color565(0,0,255));
  (*_sensor).begin();

  deviceCount = (*_sensor).getDeviceCount();

  for(unsigned c = 0; c < deviceCount; c++) {
    (*_sensor).getAddress(_address[c], c);
    char str[255];
    sprintf(str, "Found %x", _address[c]);
    (*_ui).updateStatus(str, (*_ui).screen->color565(0,0,255));  
    sprintf(str, "%x", _address[c]);
    (*ps).publish("display/kitchen/sensor/discovery", str);
  }
};

void UI_Sensor_Dallas::loop(PubSubClient* ps) {

  if(millis() - _lastUpdate > 1000 && _requestedTemp == false) {
    (*_sensor).setWaitForConversion(false);
    (*_sensor).requestTemperatures(); 
    _requestedTemp = true;
  }

  if (millis() - _lastUpdate > 2000) {
    float temp;
    char topic[255];
    char tmpStr[255];
    
    deviceCount = (*_sensor).getDeviceCount();

    for(unsigned i = 0; i < deviceCount; i++) {
      temp = (*_sensor).getTempC(_address[i]);
      if(_lastValue[i] != temp) {
        _lastValue[i] = temp;
        sprintf(topic, "display/kitchen/sensor/%x", _address[i]);
        sprintf(tmpStr, "%f", temp);
        (*ps).publish(topic, tmpStr);              
      }
    }
    _requestedTemp = false;
    _lastUpdate = millis();
  }

};


