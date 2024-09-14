//
// Created by gabe0 on 4/16/2024.
//

#include <iostream>
#include <pthread.h>
#include <thread>
#include <chrono>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include "log.h"
#include "consumer.h"
#include "broker.h"

#define	NSPERMS		1000000
#define MSPERSEC	1000

using namespace std;
void *consumer(void *ptr){
    Consumer* consumer = static_cast<Consumer*>(ptr);
    struct timespec	SleepTime;
    struct timespec minisleep;
    Broker *broker = consumer->broker;
    int type = consumer->consumerType;

    while (true){
        int DelayMS2 = broker->consumptionTimes[type];

        // Wait until there is a request to consume
        sem_wait(&broker->unconsumed);

        SleepTime.tv_sec = DelayMS2 / MSPERSEC;
        SleepTime.tv_nsec = (DelayMS2 % MSPERSEC) * NSPERMS;

        // We lock buffer so consumer can access it safely
        pthread_mutex_lock(&broker->bufferMutex);
        if(broker->boundedBuffer.empty()){ // If the buffer is empty then there's nothing to do

        }
        else{ // Otherwise we grab the oldest request in the queue and consume it

            Requests requestType = broker->boundedBuffer.front();
            if(consumer->consumerType == DeliveryServiceA){ // Prepare for logging
                if(requestType == Pizza){
                    broker->consumedA[Pizza] += 1;
                }
                else{
                    broker->consumedA[Sandwich] += 1;
                }
            }
            else{
                if(requestType == Pizza){
                    broker->consumedB[Pizza] += 1;
                }
                else{
                    broker->consumedB[Sandwich] += 1;
                }
            }

            broker->boundedBuffer.pop(); // Remove request from buffer
            broker->requestsPerType[requestType] -= 1;
            RequestRemoved removed;
            if(consumer->consumerType == DeliveryServiceA){
                removed.consumed = broker->consumedA;
            }
            else{
                removed.consumed = broker->consumedB;
            }
            removed.consumer = consumer->consumerType;
            removed.type = requestType;
            removed.inBrokerQueue = broker->requestsPerType;
            log_removed_request(removed);

            if(requestType == Sandwich){ // Signal if we've removed a sandwich, and also that there is a new slot for producers.
                sem_post(&broker->availableSandwich);
                sem_post(&broker->availableSlots);
            }
            else{
                sem_post(&broker->availableSlots);
            }

        }

        // If we have passed the maximum requests and there's no more in the buffer to consume then we are done!
        if(broker->requestIndex == broker->numRequests && broker->boundedBuffer.empty()){
            pthread_mutex_unlock(&broker->bufferMutex);
            break;
        }
        else{ // Otherwise we just unlock and keep looping.
            pthread_mutex_unlock(&broker->bufferMutex);
        }
        // Sleep to simulate consumption of request
        nanosleep(&SleepTime, NULL);
    }
    // We can lift the barrier since we are done consuming.
    sem_post(&broker->barrier);
    pthread_exit(NULL);
    return nullptr;
}