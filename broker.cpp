//
// Created by gabe0 on 4/16/2024.
//

#include <pthread.h>
#include <thread>
#include <chrono>
#include <semaphore.h>
#include <unistd.h>
#include "log.h"
#include "broker.h"

#define	NSPERMS		1000000
#define MSPERSEC	1000

using namespace std;

void *producer(void *ptr){
    Producer *producer = static_cast<Producer*>(ptr);
    struct timespec	SleepTime;
    Broker *broker = producer->broker;


    while (true){

        int type = producer->type;
        int DelayMS = broker->productionTimes[type];
        SleepTime.tv_sec = DelayMS / MSPERSEC;
        SleepTime.tv_nsec = (DelayMS % MSPERSEC) * NSPERMS;

        // Simulate production of request
        nanosleep(&SleepTime, NULL);

        // If we are producing a sandwich, we have to wait for there to be space for it because of the pizza promotion
        // We also have to wait for there to be space in buffer
        if(producer->type == Sandwich){
            sem_wait(&broker->availableSandwich); // Decrement semaphore/block if needed
            sem_wait(&broker->availableSlots);
        }
        else{
            sem_wait(&broker->availableSlots);
        }

        // Lock buffer so producer can access it safely
        pthread_mutex_lock(&broker->bufferMutex);
        if(broker->requestIndex < broker->numRequests){ // Make sure we actually need to produce something
            if(producer->type == Pizza){
                broker->pizzaProduced += 1; // Increment stats for logging
            }
            else{
                broker->sandwichProduced += 1;
            }

            broker->boundedBuffer.push(producer->type); // Push request onto the buffer
            broker->requestIndex += 1;
            broker->requestsPerType[producer->type] += 1;
            RequestAdded added; // Prepare for logging
            added.type = producer->type;
            added.inBrokerQueue = broker->requestsPerType;
            unsigned int produced[RequestTypeN];
            produced[0] = broker->pizzaProduced;
            produced[1] = broker->sandwichProduced;
            added.produced = produced;
            log_added_request(added);
        }

        if(broker->requestIndex == broker->numRequests){ // We don't need to produce anymore
            if(broker->boundedBuffer.empty()){ // If there's nothing in our buffer we just unlock
                pthread_mutex_unlock(&broker->bufferMutex);
            }
            else{ // If there's something in our buffer we also have to post that
                pthread_mutex_unlock(&broker->bufferMutex);
                sem_post(&broker->unconsumed);
            }
            break;
        }
        // If more needs to be produced we can unlock the buffer and signal
        pthread_mutex_unlock(&broker->bufferMutex);
        sem_post(&broker->unconsumed);
    }
    return nullptr;
}