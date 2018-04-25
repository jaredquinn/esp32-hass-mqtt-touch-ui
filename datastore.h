#ifndef DATASTORE_H
#define DATASTORE_H

#include "Arduino.h"
#include "Adafruit_ILI9341.h"


#define DATASTORE_TYPE_FLOAT 16
#define DATASTORE_TYPE_BOOL 8
#define DATASTORE_TYPE_BULB 9
#define DATASTORE_TYPE_INT 4
#define DATASTORE_TYPE_DIGITS 5

#define DATASTORE_TYPE_STRING 2
#define DATASTORE_TYPE_CHAR 3

#define DATAWIDGET_BULB 256
#define DATAWIDGET_TEMP 257
#define DATAWIDGET_HUMIDITY 258

class DataStore
{
  public:
    DataStore(char *name, int dataType);
    DataStore(char *name, int dataType, char *fmt);

    void update(float f);
    void update(int i);
    void update(bool b);
    void update(String s);
    void update(char *s);

    void updateFriendlyName(char *friendlyname);
    
    void resetChange();
    long lastChange();

    char  _name[255];
    char  _friendly[255];

    char renderValue[32];

    int getIntValue();
    float getFloatValue();
    bool getBoolValue();
    String getStringValue();

    char  * getCharValue();

    long  lastUpdated = -1;
    bool  hasChanged = false;

    int   _dataType = 0;

    char  _fmtString[255];

    void _commonUpdate();

    bool _fmtRender = false;
    
    
    float _floatValue = 0;
    int   _intValue = 0;
    bool  _boolValue = 0;
    char _charValue[255];

    float _floatLast = 0;
    int   _intLast = 0;
    bool  _boolLast = 0;
    char _charLast[255];

    String  _strValue;
    String  _strLast;

};






#endif;
