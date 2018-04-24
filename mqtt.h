
/* This library is mostly deprecated */



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

  /* change our little green mqtt indicator */
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

  /* Touch handlers are next to move to UI */
 
  if (strcmp(topic, "display/kitchen/event/touch")  == 0) {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(pl);
    int x = root["x"];
    int y = root["y"];
    
    if(x > 180 && x < 180+60 && y > 50 && y < 50+50) {
      if(kitchenLight.getBoolValue() == false) {
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


  /* Change our little green dot */
  show_mqtt_activity(false);

}


void mqtt_setup() {
  myscreen.println("Connecting to MQTT Server");
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}

