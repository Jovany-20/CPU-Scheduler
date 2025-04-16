#ifndef FCFS_SCHEDULER_H
#define FCFS_SCHEDULER_H

#include <vector>
#include <queue>
#include "Process.h"
#include <QObject>
#include <QMutex>
#include <QWaitCondition>

class FCFS_Scheduler : public QObject {
    Q_OBJECT

public:
    FCFS_Scheduler(std::vector<Process>& processes);
    void schedule(bool& live);
    void setPaused(bool pause);
    int getTim() const;

signals:
    void updateGanttChart(int pid, int currentTime);
    void statsCalculated(double avgWaitingTime, double avgTurnaroundTime);
    void finished();

private:
    std::vector<Process>& processes;
    int tim;
    bool paused;
    QMutex mutex;
    QWaitCondition pauseCond;
};

#endif // FCFS_SCHEDULER_H
