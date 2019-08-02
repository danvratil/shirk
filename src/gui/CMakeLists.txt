set(shirk_gui_SRCS
    mainwindow.cpp
)

add_library(shirk_gui STATIC ${shirk_gui_SRCS})
target_link_libraries(shirk_gui
    PUBLIC
    shirk_core
    Qt5::Core
    Qt5::Widgets
)
