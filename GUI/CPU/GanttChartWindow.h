#ifndef GANTTCHARTWINDOW_H
#define GANTTCHARTWINDOW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>

class GanttChartWindow : public QWidget {
    Q_OBJECT

public:
    explicit GanttChartWindow(QWidget* parent = nullptr);
    void addBlock(int pid, int startTime);
    void setStats(double avgWaitingTime, double avgTurnaroundTime);

private:
    QHBoxLayout* layout;
    QWidget* container;
    QScrollArea* scrollArea;
    QLabel* statsLabel; // For printing the statistics
    const int fixedBlockWidth = 300; // Fixed block width
};

#endif // GANTTCHARTWINDOW_H
