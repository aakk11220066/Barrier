//
// Created by Akiva on 2020-06-01.
//

#ifndef BARRIER_BED_H
#define BARRIER_BED_H

#include <semaphore.h>
#include "../BarrierException.h"

//Not thread safe
class Bed {
private:
    sem_t bed;
    bool bedPrepared = false;

public:
    Bed(); //initializes bed with 0
    ~Bed(); //destroys bed

    //DESCRIPTION: start listening for signal but don't go to sleep yet
    //THROWS: BedAlreadyOccupied
    void registerSleeper();

    //DESCRIPTION: puts thread to sleep until awakened by another thread by call to wake()
    //USAGE: use after call to registerSleeper() or else will stay awake
    //RETURN VALUE: true if went to sleep and then woken up, false if bed was unregistered
    bool sleep();

    //DESCRIPTION: wake thread sleeping on this bed (if exists) and unregister bed (either way)
    //USAGE: use after sleeper is registered
    //THROWS: BedUnoccupied
    void wake();
};

#endif //BARRIER_BED_H
