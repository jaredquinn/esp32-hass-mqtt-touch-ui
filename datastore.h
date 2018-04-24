#ifndef DATASTORE_H
#define DATASTORE_H

#include "Arduino.h"
#include "Adafruit_ILI9341.h"


#define DATASTORE_TYPE_FLOAT 16
#define DATASTORE_TYPE_HUMIDITY 17
#define DATASTORE_TYPE_TEMP 18

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
    void update(float f);
    void update(int i);
    void update(bool b);
    void update(String s);
    void update(char *s);

    void updateFriendlyName(char *friendlyname);
    
    void resetChange();
    long lastChange();

    /*    String* value();
        bool value();
    */
    int getIntValue();
    float getFloatValue();
    bool getBoolValue();
    String getStringValue();
    char * getCharValue();

  private:
    void _commonUpdate();

    char  _friendly[255];
    char  _name[255];

    float _floatValue;
    int   _intValue;
    bool  _boolValue;
    char _charValue[255];

    float _floatLast;
    int   _intLast;
    bool  _boolLast;
    char _charLast[255];

    String  _strValue;
    String  _strLast;

    int   _dataType;
    long  _lastUpdated;
    bool  _hasChanged;
};






#endif;
