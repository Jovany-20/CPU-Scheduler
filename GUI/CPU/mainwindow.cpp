#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "process.h"
#include "PreemptivePriorityScheduler.h"
#include "NonPreemptivePriorityScheduler.h"
#include "GanttChartWindow.h"
#include "FCFS_Scheduler.h"
#include "SJFNonPreemptiveScheduler.h"
#include "SJFPreemptiveScheduler.h"
#include "RoundRobinScheduler.h"
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QVector>
#include <QCheckBox>
#include <QThread>
#include <QTimer>
#include <QIntValidator>
#include <vector>






class PreemptiveWindow : public QWidget {
public:
    PreemptiveWindow(QWidget* parent = nullptr)
        : QWidget(parent), processCount(1), live(false), isPaused(false), scheduler(nullptr) {
        setWindowTitle("Preemptive Priority Window");

        mainLayout = new QVBoxLayout(this);
        QFormLayout* formLayout = new QFormLayout();

        idLabel = new QLabel(QString::number(processCount));
        burstTimeEdit = new QLineEdit();
        priorityEdit = new QLineEdit();
        arrivalTimeEdit = new QLineEdit();

        formLayout->addRow("ID:", idLabel);
        formLayout->addRow("Burst Time:", burstTimeEdit);
        formLayout->addRow("Priority:", priorityEdit);
        formLayout->addRow("Arrival Time:", arrivalTimeEdit);
        minArrivalTime=0;
        QPushButton* addButton = new QPushButton("Add Process");
        connect(addButton, &QPushButton::clicked, this, &PreemptiveWindow::addProcess);

        liveCheckBox = new QCheckBox("Live");
        connect(liveCheckBox, &QCheckBox::toggled, this, &PreemptiveWindow::toggleLive);

        startButton = new QPushButton("Start");
        connect(startButton, &QPushButton::clicked, this, &PreemptiveWindow::startScheduling);

        endLiveButton = new QPushButton("End Live");
        endLiveButton->setEnabled(false);
        connect(endLiveButton, &QPushButton::clicked, this, &PreemptiveWindow::endLive);

        // Initially, the pause/resume button is not created.
        pauseResumeButton = nullptr;

        mainLayout->addLayout(formLayout);
        mainLayout->addWidget(addButton);
        mainLayout->addWidget(liveCheckBox);
        mainLayout->addWidget(startButton);
        mainLayout->addWidget(endLiveButton);

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &PreemptiveWindow::updateArrivalTime);
    }

    // This function is used as a slot to handle Gantt chart updates.
    void handleGanttChartUpdate(int pid, int currentTime) {
        ganttChartWindow->addBlock(pid, currentTime,processes);
    }

private slots:
    void addProcess() {
        QString arrivalText = arrivalTimeEdit->text();
        QString burstText = burstTimeEdit->text();
        QString priorityText = priorityEdit->text();

        int burstTime = burstText.toInt();
        int priority = priorityText.toInt();
        int arrivalTime = arrivalText.toInt();

        bool hasError = false;

        if (arrivalText.isEmpty() || arrivalTime < minArrivalTime) {
            arrivalTimeEdit->setText(QString::number(minArrivalTime));
            hasError = true;
        }

        if (priorityText.isEmpty() || priority < 1) {
            priorityEdit->setText("1");
            hasError = true;
        }

        if (burstText.isEmpty() || burstTime < 1) {
            burstTimeEdit->setText("1");
            hasError = true;
        }

        if (hasError)
            return; // Return now so the user can correct the entries

        processes.push_back(Process(processCount, burstTime, priority, arrivalTime));
        processCount++;

        idLabel->setText(QString::number(processCount));
        burstTimeEdit->clear();
        priorityEdit->clear();
        arrivalTimeEdit->clear();
    }

    void toggleLive(bool checked) {
        live = checked;
        endLiveButton->setEnabled(live);
    }

    // New slot that toggles pause/resume.
    void togglePause() {
        if (!isPaused) {
            // Pause the scheduler and timer
            isPaused = true;
            pauseResumeButton->setText("Resume");

            // Signal the scheduler to pause.
            if (scheduler) {
                scheduler->setPaused(true);
                QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                          Q_ARG(bool, true));
            }
            timer->stop();
        } else {
            // Resume the scheduler and timer.
            isPaused = false;
            pauseResumeButton->setText("Pause");

            if (scheduler) {
                scheduler->setPaused(false);
                QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                          Q_ARG(bool, false));
            }
            timer->start(800);
        }
    }

