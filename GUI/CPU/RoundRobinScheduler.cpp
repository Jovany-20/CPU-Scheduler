#include "RoundRobinScheduler.h"
#include <QThread>
#include <chrono>
#include <thread>
#include <iostream>

RoundRobinScheduler::RoundRobinScheduler(int quantum, std::vector<Process>& processes)
    : timeQuantum(quantum), processes(processes), tim(0), paused(false) {}

void RoundRobinScheduler::schedule(bool& live) {
    int n = processes.size();
    std::queue<Process*> readyQueue;
    int currentTime = 0;
    int completed = 0;
    int totalWaitingTime = 0;
    int totalTurnaroundTime = 0;
    tim = 0;

    std::vector<bool> isInQueue(n, false);

    while (live || completed < processes.size()) {
        // Pause handling
        {
            QMutexLocker locker(&mutex);
            while (paused) {
                pauseCond.wait(&mutex);
            }
        }

        for (auto& proc : processes) {
            if (proc.getArrivalTime() == currentTime && proc.getRemainingTime() > 0 && !isInQueue[proc.getPid()]) {
                readyQueue.push(&proc);
                isInQueue[proc.getPid()] = true;
            }
        }

        if (!readyQueue.empty()) {
            Process* currentProcess = readyQueue.front();
            readyQueue.pop();

            if (currentProcess->getStartTime() == -1) {
                currentProcess->setStartTime(currentTime);
            }

            int slice = std::min(timeQuantum, currentProcess->getRemainingTime());

            for (int t = 0; t < slice; t++) {
                // Pause handling
                {
                    QMutexLocker locker(&mutex);
                    while (paused) {
                        pauseCond.wait(&mutex);
                    }
                }

                emit updateGanttChart(currentProcess->getPid(), currentTime);
                if (live) QThread::sleep(1);
                currentTime++;
                tim = currentTime;

                currentProcess->setRemainingTime(currentProcess->getRemainingTime() - 1);

                for (auto& proc : processes) {
                    if (proc.getArrivalTime() == currentTime && proc.getRemainingTime() > 0 && !isInQueue[proc.getPid()]) {
                        readyQueue.push(&proc);
                        isInQueue[proc.getPid()] = true;
                    }
                }

                if (currentProcess->getRemainingTime() == 0) {
                    currentProcess->setCompletionTime(currentTime);
                    completed++;
                    totalWaitingTime += currentProcess->calcWaitingTime();
                    totalTurnaroundTime += currentProcess->calcTurnaroundTime();
                    break;
                }
            }

            if (currentProcess->getRemainingTime() > 0) {
                readyQueue.push(currentProcess);
            }

        } else {
            emit updateGanttChart(-1, currentTime);
            if (live) QThread::sleep(1);
            currentTime++;
            tim = currentTime;
        }
    }

    double avgWaitingTime = static_cast<double>(totalWaitingTime) / processes.size();
    double avgTurnaroundTime = static_cast<double>(totalTurnaroundTime) / processes.size();
    emit statsCalculated(avgWaitingTime, avgTurnaroundTime);
    emit finished();
}

void RoundRobinScheduler::setPaused(bool pause) {
    QMutexLocker locker(&mutex);
    paused = pause;
    if (!paused) {
        pauseCond.wakeAll();
    }
}

int RoundRobinScheduler::getTim() const {
    return tim;
}

void RoundRobinScheduler::printGanttChart(int pid, int currentTime) {
    // This function is not used in the GUI version, but kept for consistency
}
