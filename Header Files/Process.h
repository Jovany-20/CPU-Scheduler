#ifndef PROCESS_H
#define PROCESS_H

#include <algorithm> // for std::max
#include <iostream>
using namespace std;
class Process {
public:
    int pid, burstTime, priority, arrivalTime, startTime, lastProcessedTime, remainingTime, completionTime;

    Process(int pid, int burstTime, int priority, int arrivalTime)
        : pid(pid), burstTime(burstTime), priority(priority), arrivalTime(arrivalTime),
        startTime(-1), lastProcessedTime(-1), remainingTime(burstTime), completionTime(-1) {
    }

    Process(int pid, int burstTime, int arrivalTime)
        : Process(pid, burstTime, 5, arrivalTime) {
    }

    int getPid() const { return pid; }
    int getBurstTime() const { return burstTime; }
    int getPriority() const { return priority; }
    int getArrivalTime() const { return arrivalTime; }
    int getStartTime() const { return startTime; }
    int getLastProcessedTime() const { return lastProcessedTime; }
    int getRemainingTime() const { return remainingTime; }
    int getCompletionTime() const { return completionTime; }

    void setPid(int p) { pid = p; }
    void setBurstTime(int b) { burstTime = b; }
    void setPriority(int p) { priority = p; }
    void setArrivalTime(int a) { arrivalTime = a; }
    void setStartTime(int s) { startTime = s; }
    void setLastProcessedTime(int l) { lastProcessedTime = l; }
    void setRemainingTime(int r) { remainingTime = r; }
    void setCompletionTime(int c) { completionTime = c; }

    int calcTurnaroundTime() const { return completionTime - arrivalTime; }
    int calcWaitingTime() const { return completionTime - arrivalTime - burstTime; }

    void decrement(int quantum) {
        remainingTime = max(remainingTime - quantum, 0);
    }
};

#endif // PROCESS_H