signals:
    void updateGanttChart(int pid, int currentTime);
    void startScheduling() {
        if (!live) {
            clearPage();
        } else {
            liveCheckBox->hide();
            // Hide the start button so the user can’t click it again.
            startButton->hide();
            timer->start(800);
        }
        ganttChartWindow = new GanttChartWindow();
        ganttChartWindow->show();

        // (If live, create the pause/resume button as before.)
        if (live) {
            pauseResumeButton = new QPushButton("Pause", this);
            mainLayout->addWidget(pauseResumeButton);
            connect(pauseResumeButton, &QPushButton::clicked, this, &PreemptiveWindow::togglePause);
        }

        QThread* thread = new QThread;
        scheduler = new PreemptivePriorityScheduler(processes);
        scheduler->moveToThread(thread);

        connect(thread, &QThread::started, [this]() {
            connect(scheduler, &PreemptivePriorityScheduler::updateGanttChart,
                    this, &PreemptiveWindow::handleGanttChartUpdate, Qt::QueuedConnection);
            // Connect the statsCalculated signal if you use it
            connect(scheduler, &PreemptivePriorityScheduler::statsCalculated,
                    ganttChartWindow, &GanttChartWindow::setStats, Qt::QueuedConnection);
            scheduler->schedule(live);
        });

        connect(scheduler, &PreemptivePriorityScheduler::finished, thread, &QThread::quit);
        connect(scheduler, &PreemptivePriorityScheduler::finished, scheduler, &PreemptivePriorityScheduler::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
    }

    void updateArrivalTime() {
        if (scheduler) {
            minArrivalTime = scheduler->getTim() + 2;
            arrivalTimeEdit->setText(QString::number(minArrivalTime));
            arrivalTimeEdit->setValidator(new QIntValidator(minArrivalTime, INT_MAX, this));
        }
    }

    void endLive() {
        live = false;
        endLiveButton->setEnabled(false);
        timer->stop();
        clearPage();
        if (scheduler) {
            scheduler->setPaused(false);
            QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                      Q_ARG(bool, false));
        }
    }

    void clearPage() {
        QLayoutItem* item;
        while ((item = mainLayout->takeAt(0)) != nullptr) {
            if(item->widget()) {
                delete item->widget();
            }
            delete item;
        }
    }

private:
    QVBoxLayout* mainLayout;
    QLabel* idLabel;
    QLineEdit* burstTimeEdit;
    QLineEdit* priorityEdit;
    QLineEdit* arrivalTimeEdit;
    std::vector<Process> processes;
    int processCount;
    QCheckBox* liveCheckBox;
    QPushButton* startButton;
    QPushButton* endLiveButton;
    // New pause/resume button.
    QPushButton* pauseResumeButton;
    bool live;
    bool isPaused;   // flag indicating whether we are paused
    int minArrivalTime;
    QTimer* timer;
    PreemptivePriorityScheduler* scheduler;
    GanttChartWindow* ganttChartWindow;
};

class NonPreemptiveWindow : public QWidget {
public:
    NonPreemptiveWindow(QWidget* parent = nullptr)
        : QWidget(parent), processCount(1), live(false), isPaused(false), scheduler(nullptr) {
        setWindowTitle("NonPreemptive Priority Window");

        mainLayout = new QVBoxLayout(this);
        QFormLayout* formLayout = new QFormLayout();

        idLabel = new QLabel(QString::number(processCount));
        burstTimeEdit = new QLineEdit();
        priorityEdit = new QLineEdit();
        arrivalTimeEdit = new QLineEdit();

        formLayout->addRow("ID:", idLabel);
        formLayout->addRow("Burst Time:", burstTimeEdit);
        formLayout->addRow("Priority:", priorityEdit);
        formLayout->addRow("Arrival Time:", arrivalTimeEdit);
        minArrivalTime=0;
        QPushButton* addButton = new QPushButton("Add Process");
        connect(addButton, &QPushButton::clicked, this, &NonPreemptiveWindow::addProcess);

        liveCheckBox = new QCheckBox("Live");
        connect(liveCheckBox, &QCheckBox::toggled, this, &NonPreemptiveWindow::toggleLive);

        startButton = new QPushButton("Start");
        connect(startButton, &QPushButton::clicked, this, &NonPreemptiveWindow::startScheduling);

        endLiveButton = new QPushButton("End Live");
        endLiveButton->setEnabled(false);
        connect(endLiveButton, &QPushButton::clicked, this, &NonPreemptiveWindow::endLive);

        // Initially, the pause/resume button is not created.
        pauseResumeButton = nullptr;

        mainLayout->addLayout(formLayout);
        mainLayout->addWidget(addButton);
        mainLayout->addWidget(liveCheckBox);
        mainLayout->addWidget(startButton);
        mainLayout->addWidget(endLiveButton);

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &NonPreemptiveWindow::updateArrivalTime);
    }

    // Slot to update the Gantt chart.
    void handleGanttChartUpdate(int pid, int currentTime) {
        ganttChartWindow->addBlock(pid, currentTime,processes);
    }

