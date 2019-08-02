set(shirk_core_SRCS
    config.cpp
    teamsmodel.cpp
)

add_library(shirk_core STATIC ${shirk_core_SRCS})
target_link_libraries(shirk_core
    PUBLIC
    Qt5::Core
)
