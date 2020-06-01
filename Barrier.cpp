//
// Created by Akiva on 2020-06-01.
//

#include "Barrier.h"

void Barrier::wait() {
    barrierAccessLock.lock();

    if (!isFull()) { //sort thread by type - sleeper type thread or waker type thread
        //if not full yet - go to sleep until fills up

        cond_beds[nextEmptyBed++].cond_wait(); //unlocks lock
    } else {
        //signal everyone to wake up
        for (ConditionVariable &cond_bed : cond_beds) {
            cond_bed.cond_signal();
        }
        reset();
    }
    barrierAccessLock.unlock();
}

Barrier::Barrier(unsigned int num_of_threads) :
        numOfThreads(num_of_threads),
        cond_beds(std::vector<ConditionVariable>(numOfThreads - 1, ConditionVariable(barrierAccessLock))) {}


bool Barrier::isFull() {
    if (!barrierAccessLock.isLocked()) throw LockNotAcquired();
    return nextEmptyBed == numOfThreads - 1;
}

void Barrier::reset() {
    nextEmptyBed = 0; //already is atomic
}

