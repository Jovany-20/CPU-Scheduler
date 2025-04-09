#include "Process.h"
#ifndef COMPARE_PRIORITY_H
#define COMPARE_PRIORITY_H
struct ComparePriority {
    bool operator()(Process& p1, Process& p2) {
        return p1.getPriority() > p2.getPriority();
    }
};
#endif // COMPARE_PRIORITY_H