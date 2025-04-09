#include "headers.h"
#include "Process.h"
#include "comparePriority.h"
#ifndef NON_PREEMPTIVE_PRIORITY_SCHEDULER_H
#define NON_PREEMPTIVE_PRIORITY_SCHEDULER_H

class NonPreemptivePriorityScheduler {
private:
    vector<Process>& processes;
     int tim;

    
public:

    NonPreemptivePriorityScheduler(vector<Process>& processes) : processes(processes) {}
     int getTim() {
        return tim;
    }
    
    void schedule(bool & live) {
        tim = 0;
        priority_queue<Process, vector<Process>, ComparePriority> readyQueue;
        int totalWaitingTime = 0;
        int totalTurnaroundTime = 0;
        

        int completed = 0;

        while (live ? true : completed < processes.size()) { // true to keep running and allow the user to add new processes at any time.
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
                    if (live) { this_thread::sleep_for(chrono::seconds(1)); }
                }

                currentProcess.setCompletionTime(tim);
                completed++;
                totalWaitingTime += currentProcess.calcWaitingTime();
                totalTurnaroundTime += currentProcess.calcTurnaroundTime();


            }
            else {
                printGanttChart(-1, tim);
                if (live) { this_thread::sleep_for(chrono::seconds(1)); }
                tim++;
                
            }

        }
        int numProcesses = processes.size();
        double avgWaitingTime = static_cast<double>(totalWaitingTime) / numProcesses;
        double avgTurnaroundTime = static_cast<double>(totalTurnaroundTime) / numProcesses;

        cout << "\nAverage Waiting Time: " << avgWaitingTime << endl;
        cout << "Average Turnaround Time: " << avgTurnaroundTime << endl;
    }

    void printGanttChart(int pid, int currentTime) {

        if (pid == -1) {
            cout << "Idle";
            cout.flush();// Print Idle if no process is running
        }
        else {
            cout << "(P" << pid;
            cout << "," << currentTime;
            cout << ")";
            cout << "-";

            cout.flush();
        }
    }



};
#endif // NON_PREEMPTIVE_PRIORITY_SCHEDULER_H