#ifndef SCHEDULERWORKER_H
#define SCHEDULERWORKER_H

#include <QObject>
#include <vector>
#include "Process.h" // Include your Process class header

class SchedulerWorker : public QObject {
    Q_OBJECT

public:
    SchedulerWorker(std::vector<Process>& processes, bool live);

public slots:
    void process();

signals:
    void updateGanttChart(int pid, int currentTime);
    void finished();

private:
    std::vector<Process>& processes;
    bool live;
};

#endif // SCHEDULERWORKER_H
