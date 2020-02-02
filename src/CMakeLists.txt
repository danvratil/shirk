include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(3rdparty)
add_subdirectory(utils)
add_subdirectory(slackapi)
add_subdirectory(core)
add_subdirectory(gui)

set(shirk_SRCS
    main.cpp
)

add_executable(shirk ${shirk_SRCS})
target_link_libraries(shirk
    shirk_slackapi
    shirk_core
    #shirk_gui
    shirk_utils
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    KF5::DBusAddons
    KF5::Kirigami2
    KF5::Package
)
target_include_directories(shirk
    PRIVATE $<BUILD_INTERFACE:${shirk_BINARY_DIR}>
)

install(TARGETS shirk DESTINATION ${BIN_INSTALL_DIR})
