#ifndef PROCESS_H
#define PROCESS_H

#include <algorithm> // for std::max
#include <iostream>
using namespace std;
class Process {
public:
    // Data members
    int pid;
    int burstTime;
    int priority;
    int arrivalTime;
    int startTime;
    int lastProcessedTime;
    int remainingTime;
    int completionTime;

    // Constructors
    Process(int pid, int burstTime, int priority, int arrivalTime);
    Process(int pid, int burstTime, int arrivalTime);

    // Accessor functions
    int getPid() const;
    int getBurstTime() const;
    int getPriority() const;
    int getArrivalTime() const;
    int getStartTime() const;
    int getLastProcessedTime() const;
    int getRemainingTime() const;
    int getCompletionTime() const;

    // Mutator functions
    void setPid(int p);
    void setBurstTime(int b);
    void setPriority(int p);
    void setArrivalTime(int a);
    void setStartTime(int s);
    void setLastProcessedTime(int l);
    void setRemainingTime(int r);
    void setCompletionTime(int c);

    // Utility functions
    int calcTurnaroundTime() const;
    int calcWaitingTime() const;
    void decrement(int quantum);
};

#endif // PROCESS_H