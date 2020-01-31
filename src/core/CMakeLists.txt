configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/shirk_config.h)

set(shirk_core_SRCS
    authcontroller.cpp
    config.cpp
    conversation.cpp
    conversationsmodel.cpp
    future.cpp
    iconloader.cpp
    networkdispatcher.cpp
    rtmcontroller.cpp
    rtmeventlistener.cpp
    rtmsocket.cpp
    team.cpp
    teamcontroller.cpp
    teamsmodel.cpp
    user.cpp
    usermanager.cpp
)

ecm_qt_declare_logging_category(shirk_core_SRCS
    HEADER core_debug.h
    IDENTIFIER LOG_CORE
    CATEGORY_NAME cz.dvratil.shirk.core
)

add_library(shirk_core STATIC ${shirk_core_SRCS})
target_link_libraries(shirk_core
    PUBLIC
    shirk_utils
    shirk_slackapi
    Qt5::Core
    Qt5::Gui
    Qt5::Network
    PRIVATE
    Qt5::WebSockets
)
target_include_directories(shirk_core
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PUBLIC
    $<BUILD_INTERFACE:${shirk_SOURCE_DIR}/src>
)
