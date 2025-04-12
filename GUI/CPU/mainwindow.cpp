#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "process.h"
#include "PreemptivePriorityScheduler.h"
#include "NonPreemptivePriorityScheduler.h"
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
            scheduler = new PreemptivePriorityScheduler(processes);
            scheduler->schedule(live);
        } else {
            liveCheckBox->hide();
            scheduler = new PreemptivePriorityScheduler(processes);
            timer->start(800);

            SchedulerThread* thread = new SchedulerThread([this]() {
                scheduler->schedule(live);
            });
            thread->start();
        }
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
        // Remove all widgets from the layout
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
};

class NonPreemptiveWindow : public QWidget {
public:
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
            scheduler = new NonPreemptivePriorityScheduler(processes);
            scheduler->schedule(live);
        } else {
            liveCheckBox->hide();
            scheduler = new NonPreemptivePriorityScheduler(processes);
            timer->start(800);

            SchedulerThread* thread = new SchedulerThread([this]() {
                scheduler->schedule(live);
            });
            thread->start();
        }
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
        // Remove all widgets from the layout
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
        PreemptiveWindow* newWindow = new PreemptiveWindow();
        newWindow->show();
    } else if (selectedOption == "NonPreemptive Priority") {
        NonPreemptiveWindow* newWindow = new NonPreemptiveWindow();
        newWindow->show();
    }
}
