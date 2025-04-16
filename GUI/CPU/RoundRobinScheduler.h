#ifndef ROUNDROBIN_SCHEDULER_H
#define ROUNDROBIN_SCHEDULER_H

#include <vector>
#include <queue>
#include "Process.h"
#include <QObject>
#include <QMutex>
#include <QWaitCondition>

class RoundRobinScheduler : public QObject {
    Q_OBJECT

public:
    RoundRobinScheduler(int quantum, std::vector<Process>& processes);
    void schedule(bool& live);
    void setPaused(bool pause);
    int getTim() const;

signals:
    void updateGanttChart(int pid, int currentTime);
    void statsCalculated(double avgWaitingTime, double avgTurnaroundTime);
    void finished();

private:
    std::vector<Process>& processes;
    int timeQuantum;
    int tim;
    bool paused;
    QMutex mutex;
    QWaitCondition pauseCond;
    void printGanttChart(int pid, int currentTime);
};

#endif // ROUNDROBIN_SCHEDULER_H
