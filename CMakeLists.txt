cmake_minimum_required(VERSION 3.13)
project(Barrier)

set(CMAKE_CXX_STANDARD 11)

add_executable(Barrier main.cpp SyncPrimitives/Mutex.cpp SyncPrimitives/Mutex.h SyncPrimitives/Bed.cpp SyncPrimitives/Bed.h BarrierException.cpp BarrierException.h SyncPrimitives/ConditionVariable.cpp SyncPrimitives/ConditionVariable.h Barrier.cpp Barrier.h)