
#ifndef SJFpre_H
#define SJFpre_H

#include "headers.h"
#include "Process.h"
#include "ComparePriority.h"  // ensure the correct case

class SJFPreemptiveScheduler {
public:
    SJFPreemptiveScheduler(std::vector<Process>& processes);

    int getTim() const;
    void schedule(bool& live);

private:
    std::vector<Process>& processes;
    int tim;

    void incrementTim();
    void printGanttChart(int pid, int currentTime);
};

#endif // SJFpre_H
