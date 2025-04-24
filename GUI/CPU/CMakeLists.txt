cmake_minimum_required(VERSION 3.19)
project(CPU LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

# If qt_standard_project_setup() is not recognized, you can remove it or replace it with standard setup commands
# qt_standard_project_setup()

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
    SchedulerWorker.cpp
    FCFS_Scheduler.cpp
    FCFS_Scheduler.h
    RoundRobinScheduler.h
    RoundRobinScheduler.cpp
    SJFNonPreemptiveScheduler.h
    SJFNonPreemptiveScheduler.cpp
    SJFPreemptiveScheduler.h
    SJFPreemptiveScheduler.cpp
)
if(WIN32)
    set(APP_ICON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/resources/appicon.ico")

    # Verify the icon exists (critical for debugging)
    if(NOT EXISTS "${APP_ICON_PATH}")
        message(FATAL_ERROR "Icon file missing at: ${APP_ICON_PATH}")
    else()
        message(STATUS "Using icon: ${APP_ICON_PATH}")  # Log success
    endif()

    # Method 1: Set icon directly (works for most cases)
    set_target_properties(CPU PROPERTIES
        RC_ICONS "${APP_ICON_PATH}"
        WIN32_EXECUTABLE TRUE
    )

    # Method 2: Alternative .rc file approach (more reliable)
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/app.rc" "IDI_ICON1 ICON \"${APP_ICON_PATH}\"")
    target_sources(CPU PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/app.rc")
endif()
target_link_libraries(CPU
    PRIVATE
        Qt6::Core
        Qt6::Widgets
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
