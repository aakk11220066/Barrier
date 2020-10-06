//
// Created by Akiva on 2020-06-01.
//

#ifndef BARRIER_H_
#define BARRIER_H_


//"Mutex.h"

#include <pthread.h>

class Mutex {
private:
    pthread_mutex_t mutex;
    bool _isLocked = false;

public:
    Mutex();

    ~Mutex(); //NOTE: destroys lock

    void lock();

    void unlock();

    bool isLocked();
};


//"Bed.h"

#include <semaphore.h>
//"../BarrierException.h"

#include <exception>

class BarrierException : public std::exception {
};

class BedAlreadyOccupied : BarrierException {
};

class BedUnoccupied : BarrierException {
};

class LockNotAcquired : BarrierException {
};


//Not thread safe
class Bed {
private:
	unsigned int sleepers = 0;
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
    
    int getSleepersCount();
    
    void awakenAll();
};


//ConditionVariable.h
class ConditionVariable {
private:
    Mutex *lock = nullptr;

public:
    void setLock(Mutex *lock);

private:
    bool _isWaiting = false;
    Bed bed = Bed();

public:
    explicit ConditionVariable(Mutex *lock); //ctor
    ConditionVariable() = default;

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
    
    //wake everyone
    void broadcast();
};



#include <vector>
#include <semaphore.h>
#include <stdbool.h>
#include <memory>

class Barrier {
private:
    ConditionVariable waitingQueue = ConditionVariable(); //holds all processes waiting for queue to empty out prior to reuse
    const unsigned int numOfThreads;
    unsigned int nextEmptyBed = 0;
    Mutex barrierAccessLock = Mutex();
    std::vector<ConditionVariable> cond_beds; //vector of (numOfThreads - 1) ConditionVariables
    bool _isOpen = true;

public:
    explicit Barrier(unsigned int num_of_threads); //not thread-safe (doesn't need to be)
    ~Barrier() = default;

    //DESCRIPTION: returns if barrier is full and next wait will release all waiting threads
    //USAGE: after acquiring barrierAccessLock
    //THROWS: LockNotAcquired
    bool isFull();
    
    //DESCRIPTION: returns if barrier is empty and next allows threads to enter barrier
    //THROWS: LockNotAcquired
    bool isOpen();

    //DESCRIPTION: resets Barrier so it is ready for reuse.
    //IMPLEMENTATION: zeroes nextEmptyBed
    void reset();

    //DESCRIPTION:
    //SHOULDN'T THROW: LockNotAcquired, BedAlreadyOccupied, BedUnoccupied
    void wait();

    unsigned int waitingThreads() {	
	barrierAccessLock.lock();
 	unsigned int result = nextEmptyBed; 
    	barrierAccessLock.unlock();
	return result;
    }
};


#endif // BARRIER_H_
