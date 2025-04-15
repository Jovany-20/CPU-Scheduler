#ifndef GANTTCHARTWINDOW_H
#define GANTTCHARTWINDOW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QTableWidget>
#include <vector>
#include "Process.h"

class GanttChartWindow : public QWidget {
    Q_OBJECT

public:
    explicit GanttChartWindow(QWidget* parent = nullptr);
    // In addBlock, if pid is not -1, it is treated as the index in the processes vector.
    void addBlock(int pid, int startTime, std::vector<Process>& processes);
    void setStats(double avgWaitingTime, double avgTurnaroundTime);
    void updateProcessTable(const std::vector<Process>& processes);

private:
    QHBoxLayout* layout;       // Layout for the Gantt chart blocks
    QWidget* container;        // Container widget for the blocks
    QScrollArea* scrollArea;   // Scrollable area for the gantt chart blocks
    QLabel* statsLabel;        // Label for printing the statistics
    QTableWidget* processTable; // Table to display process remaining times
    const int fixedBlockWidth = 300; // Fixed width for each block
};

#endif // GANTTCHARTWINDOW_H