private slots:
    void addProcess() {
        QString arrivalText = arrivalTimeEdit->text();
        QString burstText = burstTimeEdit->text();
        QString priorityText = priorityEdit->text();

        int burstTime = burstText.toInt();
        int priority = priorityText.toInt();
        int arrivalTime = arrivalText.toInt();

        bool hasError = false;

        if (arrivalText.isEmpty() || arrivalTime < minArrivalTime) {
            arrivalTimeEdit->setText(QString::number(minArrivalTime));
            hasError = true;
        }

        if (priorityText.isEmpty() || priority < 1) {
            priorityEdit->setText("1");
            hasError = true;
        }

        if (burstText.isEmpty() || burstTime < 1) {
            burstTimeEdit->setText("1");
            hasError = true;
        }

        if (hasError)
            return;  // Return now so the user can correct the entries

        processes.push_back(Process(processCount, burstTime, priority, arrivalTime));
        processCount++;

        idLabel->setText(QString::number(processCount));
        burstTimeEdit->clear();
        priorityEdit->clear();
        arrivalTimeEdit->clear();
    }

    void toggleLive(bool checked) {
        live = checked;
        endLiveButton->setEnabled(live);
    }

    // Slot to toggle pause/resume functionality.
    void togglePause() {
        if (!isPaused) {
            // Pause the scheduler and stop the timer.
            isPaused = true;
            pauseResumeButton->setText("Resume");
            if (scheduler) {
                scheduler->setPaused(true);
                QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                          Q_ARG(bool, true));
            }
            timer->stop();
        } else {
            // Resume the scheduler and restart the timer.
            isPaused = false;
            pauseResumeButton->setText("Pause");
            if (scheduler) {
                scheduler->setPaused(false);
                QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                          Q_ARG(bool, false));
            }
            timer->start(800);
        }
    }

signals:
    void updateGanttChart(int pid, int currentTime);
    void startScheduling() {
        if (!live) {
            clearPage();
        } else {
            liveCheckBox->hide();
            // Hide the start button so it won't be clicked again.
            startButton->hide();
            timer->start(800);
        }
        ganttChartWindow = new GanttChartWindow();
        ganttChartWindow->show();

        // If live mode is enabled, create and add the pause/resume button.
        if (live) {
            pauseResumeButton = new QPushButton("Pause", this);
            mainLayout->addWidget(pauseResumeButton);
            connect(pauseResumeButton, &QPushButton::clicked, this, &NonPreemptiveWindow::togglePause);
        }

        QThread* thread = new QThread;
        // Assign the scheduler as a member variable.
        scheduler = new NonPreemptivePriorityScheduler(processes);
        scheduler->moveToThread(thread);

        connect(thread, &QThread::started, [this]() {
            connect(scheduler, &NonPreemptivePriorityScheduler::updateGanttChart,
                    this, &NonPreemptiveWindow::handleGanttChartUpdate, Qt::QueuedConnection);
            connect(scheduler, &NonPreemptivePriorityScheduler::statsCalculated,
                    ganttChartWindow, &GanttChartWindow::setStats, Qt::QueuedConnection);
            scheduler->schedule(live);
        });

        connect(scheduler, &NonPreemptivePriorityScheduler::finished, thread, &QThread::quit);
        connect(scheduler, &NonPreemptivePriorityScheduler::finished, scheduler, &NonPreemptivePriorityScheduler::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
    }

    void updateArrivalTime() {
        if (scheduler) {
            minArrivalTime = scheduler->getTim() + 2;
            arrivalTimeEdit->setText(QString::number(minArrivalTime));
            arrivalTimeEdit->setValidator(new QIntValidator(minArrivalTime, INT_MAX, this));
        }
    }

    void endLive() {
        live = false;
        endLiveButton->setEnabled(false);
        timer->stop();
        clearPage();
        if (scheduler) {
            scheduler->setPaused(false);
            QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                      Q_ARG(bool, false));
        }
    }

    void clearPage() {
        QLayoutItem* item;
        while ((item = mainLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
    }

private:
    QVBoxLayout* mainLayout;
    QLabel* idLabel;
    QLineEdit* burstTimeEdit;
    QLineEdit* priorityEdit;
    QLineEdit* arrivalTimeEdit;
    std::vector<Process> processes;
    int processCount;
    QCheckBox* liveCheckBox;
    QPushButton* startButton;
    QPushButton* endLiveButton;
    bool live;
    int minArrivalTime;
    QTimer* timer;
    NonPreemptivePriorityScheduler* scheduler;
    GanttChartWindow* ganttChartWindow;
    // Pause/resume button and flag.
    QPushButton* pauseResumeButton;
    bool isPaused;
};

class FCFSWindow : public QWidget {
public:
    FCFSWindow(QWidget* parent = nullptr)
        : QWidget(parent), processCount(1), live(false), isPaused(false), scheduler(nullptr) {
        setWindowTitle("FCFS Scheduling Window");

        mainLayout = new QVBoxLayout(this);
        QFormLayout* formLayout = new QFormLayout();

        idLabel = new QLabel(QString::number(processCount));
        burstTimeEdit = new QLineEdit();
        arrivalTimeEdit = new QLineEdit();

        formLayout->addRow("ID:", idLabel);
        formLayout->addRow("Burst Time:", burstTimeEdit);
        formLayout->addRow("Arrival Time:", arrivalTimeEdit);
        minArrivalTime = 0;

        QPushButton* addButton = new QPushButton("Add Process");
        connect(addButton, &QPushButton::clicked, this, &FCFSWindow::addProcess);

        liveCheckBox = new QCheckBox("Live");
        connect(liveCheckBox, &QCheckBox::toggled, this, &FCFSWindow::toggleLive);

        startButton = new QPushButton("Start");
        connect(startButton, &QPushButton::clicked, this, &FCFSWindow::startScheduling);

        endLiveButton = new QPushButton("End Live");
        endLiveButton->setEnabled(false);
        connect(endLiveButton, &QPushButton::clicked, this, &FCFSWindow::endLive);

        pauseResumeButton = nullptr;

        mainLayout->addLayout(formLayout);
        mainLayout->addWidget(addButton);
        mainLayout->addWidget(liveCheckBox);
        mainLayout->addWidget(startButton);
        mainLayout->addWidget(endLiveButton);

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &FCFSWindow::updateArrivalTime);
    }

    void handleGanttChartUpdate(int pid, int currentTime) {
        ganttChartWindow->addBlock(pid, currentTime, processes);
    }

private slots:
    void addProcess() {
        QString arrivalText = arrivalTimeEdit->text();
        QString burstText = burstTimeEdit->text();

        int burstTime = burstText.toInt();
        int arrivalTime = arrivalText.toInt();

        bool hasError = false;

        if (arrivalText.isEmpty() || arrivalTime < minArrivalTime) {
            arrivalTimeEdit->setText(QString::number(minArrivalTime));
            hasError = true;
        }

        if (burstText.isEmpty() || burstTime < 1) {
            burstTimeEdit->setText("1");
            hasError = true;
        }

        if (hasError)
            return;

        processes.push_back(Process(processCount, burstTime, 1, arrivalTime)); // Priority set to 1 (not used in FCFS)
        processCount++;

        idLabel->setText(QString::number(processCount));
        burstTimeEdit->clear();
        arrivalTimeEdit->clear();
    }

    void toggleLive(bool checked) {
        live = checked;
        endLiveButton->setEnabled(live);
    }

    void togglePause() {
        if (!isPaused) {
            isPaused = true;
            pauseResumeButton->setText("Resume");

            if (scheduler) {
                scheduler->setPaused(true);
                QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                          Q_ARG(bool, true));
            }
            timer->stop();
        } else {
            isPaused = false;
            pauseResumeButton->setText("Pause");

            if (scheduler) {
                scheduler->setPaused(false);
                QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                          Q_ARG(bool, false));
            }
            timer->start(800);
        }
    }

    void startScheduling() {
        if (!live) {
            clearPage();
        } else {
            liveCheckBox->hide();
            startButton->hide();
            timer->start(800);
        }
        ganttChartWindow = new GanttChartWindow();
        ganttChartWindow->show();

        if (live) {
            pauseResumeButton = new QPushButton("Pause", this);
            mainLayout->addWidget(pauseResumeButton);
            connect(pauseResumeButton, &QPushButton::clicked, this, &FCFSWindow::togglePause);
        }

        QThread* thread = new QThread;
        scheduler = new FCFS_Scheduler(processes);
        scheduler->moveToThread(thread);

        connect(thread, &QThread::started, [this]() {
            connect(scheduler, &FCFS_Scheduler::updateGanttChart,
                    this, &FCFSWindow::handleGanttChartUpdate, Qt::QueuedConnection);
            connect(scheduler, &FCFS_Scheduler::statsCalculated,
                    ganttChartWindow, &GanttChartWindow::setStats, Qt::QueuedConnection);
            scheduler->schedule(live);
        });

        connect(scheduler, &FCFS_Scheduler::finished, thread, &QThread::quit);
        connect(scheduler, &FCFS_Scheduler::finished, scheduler, &FCFS_Scheduler::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
    }

    void updateArrivalTime() {
        if (scheduler) {
            minArrivalTime = scheduler->getTim() + 2;
            arrivalTimeEdit->setText(QString::number(minArrivalTime));
            arrivalTimeEdit->setValidator(new QIntValidator(minArrivalTime, INT_MAX, this));
        }
    }

    void endLive() {
        live = false;
        endLiveButton->setEnabled(false);
        timer->stop();
        clearPage();
        if (scheduler) {
            scheduler->setPaused(false);
            QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                      Q_ARG(bool, false));
        }
    }

    void clearPage() {
        QLayoutItem* item;
        while ((item = mainLayout->takeAt(0)) != nullptr) {
            if(item->widget()) {
                delete item->widget();
            }
            delete item;
        }
    }

