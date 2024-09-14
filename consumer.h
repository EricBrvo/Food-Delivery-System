//
// Created by gabe0 on 4/16/2024.
//

#ifndef UNTITLED13_CONSUMER_H
#define UNTITLED13_CONSUMER_H



#include "broker.h"
#include "fooddelivery.h"

class Broker;
class Consumer {
public:
    Broker* broker;
    ConsumerType consumerType;
};
void *consumer(void *ptr);

#endif //UNTITLED13_CONSUMER_H
