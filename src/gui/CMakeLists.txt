set(shirk_gui_SRCS
    mainwindow.cpp
    mainwindow.ui
    addteampage.cpp
    addteampage.ui
    teampage.cpp
)

add_library(shirk_gui STATIC ${shirk_gui_SRCS})
target_include_directories(shirk_gui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/shirk_gui_autogen/include>
)
target_link_libraries(shirk_gui
    PUBLIC
    shirk_core
    Qt5::Core
    Qt5::Widgets
)
