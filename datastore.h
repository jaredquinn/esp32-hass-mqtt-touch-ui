/**
   datastore.h
   Data Store Header File
   
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
    
    
    float _floatLast = 0;
    int   _intLast = 0;
    bool  _boolLast = 0;
    char _charLast[255];

    float _floatValue = 0;
    int   _intValue = 0;
    bool  _boolValue = false;
    char _charValue[255];
    String  _strValue;
    String  _strLast;

};


#endif;
