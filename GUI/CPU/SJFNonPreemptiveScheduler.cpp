#include "SJFNonPreemptiveScheduler.h"
#include <QThread>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iostream>
#include "ComparePriority.h"
SJFNonPreemptiveScheduler::SJFNonPreemptiveScheduler(vector<Process>& processes)
    : processes(processes), tim(0),paused(false) {
}

int SJFNonPreemptiveScheduler::getTim() const {
    return tim;
}

void SJFNonPreemptiveScheduler::setPaused(bool pause) {
    QMutexLocker locker(&mutex);
    paused = pause;
    if (!paused) {
        // Wake up the waiting thread(s) when resuming.
        pauseCond.wakeAll();
    }
}

void SJFNonPreemptiveScheduler::printGanttChart(int pid, int currentTime) {
    if (pid == -1) {
        cout << "Idle";
    }
    else {
        cout << "(P" << pid << "," << currentTime << ")";
        cout << "-";
    }
    cout.flush();
}

void SJFNonPreemptiveScheduler::schedule(bool& live) {
    tim = 0;
    priority_queue<Process*, vector<Process*>, CompareRemainingTime> readyQueue;
    int totalWaitingTime = 0;
    int totalTurnaroundTime = 0;
    int completed = 0;

    while (live || completed <  processes.size()) {

        {
            QMutexLocker locker(&mutex);
            while (paused) {
                pauseCond.wait(&mutex);
            }
        }

        // Enqueue processes that have arrived.
        for (auto& proc : processes) {
            if (proc.getArrivalTime() <= tim && proc.getStartTime() == -1) {
                readyQueue.push(&proc);
                proc.setStartTime(tim);
            }
        }

        if (!readyQueue.empty()) {
            Process* currentProcess = readyQueue.top();
            readyQueue.pop();

            for (int i = 0; i < currentProcess->getBurstTime(); ++i) {

                // Check again for pause before each update.
                {
                    QMutexLocker locker(&mutex);
                    while (paused) {
                        pauseCond.wait(&mutex);
                    }
                }
                currentProcess->decrement(1);
                emit updateGanttChart(currentProcess->getPid(), tim++);
                //printGanttChart(currentProcess.getPid(), tim++);
                if (live) {
                    QThread::sleep(1);
                    //this_thread::sleep_for(chrono::seconds(1));
                }
            }

            currentProcess->setCompletionTime(tim);
            completed++;
            totalWaitingTime += currentProcess->calcWaitingTime();
            totalTurnaroundTime += currentProcess->calcTurnaroundTime();
        }
        else {
            emit updateGanttChart(-1, tim);
            if (live) {
                QThread::sleep(1);
            }
            tim++;

        }
    }

    double avgWaitingTime = static_cast<double>(totalWaitingTime) /  processes.size();
    double avgTurnaroundTime = static_cast<double>(totalTurnaroundTime) /  processes.size();
    emit statsCalculated(avgWaitingTime, avgTurnaroundTime);
    // cout << "\nAverage Waiting Time: " << avgWaitingTime << endl;
    // cout << "Average Turnaround Time: " << avgTurnaroundTime << endl;

    emit finished();
}
