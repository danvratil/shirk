#include "rtmeventlistener.h"
#include "rtmcontroller.h"
#include "core_debug.h"
#include "slackapi/events.h"
#include <qloggingcategory.h>

using namespace Shirk::Core;

RTMEventListener::RTMEventListener(RTMController &controller)
    : mController(controller)
{}

RTMEventListener::~RTMEventListener()
{
    mController.unsubscribeListener(this);
}

void RTMEventListener::rtmEvent(SlackAPI::RTM::Event *event)
{
    using Type = SlackAPI::RTM::EventType;

    switch (event->eventType()) {
    case Type::Message:
        rtmMessageEvent(static_cast<SlackAPI::RTM::MessageEvent*>(event));
        break;
    case Type::MemberJoinedChannel:
        rtmMemberJoinedChannelEvent(static_cast<SlackAPI::RTM::MemberJoinedChannelEvent*>(event));
        break;
    default:
        qCWarning(LOG_CORE) << "RTM event " << event->eventType() << " is not supported";
        Q_ASSERT(false);
        break;
    }
}

void RTMEventListener::subscribeTo(SlackAPI::RTM::EventType type)
{
    mController.subscribeListener(this, type);
}

#define DEFAULT_EVENT_HANDLER(event) \
    void RTMEventListener::rtm##event(SlackAPI::RTM::event *) \
    { \
        qCCritical(LOG_CORE, "Default implementation of " #event " event handler called!"); \
    }

DEFAULT_EVENT_HANDLER(MessageEvent)
DEFAULT_EVENT_HANDLER(MemberJoinedChannelEvent)

