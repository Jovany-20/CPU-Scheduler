#ifndef PREEMPTIVE_PRIORITY_SCHEDULER_H
#define PREEMPTIVE_PRIORITY_SCHEDULER_H

#include "headers.h"
#include "Process.h"
#include "ComparePriority.h"  // ensure the correct case
#include <QObject>
#include "GanttChartWindow.h"
class PreemptivePriorityScheduler : public QObject {
    Q_OBJECT
public:
    PreemptivePriorityScheduler(std::vector<Process>& processes);
    //GanttChartWindow* ganttChartWindow;
    int getTim() const;
    void schedule(bool& live);
    double avgWaitingTime;
    double avgTurnaroundTime;
signals:
    void statsCalculated(double avgWaitingTime, double avgTurnaroundTime);
    void updateGanttChart(int pid, int currentTime);
    void finished();
private:
    std::vector<Process>& processes;
    int tim;

    void incrementTim();
    void printGanttChart(int pid, int currentTime);
};

#endif // PREEMPTIVE_PRIORITY_SCHEDULER_H
