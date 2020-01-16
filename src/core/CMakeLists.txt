include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/shirk_config.h)

set(shirk_core_SRCS
    api.cpp
    authcontroller.cpp
    config.cpp
    iconloader.cpp
    networkdispatcher.cpp
    team.cpp
    teamcontroller.cpp
    teamsmodel.cpp
)

ecm_qt_declare_logging_category(shirk_core_SRCS
    HEADER core_debug.h
    IDENTIFIER LOG_CORE
    CATEGORY_NAME cz.dvratil.shirk.core
)

add_library(shirk_core STATIC ${shirk_core_SRCS})
target_link_libraries(shirk_core
    PUBLIC
    Qt5::Core
    Qt5::Gui
    Qt5::Network
)
