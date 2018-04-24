#include <OneWire.h>
#include <DallasTemperature.h>


#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();


bool USE_SERIAL = true;

#define USE_FAST_PINIO true

#define ONE_WIRE_BUS 17
#define MAX_SENSORS 8

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

DeviceAddress TempAddress[MAX_SENSORS]; 


#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>


#include "WiFiCredentials.h"

#include <PubSubClient.h>

#include <ArduinoJson.h>

WiFiClient espClient;
PubSubClient client(espClient);


#include "pins.h"

#include <SPI.h>

#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

Adafruit_ILI9341 myscreen = Adafruit_ILI9341(PIN_TFT_CS, PIN_TFT_DC, PIN_HSPI_MOSI, PIN_HSPI_CLK, PIN_TFT_RESET, PIN_HSPI_MISO);

#include <XPT2046_Touchscreen.h>
XPT2046_Touchscreen ts(PIN_TS_CS);

WiFiUDP ntpUDP;

#include <NTPClient.h>
#include "clock.h"

#include "UI.h"
#include "rows.h"

#include "datastore.h"
#include "dataingest.h"
#include "dataset.h"

UI ui = UI(myscreen);

NTPClient timeClient(ntpUDP, NTP_SERVER, 36000, 36000);
UI_Clock_NTP uiClock(&ui, &timeClient);


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




#include "touch.h"

#include "mqtt.h"

#include "wifi_setup.h"
#include "clock.h"
#include "sensor.h"

#include "ota.h"


void setup() {

  randomSeed(analogRead(0));

  // Turn on the screen
  screenOn = millis();
  runState = 1;

//  backlight_setup();
  local_wifi_setup();
  ota_setup();
    
  myscreen.println("Getting Time from NTP");
  timeClient.begin();


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
  
  sensor_setup();
  touch_setup();

  booted = millis();
  runState = 10;
  delay(1000);  
  

  ui.finishSetup();

}


void loop() {
  long now = millis();
  bool requestedTemp;

  ArduinoOTA.handle();
  
  if (!client.connected()) reconnect();
  client.loop();

  /* checkTouch moving to UI */
  checkTouch();

  /* Display on/off to move to UI */
  if (screenOn > 0 && now - screenOn > screenTimeout && \
      now - lastTouch > screenTimeout && screenTimeout != -1) {
     screenOn = 0; // set it here to prevent multiple triggers
     client.publish("display/kitchen/setting/display", "off");
  }


  if (now - lastTime > 1000) {
    ui.loop();
    lastTime = now;
  }

  if (now - mqttPublishTemp > 1000 && requestedTemp == false) {
    sensor_ask_values();
    requestedTemp = true;
   }
  
  if (now - mqttPublishTemp > 2000) {
    sensor_get_values();
  }

}

