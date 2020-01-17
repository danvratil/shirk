set(shirk_gui_SRCS
    mainwindow.cpp
    mainwindow.ui
    addteampage.cpp
    addteampage.ui
    teampage.cpp
)

ecm_qt_declare_logging_category(shirk_gui_SRCS
    HEADER gui_debug.h
    IDENTIFIER LOG_GUI
    CATEGORY_NAME cz.dvratil.shirk.gui
)

add_library(shirk_gui STATIC ${shirk_gui_SRCS})
target_include_directories(shirk_gui
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/shirk_gui_autogen/include>
)
target_link_libraries(shirk_gui
    PUBLIC
    shirk_core
    Qt5::Core
    Qt5::Widgets
)
