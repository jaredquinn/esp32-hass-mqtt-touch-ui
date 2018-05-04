/**
   esp32tft.ini
   ESP32 TFT TouchScreen MQTT Display Panel

   The MIT License (MIT)
   Copyright (c) 2018 by Jared Quinn

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/


#define PIN_TS_CS 22
#define PIN_TFT_LED 33
#define PIN_TFT_DC 25
#define PIN_TFT_RESET 26
#define PIN_TFT_CS 27

#define PIN_HSPI_MOSI 13
#define PIN_HSPI_CLK 14
#define PIN_HSPI_MISO 12

#define ONE_WIRE_BUS 17

#define MQTT_PUBLISH_BASE "display/kitchen"

/* Framework Includes - these will be moved into the main framework soon */
#include "datastore.h"
#include "ingester.h"

/* Display Configuration, UI and Touchscreen and setup */
#include <SPI.h>

#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <XPT2046_Touchscreen.h>

#include "UI.h"
#include "touch.h"

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

UI_Sensor_Dallas sensor(ONE_WIRE_BUS, MQTT_PUBLISH_BASE);

long screenOn = 0;
int screenBrightness = 100;
int screenTimeout = 30000;
long lastTouch = 0;

/* These guys are the next target to be fixed up and made sane! */

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
DataStore displayTemp = DataStore("display_temp", DATASTORE_TYPE_FLOAT);
DataStore bedroomTemp = DataStore("bedroom_temp", DATASTORE_TYPE_FLOAT);
DataStore bedroomHumidity = DataStore("bedroom_humidity", DATASTORE_TYPE_FLOAT);

DataStore bathroomTemp = DataStore("bathroom_temp", DATASTORE_TYPE_FLOAT);
DataStore bathroomHumidity = DataStore("bathroom_humidity", DATASTORE_TYPE_FLOAT);

DataStore alarmState = DataStore("ha_alarm", DATASTORE_TYPE_CHAR);


MQTT_Ingester ingester = MQTT_Ingester(client);


#include "wifi_setup.h"
#include "clock.h"

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  char pl[255];
  char top[128];

  strcpy(top, topic);
  
  ui.activityLight(0, true, ILI9341_BLUE);  
  for (int i = 0; i < length; i++) { if (i < 255) { pl[i] = (char)payload[i]; pl[i + 1] = NULL; } }
  ingester.process(top, pl);
  ui.activityLight(0, false);
};

void mqtt_reconnect() {
  while (!client.connected()) {    
    String clientId = "DevArd-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      ingester.subscribe();      
      client.subscribe("display/kitchen/#");
      delay(100);
    } else {      
      #ifdef USE_SERIAL
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
      #endif
      delay(5000);
    }
  }
};


void ota_setup() {
  ArduinoOTA
  .onStart([]() {
    client.publish("display/kitchen/status", "UPDATING");
    ui.updateStatus("Rcv. OTA Update", ui.screen->color565(255, 0, 0), false);
    ui.enableDrawWidgets(false);
  })
  .onEnd([]() {
    client.publish("display/kitchen/status", "FINISHED");
    ui.updateStatus("Finishing Update", ui.screen->color565(0, 255, 0), false);
    ui.enableDrawWidgets(true);
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    //    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    ui.enableDrawWidgets(true);
    client.publish("display/kitchen/status", "ERROR");
    //    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) ui.updateStatus("OTA: Auth Failed", ui.screen->color565(255, 0, 0), false);
    else if (error == OTA_BEGIN_ERROR) ui.updateStatus("OTA: Begin Failed", ui.screen->color565(255, 0, 0), false);
    else if (error == OTA_CONNECT_ERROR) ui.updateStatus("OTA: Connect Failed", ui.screen->color565(255, 0, 0), false);
    else if (error == OTA_RECEIVE_ERROR) ui.updateStatus("OTA: Rcv Error", ui.screen->color565(255, 0, 0), false);
    else if (error == OTA_END_ERROR) ui.updateStatus("OTA: End Failed", ui.screen->color565(255, 0, 0), false);
  });

  ArduinoOTA.begin();
}