private:
    QVBoxLayout* mainLayout;
    QLabel* idLabel;
    QLineEdit* burstTimeEdit;
    QLineEdit* arrivalTimeEdit;
    std::vector<Process> processes;
    int processCount;
    QCheckBox* liveCheckBox;
    QPushButton* startButton;
    QPushButton* endLiveButton;
    QPushButton* pauseResumeButton;
    bool live;
    bool isPaused;
    int minArrivalTime;
    QTimer* timer;
    FCFS_Scheduler* scheduler;
    GanttChartWindow* ganttChartWindow;
};

class RoundRobinWindow : public QWidget {
public:
    RoundRobinWindow(QWidget* parent = nullptr)
        : QWidget(parent), processCount(1), live(false), isPaused(false), scheduler(nullptr), timeQuantum(2) {
        setWindowTitle("Round Robin Window");

        mainLayout = new QVBoxLayout(this);
        QFormLayout* formLayout = new QFormLayout();

        idLabel = new QLabel(QString::number(processCount));
        burstTimeEdit = new QLineEdit();
        arrivalTimeEdit = new QLineEdit();
        quantumEdit = new QLineEdit();
        quantumEdit->setText("2"); // Default time quantum

        formLayout->addRow("ID:", idLabel);
        formLayout->addRow("Burst Time:", burstTimeEdit);
        formLayout->addRow("Arrival Time:", arrivalTimeEdit);
        formLayout->addRow("Time Quantum:", quantumEdit);
        minArrivalTime = 0;

        QPushButton* addButton = new QPushButton("Add Process");
        connect(addButton, &QPushButton::clicked, this, &RoundRobinWindow::addProcess);

        liveCheckBox = new QCheckBox("Live");
        connect(liveCheckBox, &QCheckBox::toggled, this, &RoundRobinWindow::toggleLive);

        startButton = new QPushButton("Start");
        connect(startButton, &QPushButton::clicked, this, &RoundRobinWindow::startScheduling);

        endLiveButton = new QPushButton("End Live");
        endLiveButton->setEnabled(false);
        connect(endLiveButton, &QPushButton::clicked, this, &RoundRobinWindow::endLive);

        pauseResumeButton = nullptr;

        mainLayout->addLayout(formLayout);
        mainLayout->addWidget(addButton);
        mainLayout->addWidget(liveCheckBox);
        mainLayout->addWidget(startButton);
        mainLayout->addWidget(endLiveButton);

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &RoundRobinWindow::updateArrivalTime);
    }

    void handleGanttChartUpdate(int pid, int currentTime) {
        ganttChartWindow->addBlock(pid, currentTime, processes);
    }

