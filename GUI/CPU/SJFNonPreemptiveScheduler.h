#ifndef SJFnonpre_H
#define SJFnonpre_H
#include "headers.h"
#include "Process.h"
#include "comparePriority.h"
#include <QObject>
#include "GanttChartWindow.h"
#include <QMutex>
#include <QWaitCondition>
#include <queue>
#include <vector>

class SJFNonPreemptiveScheduler : public QObject {
    Q_OBJECT
public:
    SJFNonPreemptiveScheduler(std::vector<Process>& processes);

    // Accessor to get current time
    int getTim() const;

    // Scheduling function, live indicates if new processes can be added during execution
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
    bool paused;                // true if scheduler is paused
    QMutex mutex;               // protects access to paused flag
    QWaitCondition pauseCond;   // used to block the thread until resumed

    // Utility function (optional)
    void printGanttChart(int pid, int currentTime);
};
#endif // SJFnonpre_H
