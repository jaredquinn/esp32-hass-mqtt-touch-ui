
#ifndef DATAINGEST_H
#define DATAINGEST_H

#include "datastore.h"
#include "UI.h"

#define DATAINGEST_TYPE_STATE_ONOFF 127
#define DATAINGEST_TYPE_RGB_COLOR 128

#define DATAINGEST_TYPE_SWITCH 129
#define DATAINGEST_TYPE_LIGHT 130


#define DATAINGEST_TYPE_STATE_FLOAT 131
#define DATAINGEST_TYPE_STATE_INT 4

#define DATAINGEST_TYPE_STATE_CHAR 132

class DataIngest
{
  public:
    DataIngest(char* mqtt, int sourceType, DataStore &ds);
    bool acceptTopic(UI* ui, char *topic);
    int processMQTT(UI* ui, char* topic, char* payload);
  private:
    int  _processStateChar(char* payload);
    int  _processStateOnOff(char* payload);
    int  _processRGBColour(char *payload);
    int  _processStateFloat(char* payload);
    DataStore* _ds;
    char _mqttTopic[255];
    int _sourceType;

};

#endif

