//
// Created by Akiva on 2020-06-01.
//

#ifndef BARRIER_CONDITIONVARIABLE_H
#define BARRIER_CONDITIONVARIABLE_H

#include "Mutex.h"
#include "Bed.h"

class ConditionVariable {
private:
    Mutex &lock;
    bool _isWaiting = false;
    Bed bed;

public:
    explicit ConditionVariable(Mutex &lock); //ctor

    //DESCRIPTION: unlocks lock and goes to sleep *atomically*, then
    //  (separately) reacquires lock when waking up
    //USAGE: use after acquiring lock
    //THROWS: LockNotAcquired
    //SHOULDN'T THROW: BedAlreadyOccupied
    void cond_wait();

    //DESCRIPTION: wakes sleeping thread (if exists)
    //USAGE: use after acquiring lock
    //THROWS: LockNotAcquired
    //SHOULDN'T THROW: BedUnoccupied
    void cond_signal();

    //returns if there is currently a thread sleeping on this ConditionVariable
    bool isWaiting() const;
};


#endif //BARRIER_CONDITIONVARIABLE_H
