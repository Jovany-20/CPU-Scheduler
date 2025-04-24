#include "FCFS_Scheduler.h"
#include <QThread>

FCFS_Scheduler::FCFS_Scheduler(std::vector<Process>& processes)
    : processes(processes), tim(0), paused(false) {}

int FCFS_Scheduler::getTim() const {
    return tim;
}

void FCFS_Scheduler::setPaused(bool pause) {
    QMutexLocker locker(&mutex);
    paused = pause;
    if (!paused) {
        pauseCond.wakeAll();
    }
}

void FCFS_Scheduler::schedule(bool& live) {
    std::queue<int> readyQueue;
    tim = 0;
    int completed = 0;
    int totalWaitingTime = 0;
    int totalTurnaroundTime = 0;
    std::vector<bool> isInQueue(processes.size(), false);

    while (live || completed < processes.size()) {
        // Pause handling
        {
            QMutexLocker locker(&mutex);
            while (paused) {
                pauseCond.wait(&mutex);
            }
        }

        for (int i = 0; i < (int)processes.size(); ++i) {
            if (!isInQueue[i] && processes[i].getArrivalTime() == tim && processes[i].getRemainingTime() > 0) {
                readyQueue.push(i);
                isInQueue[i] = true;
            }
        }

        if (!readyQueue.empty()) {
            int index = readyQueue.front();
            Process& process = processes[index];
            readyQueue.pop();

            process.setStartTime(tim);

            for (int i = 0; i < process.getBurstTime(); ++i) {
                // Pause handling
                {
                    QMutexLocker locker(&mutex);
                    while (paused) {
                        pauseCond.wait(&mutex);
                    }
                }
                process.decrement(1);
                emit updateGanttChart(process.getPid(), tim);
                if (live) QThread::sleep(1);
                tim++;

                for (int j = 0; j < (int)processes.size(); ++j) {
                    if (!isInQueue[j] && processes[j].getArrivalTime() == tim && processes[j].getRemainingTime() > 0) {
                        readyQueue.push(j);
                        isInQueue[j] = true;
                    }
                }
            }

            process.setCompletionTime(tim);

            process.setLastProcessedTime(tim);
            totalWaitingTime += process.calcWaitingTime();
            totalTurnaroundTime += process.calcTurnaroundTime();
            completed++;
        } else {
            emit updateGanttChart(-1, tim);
            if (live) QThread::sleep(1);
            tim++;
        }

        if ((int)isInQueue.size() < (int)processes.size()) {
            int diff = (int)processes.size() - (int)isInQueue.size();
            isInQueue.insert(isInQueue.end(), diff, false);
        }
    }

    double avgWaitingTime = static_cast<double>(totalWaitingTime) / processes.size();
    double avgTurnaroundTime = static_cast<double>(totalTurnaroundTime) / processes.size();
    emit statsCalculated(avgWaitingTime, avgTurnaroundTime);
    emit finished();
}
