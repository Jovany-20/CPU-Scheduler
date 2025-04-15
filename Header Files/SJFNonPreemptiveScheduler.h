
#ifndef SJFnonpre_H
#define SJFnonpre_H
#include "headers.h"
#include "Process.h"
#include "comparePriority.h"


class SJFNonPreemptiveScheduler {
public:
    SJFNonPreemptiveScheduler(std::vector<Process>& processes);

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
#endif // SJFnonpre_H
