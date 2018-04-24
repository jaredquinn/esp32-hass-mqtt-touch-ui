#include "datastore.h"


DataStore::DataStore(char *name, int dataType)
{
  strcpy(_name, name);
  _dataType = dataType;
  _hasChanged = false;
  _lastUpdated = 0;
};

void DataStore::_commonUpdate() {
  _lastUpdated = millis();
  _hasChanged = true;
  char str[255];
  sprintf(str, "Updated %s", _name);  
};


void DataStore::resetChange() {
  _hasChanged = false;  
};

void DataStore::update(float f) {
  if(f != _floatValue) {
    _floatLast = _floatValue;
    _floatValue = f;
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
  if(i != _intValue) {
    _intLast = _intValue;
    _intValue = i;
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
    _commonUpdate();
  }
};

char * DataStore::getCharValue() {
  return _charValue;
};

long DataStore::lastChange() {
  return _lastUpdated;
};



