cmake_minimum_required(VERSION 3.11)

set(SHIRK_VERSION "0.1")
project(shirk VERSION ${SHIRK_VERSION})

# Setup
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ECM setup
set(KF5_REQUIRED_VERSION "5.50.0")
find_package(ECM ${KF5_REQUIRED_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH})

# Includes
include(CheckIncludeFile)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMSetupVersion)
include(ECMQtDeclareLoggingCategory)
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
find_package(Qt5WebSockets ${QT_REQUIRED_VERSION} REQUIRED)

find_package(KF5DBusAddons ${KF5_REQUIRED_VERSION} REQUIRED)
find_package(KF5Notifications ${KF5_REQUIRED_VERSION} REQUIRED)


add_definitions(
    -DQT_USE_STRINGBUILDER
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_USE_FAST_OPERATOR_PLUS
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic")

kde_enable_exceptions()

if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(autotests)
endif ()

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

