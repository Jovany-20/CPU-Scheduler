#include "headers.h"
#include "Process.h"
#include "comparePriority.h"
#ifndef PREEMPTIVE_PRIORITY_SCHEDULER_H
#define PREEMPTIVE_PRIORITY_SCHEDULER_H
class PreemptivePriorityScheduler {
private:
    vector<Process>& processes;
    int tim;
    

public:
    PreemptivePriorityScheduler(vector<Process>& processes) : processes(processes), tim(0) {}

    int getTim() {
        
        return tim;
    }

    void incrementTim() {
       
        tim++;
    }

    void schedule(bool& live) {
        int totalWaitingTime = 0;
        int totalTurnaroundTime = 0;
        int completed = 0;

        while (live || completed < processes.size()) {
            int currentTime = getTim(); // Get current time once per iteration
            vector<int> eligibleIndices;

            
                
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
                // Sort by priority (ascending: lower value = higher priority)
                sort(eligibleIndices.begin(), eligibleIndices.end(), [this](int a, int b) {
                    return processes[a].getPriority() < processes[b].getPriority();
                    });

                int currentIndex = eligibleIndices[0];
                Process& currentProcess = processes[currentIndex];

                printGanttChart(currentProcess.getPid(), currentTime);

                // Decrement remaining time
                
                    
                    currentProcess.decrement(1);
                

                // Check if process completed
                if (currentProcess.getRemainingTime() == 0) {
                    
                    currentProcess.setCompletionTime(currentTime + 1);
                    completed++;
                    totalWaitingTime += currentProcess.calcWaitingTime();
                    totalTurnaroundTime += currentProcess.calcTurnaroundTime();
                }

                if (live) {
                    this_thread::sleep_for(chrono::seconds(1));
                }

                incrementTim(); // Increment time AFTER processing
            }
            else {
                
                printGanttChart(-1, currentTime);
                incrementTim(); // Increment time even when idle
                if (live) {
                    this_thread::sleep_for(chrono::seconds(1));
                }

            }
        }

        // Output averages
        cout << "\nAverage Waiting Time: " << (double)totalWaitingTime / processes.size()
            << "\nAverage Turnaround Time: " << (double)totalTurnaroundTime / processes.size() << endl;
    }

    void printGanttChart(int pid, int currentTime) {
        if (pid == -1) {
            cout << "Idle";
        }
        else {
            cout << "(P" << pid << "," << currentTime << ")-";
        }
        cout.flush();
    }
};
#endif