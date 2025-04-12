#include "NonPreemptivePriorityScheduler.h"


NonPreemptivePriorityScheduler::NonPreemptivePriorityScheduler(vector<Process>& processes)
    : processes(processes), tim(0) {
}

int NonPreemptivePriorityScheduler::getTim() const {
    return tim;
}

void NonPreemptivePriorityScheduler::printGanttChart(int pid, int currentTime) {
    if (pid == -1) {
        cout << "Idle";
    }
    else {
        cout << "(P" << pid << "," << currentTime << ")";
        cout << "-";
    }
    cout.flush();
}

void NonPreemptivePriorityScheduler::schedule(bool& live) {
    tim = 0;
    priority_queue<Process, vector<Process>, ComparePriority> readyQueue;
    int totalWaitingTime = 0;
    int totalTurnaroundTime = 0;
    int completed = 0;
    //const int numProcesses = processes.size();

    while (live || completed < processes.size()) {
        // Enqueue processes that have arrived.
        for (auto& proc : processes) {
            if (proc.getArrivalTime() <= tim && proc.getStartTime() == -1) {
                readyQueue.push(proc);
                proc.setStartTime(tim);
            }
        }

        if (!readyQueue.empty()) {
            Process currentProcess = readyQueue.top();
            readyQueue.pop();

            for (int i = 0; i < currentProcess.getRemainingTime(); ++i) {
                printGanttChart(currentProcess.getPid(), tim++);
                if (live) {
                    this_thread::sleep_for(chrono::seconds(1));
                }
            }

            currentProcess.setCompletionTime(tim);
            completed++;
            totalWaitingTime += currentProcess.calcWaitingTime();
            totalTurnaroundTime += currentProcess.calcTurnaroundTime();
        }
        else {
            printGanttChart(-1, tim);
            if (live) {
                this_thread::sleep_for(chrono::seconds(1));
            }
            tim++;
        }
    }

    double avgWaitingTime = static_cast<double>(totalWaitingTime) / processes.size();
    double avgTurnaroundTime = static_cast<double>(totalTurnaroundTime) / processes.size();

    cout << "\nAverage Waiting Time: " << avgWaitingTime << endl;
    cout << "Average Turnaround Time: " << avgTurnaroundTime << endl;
}
