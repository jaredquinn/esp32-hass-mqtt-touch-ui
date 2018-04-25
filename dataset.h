#ifndef DATASET_H
#define DATASET_H

#include "dataingest.h"
#include <PubSubClient.h>

class DataSet
{
  public:
    DataSet();
    void addIngester(DataIngest& ingester);
    int process(UI* ui, char *topic, char *payload, char *basepart);
    void subscribe(PubSubClient* mqtt);

  private:
    int _count;
    
    DataIngest *_ingesters[32];
};


#endif

