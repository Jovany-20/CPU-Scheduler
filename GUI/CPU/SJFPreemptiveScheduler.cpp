#include "SJFPreemptiveScheduler.h"
#include "GanttChartWindow.h"


SJFPreemptiveScheduler::SJFPreemptiveScheduler(vector<Process>& processes)
    : processes(processes), tim(0) , paused(false){
}

int SJFPreemptiveScheduler::getTim() const {
    return tim;
}

void SJFPreemptiveScheduler::incrementTim() {
    tim++;
}

void SJFPreemptiveScheduler::setPaused(bool pause) {
    // Lock the mutex when updating the pause flag.
    QMutexLocker locker(&mutex);
    paused = pause;
    if (!paused) {
        // If resuming, wake up all waiting threads.
        pauseCond.wakeAll();
    }
}

void SJFPreemptiveScheduler::schedule(bool& live) {
    int totalWaitingTime = 0;
    int totalTurnaroundTime = 0;
    int completed = 0;

    while (live || completed < processes.size()) {
        {
            QMutexLocker locker(&mutex);
            while (paused) {
                // Wait until resume is signaled. The mutex is unlocked during wait.
                pauseCond.wait(&mutex);
            }
        }
        int currentTime = getTim();
        vector<int> eligibleIndices;
        // Identify eligible processes based on arrival and remaining time.
        for (int i = 0; i < processes.size(); ++i) {
            Process& proc = processes[i];
            if (proc.getArrivalTime() <= currentTime && proc.getRemainingTime() > 0) {
                if (proc.getStartTime() == -1) {
                    proc.setStartTime(currentTime);
                }
                eligibleIndices.push_back(i);
            }
        }

        if (!eligibleIndices.empty()) {
            // Sort eligible processes by priority (lower numerical value means higher priority).
            sort(eligibleIndices.begin(), eligibleIndices.end(),
                 [this](int a, int b) {
                     return processes[a].getRemainingTime() < processes[b].getRemainingTime();
                 }
                 );

            int currentIndex = eligibleIndices[0];
            Process& currentProcess = processes[currentIndex];

            emit updateGanttChart(currentProcess.getPid(), currentTime);
            currentProcess.decrement(1);

            if (currentProcess.getRemainingTime() == 0) {
                currentProcess.setCompletionTime(currentTime + 1);
                completed++;
                totalWaitingTime += currentProcess.calcWaitingTime();
                totalTurnaroundTime += currentProcess.calcTurnaroundTime();
            }

            if (live) {
                this_thread::sleep_for(chrono::seconds(1));
            }

            incrementTim();
        }
        else {
            emit updateGanttChart(-1, currentTime);
            incrementTim();
            if (live) {
                this_thread::sleep_for(chrono::seconds(1));
            }

        }
    }

    double avgWaitingTime = static_cast<double>(totalWaitingTime) / processes.size();
    double avgTurnaroundTime = static_cast<double>(totalTurnaroundTime) / processes.size();
    emit statsCalculated(avgWaitingTime, avgTurnaroundTime);

    // cout << "\nAverage Waiting Time: " << avgWaitingTime << endl;
    // cout << "Average Turnaround Time: " << avgTurnaroundTime << endl;

    emit finished();
}
