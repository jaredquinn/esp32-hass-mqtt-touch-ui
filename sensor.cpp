
#include <PubSubClient.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <ArduinoJson.h>

#include "sensor.h"

UI_Sensor_Dallas::UI_Sensor_Dallas(uint8_t pinOneWire, char *bt) {
  _ow = new OneWire(pinOneWire);
  _sensor = new DallasTemperature(_ow);
  strcpy(baseTopic, bt);
  
};

void UI_Sensor_Dallas::setup(PubSubClient * ps) {
  (*_sensor).begin();

  //(*_sensor).setResolution(tempDeviceAddress, 9);
  (*_sensor).setWaitForConversion(false);
  (*_sensor).requestTemperatures(); 

};

void UI_Sensor_Dallas::_sendHAconfig(PubSubClient * ps) {

  char idStr[255], target[64], topic[64], jsonStr[255], deviceCountStr[32];
  
  for(unsigned i = 0; i < deviceCount; i++) {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    
    (*_sensor).getAddress(_address[i], i);;

    sprintf(idStr, "%x", _address[i]);
    sprintf(topic, "%s/sensor/temperature_%x/state", baseTopic, _address[i]);
    sprintf(target, "homeassistant/sensor/temperature_%x/config", _address[i]);

    root["name"] = idStr;
    root["device_class"] = "temperature";
    root["state_topic"] = topic;
    root["unit_of_measurement"] = "°C";

    root.printTo(jsonStr, sizeof(jsonStr));
    (*ps).publish(target, jsonStr);
  }

};

/*
 * 
 */
void UI_Sensor_Dallas::loop(PubSubClient* ps) {
  
  float  temp = 0;
  char diffStr[32];
  char topic[255];
  char tmpStr[255];

  /* Reset the OneWire bus every 120 seconds */
  if(millis() - _lastValid > 10000 && millis() - _lastReset > 10000) {
    sprintf(topic, "%s/event", baseTopic);
    (*ps).publish(topic, "onewire-reset");
    (*_sensor).begin();
    _lastReset = millis();
  }
  /* If it's been more than one second since our reset,
   *  check the bus for devices again
   */
  if(millis() - _lastReset > 1000 && millis() - _lastConfig > 10000) {
    _sendHAconfig(ps);
    _lastConfig = millis();
  }

  /* If it's been more 1.5 seconds since the last bus reset
   *  then we can check the temperatures.
   */
  if(millis() - _lastReset > 1200 && millis() - _lastUpdate > 1000) {
    /* If we haven't requested temperatures, do that now,
     *  we don't want to delay and lock the CPU, so we'll 
     *  get the values later.
     */
    _polls++;
    
      /* Read the values, we do this on alternating calls every second.
       */
      deviceCount = (*_sensor).getDeviceCount();
  
      for(unsigned i = 0; i < deviceCount; i++) {
        temp = (*_sensor).getTempC(_address[i]);

        /* Ignore temperatures outside of range */
        if(temp > -127.00 && temp < 85.00) {
          /* calculate the difference from the last reading */
          
          _lastValid = millis();
          
          float diff = (10000 * (float) temp)-(10000 * (float) _lastValue[i]);          
          if(temp == _lastValue[i] || abs(diff) < 500) {
            //(*ps).publish("display/kitchen/event", "Temperature is not different");
          } else {
            _lastValue[i] = temp;
            _lastSent[i] = millis();           
            
            sprintf(topic, "%s/sensor/temperature_%x/state", baseTopic, _address[i]);
            sprintf(tmpStr, "%f", temp);
            (*ps).publish(topic, tmpStr);                 
          }
        }      
      }
      (*_sensor).requestTemperatures(); 
      _lastUpdate = millis();
   }
};


