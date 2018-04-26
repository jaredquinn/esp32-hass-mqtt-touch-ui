/**
 * example.cpp 
 * Example Sketch using the Framework
 * 
 * The MIT License (MIT)
 * Copyright (c) 2018 by Jared Quinn
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


/* Framework Includes - these will be moved into the main framework soon */
#include "datastore.h"
#include "dataingest.h"
#include "dataset.h"

/* Display Configuration, UI and Touchscreen and setup */
#include <SPI.h>

#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <XPT2046_Touchscreen.h>
#include "UI.h"
#include "touch.h"

#define PIN_TS_CS 22
//#define PIN_TS_IRQ 5

#define PIN_TFT_LED 33
#define PIN_TFT_DC 25
#define PIN_TFT_RESET 26
#define PIN_TFT_CS 27

#define PIN_HSPI_MOSI 13
#define PIN_HSPI_CLK 14
#define PIN_HSPI_MISO 12

#define ONE_WIRE_BUS 17

Adafruit_ILI9341 myscreen = Adafruit_ILI9341(PIN_TFT_CS, PIN_TFT_DC, PIN_HSPI_MOSI, PIN_HSPI_CLK, PIN_TFT_RESET, PIN_HSPI_MISO);
XPT2046_Touchscreen ts(PIN_TS_CS);
UI ui = UI(myscreen, PIN_TFT_LED);
UI_Touch touch = UI_Touch(&ui, &ts);

/* Network related stuff */
#include <ArduinoJson.h>

#include <WiFiUdp.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>

#include "clock.h"
#include "WiFiCredentials.h"


WiFiClient espClient;
WiFiUDP ntpUDP;
PubSubClient client(espClient);
NTPClient timeClient(ntpUDP, NTP_SERVER, 36000, 36000);
UI_Clock_NTP uiClock(&ui, &timeClient);

#include "sensor.h"

UI_Sensor_Dallas sensor(ONE_WIRE_BUS, &ui);
/* Sensors */



long booted = 0;
long uptime = 0;

long screenOn = 0;
int screenBrightness = 100;
int screenTimeout = 30000;

long lastTouch = 0;

long lastStatusPush = 0;

bool ranBefore = false;

long lastTime = 0;
long lastCheckTemp = 0;
float currentTemp = 0;
float lastTemp = 0;
int mqttPublishTemp = 0;
int hall = 0;

float mqttTemp = 0;


int runState = 0;

/* Data Store to be replaced with a single dynamic data store class soon */

DataStore bathroomLight = DataStore("bathroom_light", DATASTORE_TYPE_BOOL);
DataStore kitchenLight = DataStore("kitchen_light", DATASTORE_TYPE_BOOL);
DataStore aquariumLight = DataStore("fish_light", DATASTORE_TYPE_BOOL);
DataStore loungeLight = DataStore("lounge_light", DATASTORE_TYPE_BOOL);
DataStore loungeExtraLight = DataStore("lounge_light_extra", DATASTORE_TYPE_BOOL);
DataStore diningLight = DataStore("dining_light", DATASTORE_TYPE_BOOL);
DataStore hallwayLight = DataStore("hallway_light", DATASTORE_TYPE_BOOL);

DataStore systemLoad1m = DataStore("system_load_1m", DATASTORE_TYPE_INT);

DataStore kitchenTemp = DataStore("kitchen_temp", DATASTORE_TYPE_FLOAT);
DataStore outsideTemp = DataStore("outside_temp", DATASTORE_TYPE_FLOAT);
DataStore aquariumTemp = DataStore("aquarium_temp", DATASTORE_TYPE_FLOAT);

DataStore bedroomTemp = DataStore("bedroom_temp", DATASTORE_TYPE_FLOAT);
DataStore bedroomHumidity = DataStore("bedroom_humidity", DATASTORE_TYPE_FLOAT);

DataStore bathroomTemp = DataStore("bathroom_temp", DATASTORE_TYPE_FLOAT);
DataStore bathroomHumidity = DataStore("bathroom_humidity", DATASTORE_TYPE_FLOAT);

DataStore alarmState = DataStore("ha_alarm", DATASTORE_TYPE_CHAR);

DataSet myset = DataSet();

DataIngest mqttBathroomLight = DataIngest( "light/bathroom_light", DATAINGEST_TYPE_LIGHT, bathroomLight);
DataIngest mqttKitchenLight  = DataIngest( "light/kitchen_light", DATAINGEST_TYPE_LIGHT, kitchenLight);
DataIngest mqttLoungeLight  = DataIngest( "light/lounge_light", DATAINGEST_TYPE_LIGHT, loungeLight);
DataIngest mqttLoungeExtraLight  = DataIngest( "light/lounge_light_extra", DATAINGEST_TYPE_LIGHT, loungeExtraLight);
DataIngest mqttAquariumLight = DataIngest( "switch/fish_lamp", DATAINGEST_TYPE_SWITCH, aquariumLight);
DataIngest mqttDiningLight = DataIngest("light/dining_light", DATAINGEST_TYPE_LIGHT, diningLight);
DataIngest mqttHallwayLight = DataIngest("light/hallway_light", DATAINGEST_TYPE_LIGHT, hallwayLight);

