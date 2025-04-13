#ifndef NON_PREEMPTIVE_PRIORITY_SCHEDULER_H
#define NON_PREEMPTIVE_PRIORITY_SCHEDULER_H
#include "headers.h"
#include "Process.h"
#include "comparePriority.h"
#include <QObject>
#include "GanttChartWindow.h"

class NonPreemptivePriorityScheduler : public QObject {
    Q_OBJECT
public:
    NonPreemptivePriorityScheduler(std::vector<Process>& processes);

    // Accessor to get current time
    int getTim() const;
    double avgWaitingTime;
    double avgTurnaroundTime;
    // Scheduling function, live indicates if new processes can be added during execution
    void schedule(bool& live);
   // GanttChartWindow* ganttChartWindow;

signals:
    void statsCalculated(double avgWaitingTime, double avgTurnaroundTime);
    void updateGanttChart(int pid, int currentTime);
    void finished();
private:
    std::vector<Process>& processes;
    int tim;

    // Utility function to print the Gantt chart output.
    void printGanttChart(int pid, int currentTime);
};
#endif // NON_PREEMPTIVE_PRIORITY_SCHEDULER_H
