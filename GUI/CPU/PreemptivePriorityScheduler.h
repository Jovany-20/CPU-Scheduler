#ifndef PREEMPTIVE_PRIORITY_SCHEDULER_H
#define PREEMPTIVE_PRIORITY_SCHEDULER_H

#include "headers.h"
#include "Process.h"
#include "ComparePriority.h"  // ensure the correct case
#include <QObject>
#include "GanttChartWindow.h"
#include <QMutex>
#include <QWaitCondition>

class PreemptivePriorityScheduler : public QObject {
    Q_OBJECT
public:
    PreemptivePriorityScheduler(std::vector<Process>& processes);
    int getTim() const;
    void schedule(bool& live);
    double avgWaitingTime;
    double avgTurnaroundTime;

public slots:
    // Slot to pause/resume the scheduler.
    void setPaused(bool pause);

signals:
    void statsCalculated(double avgWaitingTime, double avgTurnaroundTime);
    void updateGanttChart(int pid, int currentTime);
    void finished();

private:
    std::vector<Process>& processes;
    int tim;
    bool paused;                // flag to indicate whether scheduling is paused
    QMutex mutex;               // mutex to synchronize access to 'paused'
    QWaitCondition pauseCond;   // wait condition used to pause the loop

    void incrementTim();
};

#endif // PREEMPTIVE_PRIORITY_SCHEDULER_H
