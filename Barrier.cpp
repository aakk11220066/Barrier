//
// Created by Akiva on 2020-06-01.
//

//barrier.cpp
#include "Barrier.h"

#define DEBUG_PRINT(errmsg) /*std::cerr << errmsg << std::endl*/ //DEBUG

void Barrier::wait() {
    barrierAccessLock.lock();
	
	//check if permitted access to Barrier
	if (!isOpen()) {
		DEBUG_PRINT("Waiting for barrier to reopen");
		waitingQueue.cond_wait();
	}
	
    if (!isFull()) { //sort thread by type - sleeper type thread or waker type thread
        //if not full yet - go to sleep until fills up
		unsigned int selectedBed = nextEmptyBed++;
		DEBUG_PRINT("Going to sleep on bed number "<<selectedBed);
        cond_beds[selectedBed].cond_wait(); //unlocks lock
        
        //upon waking up, 
        nextEmptyBed--;
        
        //check if last one to exit and if so - reset
        if (nextEmptyBed == 0) reset();
    } else {
		//close barrier until emptied
		_isOpen = false;
        //signal everyone to wake up
        for (ConditionVariable &cond_bed : cond_beds) {
			DEBUG_PRINT("Signalling");
            cond_bed.cond_signal();
        }
    }
    barrierAccessLock.unlock();
}

Barrier::Barrier(unsigned int num_of_threads) :
        numOfThreads(num_of_threads),
        cond_beds(std::vector<ConditionVariable>(num_of_threads - 1)) {

    for (ConditionVariable &cond_bed : cond_beds) cond_bed.setLock(&barrierAccessLock);
    waitingQueue.setLock(&barrierAccessLock);
}

bool Barrier::isFull() {
    if (!barrierAccessLock.isLocked()) throw LockNotAcquired();
    return nextEmptyBed == (numOfThreads-1);
}

bool Barrier::isOpen() {
	if (!barrierAccessLock.isLocked()) throw LockNotAcquired();
    return _isOpen;
}

void Barrier::reset() {
	DEBUG_PRINT("Resetting, currently nextEmptyBed is "<<nextEmptyBed);
    _isOpen = true;
    //wake everyone in waitingQueue 
    waitingQueue.broadcast();
}


//bed.cpp
#include <stdio.h>
#include <cassert>


#define DEBUG_PRINT(errmsg) /*std::cerr << errmsg << std::endl*/ //DEBUG

Bed::Bed() {
    const int SHARED_WITH_THREADS = 0;
    if (sem_init(&bed, SHARED_WITH_THREADS, 0)) perror("Bed ctor");
}

Bed::~Bed() {
    if (sem_destroy(&bed)) perror("Bed dtor");
}

void Bed::registerSleeper() {
    //if (bedPrepared) throw BedAlreadyOccupied();
    //bedPrepared = true;
	sleepers++;
}

bool Bed::sleep() {
    //if (!bedPrepared) return false;
    if (sem_wait(&bed)) perror("Bed::sleep");
    return true;
}

void Bed::wake() {
	sleepers--;
    //if (!bedPrepared) throw BedUnoccupied();
    //bedPrepared = false;

    //wake whoever is in the bed
    if (sem_post(&bed)) perror("Bed::wake sem_post");

    //check if woke someone or if bed was unoccupied
    int semaphoreCount;
    if (sem_getvalue(&bed, &semaphoreCount)) perror("Bed::wake sem_getvalue");
    //assert(semaphoreCount >= 0);
    
    //DEBUG_PRINT("Thread with id "<<gettid()<<" posted to bed, leaving semaphoreCount = "<<semaphoreCount);
    

    //if bed was empty, decrement it back to 0
    /*if (semaphoreCount > 0) {
        assert(semaphoreCount == 1);
        if (sem_wait(&bed)) perror("Bed::wake sem_wait");
    }*/ //commented out to preserve bed wake status if sleeper about to go to sleep
}

int Bed::getSleepersCount(){
    /*int semaphoreCount;
    if (sem_getvalue(&bed, &semaphoreCount)) perror("Bed::wake sem_getvalue");
    DEBUG_PRINT("getSleepersCount returning "<<semaphoreCount);
    return semaphoreCount;*/
    return sleepers;
}

void Bed::awakenAll(){
	DEBUG_PRINT("Awakening all sleepers, with getSleepersCount() = "<<getSleepersCount());
	for (int waiters = getSleepersCount(); waiters>0; waiters--) {
		wake();
		DEBUG_PRINT("Awakening sleeper, getSleepersCount() = "<<getSleepersCount());
	}
}


//ConditionVariable.cpp

ConditionVariable::ConditionVariable(Mutex *lock) : lock(lock) {}

void ConditionVariable::cond_wait() {
    if (!(lock->isLocked())) throw LockNotAcquired(); //didn't follow usage instructions
	
	//DEBUG_PRINT("Thread with id " << gettid() << " waiting...");
	
    bed.registerSleeper();
    //_isWaiting = true; //log that a process is sleeping here
    lock->unlock();
    bed.sleep(); //go to sleep unless signaled in the meanwhile to awaken

    //after reawakening, reacquire lock
    lock->lock();
    //DEBUG_PRINT("Thread with id " << gettid() << " awoke!");
}

void ConditionVariable::cond_signal() {
    if (!(lock->isLocked())) throw LockNotAcquired();
    //DEBUG_PRINT("Thread with id " << gettid() << " signalling");
    //_isWaiting = false;
    bed.wake();
}

void ConditionVariable::broadcast(){
	//assert(lock->isLocked());
	DEBUG_PRINT("Broadcasting");
	bed.awakenAll();
}

bool ConditionVariable::isWaiting() const {
    return _isWaiting;
}

void ConditionVariable::setLock(Mutex *lock) {
    ConditionVariable::lock = lock;
}


//mutex.cpp
#include <stdio.h>

#define DEBUG_PRINT(errmsg) /*std::cerr << errmsg << std::endl*/ //DEBUG

Mutex::Mutex() {
    if (pthread_mutex_init(&mutex, NULL)) perror("Mutex::ctor");
}

Mutex::~Mutex() {
    if (pthread_mutex_destroy(&mutex)) perror("Mutex::ctor");
}

void Mutex::lock() {
    if (pthread_mutex_lock(&mutex)) perror("Mutex::lock");
    //DEBUG_PRINT("Thread with id " << gettid() << " acquired lock...");
    _isLocked = true;
}

void Mutex::unlock() {
    _isLocked = false;
    //DEBUG_PRINT("Thread with id " << gettid() << " released lock.");
    if (pthread_mutex_unlock(&mutex)) perror("Mutex::unlock");
}

bool Mutex::isLocked() {
    /*if (!pthread_mutex_trylock(&mutex)) {
		unlock();
		return false;
	};
	return true;*/ return true;//DEBUG
}
