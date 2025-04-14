#include "../Header Files/RoundRobinScheduler.h"
RoundRobinScheduler::RoundRobinScheduler(int quantum, std::vector<Process>& processes)
    : timeQuantum(quantum), processes(processes), tim(0), itr(0) {}

void RoundRobinScheduler::schedule(bool& live) {
    int n = processes.size();
    std::queue<Process> readyQueue;
    int currentTime = 0;
    int completed = 0;
    int totalWaitingTime = 0;
    int totalTurnaroundTime = 0;
    tim = 0;

    std::vector<bool> isInQueue(n, false);

    while (live || completed < processes.size()) {
        for (auto& proc : processes) {
            if (proc.getArrivalTime() == currentTime && proc.getRemainingTime() > 0 && !isInQueue[proc.getPid()]) {
                readyQueue.push(proc);
                isInQueue[proc.getPid()] = true;
            }
        }

        if (!readyQueue.empty()) {
            Process currentProcess = readyQueue.front();
            readyQueue.pop();

            if (currentProcess.getStartTime() == -1) {
                currentProcess.setStartTime(currentTime);
            }

            int slice = (((timeQuantum) < (currentProcess.getRemainingTime())) ? (timeQuantum) : (currentProcess.getRemainingTime()));

            for (int t = 0; t < slice; t++) {
                printGanttChart(currentProcess.getPid(), currentTime);
                if (live) std::this_thread::sleep_for(std::chrono::seconds(1));
                currentTime++;
                tim = currentTime;

                currentProcess.setRemainingTime(currentProcess.getRemainingTime() - 1);

                for (auto& proc : processes) {
                    if (proc.getArrivalTime() == currentTime && proc.getRemainingTime() > 0 && !isInQueue[proc.getPid()]) {
                        readyQueue.push(proc);
                        isInQueue[proc.getPid()] = true;
                    }
                }

                if (currentProcess.getRemainingTime() == 0) {
                    currentProcess.setCompletionTime(currentTime);
                    completed++;
                    totalWaitingTime += currentProcess.calcWaitingTime();
                    totalTurnaroundTime += currentProcess.calcTurnaroundTime();
                    break;
                }
            }

            if (currentProcess.getRemainingTime() > 0) {
                readyQueue.push(currentProcess);
            }

        } else {
            printGanttChart(-1, currentTime);
            if (live) std::this_thread::sleep_for(std::chrono::seconds(1));
            currentTime++;
            tim = currentTime;

        }
    }

    std::cout << "\nAverage Waiting Time: " << (double)totalWaitingTime / processes.size()
              << "\nAverage Turnaround Time: " << (double)totalTurnaroundTime / processes.size() << std::endl;
}

void RoundRobinScheduler::printGanttChart(int pid, int currentTime) {
    if (pid == -1) {
        std::cout << "Idle";
    } else {
        std::cout << "(P" << pid << "," << currentTime << ")-";
    }
    std::cout.flush();
}
