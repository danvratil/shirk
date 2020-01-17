set(shirk_api_SRCS
    conversations.cpp
    oauth.cpp
    teaminfo.cpp
)

add_library(shirk_api STATIC ${shirk_api_SRCS})
target_link_libraries(shirk_api
    PUBLIC
    shirk_utils
    Qt5::Core
)
target_include_directories(shirk_api
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>;$<BUILD_INTERFACE:${shirk_SOURCE_DIR}/src>
)

