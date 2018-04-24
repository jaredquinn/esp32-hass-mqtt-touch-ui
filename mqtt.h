
bool fishLampState = false;
bool kitchenLightState = false;
bool bathroomLightState = false;


void mqtt_send(char* topic, char *devtype, char *leaf, char* msg) {

  char fulltopic[100];

  snprintf (fulltopic, 100, "%s/%s/%s/%s_%s/%s", HA_MQTT_BASE, devtype, HA_MQTT_NODENAME, HA_MQTT_PREFIX, topic, leaf);

  if (USE_SERIAL) {
    Serial.print("Publish message: ");
    Serial.print(fulltopic);
    Serial.print(":");
    Serial.println(msg);
  }
  client.publish(fulltopic, msg);
}

void send_discovery() {

  char configString[255];
  char fulltopic[100];
  /*
    snprintf (fulltopic, 100, "%s/%s/%s/%s_%s/%s", HA_MQTT_BASE, "sensor", HA_MQTT_NODENAME, HA_MQTT_PREFIX, "button_3", "state");
    snprintf (configString, 255, "{ \"device_class\": \"sensor\", \"name\": \"Temperature\", \"state_topic\": \"%s\", \"value_template\": \"{{ value_json.state}}\" }", fulltopic);

    mqtt_send("button_1", "sensor", "config", configString);
  */

}


void reconnect() {
  while (!client.connected()) {
    if (USE_SERIAL) {
      Serial.print("Attempting MQTT connection...");
    }
    
    // Create a random client ID
    String clientId = "DevArd-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      if (USE_SERIAL) {
        Serial.println("connected");
      }
      client.subscribe("homeassistant/light/#");
      client.subscribe("homeassistant/sensor/#");
      client.subscribe("homeassistant/switch/#");
      client.subscribe("homeassistant/alarm_control_panel/#");

      client.subscribe("display/kitchen/#");

      delay(100);

        client.publish("display/kitchen/status/last", "HELLO");  
        send_discovery();
    } else {
      if (USE_SERIAL) {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
      }
      delay(5000);
    }
  }
}



