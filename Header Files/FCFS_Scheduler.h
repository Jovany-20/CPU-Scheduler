#ifndef FCFS_SCHEDULER_H
#define FCFS_SCHEDULER_H
#include "headers.h"
#include "Process.h"

class FCFS_Scheduler {
private:
    std::vector<Process>& processes;

    void printGanttChart(int pid, int currentTime);

public:
    // Constructor
    FCFS_Scheduler(std::vector<Process>& processes);

    // Scheduling function
    void schedule(bool& live);
};





#endif // FCFS_SCHEDULER_H
