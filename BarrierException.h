//
// Created by Akiva on 2020-06-01.
//

#ifndef BARRIER_BARRIEREXCEPTIONS_H
#define BARRIER_BARRIEREXCEPTIONS_H


#include <exception>

class BarrierException : public std::exception {
};

class BedAlreadyOccupied : BarrierException {
};

class BedUnoccupied : BarrierException {
};

class LockNotAcquired : BarrierException {
};

#endif //BARRIER_BARRIEREXCEPTIONS_H
