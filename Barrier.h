//
// Created by Akiva on 2020-06-01.
//

#ifndef BARRIER_H_
#define BARRIER_H_

#include "SyncPrimitives/ConditionVariable.h"
#include <vector>
#include <semaphore.h>

class Barrier {
private:
    const unsigned int numOfThreads;
    unsigned int nextEmptyBed = 0;
    Mutex barrierAccessLock = Mutex();
    std::vector<ConditionVariable> cond_beds; //vector of (numOfThreads - 1) ConditionVariables

public:
    explicit Barrier(unsigned int num_of_threads); //not thread-safe (doesn't need to be)
    ~Barrier() = default;

    //DESCRIPTION: returns if barrier is full and next wait will release all waiting threads
    //USAGE: after acquiring barrierAccessLock
    //THROWS: LockNotAcquired
    bool isFull();

    //DESCRIPTION: resets Barrier so it is ready for reuse.
    //IMPLEMENTATION: zeroes nextEmptyBed
    void reset();

    //DESCRIPTION:
    //SHOULDN'T THROW: LockNotAcquired, BedAlreadyOccupied, BedUnoccupied
    void wait();
};


#endif // BARRIER_H_