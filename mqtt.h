
/* This library is mostly deprecated */



void mqtt_send(char* topic, char *devtype, char *leaf, char* msg) {

  char fulltopic[100];

  snprintf (fulltopic, 100, "%s/%s/%s/%s_%s/%s", HA_MQTT_BASE, devtype, HA_MQTT_NODENAME, HA_MQTT_PREFIX, topic, leaf);

  #ifdef USE_SERIAL
    Serial.print("Publish message: ");
    Serial.print(fulltopic);
    Serial.print(":");
    Serial.println(msg);
  #endif
  client.publish(fulltopic, msg);
}



void reconnect(DataSet * set) {
  while (!client.connected()) {
    #ifdef USE_SERIAL
      Serial.print("Attempting MQTT connection...");
    #endif
    
    // Create a random client ID
    String clientId = "DevArd-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    
    if (client.connect(clientId.c_str())) {

      (*set).subscribe(&client);      
/*      client.subscribe("homeassistant/light/#");
      client.subscribe("homeassistant/sensor/#");
      client.subscribe("homeassistant/switch/#");
      client.subscribe("homeassistant/alarm_control_panel/#");
*/
      client.subscribe("display/kitchen/#");

      delay(100);

      //  client.publish("display/kitchen/status/last", "HELLO");  
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



void callback(char* topic, byte* payload, unsigned int length) {

  /* change our little green mqtt indicator */
  ui.activityLight(0, true, ILI9341_BLUE);
  
  char pl[255];
  char o[255];

  //sprintf(pl, "%s", payload);
  
  for (int i = 0; i < length; i++) {
    if (i < 255) {
      pl[i] = (char)payload[i];
      pl[i + 1] = NULL;
    }
  }

  ts_mqtt(topic, pl);
  myset.process(&ui, topic, pl, "homeassistant");
  ui.activityLight(0, false);

};


void mqtt_setup() {
  myscreen.println("Connecting to MQTT Server");
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
};
