cmake_minimum_required(VERSION 3.19)
project(CPU LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(CPU
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ComparePriority.h
    headers.h
    NonPreemptivePriorityScheduler.h
    PreemptivePriorityScheduler.h
    Process.h
    NonPreemptivePriorityScheduler.cpp
    PreemptivePriorityScheduler.cpp
    Process.cpp
    GanttChartWindow.h
    GanttChartWindow.cpp
    SchedulerWorker.h
    SchedulerWorker.h
    SchedulerWorker.cpp
)

target_link_libraries(CPU
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS CPU
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET CPU
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
