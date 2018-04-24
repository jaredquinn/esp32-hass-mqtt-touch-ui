
TS_Point lastPoint;

int ts_minx = 351;
int ts_miny = 262;
int ts_maxx = 3777;
int ts_maxy = 3771;

int smallX = 0;
int smallY = 0;
int largeX = 0;
int largeY = 0;
int convertedX = 0;
int convertedY = 0;

void touch_setup() {
  myscreen.println("Launching User Interface");
  if (!ts.begin()) {
   Serial.println("Failed to Start Touch");
   myscreen.println("Failed to Start Touch");
  }
  ts.setRotation(2);
}

void checkTouch() {
  // TOP LEFT      Pressure = 743, x = 3524, y = 1082
  // TOP RIGHT    Pressure = 1514, x = 3524, y = 3457

  // BOTTOM LEFT  Pressure = 1926, x = 408, y = 587
  // BOTTOM_RIGHT  Pressure = 1418, x = 408, y = 3649

  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    
    if(p != lastPoint) {
      
      convertedX = map(p.x, ts_minx, ts_maxx, 0, myscreen.width());
      convertedY = map(p.y, ts_miny, ts_maxy, 0, myscreen.height());

      //myscreen.fillRect(convertedX, convertedY, 1, 1, ILI9341_GREEN);
      Serial.printf("Z %d X %d Y %d:  X %d>%d, Y %d>%d  (Converted X %d, Y %d) [min x%d y%d max x%d y%d] \n", p.z, p.x, p.y, convertedX, convertedY, ts_minx, ts_miny, ts_maxx, ts_maxy);

      if(screenOn == 0) {        
        client.publish("display/kitchen/setting/display", "on");
      } else {
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();    
        char jsonStr[255];
        root["x"] = convertedX;
        root["y"] = convertedY;
        root["z"] = p.z;
        root["time"] = millis();
        root.printTo(jsonStr);
        client.publish("display/kitchen/event/touch", jsonStr);
      }
      
      lastPoint = p;
      lastTouch = millis();
    }
  }
}

void ts_mqtt(char* topic, char pl[255]) {
  
  if (strcmp(topic, "display/kitchen/setting/ts_minx") == 0) {
    ts_minx = atoi(pl);
  }

  if (strcmp(topic, "display/kitchen/setting/ts_maxx") == 0) {
    ts_maxx = atoi(pl);
  }
  
  if (strcmp(topic, "display/kitchen/setting/ts_miny") == 0) {
    ts_miny = atoi(pl);
  }

  if (strcmp(topic, "display/kitchen/setting/ts_maxy") == 0) {
    ts_maxy = atoi(pl);
  }

}
