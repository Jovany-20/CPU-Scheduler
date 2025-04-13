#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "process.h"
#include "PreemptivePriorityScheduler.h"
#include "NonPreemptivePriorityScheduler.h"
#include "GanttChartWindow.h"
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




class SchedulerThread : public QThread {
public:
    SchedulerThread(std::function<void()> func) : func(func) {}

protected:
    void run() override {
        func();
    }

private:
    std::function<void()> func;
};

class PreemptiveWindow : public QWidget {
public:
    //GanttChartWindow* ganttChartWindow;
    PreemptiveWindow(QWidget* parent = nullptr) : QWidget(parent), processCount(1), live(false) {
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

        QPushButton* addButton = new QPushButton("Add Process");
        connect(addButton, &QPushButton::clicked, this, &PreemptiveWindow::addProcess);

        liveCheckBox = new QCheckBox("Live");
        connect(liveCheckBox, &QCheckBox::toggled, this, &PreemptiveWindow::toggleLive);

        startButton = new QPushButton("Start");
        connect(startButton, &QPushButton::clicked, this, &PreemptiveWindow::startScheduling);

        endLiveButton = new QPushButton("End Live");
        endLiveButton->setEnabled(false);
        connect(endLiveButton, &QPushButton::clicked, this, &PreemptiveWindow::endLive);

        mainLayout->addLayout(formLayout);
        mainLayout->addWidget(addButton);
        mainLayout->addWidget(liveCheckBox);
        mainLayout->addWidget(startButton);
        mainLayout->addWidget(endLiveButton);

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &PreemptiveWindow::updateArrivalTime);
    }
    void handleGanttChartUpdate(int pid, int currentTime) {
        // Update the Gantt chart here
        ganttChartWindow->addBlock(pid, currentTime);
    }
private slots:
    void addProcess() {
        int burstTime = burstTimeEdit->text().toInt();
        int priority = priorityEdit->text().toInt();
        int arrivalTime = arrivalTimeEdit->text().toInt();

        if (arrivalTime < minArrivalTime) {
            arrivalTimeEdit->setText(QString::number(minArrivalTime));
            return;
        }

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
signals:
    void updateGanttChart(int pid, int currentTime);
    void startScheduling() {
        if (!live) {
            clearPage();


        } else {
            liveCheckBox->hide();

            timer->start(800);

        }
        ganttChartWindow = new GanttChartWindow();
        ganttChartWindow->show();

        QThread* thread = new QThread;
        // Note: Instead of declaring a local scheduler, assign to your member variable if needed.
        scheduler = new PreemptivePriorityScheduler(processes);
        scheduler->moveToThread(thread);

        connect(thread, &QThread::started, [this]() {
            connect(scheduler, &PreemptivePriorityScheduler::updateGanttChart,
                    this, &PreemptiveWindow::handleGanttChartUpdate, Qt::QueuedConnection);
            // Connect the statsCalculated signal to the GanttChartWindow's setStats slot.
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
    }

    void clearPage() {
        QLayoutItem* item;
        while ((item = mainLayout->takeAt(0)) != nullptr) {
            delete item->widget();
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
    PreemptivePriorityScheduler* scheduler;
    GanttChartWindow* ganttChartWindow;
};

class NonPreemptiveWindow : public QWidget {
public:
    //GanttChartWindow* ganttChartWindow;
    NonPreemptiveWindow(QWidget* parent = nullptr) : QWidget(parent), processCount(1), live(false) {
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

        QPushButton* addButton = new QPushButton("Add Process");
        connect(addButton, &QPushButton::clicked, this, &NonPreemptiveWindow::addProcess);

        liveCheckBox = new QCheckBox("Live");
        connect(liveCheckBox, &QCheckBox::toggled, this, &NonPreemptiveWindow::toggleLive);

        startButton = new QPushButton("Start");
        connect(startButton, &QPushButton::clicked, this, &NonPreemptiveWindow::startScheduling);

        endLiveButton = new QPushButton("End Live");
        endLiveButton->setEnabled(false);
        connect(endLiveButton, &QPushButton::clicked, this, &NonPreemptiveWindow::endLive);

        mainLayout->addLayout(formLayout);
        mainLayout->addWidget(addButton);
        mainLayout->addWidget(liveCheckBox);
        mainLayout->addWidget(startButton);
        mainLayout->addWidget(endLiveButton);

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &NonPreemptiveWindow::updateArrivalTime);
    }
    void handleGanttChartUpdate(int pid, int currentTime) {
        // Update the Gantt chart here
    ganttChartWindow->addBlock(pid, currentTime);
    }
private slots:
    void addProcess() {
        int burstTime = burstTimeEdit->text().toInt();
        int priority = priorityEdit->text().toInt();
        int arrivalTime = arrivalTimeEdit->text().toInt();

        if (arrivalTime < minArrivalTime) {
            arrivalTimeEdit->setText(QString::number(minArrivalTime));
            return;
        }

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

    void startScheduling() {
        if (!live) {
            clearPage();



        } else {
            liveCheckBox->hide();

            timer->start(800);
        }
        ganttChartWindow = new GanttChartWindow();
        ganttChartWindow->show();

        QThread* thread = new QThread;
        // Note: Instead of declaring a local scheduler, assign to your member variable if needed.
        scheduler = new NonPreemptivePriorityScheduler(processes);
        scheduler->moveToThread(thread);

        connect(thread, &QThread::started, [this]() {
            connect(scheduler, &NonPreemptivePriorityScheduler::updateGanttChart,
                    this, &NonPreemptiveWindow::handleGanttChartUpdate, Qt::QueuedConnection);
            // Connect the statsCalculated signal to the GanttChartWindow's setStats slot.
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
    }

    void clearPage() {
        QLayoutItem* item;
        while ((item = mainLayout->takeAt(0)) != nullptr) {
            delete item->widget();
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
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::openNewWindow);

    ui->comboBox->addItem("Preemptive Priority");
    ui->comboBox->addItem("NonPreemptive Priority");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openNewWindow()
{
    QString selectedOption = ui->comboBox->currentText();

    if (selectedOption == "Preemptive Priority") {
        // GanttChartWindow* g = new GanttChartWindow();
        // g->show();
        // g->addBlock(0,1);
        // g->addBlock(0,2);
        // g->addBlock(0,2);
        // g->addBlock(0,2);
        // g->addBlock(0,2);
        PreemptiveWindow* newWindow = new PreemptiveWindow();
        newWindow->show();
    } else if (selectedOption == "NonPreemptive Priority") {
         NonPreemptiveWindow* newWindow = new NonPreemptiveWindow();
        newWindow->show();
    }
}

