//
// Created by User on 2020-06-01.
//

#include "Bed.h"
#include <stdio.h>
#include <cassert>

Bed::Bed() {
    const int SHARED_WITH_THREADS = 0;
    if (sem_init(&bed, SHARED_WITH_THREADS, 0)) perror("Bed ctor");
}

Bed::~Bed() {
    if (sem_destroy(&bed)) perror("Bed dtor");
}

void Bed::registerSleeper() {
    if (bedPrepared) throw BedAlreadyOccupied();
    bedPrepared = true;
}

bool Bed::sleep() {
    if (!bedPrepared) return false;
    if (sem_wait(&bed)) perror("Bed::sleep");
    return true;
}

void Bed::wake() {
    if (!bedPrepared) throw BedUnoccupied();
    bedPrepared = false;

    //wake whoever is in the bed
    if (sem_post(&bed)) perror("Bed::wake sem_post");

    //check if woke someone or if bed was unoccupied
    int semaphoreCount;
    if (sem_getvalue(&bed, &semaphoreCount)) perror("Bed::wake sem_getvalue");
    assert(semaphoreCount >= 0);

    //if bed was empty, decrement it back to 0
    /*if (semaphoreCount > 0) {
        assert(semaphoreCount == 1);
        if (sem_wait(&bed)) perror("Bed::wake sem_wait");
    }*/ //commented out to preserve bed wake status if sleeper about to go to sleep
}
