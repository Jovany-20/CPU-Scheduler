#ifndef ROUNDROBIN_SCHEDULER_H
#define ROUNDROBIN_SCHEDULER_H
#include "headers.h"
#include "Process.h"

class RoundRobinScheduler {
private:
    std::vector<Process>& processes;
    int timeQuantum;
    int tim;
    int itr;

    void printGanttChart(int pid, int currentTime);

public:
    // Constructor
    RoundRobinScheduler(int quantum, std::vector<Process>& processes);

    // Scheduling function
    void schedule(bool& live);
};

#endif // ROUNDROBIN_SCHEDULER_H