private slots:
    void addProcess() {
        QString arrivalText = arrivalTimeEdit->text();
        QString burstText = burstTimeEdit->text();
        QString quantumText = quantumEdit->text();

        int burstTime = burstText.toInt();
        int arrivalTime = arrivalText.toInt();
        timeQuantum = quantumText.toInt();

        bool hasError = false;

        if (arrivalText.isEmpty() || arrivalTime < minArrivalTime) {
            arrivalTimeEdit->setText(QString::number(minArrivalTime));
            hasError = true;
        }

        if (burstText.isEmpty() || burstTime < 1) {
            burstTimeEdit->setText("1");
            hasError = true;
        }

        if (quantumText.isEmpty() || timeQuantum < 1) {
            quantumEdit->setText("1");
            hasError = true;
        }

        if (hasError)
            return;

        processes.push_back(Process(processCount, burstTime, 1, arrivalTime)); // Priority set to 1 (not used in RR)
        processCount++;

        idLabel->setText(QString::number(processCount));
        burstTimeEdit->clear();
        arrivalTimeEdit->clear();
    }

    void toggleLive(bool checked) {
        live = checked;
        endLiveButton->setEnabled(live);
    }

    void togglePause() {
        if (!isPaused) {
            isPaused = true;
            pauseResumeButton->setText("Resume");

            if (scheduler) {
                scheduler->setPaused(true);
                QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                          Q_ARG(bool, true));
            }
            timer->stop();
        } else {
            isPaused = false;
            pauseResumeButton->setText("Pause");

            if (scheduler) {
                scheduler->setPaused(false);
                QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                          Q_ARG(bool, false));
            }
            timer->start(800);
        }
    }

    void startScheduling() {
        if (!live) {
            clearPage();
        } else {
            liveCheckBox->hide();
            startButton->hide();
            timer->start(800);
        }
        ganttChartWindow = new GanttChartWindow();
        ganttChartWindow->show();

        if (live) {
            pauseResumeButton = new QPushButton("Pause", this);
            mainLayout->addWidget(pauseResumeButton);
            connect(pauseResumeButton, &QPushButton::clicked, this, &RoundRobinWindow::togglePause);
        }

        QThread* thread = new QThread;
        scheduler = new RoundRobinScheduler(timeQuantum, processes);
        scheduler->moveToThread(thread);

        connect(thread, &QThread::started, [this]() {
            connect(scheduler, &RoundRobinScheduler::updateGanttChart,
                    this, &RoundRobinWindow::handleGanttChartUpdate, Qt::QueuedConnection);
            connect(scheduler, &RoundRobinScheduler::statsCalculated,
                    ganttChartWindow, &GanttChartWindow::setStats, Qt::QueuedConnection);
            scheduler->schedule(live);
        });

        connect(scheduler, &RoundRobinScheduler::finished, thread, &QThread::quit);
        connect(scheduler, &RoundRobinScheduler::finished, scheduler, &RoundRobinScheduler::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
    }

    void updateArrivalTime() {
        if (scheduler) {
            minArrivalTime = scheduler->getTim() + 2;
            arrivalTimeEdit->setText(QString::number(minArrivalTime));
            arrivalTimeEdit->setValidator(new QIntValidator(minArrivalTime, INT_MAX, this));
        }
    }

    void endLive() {
        live = false;
        endLiveButton->setEnabled(false);
        timer->stop();
        clearPage();
        if (scheduler) {
            scheduler->setPaused(false);
            QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                      Q_ARG(bool, false));
        }
    }

    void clearPage() {
        QLayoutItem* item;
        while ((item = mainLayout->takeAt(0)) != nullptr) {
            if(item->widget()) {
                delete item->widget();
            }
            delete item;
        }
    }

