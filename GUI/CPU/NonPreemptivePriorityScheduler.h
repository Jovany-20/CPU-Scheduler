#ifndef NON_PREEMPTIVE_PRIORITY_SCHEDULER_H
#define NON_PREEMPTIVE_PRIORITY_SCHEDULER_H

#include "headers.h"
#include "Process.h"
#include "comparePriority.h"
#include <QObject>
#include "GanttChartWindow.h"
#include <QMutex>
#include <QWaitCondition>
#include <queue>
#include <vector>

class NonPreemptivePriorityScheduler : public QObject {
    Q_OBJECT
public:
    NonPreemptivePriorityScheduler(std::vector<Process>& processes);
    // Accessor for current time.
    int getTim() const;
    // Main scheduling function; 'live' indicates whether new processes might be added during execution.
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
    // Pause-control members:
    bool paused;                // true if scheduler is paused
    QMutex mutex;               // protects access to paused flag
    QWaitCondition pauseCond;   // used to block the thread until resumed

    // Utility function (optional)
    void printGanttChart(int pid, int currentTime);
};

#endif // NON_PREEMPTIVE_PRIORITY_SCHEDULER_H