void setup() {

  randomSeed(analogRead(0));
  ui.initializeScreen();

  local_wifi_setup();
  ota_setup();

  ingester.addIngester("statestream/sensor/load_1m", enumIngestType::STATE_INT, systemLoad1m);
  ingester.addIngester("statestream/light/bathroom_light", enumIngestType::STATE_ONOFF, bathroomLight);
  ingester.addIngester("statestream/light/kitchen_light", enumIngestType::STATE_ONOFF, kitchenLight);
  ingester.addIngester("statestream/switch/fish_lamp", enumIngestType::STATE_ONOFF, aquariumLight);
  ingester.addIngester("statestream/sensor/broadlink_sensor_temperature", enumIngestType::STATE_FLOAT, kitchenTemp);
  ingester.addIngester("statestream/sensor/aquarium_temperature", enumIngestType::STATE_FLOAT, outsideTemp);
  ingester.addIngester("statestream/sensor/temperature_158d0001c15683", enumIngestType::STATE_FLOAT, bedroomTemp);
  ingester.addIngester("statestream/sensor/humidity_158d0001c15683", enumIngestType::STATE_FLOAT, bedroomHumidity);
  ingester.addIngester("statestream/sensor/temperature_158d0001c15683", enumIngestType::STATE_FLOAT, bathroomTemp);
  ingester.addIngester("statestream/sensor/humidity_158d0001c15683", enumIngestType::STATE_FLOAT, bathroomHumidity);
  ingester.addIngester("statestream/light/lounge_light", enumIngestType::STATE_ONOFF, loungeLight);
  ingester.addIngester("statestream/alarm_control_panel/ha_alarm", enumIngestType::STATE_CHAR, alarmState);
  ingester.addIngester("statestream/light/lounge_light_extra", enumIngestType::STATE_ONOFF, loungeExtraLight);
  ingester.addIngester("statestream/light/dining_light", enumIngestType::STATE_ONOFF, diningLight);
  ingester.addIngester("statestream/light/hallway_light", enumIngestType::STATE_ONOFF, hallwayLight);
  ingester.addIngester("statestream/sensor/display_temperature", enumIngestType::STATE_FLOAT, displayTemp);

  /* The current model of object and add will be replaced by a single add like UI shortly */

  ui.addWidget(UI_POS_1_1, UI_WIDGET_FLOAT, "Kitchen", "C", kitchenTemp);
  ui.addWidget(UI_POS_1_2, UI_WIDGET_FLOAT, "Outside", "C", outsideTemp);
  ui.addWidget(UI_POS_1_3, UI_WIDGET_BULB, "Kitchn", "", kitchenLight);
  ui.addWidget(UI_POS_1_4, UI_WIDGET_BULB, "Lounge", "", loungeLight);

  ui.addWidget(UI_POS_2_1, UI_WIDGET_FLOAT, "Attached", "C", displayTemp);
//ui.addWidget(UI_POS_2_2, UI_WIDGET_IMAGE, sunny);
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

  ui.addWidget(UI_BUTTON_1, UI_WIDGET_BUTTON, "Temp");
  ui.addWidget(UI_BUTTON_2, UI_WIDGET_BUTTON, "Light");

  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(mqtt_callback);

  //ingester.setup();
  
  client.publish("display/kitchen/status", "BOOTING");

  touch.begin();
  sensor.setup(&client);

  delay(1000);
  ui.finishSetup();
}


void loop() {
  long now = millis();

  if (!client.connected()) {
    mqtt_reconnect();
  }

  ArduinoOTA.handle();

  client.loop();
  touch.loop(&client);
  uiClock.loop();
  ui.loop();
  sensor.loop(&client);

}