private:
    QVBoxLayout* mainLayout;
    QLabel* idLabel;
    QLineEdit* burstTimeEdit;
    QLineEdit* arrivalTimeEdit;
    QLineEdit* quantumEdit;
    std::vector<Process> processes;
    int processCount;
    int timeQuantum;
    QCheckBox* liveCheckBox;
    QPushButton* startButton;
    QPushButton* endLiveButton;
    QPushButton* pauseResumeButton;
    bool live;
    bool isPaused;
    int minArrivalTime;
    QTimer* timer;
    RoundRobinScheduler* scheduler;
    GanttChartWindow* ganttChartWindow;
};

class SJFNonPreemptivewindow : public QWidget {
public:
    SJFNonPreemptivewindow(QWidget* parent = nullptr)
        : QWidget(parent), processCount(1), live(false), isPaused(false), scheduler(nullptr) {
        setWindowTitle("NonPreemptive Priority Window");

        mainLayout = new QVBoxLayout(this);
        QFormLayout* formLayout = new QFormLayout();

        idLabel = new QLabel(QString::number(processCount));
        burstTimeEdit = new QLineEdit();
        //priorityEdit = new QLineEdit();
        arrivalTimeEdit = new QLineEdit();

        formLayout->addRow("ID:", idLabel);
        formLayout->addRow("Burst Time:", burstTimeEdit);
        //formLayout->addRow("Priority:", priorityEdit);
        formLayout->addRow("Arrival Time:", arrivalTimeEdit);
        minArrivalTime=0;
        QPushButton* addButton = new QPushButton("Add Process");
        connect(addButton, &QPushButton::clicked, this, &SJFNonPreemptivewindow::addProcess);

        liveCheckBox = new QCheckBox("Live");
        connect(liveCheckBox, &QCheckBox::toggled, this, &SJFNonPreemptivewindow::toggleLive);

        startButton = new QPushButton("Start");
        connect(startButton, &QPushButton::clicked, this, &SJFNonPreemptivewindow::startScheduling);

        endLiveButton = new QPushButton("End Live");
        endLiveButton->setEnabled(false);
        connect(endLiveButton, &QPushButton::clicked, this, &SJFNonPreemptivewindow::endLive);

        // Initially, the pause/resume button is not created.
        pauseResumeButton = nullptr;

        mainLayout->addLayout(formLayout);
        mainLayout->addWidget(addButton);
        mainLayout->addWidget(liveCheckBox);
        mainLayout->addWidget(startButton);
        mainLayout->addWidget(endLiveButton);

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &SJFNonPreemptivewindow::updateArrivalTime);
    }

    // Slot to update the Gantt chart.
    void handleGanttChartUpdate(int pid, int currentTime) {
        ganttChartWindow->addBlock(pid, currentTime,processes);
    }

private slots:
    void addProcess() {
        QString arrivalText = arrivalTimeEdit->text();
        QString burstText = burstTimeEdit->text();
       // QString priorityText = priorityEdit->text();

        int burstTime = burstText.toInt();
        //int priority = priorityText.toInt();
        int arrivalTime = arrivalText.toInt();

        bool hasError = false;

        if (arrivalText.isEmpty() || arrivalTime < minArrivalTime) {
            arrivalTimeEdit->setText(QString::number(minArrivalTime));
            hasError = true;
        }



        if (burstText.isEmpty() || burstTime < 1) {
            burstTimeEdit->setText("1");
            hasError = true;
        }

        if (hasError)
            return;  // Return now so the user can correct the entries

        processes.push_back(Process(processCount, burstTime,  arrivalTime));
        processCount++;

        idLabel->setText(QString::number(processCount));
        burstTimeEdit->clear();
        //priorityEdit->clear();
        arrivalTimeEdit->clear();
    }

    void toggleLive(bool checked) {
        live = checked;
        endLiveButton->setEnabled(live);
    }

    // Slot to toggle pause/resume functionality.
    void togglePause() {
        if (!isPaused) {
            // Pause the scheduler and stop the timer.
            isPaused = true;
            pauseResumeButton->setText("Resume");
            if (scheduler) {
                scheduler->setPaused(true);
                QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                          Q_ARG(bool, true));
            }
            timer->stop();
        } else {
            // Resume the scheduler and restart the timer.
            isPaused = false;
            pauseResumeButton->setText("Pause");
            if (scheduler) {
                scheduler->setPaused(false);
                QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                          Q_ARG(bool, false));
            }
            timer->start(800);
        }
    }

