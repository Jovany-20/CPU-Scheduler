#include "SchedulerWorker.h"
#include "NonPreemptivePriorityScheduler.h" // Include your scheduler class

SchedulerWorker::SchedulerWorker(std::vector<Process>& processes, bool live)
    : processes(processes), live(live) {}

void SchedulerWorker::process() {
    NonPreemptivePriorityScheduler scheduler(processes);

    // Connect the scheduler's update signal to the worker's update signal
    connect(&scheduler, &NonPreemptivePriorityScheduler::updateGanttChart, this, &SchedulerWorker::updateGanttChart);

    scheduler.schedule(live);

    emit finished();
}
