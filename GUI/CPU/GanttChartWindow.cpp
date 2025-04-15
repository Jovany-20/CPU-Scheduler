#include "GanttChartWindow.h"
#include <QFrame>
#include <QScrollBar>
#include <QTimer>
#include <QHeaderView>
#include <QCoreApplication>

GanttChartWindow::GanttChartWindow(QWidget* parent) : QWidget(parent) {
    // Full screen by default.
    setWindowState(Qt::WindowMaximized);
    setWindowTitle("Gantt Chart");

    // --- Create the Gantt chart area ---
    container = new QWidget(this);
    layout = new QHBoxLayout(container);
    layout->setSpacing(0);
    // Remove any margins so blocks touch each other.
    layout->setContentsMargins(0, 0, 0, 0);
    container->setLayout(layout);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(container);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // --- Create the stats label ---
    statsLabel = new QLabel(this);
    statsLabel->setAlignment(Qt::AlignCenter);
    statsLabel->setText("Statistics will be shown here after scheduling finishes.");

    // --- Create the process table ---
    processTable = new QTableWidget(this);
    processTable->setColumnCount(2);
    processTable->setHorizontalHeaderLabels(QStringList() << "Process" << "Remaining Time");
    processTable->horizontalHeader()->setStretchLastSection(true);
    processTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    processTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    processTable->setSelectionMode(QAbstractItemView::SingleSelection);

    // --- Layout the components ---
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    // Adjust the stretch factors to assign roughly 1/3 of the height for the Gantt chart and 2/3 for the table.
    mainLayout->addWidget(scrollArea, 1);
    mainLayout->addWidget(processTable, 2);
    mainLayout->addWidget(statsLabel, 0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);
}

void GanttChartWindow::addBlock(int pid, int startTime, std::vector<Process>& processes) {
    // Check if we are scrolled completely to the right.
    QScrollBar* hScrollBar = scrollArea->horizontalScrollBar();
    bool autoScroll = (hScrollBar->value() == hScrollBar->maximum());

    // Create a vertical layout for the block and its time label.
    QVBoxLayout* blockLayout = new QVBoxLayout();
    blockLayout->setSpacing(0);
    blockLayout->setContentsMargins(0, 0, 0, 0);

    // If pid is -1 or negative, display Idle; otherwise, display the process id.
    QString blockText;
    if (pid == -1 || pid < 0) {
        blockText = "Idle";
    } else {
        blockText = QString("P%1").arg(pid);
    }

    // Create the block label.
    QLabel* block = new QLabel(blockText, this);
    block->setFrameStyle(QFrame::Box | QFrame::Plain);
    block->setAlignment(Qt::AlignCenter);
    block->setFixedWidth(fixedBlockWidth);

    // Create the time label.
    QLabel* timeLabel = new QLabel(QString::number(startTime), this);
    timeLabel->setAlignment(Qt::AlignLeft);
    timeLabel->setFixedWidth(fixedBlockWidth);

    blockLayout->addWidget(block);
    blockLayout->addWidget(timeLabel);

    // Add the block's layout to the main horizontal layout.
    layout->addLayout(blockLayout);

    // Update the container's minimum width to exactly fit all blocks.
    int blockCount = layout->count();
    container->setMinimumWidth(blockCount * fixedBlockWidth);

    // Process pending events to ensure the layout updates.
    QCoreApplication::processEvents();

    // Auto-scroll to the end if already scrolled completely right.
    if (autoScroll) {
        QTimer::singleShot(0, this, [this, hScrollBar]() {
            hScrollBar->setValue(hScrollBar->maximum());
        });
    }

    // Update the process table.
    updateProcessTable(processes);
}

void GanttChartWindow::setStats(double avgWaitingTime, double avgTurnaroundTime) {
    QString stats = QString("Average Waiting Time: %1\nAverage Turnaround Time: %2")
    .arg(avgWaitingTime)
        .arg(avgTurnaroundTime);
    statsLabel->setText(stats);
}

void GanttChartWindow::updateProcessTable(const std::vector<Process>& processes) {
    processTable->clearContents();
    processTable->setRowCount(static_cast<int>(processes.size()));

    for (int i = 0; i < static_cast<int>(processes.size()); ++i) {
        const Process& proc = processes[i];
        // Get the remaining time from the process.
        int remainingTime = proc.getRemainingTime();

        // Create table items.
        QTableWidgetItem* processItem = new QTableWidgetItem(QString("P%1").arg(proc.getPid()));
        QTableWidgetItem* remainingItem = new QTableWidgetItem(QString::number(remainingTime));

        processTable->setItem(i, 0, processItem);
        processTable->setItem(i, 1, remainingItem);
    }

    // Force the table to update.
    processTable->viewport()->update();
}