signals:
    void updateGanttChart(int pid, int currentTime);
    void startScheduling() {
        if (!live) {
            clearPage();
        } else {
            liveCheckBox->hide();
            // Hide the start button so it won't be clicked again.
            startButton->hide();
            timer->start(800);
        }
        ganttChartWindow = new GanttChartWindow();
        ganttChartWindow->show();

        // If live mode is enabled, create and add the pause/resume button.
        if (live) {
            pauseResumeButton = new QPushButton("Pause", this);
            mainLayout->addWidget(pauseResumeButton);
            connect(pauseResumeButton, &QPushButton::clicked, this, &SJFNonPreemptivewindow::togglePause);
        }

        QThread* thread = new QThread;
        // Assign the scheduler as a member variable.
        scheduler = new SJFNonPreemptiveScheduler(processes);
        scheduler->moveToThread(thread);

        connect(thread, &QThread::started, [this]() {
            connect(scheduler, &SJFNonPreemptiveScheduler::updateGanttChart,
                    this, &SJFNonPreemptivewindow::handleGanttChartUpdate, Qt::QueuedConnection);
            connect(scheduler, &SJFNonPreemptiveScheduler::statsCalculated,
                    ganttChartWindow, &GanttChartWindow::setStats, Qt::QueuedConnection);
            scheduler->schedule(live);
        });

        connect(scheduler, &SJFNonPreemptiveScheduler::finished, thread, &QThread::quit);
        connect(scheduler, &SJFNonPreemptiveScheduler::finished, scheduler, &SJFNonPreemptiveScheduler::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
    }

    void updateArrivalTime() {
        if (scheduler) {
            minArrivalTime = scheduler->getTim() + 2;
            arrivalTimeEdit->setText(QString::number(minArrivalTime));
            arrivalTimeEdit->setValidator(new QIntValidator(minArrivalTime, INT_MAX, this));
        }
    }

    void endLive() {
        live = false;
        endLiveButton->setEnabled(false);
        timer->stop();
        clearPage();
        if (scheduler) {
            scheduler->setPaused(false);
            QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                      Q_ARG(bool, false));
        }
    }

    void clearPage() {
        QLayoutItem* item;
        while ((item = mainLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
    }

private:
    QVBoxLayout* mainLayout;
    QLabel* idLabel;
    QLineEdit* burstTimeEdit;
    //QLineEdit* priorityEdit;
    QLineEdit* arrivalTimeEdit;
    std::vector<Process> processes;
    int processCount;
    QCheckBox* liveCheckBox;
    QPushButton* startButton;
    QPushButton* endLiveButton;
    bool live;
    int minArrivalTime;
    QTimer* timer;
    SJFNonPreemptiveScheduler* scheduler;
    GanttChartWindow* ganttChartWindow;
    // Pause/resume button and flag.
    QPushButton* pauseResumeButton;
    bool isPaused;
};

class SJFPreemptivewindow : public QWidget {
public:
    SJFPreemptivewindow(QWidget* parent = nullptr)
        : QWidget(parent), processCount(1), live(false), isPaused(false), scheduler(nullptr) {
        setWindowTitle("NonPreemptive Priority Window");

        mainLayout = new QVBoxLayout(this);
        QFormLayout* formLayout = new QFormLayout();

        idLabel = new QLabel(QString::number(processCount));
        burstTimeEdit = new QLineEdit();
        //priorityEdit = new QLineEdit();
        arrivalTimeEdit = new QLineEdit();

        formLayout->addRow("ID:", idLabel);
        formLayout->addRow("Burst Time:", burstTimeEdit);
        //formLayout->addRow("Priority:", priorityEdit);
        formLayout->addRow("Arrival Time:", arrivalTimeEdit);
        minArrivalTime=0;
        QPushButton* addButton = new QPushButton("Add Process");
        connect(addButton, &QPushButton::clicked, this, &SJFPreemptivewindow::addProcess);

        liveCheckBox = new QCheckBox("Live");
        connect(liveCheckBox, &QCheckBox::toggled, this, &SJFPreemptivewindow::toggleLive);

        startButton = new QPushButton("Start");
        connect(startButton, &QPushButton::clicked, this, &SJFPreemptivewindow::startScheduling);

        endLiveButton = new QPushButton("End Live");
        endLiveButton->setEnabled(false);
        connect(endLiveButton, &QPushButton::clicked, this, &SJFPreemptivewindow::endLive);

        // Initially, the pause/resume button is not created.
        pauseResumeButton = nullptr;

        mainLayout->addLayout(formLayout);
        mainLayout->addWidget(addButton);
        mainLayout->addWidget(liveCheckBox);
        mainLayout->addWidget(startButton);
        mainLayout->addWidget(endLiveButton);

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &SJFPreemptivewindow::updateArrivalTime);
    }

    // Slot to update the Gantt chart.
    void handleGanttChartUpdate(int pid, int currentTime) {
        ganttChartWindow->addBlock(pid, currentTime,processes);
    }

private slots:
    void addProcess() {
        QString arrivalText = arrivalTimeEdit->text();
        QString burstText = burstTimeEdit->text();
        // QString priorityText = priorityEdit->text();

        int burstTime = burstText.toInt();
        //int priority = priorityText.toInt();
        int arrivalTime = arrivalText.toInt();

        bool hasError = false;

        if (arrivalText.isEmpty() || arrivalTime < minArrivalTime) {
            arrivalTimeEdit->setText(QString::number(minArrivalTime));
            hasError = true;
        }



        if (burstText.isEmpty() || burstTime < 1) {
            burstTimeEdit->setText("1");
            hasError = true;
        }

        if (hasError)
            return;  // Return now so the user can correct the entries

        processes.push_back(Process(processCount, burstTime,  arrivalTime));
        processCount++;

        idLabel->setText(QString::number(processCount));
        burstTimeEdit->clear();
        //priorityEdit->clear();
        arrivalTimeEdit->clear();
    }

    void toggleLive(bool checked) {
        live = checked;
        endLiveButton->setEnabled(live);
    }

    // Slot to toggle pause/resume functionality.
    void togglePause() {
        if (!isPaused) {
            // Pause the scheduler and stop the timer.
            isPaused = true;
            pauseResumeButton->setText("Resume");
            if (scheduler) {
                scheduler->setPaused(true);
                QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                          Q_ARG(bool, true));
            }
            timer->stop();
        } else {
            // Resume the scheduler and restart the timer.
            isPaused = false;
            pauseResumeButton->setText("Pause");
            if (scheduler) {
                scheduler->setPaused(false);
                QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                          Q_ARG(bool, false));
            }
            timer->start(800);
        }
    }

