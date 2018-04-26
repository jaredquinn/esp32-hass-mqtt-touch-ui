
#include "dataset.h"
#include "dataingest.h"


DataSet::DataSet() {
  _count = 0;
};

void DataSet::addIngester(DataIngest& ingester) {
  _ingesters[_count] = &ingester;
  _count++;
};

void DataSet::subscribe(PubSubClient* mqtt) {
  char tmp[255];
  for(int c = 0; c < _count; c++){   
      sprintf(tmp, "%s/#", _ingesters[c]->mqttTopic);
      (*mqtt).subscribe(tmp);
  }
}

int DataSet::process(UI* ui, char *topic, char *payload, char *basepart) {
  int v = 0;
  
  for(int c = 0; c < _count; c++){
    v = v + _ingesters[c]->processMQTT( ui, topic, payload);
  }
  return v;
  
};


