/* Ingest Manager */
#include "dataingest.h"
#include "datastore.h"

DataIngest::DataIngest(char* mqtt, int sourceType, DataStore& ds) {
  sprintf(mqttTopic, "%s/%s", "homeassistant", mqtt);
  _sourceType = sourceType;
  _ds = &ds;
};

bool DataIngest::acceptTopic(UI* ui, char *topic) {
  char str[255];
  
  if(strstr(topic, mqttTopic) != NULL) return true;
  return false;
};

int DataIngest::_processStateOnOff(char* payload) {
  if(strcmp(payload, "on")==0) {
    _ds->update(true);  
    return 1;
  }
  if(strcmp(payload, "off")==0) {
    _ds->update(false);
    return 1;
  }
  return 0;
};


int DataIngest::_processStateFloat(char *payload) {
  _ds->update((float) atof(payload));  
  return 1;
};


int DataIngest::_processRGBColour(char *payload) {
  return 0;
};

int DataIngest::_processStateChar(char *payload) {
  _ds->update(payload);
  return 1;
};

int DataIngest::processMQTT(UI* ui, char *topic, char *payload) {
  int updates = 0;
  
  if(acceptTopic(ui, topic)) {
    char str[255];

    if(strstr(topic, "/friendly_name") != NULL) {
      _ds->updateFriendlyName(payload);
    }
    if(_sourceType == DATAINGEST_TYPE_STATE_FLOAT) {
        if(strstr(topic, "/state") != NULL) {
        updates = updates + _processStateFloat(payload);
      }
    }
    if(_sourceType == DATAINGEST_TYPE_STATE_CHAR) {
      if(strstr(topic, "/state") != NULL) {
        updates = updates + _processStateChar(payload); 
      }
    }

    if(_sourceType == DATAINGEST_TYPE_LIGHT || DATAINGEST_TYPE_SWITCH) {
      if(strstr(topic, "/state") != NULL) {
        updates = updates + _processStateOnOff(payload);
      }
      if(strstr(topic, "/rgb_colour") != NULL) {
        updates = updates + _processRGBColour(payload);
      }
    }
    
    if(_sourceType == DATAINGEST_TYPE_STATE_ONOFF) {
      updates = updates + _processStateOnOff(payload);
    }
  }
  return updates;
};


