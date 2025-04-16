#include "NonPreemptivePriorityScheduler.h"
#include <QThread>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iostream>
#include "ComparePriority.h"

using namespace std;

NonPreemptivePriorityScheduler::NonPreemptivePriorityScheduler(vector<Process>& processes)
    : processes(processes), tim(0), paused(false) {
}

int NonPreemptivePriorityScheduler::getTim() const {
    return tim;
}

void NonPreemptivePriorityScheduler::setPaused(bool pause) {
    QMutexLocker locker(&mutex);
    paused = pause;
    if (!paused) {
        // Wake up the waiting thread(s) when resuming.
        pauseCond.wakeAll();
    }
}

void NonPreemptivePriorityScheduler::printGanttChart(int pid, int currentTime) {
    // Not used; your update signal handles drawing.
}

void NonPreemptivePriorityScheduler::schedule(bool& live) {
    tim = 0;
    // Create a priority_queue for scheduling based on the custom ComparePriority.
    priority_queue<Process*, vector<Process*>, ComparePriority> readyQueue;
    int totalWaitingTime = 0;
    int totalTurnaroundTime = 0;
    int completed = 0;

    while (live || completed < processes.size()) {
        // --- Pause Handling: Check pause flag at the start of the loop ---
        {
            QMutexLocker locker(&mutex);
            while (paused) {
                pauseCond.wait(&mutex);
            }
        }

        // Enqueue processes that have arrived.
        for (auto &proc : processes) {
            if (proc.getArrivalTime() <= tim && proc.getStartTime() == -1) {
                readyQueue.push(&proc);
                proc.setStartTime(tim);
            }
        }

        if (!readyQueue.empty()) {
            Process* currentProcess = readyQueue.top();
            readyQueue.pop();

            // Process the entire burst of the current process.
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
                if (live) {
                    // Sleep for 1 second using QThread::sleep.
                    QThread::sleep(1);
                }
            }

            currentProcess->setCompletionTime(tim);
            completed++;
            totalWaitingTime += currentProcess->calcWaitingTime();
            totalTurnaroundTime += currentProcess->calcTurnaroundTime();
        }
        else {
            // If no process is ready, output an "idle" interval.
            emit updateGanttChart(-1, tim);
            if (live) {
                QThread::sleep(1);
            }
            tim++;
        }
    }

    double avgWaitingTime = static_cast<double>(totalWaitingTime) / processes.size();
    double avgTurnaroundTime = static_cast<double>(totalTurnaroundTime) / processes.size();
    emit statsCalculated(avgWaitingTime, avgTurnaroundTime);
    // cout << "\nAverage Waiting Time: " << avgWaitingTime << endl;
    // cout << "Average Turnaround Time: " << avgTurnaroundTime << endl;

    emit finished();
}
