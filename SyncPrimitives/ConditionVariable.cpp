//
// Created by Akiva on 2020-06-01.
//

#include "ConditionVariable.h"

ConditionVariable::ConditionVariable(Mutex *lock) : lock(lock) {}

void ConditionVariable::cond_wait() {
    if (!(lock->isLocked())) throw LockNotAcquired(); //didn't follow usage instructions

    bed.registerSleeper();
    lock->unlock();
    _isWaiting = true; //log that a process is sleeping here
    bed.sleep(); //go to sleep unless signaled in the meanwhile to awaken

    //after reawakening, reacquire lock
    lock->lock();
}

void ConditionVariable::cond_signal() {
    if (!(lock->isLocked())) throw LockNotAcquired();
    _isWaiting = false;
    bed.wake();
}

bool ConditionVariable::isWaiting() const {
    return _isWaiting;
}

void ConditionVariable::setLock(Mutex *lock) {
    ConditionVariable::lock = lock;
}

