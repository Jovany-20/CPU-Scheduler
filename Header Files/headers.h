#ifndef HEADERS_H
#define HEADERS_H

#include <iostream>
#include <vector>
#include <queue>
#include "Process.h"
#include "ComparePriority.h"
#include <thread>
#include <chrono>
#include <fstream>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <sys/types.h>
#include <cstdlib>
#include <mutex>
using namespace std;

#endif // HEADERS_H

