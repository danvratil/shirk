set(shirk_utils_SRCS)

add_library(shirk_utils STATIC ${shirk_utils_SRCS})
target_link_libraries(shirk_utils
    PUBLIC
    Qt5::Core
)
target_include_directories(shirk_utils
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
