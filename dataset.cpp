
#include "dataset.h"
#include "dataingest.h"

DataSet::DataSet() {
  _count = -1;
};

void DataSet::addIngester(DataIngest& ingester) {
  _count++;
  _ingesters[_count] = &ingester;
};

int DataSet::process(UI* ui, char *topic, char *payload, char *basepart) {
  int v = 0;
  
  for(int c = 0; c <= _count; c++){
    v = v + _ingesters[c]->processMQTT( ui, topic, payload);
  }
  return v;
  
};