void callback(char* topic, byte* payload, unsigned int length) {

  show_mqtt_activity(true);
  
  char pl[255];
  if (USE_SERIAL) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
  }
  for (int i = 0; i < length; i++) {
    if (i < 255) {
      pl[i] = (char)payload[i];
      pl[i + 1] = NULL;
    }
    if (USE_SERIAL) {
      Serial.print((char)payload[i]);
    }
  }
  if (USE_SERIAL) {
    Serial.println();
  }

  ts_mqtt(topic, pl);

  
  //drawStatus(topic, ILI9341_ORANGE);

  char o[255];
  int c = myset.process(&ui, topic, pl, "homeassistant");
  if(c != 0) {
    sprintf(o, "Updated %d entries", c);
    client.publish("display/kitchen/event/update", o);
  }
  
  if (strcmp(topic, "display/kitchen/setting/time_offset") == 0) {
    timeClient.setTimeOffset(atoi(pl));
  }


  if (strcmp(topic, "display/kitchen/setting/brightness") == 0) {
        screenBrightness = (atoi(pl)/100)*255;
        if(screenOn > 0) {
//          ledcWrite(pwm_ledChannel, screenBrightness);
        }
  }
   if (strcmp(topic, "display/kitchen/setting/display_timeout") == 0) {
        screenTimeout = (atoi(pl)*1000);
  }

  if (strcmp(topic, "display/kitchen/setting/display") == 0) {
    if(strcmp(pl, "on") == 0) {
//        ledcWrite(pwm_ledChannel, screenBrightness);
        screenOn = millis();
        ranBefore = false;      
    } 
    if(strcmp(pl, "off") == 0) {
//        ledcWrite(pwm_ledChannel, 0);
        screenOn = 0;  
    }
  }

  if (strcmp(topic, "display/kitchen/command") == 0) {
    if(strcmp(pl, "reboot") == 0) {
      ESP.restart();
    }
  }

  /*
  if (strcmp(topic, "homeassistant/light/bathroom_light/state") == 0) {
    Serial.println(pl);
    
    if(strcmp(pl, "on") == 0) {
      bathroomLight.update(true);
      bathroomLightState = true;
      drawStatus("Bathroom Light On", ILI9341_BLUE);
    } else {
      bathroomLightState = false; 
      bathroomLight.update(false);
      drawStatus("Bathroom Light Off", ILI9341_BLUE);
    }
    drawBulb(COL_BATHROOM_BULB, ROW_BATHROOM, bathroomLightState);    
  }
  */
  /*
  if (strcmp(topic, "homeassistant/switch/fish_lamp/state") == 0) {
    Serial.println(pl);
    if(strcmp(pl, "on") == 0) {
      fishLampState = true;
      drawStatus("Fish Lamp On", ILI9341_BLUE);
    } else {
      fishLampState = false; 
      drawStatus("Fish Lamp Off", ILI9341_BLUE);
    }
    drawBulb(COL_AQUARIUM_BULB, ROW_AQUARIUM, fishLampState);    
  }
  
  if (strcmp(topic, "homeassistant/light/kitchen_light/state") == 0) {
    Serial.println(pl);
    if(strcmp(pl, "on") == 0) {
      kitchenLightState = true;
      drawStatus("Kitchen Light On", ILI9341_BLUE);
    } else {
      kitchenLightState = false;
      drawStatus("Kitchen Light Off", ILI9341_BLUE);
    }
    drawBulb(COL_KITCHEN_BULB, ROW_KITCHEN, kitchenLightState);
  }
  */  

  if (strcmp(topic, "display/kitchen/event/touch")  == 0) {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(pl);
    int x = root["x"];
    int y = root["y"];
    
    if(x > 180 && x < 180+60 && y > 50 && y < 50+50) {
      if(kitchenLightState == false) {
        client.publish("display/kitchen/event/switch/kitchen_light", "on");
      } else {
        client.publish("display/kitchen/event/switch/kitchen_light", "off");
      }
      drawStatus("Switching Kithchen Light", ILI9341_GREEN);
    }
    


    if(x > COL_BATHROOM_BULB && x < COL_BATHROOM_BULB+60 && y > ROW_BATHROOM && y < ROW_BATHROOM+50) {
      if(bathroomLight.getBoolValue() == false) {
        client.publish("display/kitchen/event/switch/bathroom_light", "on");
      } else {
        client.publish("display/kitchen/event/switch/bathroom_light", "off");
      }
      drawStatus("Switching Bathroom Light", ILI9341_GREEN);
    }
  }


  /*
  
  if (strcmp(topic, "homeassistant/sensor/temperature_158d0001c15683/state") == 0) {
    float temp;
    temp = atof(pl);
    drawDecimal(COL_BEDROOM_TEMP, ROW_BEDROOM, temp);
    //drawDecimal(COL_BATHROOM_TEMP, ROW_BATHROOM, temp);
  }


  if (strcmp(topic, "homeassistant/sensor/humidity_158d0001c15683/state") == 0) {
    float hum;
    hum = atof(pl);
    drawDecimal(COL_BEDROOM_HUM, ROW_BEDROOM, hum);
    drawDecimal(COL_BATHROOM_HUM, ROW_BATHROOM, hum);
  }


  if (strcmp(topic, "homeassistant/sensor/kitchen_temperature/state") == 0) {
    float temp;
    temp = atof(pl);
    drawDecimal(COL_KITCHEN, ROW_KITCHEN, temp);
  }
  

  if (strcmp(topic, "homeassistant/sensor/aquarium_temperature/state") == 0) {
    float temp;
    temp = atof(pl);
    drawDecimal(COL_DEVICE_EXT, ROW_DEVICE, temp);
  }

*/
  show_mqtt_activity(false);

}


void mqtt_setup() {
  myscreen.println("Connecting to MQTT Server");
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}

