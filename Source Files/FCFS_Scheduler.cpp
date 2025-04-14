#include "../Header Files/FCFS_Scheduler.h"

FCFS_Scheduler::FCFS_Scheduler(std::vector<Process>& processes)
    : processes(processes) {}

void FCFS_Scheduler::schedule(bool& live) {
    std::queue<int> readyQueue;
    int currentTime = 0;
    int completed = 0;
    int totalWaitingTime = 0;
    int totalTurnaroundTime = 0;
    std::vector<bool> isInQueue(processes.size(), false);

    while (live || completed < processes.size()) {
        for (int i = 0; i < (int)processes.size(); ++i) {
            if (!isInQueue[i] && processes[i].getArrivalTime() == currentTime && processes[i].getRemainingTime() > 0) {
                readyQueue.push(i);
                isInQueue[i] = true;
            }
        }

        if (!readyQueue.empty()) {
            int index = readyQueue.front();
            Process& process = processes[index];
            readyQueue.pop();

            process.setStartTime(currentTime);

            for (int i = 0; i < process.getBurstTime(); ++i) {
                printGanttChart(process.getPid(), currentTime);
                if (live) std::this_thread::sleep_for(std::chrono::seconds(1));
                currentTime++;

                for (int j = 0; j < (int)processes.size(); ++j) {
                    if (!isInQueue[j] && processes[j].getArrivalTime() == currentTime && processes[j].getRemainingTime() > 0) {
                        readyQueue.push(j);
                        isInQueue[j] = true;
                    }
                }
            }

            process.setCompletionTime(currentTime);
            process.setRemainingTime(0);
            process.setLastProcessedTime(currentTime);
            totalWaitingTime += process.calcWaitingTime();
            totalTurnaroundTime += process.calcTurnaroundTime();
            completed++;
        } else {
            printGanttChart(-1, currentTime);
            if (live) std::this_thread::sleep_for(std::chrono::seconds(1));
            currentTime++;
        }

        if ((int)isInQueue.size() < (int)processes.size()) {
            int diff = (int)processes.size() - (int)isInQueue.size();
            isInQueue.insert(isInQueue.end(), diff, false);
        }
    }

    std::cout << "\nAverage Waiting Time: " << (double)totalWaitingTime / processes.size()
              << "\nAverage Turnaround Time: " << (double)totalTurnaroundTime / processes.size() << std::endl;
}

void FCFS_Scheduler::printGanttChart(int pid, int currentTime) {
    if (pid == -1) {
        std::cout << "Idle";
    } else {
        std::cout << "(P" << pid << "," << currentTime << ")-";
    }
    std::cout.flush();
}
