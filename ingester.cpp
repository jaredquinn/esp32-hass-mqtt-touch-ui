/**
   ingester.h
   MQTT Ingester Class
   
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

#include "ingester.h"

MQTT_Ingester::MQTT_Ingester(PubSubClient &psc) {
  mqtt = &psc;
  _count = -1;
};


void MQTT_Ingester::addIngester(char *mqttTopic, enum enumIngestType ingestType, DataStore &ds) {
  _count++;
  strcpy(topics[_count].topic, mqttTopic);
  topics[_count].ingestType = ingestType;
  topics[_count].ds = &ds;
};

void MQTT_Ingester::subscribe() {
  for(int c = 0; c < _count; c++) { 
      char tmp[255];
      sprintf(tmp, "%s/#", topics[c].topic);
      (*mqtt).subscribe(tmp);
      (*mqtt).publish("/display/kitchen/subscribed", tmp);
  }
}

int MQTT_Ingester::process(char *topic, char *payload) {
  int v = 0;
  
  for(int c = 0; c < _count; c++){
    if(strstr(topic, topics[c].topic) != NULL) {
      v = v + processPayload(c, topic, payload);
    }
  }
  return v;
};


int MQTT_Ingester::processPayload(int c, char *topic, char *payload) {

  char str[255];
  int updates = 0;

  if(strstr(topic, "/state") != NULL) {

    switch(topics[c].ingestType) {
      case enumIngestType::STATE_FLOAT:  topics[c].ds->update((float) atof(payload)); break;       
      case enumIngestType::STATE_CHAR:   topics[c].ds->update(payload); break;
      case enumIngestType::STATE_ONOFF:  topics[c].ds->update((strcmp(payload, "on") == 0)); break;    
    }
  }
    
  return updates;
};



