set(shirk_slackapi_SRCS
    conversations.cpp
    files.cpp
    oauth.cpp
    events.cpp
    rtm.cpp
    teaminfo.cpp
    user.cpp
)

add_library(shirk_slackapi STATIC ${shirk_slackapi_SRCS})
target_link_libraries(shirk_slackapi
    PUBLIC
    shirk_utils
    Qt5::Core
)
target_include_directories(shirk_slackapi
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

