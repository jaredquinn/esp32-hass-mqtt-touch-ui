#ifndef DATASET_H
#define DATASET_H

#include "dataingest.h"

class DataSet
{
  public:
    DataSet();
    void addIngester(DataIngest& ingester);
    int process(UI* ui, char *topic, char *payload, char *basepart);
  private:
    int _count;
    
    DataIngest *_ingesters[32];
};


#endif

