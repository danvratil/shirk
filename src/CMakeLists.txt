add_subdirectory(utils)
add_subdirectory(api)
add_subdirectory(core)
add_subdirectory(gui)

set(shirk_SRCS
    main.cpp
)

add_executable(shirk ${shirk_SRCS})
target_link_libraries(shirk
    shirk_api
    shirk_core
    shirk_gui
    shirk_utils
    Qt5::Core
    KF5::DBusAddons
)
target_include_directories(shirk
    PRIVATE $<BUILD_INTERFACE:${shirk_BINARY_DIR}>
)

install(TARGETS shirk DESTINATION ${BIN_INSTALL_DIR})
