
#include <PubSubClient.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <ArduinoJson.h>

#include "sensor.h"

UI_Sensor_Dallas::UI_Sensor_Dallas(uint8_t pinOneWire, UI * ui) {
  _ui = ui;
  _ow = new OneWire(pinOneWire);
  _sensor = new DallasTemperature(_ow);

  
};

void UI_Sensor_Dallas::setup(PubSubClient * ps) {
  (*_ui).updateStatus("Setting up OneWire", (*_ui).screen->color565(0,0,255));
  (*_sensor).begin();
};

void UI_Sensor_Dallas::_sendHAconfig(PubSubClient * ps) {

  char idStr[255], topic[64], jsonStr[255], deviceCountStr[32];
  
  deviceCount = (*_sensor).getDeviceCount();
  sprintf(deviceCountStr, "devices: %d polls: %d", deviceCount, _polls);
  
  for(unsigned i = 0; i < deviceCount; i++) {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    
    (*_sensor).getAddress(_address[i], i);;

    sprintf(idStr, "%x", _address[i]);
    sprintf(topic, "display/kitchen/sensor/temperature_%x", _address[i]);

    root["name"] = idStr;
    root["device_class"] = "temperature";
    root["state_topic"] = topic;
    root["unit_of_measurement"] = "°C";

    root.printTo(jsonStr, sizeof(jsonStr));
    (*ps).publish("homeassistant/sensor/temperature_%x/config", jsonStr);
  }
  (*ps).publish("display/kitchen/sensor_count", deviceCountStr);
};

/*
 * 
 */
void UI_Sensor_Dallas::loop(PubSubClient* ps) {
  
  float  temp = 0;
  float  diff = 0;
  char diffStr[32];
  char topic[255];
  char tmpStr[255];

  /* Reset the OneWire bus every 120 seconds */
  if(millis() - _lastReset > _resetFrequency) {
    //(*_sensor).begin();
    _lastReset = millis();
  }
  /* If it's been more than one second since our reset,
   *  check the bus for devices again
   */
  if(millis() - _lastReset > 5000 && millis() - _lastConfig > _resetFrequency) {
    _sendHAconfig(ps);
    _lastConfig = millis();
  }

  if(_resetFrequency < 60000 && _polls > 30) {
    _resetFrequency = 60000;
  }

  /* If it's been more 1.5 seconds since the last bus reset
   *  then we can check the temperatures.
   */
  if(millis() - _lastReset > 1500 && millis() - _lastUpdate > 5000) {
    /* If we haven't requested temperatures, do that now,
     *  we don't want to delay and lock the CPU, so we'll 
     *  get the values later.
     */
    _polls++;
    
    if(!_requestedTemp) {
      (*_sensor).setWaitForConversion(false);
      (*_sensor).requestTemperatures(); 
      _requestedTemp = true;
    } else {  
      /* Read the values, we do this on alternating calls every second.
       */
      deviceCount = (*_sensor).getDeviceCount();
  
      for(unsigned i = 0; i < deviceCount; i++) {
        temp = (*_sensor).getTempC(_address[i]);

        /* Ignore temperatures outside of range */
        if(temp > -127.00 && temp < 85.00) {
          /* calculate the difference from the last reading */
          
          diff = (10000 * (float) temp)-(10000 * (float) _lastValue[i]);          
          sprintf(diffStr, "%f", diff);
          
          (*ps).publish("display/kitchen/difference", diffStr);   

          if(_lastSent[i] == 0 || diff < 500 || diff > 500) {         
            _lastValue[i] = temp;
                      
            sprintf(topic, "display/kitchen/sensor/temperature_%x", _address[i]);
            sprintf(tmpStr, "%f", temp);
            (*ps).publish(topic, tmpStr);   
  
            _lastSent[i] = millis();           
          }
        }
      }
      _requestedTemp = false;
    }    
    _lastUpdate = millis();  
  }
};