DataIngest mqttSystemLoad1m = DataIngest( "sensor/load_1m", DATAINGEST_TYPE_STATE_INT, systemLoad1m);

DataIngest mqttKitchenTemp = DataIngest( "sensor/kitchen_temperature", DATAINGEST_TYPE_STATE_FLOAT, kitchenTemp);
DataIngest mqttOutsideTemp = DataIngest( "sensor/aquarium_temperature", DATAINGEST_TYPE_STATE_FLOAT, outsideTemp);

DataIngest mqttBedroomTemp = DataIngest( "sensor/temperature_158d0001c15683", DATAINGEST_TYPE_STATE_FLOAT, bedroomTemp);
DataIngest mqttBedroomHumidity = DataIngest( "sensor/humidity_158d0001c15683", DATAINGEST_TYPE_STATE_FLOAT, bedroomHumidity);

DataIngest mqttBathroomTemp = DataIngest( "sensor/temperature_158d0001c15683", DATAINGEST_TYPE_STATE_FLOAT, bathroomTemp);
DataIngest mqttBathroomHumidity = DataIngest( "sensor/humidity_158d0001c15683", DATAINGEST_TYPE_STATE_FLOAT, bathroomHumidity);

DataIngest mqttAlarmState = DataIngest("alarm_control_panel/ha_alarm", DATAINGEST_TYPE_STATE_CHAR, alarmState);



#include "mqtt.h"

#include "wifi_setup.h"
#include "clock.h"

#include "ota.h"


void setup() {
  randomSeed(analogRead(0));
  ui.initializeScreen();
  
  local_wifi_setup();
  ota_setup();  
  
  /* The current model of object and add will be replaced by a single add like UI shortly */
  myset.addIngester(mqttSystemLoad1m);
  myset.addIngester(mqttBathroomLight);
  myset.addIngester(mqttKitchenLight);
  myset.addIngester(mqttAquariumLight);
  myset.addIngester(mqttKitchenTemp);
  myset.addIngester(mqttOutsideTemp);
  myset.addIngester(mqttBedroomTemp);
  myset.addIngester(mqttBedroomHumidity);
  myset.addIngester(mqttLoungeLight);
  myset.addIngester(mqttAlarmState);
  myset.addIngester(mqttLoungeExtraLight);
  myset.addIngester(mqttDiningLight);
  myset.addIngester(mqttHallwayLight);

  ui.addWidget(UI_POS_1_1, UI_WIDGET_FLOAT, "Kitchen", "C", kitchenTemp);
  ui.addWidget(UI_POS_1_2, UI_WIDGET_FLOAT, "Outside", "C", outsideTemp);
  ui.addWidget(UI_POS_1_3, UI_WIDGET_BULB, "Kitchn", "", kitchenLight);
  ui.addWidget(UI_POS_1_4, UI_WIDGET_BULB, "Lounge", "", loungeLight);
  
  
  ui.addWidget(UI_POS_2_3, UI_WIDGET_BULB, " Fish ", "", aquariumLight);
  ui.addWidget(UI_POS_2_4, UI_WIDGET_BULB, "Lnge X", "", loungeExtraLight);

  ui.addWidget(UI_POS_3_1, UI_WIDGET_FLOAT, "Bathroom", "C", bathroomTemp);
  ui.addWidget(UI_POS_3_2, UI_WIDGET_FLOAT, "Bathroom", "%H", bathroomHumidity);
  ui.addWidget(UI_POS_3_3, UI_WIDGET_BULB, " Bath ", "", bathroomLight);
  ui.addWidget(UI_POS_3_4, UI_WIDGET_BULB, "Dining", "", diningLight);
  
  ui.addWidget(UI_POS_4_1, UI_WIDGET_FLOAT, "Bedroom", "C", bedroomTemp);
  ui.addWidget(UI_POS_4_2, UI_WIDGET_FLOAT, "Bedroom", "%H", bedroomHumidity);
  ui.addWidget(UI_POS_4_3, UI_WIDGET_BULB, " Hall ", "", hallwayLight);
  ui.addWidget(UI_POS_4_4, UI_WIDGET_HOUSE, " Alarm", "", alarmState);
  
  ui.addButton(UI_BUTTON_1, UI_WIDGET_BUTTON, "Temp");
  ui.addButton(UI_BUTTON_2, UI_WIDGET_BUTTON, "Light");
  
  mqtt_setup();

  touch.begin();
  sensor.setup(&client);
  
  booted = millis();
  runState = 10;
  delay(1000);  
  

  ui.finishSetup();

}


void loop() {
  long now = millis();

  ArduinoOTA.handle();
  
  if (!client.connected()) {
    reconnect(&myset);
  }
  client.loop();

  /* checkTouch moving to UI */
  touch.loop(&client);
  

  /* Display on/off to move to UI */
  if (screenOn > 0 && now - screenOn > screenTimeout && \
      now - lastTouch > screenTimeout && screenTimeout != -1) {
     screenOn = 0; // set it here to prevent multiple triggers
     client.publish("display/kitchen/setting/display", "off");
  }


  uiClock.loop();
  
  if (now - lastTime > 250) {
    ui.loop();
    lastTime = now;
  }
  
  sensor.loop(&client);

  

}

