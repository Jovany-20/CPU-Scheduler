#include "GanttChartWindow.h"
#include <QFrame>
#include <QScrollBar>
#include <QTimer>

GanttChartWindow::GanttChartWindow(QWidget* parent) : QWidget(parent) {
    setWindowTitle("Gantt Chart");

    // Create the container and layout that holds the blocks.
    container = new QWidget(this);
    layout = new QHBoxLayout(container);
    layout->setSpacing(0); // No space between blocks
    container->setLayout(layout);

    // Create the scroll area with the container.
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(container);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Create the stats label for showing avg values.
    statsLabel = new QLabel(this);
    statsLabel->setAlignment(Qt::AlignCenter);
    statsLabel->setText("Statistics will be shown here after scheduling finishes.");

    // Layout combining the scroll area and stats label.
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(statsLabel);
    setLayout(mainLayout);
}

void GanttChartWindow::addBlock(int pid, int startTime) {
    // Check if we are currently scrolled to the far right.
    QScrollBar* hScrollBar = scrollArea->horizontalScrollBar();
    bool autoScroll = (hScrollBar->value() == hScrollBar->maximum());

    // Create a vertical layout for the block and its time label.
    QVBoxLayout* blockLayout = new QVBoxLayout();
    blockLayout->setSpacing(0);

    // Create the block label.
    QString blockText = (pid == -1) ? "Idle" : QString("P%1").arg(pid);
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

    // Add this block's layout to the main horizontal layout.
    layout->addLayout(blockLayout);

    // Auto-scroll if the scrollbar was already at the end.
    if (autoScroll) {
        QTimer::singleShot(0, this, [hScrollBar]() {
            hScrollBar->setValue(hScrollBar->maximum());
        });
    }
}

void GanttChartWindow::setStats(double avgWaitingTime, double avgTurnaroundTime) {
    QString stats = QString("Average Waiting Time: %1\nAverage Turnaround Time: %2")
    .arg(avgWaitingTime)
        .arg(avgTurnaroundTime);
    statsLabel->setText(stats);
}
