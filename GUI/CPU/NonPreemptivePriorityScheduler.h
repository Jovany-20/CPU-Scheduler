#ifndef NON_PREEMPTIVE_PRIORITY_SCHEDULER_H
#define NON_PREEMPTIVE_PRIORITY_SCHEDULER_H
#include "headers.h"
#include "Process.h"
#include "comparePriority.h"


class NonPreemptivePriorityScheduler {
public:
    NonPreemptivePriorityScheduler(std::vector<Process>& processes);

    // Accessor to get current time
    int getTim() const;

    // Scheduling function, live indicates if new processes can be added during execution
    void schedule(bool& live);

private:
    std::vector<Process>& processes;
    int tim;

    // Utility function to print the Gantt chart output.
    void printGanttChart(int pid, int currentTime);
};
#endif // NON_PREEMPTIVE_PRIORITY_SCHEDULER_H