signals:
    void updateGanttChart(int pid, int currentTime);
    void startScheduling() {
        if (!live) {
            clearPage();
        } else {
            liveCheckBox->hide();
            // Hide the start button so it won't be clicked again.
            startButton->hide();
            timer->start(800);
        }
        ganttChartWindow = new GanttChartWindow();
        ganttChartWindow->show();

        // If live mode is enabled, create and add the pause/resume button.
        if (live) {
            pauseResumeButton = new QPushButton("Pause", this);
            mainLayout->addWidget(pauseResumeButton);
            connect(pauseResumeButton, &QPushButton::clicked, this, &SJFPreemptivewindow::togglePause);
        }

        QThread* thread = new QThread;
        // Assign the scheduler as a member variable.
        scheduler = new SJFPreemptiveScheduler(processes);
        scheduler->moveToThread(thread);

        connect(thread, &QThread::started, [this]() {
            connect(scheduler, &SJFPreemptiveScheduler::updateGanttChart,
                    this, &SJFPreemptivewindow::handleGanttChartUpdate, Qt::QueuedConnection);
            connect(scheduler, &SJFPreemptiveScheduler::statsCalculated,
                    ganttChartWindow, &GanttChartWindow::setStats, Qt::QueuedConnection);
            scheduler->schedule(live);
        });

        connect(scheduler, &SJFPreemptiveScheduler::finished, thread, &QThread::quit);
        connect(scheduler, &SJFPreemptiveScheduler::finished, scheduler, &SJFPreemptiveScheduler::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
    }

    void updateArrivalTime() {
        if (scheduler) {
            minArrivalTime = scheduler->getTim() + 2;
            arrivalTimeEdit->setText(QString::number(minArrivalTime));
            arrivalTimeEdit->setValidator(new QIntValidator(minArrivalTime, INT_MAX, this));
        }
    }

    void endLive() {
        live = false;
        endLiveButton->setEnabled(false);
        timer->stop();
        clearPage();
        if (scheduler) {
            scheduler->setPaused(false);
            QMetaObject::invokeMethod(scheduler, "setPaused", Qt::QueuedConnection,
                                      Q_ARG(bool, false));
        }
    }

    void clearPage() {
        QLayoutItem* item;
        while ((item = mainLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
    }

private:
    QVBoxLayout* mainLayout;
    QLabel* idLabel;
    QLineEdit* burstTimeEdit;
    //QLineEdit* priorityEdit;
    QLineEdit* arrivalTimeEdit;
    std::vector<Process> processes;
    int processCount;
    QCheckBox* liveCheckBox;
    QPushButton* startButton;
    QPushButton* endLiveButton;
    bool live;
    int minArrivalTime;
    QTimer* timer;
    SJFPreemptiveScheduler* scheduler;
    GanttChartWindow* ganttChartWindow;
    // Pause/resume button and flag.
    QPushButton* pauseResumeButton;
    bool isPaused;
};



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::openNewWindow);
    ui->comboBox->addItem("Preemptive Priority");
    ui->comboBox->addItem("NonPreemptive Priority");
    ui->comboBox->addItem("First-Come-First-Served");
    ui->comboBox->addItem("Round Robin");
    ui->comboBox->addItem("Shortest Job First(NonPreemptive)");
    ui->comboBox->addItem("Shortest Job First(Preemptive)");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openNewWindow()
{
    QString selectedOption = ui->comboBox->currentText();

    if (selectedOption == "Preemptive Priority") {
        PreemptiveWindow* newWindow = new PreemptiveWindow();
        newWindow->show();
    } else if (selectedOption == "NonPreemptive Priority") {
        NonPreemptiveWindow* newWindow = new NonPreemptiveWindow();
        newWindow->show();
    } else if (selectedOption == "First-Come-First-Served") {
        FCFSWindow* newWindow = new FCFSWindow();
        newWindow->show();
    } else if (selectedOption == "Round Robin") {
        RoundRobinWindow* newWindow = new RoundRobinWindow();
        newWindow->show();
    }
    else if (selectedOption == "Shortest Job First(NonPreemptive)") {
        SJFNonPreemptivewindow* newWindow = new SJFNonPreemptivewindow();
        newWindow->show();
    }
    else if (selectedOption == "Shortest Job First(Preemptive)") {
        SJFPreemptivewindow* newWindow = new SJFPreemptivewindow();
        newWindow->show();
    }
}

