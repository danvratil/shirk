#pragma once

namespace Shirk::SlackAPI::RTM
{
enum class EventType;
class Event;
class MessageEvent;
class MemberJoinedChannelEvent;
}

namespace Shirk::Core
{

class RTMController;

class RTMEventListener
{
    friend class RTMController;
public:
    explicit RTMEventListener(RTMController &controller);
    virtual ~RTMEventListener();

    template<typename Event>
    void subscribeTo()
    {
        subscribeTo(Event::type);
    }

protected:
    virtual void rtmEvent(SlackAPI::RTM::Event *event);

    virtual void rtmMessageEvent(SlackAPI::RTM::MessageEvent *event);
    virtual void rtmMemberJoinedChannelEvent(SlackAPI::RTM::MemberJoinedChannelEvent *event);

private:
    void subscribeTo(SlackAPI::RTM::EventType type);

private:
    RTMController &mController;
};


} // namespace
