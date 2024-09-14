//
// Created by gabe0 on 4/15/2024.
//

#ifndef UNTITLED13_BROKER_H
#define UNTITLED13_BROKER_H

#include <string>
#include <queue>
#include "consumer.h"
#include <pthread.h>
#include <semaphore.h>
#include "fooddelivery.h"
#include "log.h"

using namespace std;

class Broker{
public:

    // Defaults
    unsigned int numRequests = 100;
    unsigned int serviceAsleep = 0;
    unsigned int serviceBsleep = 0;
    unsigned int pizzaProductionTime = 0;
    unsigned int sandwichProductionTime = 0;
    unsigned int consumedA[RequestTypeN] = {0};
    unsigned int consumedB[RequestTypeN] = {0};
    unsigned int productionTimes[RequestTypeN] = {0};
    unsigned int consumptionTimes[ConsumerTypeN] = {0};


    // Bounded buffer which holds requests
    queue<RequestType> boundedBuffer = queue<RequestType>();

    // Semaphores
    sem_t availableSlots; // Wait for there to be an available slot to add request
    sem_t unconsumed; // Wait for an unconsumed request to enter the queue
    sem_t availableSandwich; // There is a maximum number of sandwiches
    sem_t barrier; // Wait for all consumer threads to finish


    pthread_mutex_t bufferMutex;
    unsigned int requestIndex; // What number request are we on?

    unsigned int pizzaProduced = 0;
    unsigned int sandwichProduced = 0;

    unsigned int requestsPerType[RequestTypeN];
};

class Producer{
public:
    Broker* broker;
    RequestType type;

};

void *producer(void *ptr);

#endif //UNTITLED13_BROKER_H
