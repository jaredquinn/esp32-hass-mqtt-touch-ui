/**
   datastore.cpp
   Data Store Manager
   
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
#include "datastore.h"


DataStore::DataStore(char *name, int dataType, char *fmt)
{
  strcpy(_name, name);
  strcpy(_fmtString, fmt);
  _fmtRender = true;
  dataType = dataType;
  hasChanged = false;
  lastUpdated = 0;
};

DataStore::DataStore(char *name, int dataType)
{
  strcpy(_name, name);
  dataType = dataType;
  hasChanged = false;
  lastUpdated = 0;
};

void DataStore::_commonUpdate() {
  lastUpdated = millis();
  hasChanged = true;
  char str[255];
  sprintf(str, "Updated %s", _name);  
};


void DataStore::resetChange() {
  hasChanged = false;  
};

void DataStore::update(float f) {
  if(f != _floatValue) {
    _floatLast = _floatValue;
    _floatValue = f;
    if(_fmtRender) sprintf(renderValue, _fmtString, _floatValue);
    _commonUpdate();
  }
};

float DataStore::getFloatValue() {
  return _floatValue;
};

void DataStore::update(bool b) {
  if(b != _boolValue) {
    _boolLast = _boolValue;
    _boolValue = b;
    if(_fmtRender) sprintf(renderValue, _fmtString, _boolValue ? "true" : "false");
    _commonUpdate();
  }
};

void DataStore::updateFriendlyName(char *friendlyname) {
  if(strcmp(_friendly, friendlyname) != 0) {
    strcpy(_friendly, friendlyname);
    _commonUpdate();
  }
}

bool DataStore::getBoolValue() {
  return _boolValue;
};

void DataStore::update(int i) {
  if(_intValue != i) {
    _intLast = _intValue;
    _intValue = i;
    if(_fmtRender) sprintf(renderValue, _fmtString, _intValue);
    _commonUpdate();
  }
};

int DataStore::getIntValue() {
  return _intValue;
};

void DataStore::update(String s) {
  if(s != _strValue) {
    _strLast = _strValue;
    _strValue = s;
    if(_fmtRender) sprintf(renderValue, _fmtString, s.c_str());
    _commonUpdate();
  }
};

String DataStore::getStringValue() {
  return _strValue;
};


void DataStore::update(char *s) {
  if(strcmp(s, _charValue) != 0) {
    strcpy(_charLast, _charValue);
    strcpy(_charValue, s);
    if(_fmtRender) sprintf(renderValue, _fmtString, s);
    _commonUpdate();
  }
};

char * DataStore::getCharValue() {
  return _charValue;
};


long DataStore::lastChange() {
  return lastUpdated;
};




