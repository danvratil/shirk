cmake_minimum_required(VERSION 3.11)

set(SHIRK_VERSION "0.1")
project(shirk VERSION ${SHIRK_VERSION})

# Setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ECM setup
set(KF5_REQUIRED_VERSION "5.50.0")
find_package(ECM ${KF5_REQUIRED_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH})

# Includes
include(ECMSetupVersion)
include(FeatureSummary)

ecm_setup_version(PROJECT
    VARIABLE_PREFIX SHIRK
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/shirk_version.h"
)

# Find packages
set(QT_REQUIRED_VERSION "5.11.0")

find_package(Qt5Core ${QT_REQUIRED_VERSION} REQUIRED)
find_package(Qt5Network ${QT_REQUIRED_VERSION} REQUIRED)
find_package(Qt5Test ${QT_REQUIRED_VERSION} REQUIRED)
find_package(Qt5Widgets ${QT_REQUIRED_VERSION} REQUIRED)

find_package(KF5DBusAddons ${KF5_REQUIRED_VERSION} REQUIRED)
find_package(KF5Notifications ${KF5_REQUIRED_VERSION} REQUIRED)


if (BUILD_TESTING)
    add_subdirectory(autotests)
endif ()

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
