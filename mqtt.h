
/* This library is mostly deprecated */
/* it will be moved to the DataSet class to provide the framework interfacce around MQTT */

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
    String clientId = "DevArd-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      (*set).subscribe(&client);      
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



void callback(char* topic, byte* payload, unsigned int length) {

  /* change our little green mqtt indicator */
  ui.activityLight(0, true, ILI9341_BLUE);
  
  char pl[255];
  char o[255];

  for (int i = 0; i < length; i++) {
    if (i < 255) { pl[i] = (char)payload[i]; pl[i + 1] = NULL; }
  }

  //ts_mqtt(topic, pl);
  myset.process(&ui, topic, pl, "homeassistant");
  
  ui.activityLight(0, false);
};


void mqtt_setup() {
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
};
