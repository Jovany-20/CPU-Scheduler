#include "../Header Files/Process.h"

// Constructor definitions
Process::Process(int pid, int burstTime, int priority, int arrivalTime)
    : pid(pid), burstTime(burstTime), priority(priority), arrivalTime(arrivalTime),
    startTime(-1), lastProcessedTime(-1), remainingTime(burstTime), completionTime(-1) {
}

Process::Process(int pid, int burstTime, int arrivalTime)
    : Process(pid, burstTime, 5, arrivalTime) {
}

// Accessor implementations
int Process::getPid() const { return pid; }
int Process::getBurstTime() const { return burstTime; }
int Process::getPriority() const { return priority; }
int Process::getArrivalTime() const { return arrivalTime; }
int Process::getStartTime() const { return startTime; }
int Process::getLastProcessedTime() const { return lastProcessedTime; }
int Process::getRemainingTime() const { return remainingTime; }
int Process::getCompletionTime() const { return completionTime; }

// Mutator implementations
void Process::setPid(int p) { pid = p; }
void Process::setBurstTime(int b) { burstTime = b; }
void Process::setPriority(int p) { priority = p; }
void Process::setArrivalTime(int a) { arrivalTime = a; }
void Process::setStartTime(int s) { startTime = s; }
void Process::setLastProcessedTime(int l) { lastProcessedTime = l; }
void Process::setRemainingTime(int r) { remainingTime = r; }
void Process::setCompletionTime(int c) { completionTime = c; }

// Utility functions
int Process::calcTurnaroundTime() const {
    return completionTime - arrivalTime;
}
int Process::calcWaitingTime() const {
    return completionTime - arrivalTime - burstTime;
}
void Process::decrement(int quantum) {
    remainingTime = std::max(remainingTime - quantum, 0);
}
