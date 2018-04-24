

void sensor_setup() {
  myscreen.println("Starting OneWire BUS for External Temp");
  sensors.begin();
}

void sensor_get_values() {          
  currentTemp = sensors.getTempCByIndex(0);
  
  if (currentTemp != lastTemp && currentTemp < 80 && currentTemp > -127) {
    drawDecimal(COL_AQUARIUM_TEMP, ROW_AQUARIUM, currentTemp);
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();    
    char jsonStr[255];
    root["temperature"] = currentTemp;
    root.printTo(jsonStr);
    client.publish("display/kitchen/temperature", jsonStr);

    lastTemp = currentTemp;
    mqttPublishTemp = millis();  
  }
}

void sensor_ask_values() {
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures(); 
}
