

#include <iostream>
#include <string>
#include <unistd.h>
#include "broker.h"
#include "log.h"

using namespace std;
int main(int argc, char **argv) {


    // maxRequests is bounded buffer size, maxSandwiches is for pizza promotion
    int maxRequests = 20;
    int maxSandwiches = 8;
    Broker broker;

    // Parsing command line args
    int o = 0;
    while ((o = getopt(argc, argv, "n:a:b:p:s:")) != -1) {
        switch (o){
            case 'n':{
                broker.numRequests = stoi(optarg);
                break;
            }
            case 'a':{
                broker.consumptionTimes[DeliveryServiceA] = stoi(optarg);
                break;
            }
            case 'b':{
                broker.consumptionTimes[DeliveryServiceB] = stoi(optarg);
                break;
            }
            case 'p':{
                broker.productionTimes[Pizza] = stoi(optarg);
                break;
            }
            case 's':{
                broker.productionTimes[Sandwich] = stoi(optarg);
                break;
            }
        }

    }

    // All items start with 0 requests...
    for (int i = 0; i < RequestTypeN; ++i) {
        broker.requestsPerType[i] = 0;
    }

    broker.requestIndex = 0;

    // Producers and consumers must have access to broker for shared data
    Producer pizzaProducer;
    pizzaProducer.broker = &broker;
    pizzaProducer.type = Pizza;

    Producer sandwichProducer;
    sandwichProducer.broker = &broker;
    sandwichProducer.type = Sandwich;

    Consumer serviceAConsumer;
    serviceAConsumer.broker = &broker;
    serviceAConsumer.consumerType = DeliveryServiceA;

    Consumer serviceBConsumer;
    serviceBConsumer.broker = &broker;
    serviceBConsumer.consumerType = DeliveryServiceB;


    // Initialize semaphores held in broker
    // Since they are unnamed we set pshared to 0.
    sem_init(&broker.availableSlots, 0, maxRequests); // Value set to maxRequests to allow buffer to be filled
    sem_init(&broker.unconsumed,0,0); // Starts at 0 since there are no unconsumed requests at start
    sem_init(&broker.availableSandwich,0, maxSandwiches); // maxSandwiches to allow maximum amt of sandwiches
    sem_init(&broker.barrier, 0, 0);

    // Initialize mutex for exclusive access to buffer
    pthread_mutex_init(&broker.bufferMutex, NULL);

    // Start both producer and consumer threads
    pthread_t pizzaProdThread;
    pthread_create(&pizzaProdThread, NULL, &producer, &pizzaProducer);
    pthread_t sandwichProdThread;
    pthread_create(&sandwichProdThread, NULL, &producer, &sandwichProducer);
    pthread_t AconsumeThread;
    pthread_create(&AconsumeThread, NULL, &consumer, &serviceAConsumer);
    pthread_t BconsumeThread;
    pthread_create(&BconsumeThread, NULL, &consumer, &serviceBConsumer);

    // Now wait until all threads have finished
    sem_wait(&broker.barrier);

    // Prepare logging
    unsigned int produced[RequestTypeN];
    produced[0] = broker.pizzaProduced;
    produced[1] = broker.sandwichProduced;

    unsigned int *consumed[RequestTypeN];
    consumed[0]= broker.consumedA;
    consumed[1]= broker.consumedB;

    log_production_history(produced, consumed);

    exit(0);
}

