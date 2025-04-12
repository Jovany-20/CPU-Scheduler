#include "PreemptivePriorityScheduler.h"


PreemptivePriorityScheduler::PreemptivePriorityScheduler(vector<Process>& processes)
    : processes(processes), tim(0) {
}

int PreemptivePriorityScheduler::getTim() const {
    return tim;
}

void PreemptivePriorityScheduler::incrementTim() {
    tim++;
}

void PreemptivePriorityScheduler::printGanttChart(int pid, int currentTime) {
    if (pid == -1) {
        cout << "Idle";
    }
    else {
        cout << "(P" << pid << "," << currentTime << ")-";
    }
    cout.flush();
}

void PreemptivePriorityScheduler::schedule(bool& live) {
    int totalWaitingTime = 0;
    int totalTurnaroundTime = 0;
    int completed = 0;
    //const int numProcesses = processes.size();

    while (live || completed < processes.size()) {
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
                     return processes[a].getPriority() < processes[b].getPriority();
                 }
                 );

            int currentIndex = eligibleIndices[0];
            Process& currentProcess = processes[currentIndex];

            printGanttChart(currentProcess.getPid(), currentTime);
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
            printGanttChart(-1, currentTime);
            incrementTim();
            if (live) {
                this_thread::sleep_for(chrono::seconds(1));
            }
        }
    }

    cout << "\nAverage Waiting Time: "
         << static_cast<double>(totalWaitingTime) / processes.size()
         << "\nAverage Turnaround Time: "
         << static_cast<double>(totalTurnaroundTime) / processes.size()
         << endl;
}
