include_directories(
    ${shirk_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(core)
add_subdirectory(gui)

set(shirk_SRCS
    main.cpp
)

add_executable(shirk ${shirk_SRCS})
target_link_libraries(shirk
    shirk_core
    shirk_gui
    Qt5::Core
    KF5::DBusAddons
)

install(TARGETS shirk DESTINATION ${BIN_INSTALL_DIR})
