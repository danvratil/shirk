#include "future.h"

#include <algorithm>

using namespace Shirk::Core;

void detail::SharedStateBase::addObserver(SharedStateObserver *observer)
{
    observers.push_back(observer);
}

void detail::SharedStateBase::removeObserver(SharedStateObserver *observer)
{
    std::erase_if(observers, std::bind(std::equal_to<void>{}, observer, std::placeholders::_1));
}

FutureWatcher::FutureWatcher(Callback &&cb)
    : mCallback(std::move(cb))
{}

FutureWatcher::~FutureWatcher()
{
    std::for_each(mStates.begin(), mStates.end(), [this](auto &state) {
        state->removeObserver(this);
    });
}

void FutureWatcher::addSharedState(std::shared_ptr<detail::SharedStateBase> state)
{
    mStates.push_back(std::move(state));
    mStates.back()->addObserver(this);
}

bool FutureWatcher::isFinished() const
{
    return mErrors.size() + mFinished == mStates.size();
}

void FutureWatcher::notify(detail::SharedStateBase *state)
{
    if (state->error.has_value()) {
        mErrors.push_back(state->error.value());
    } else {
        ++mFinished;
    }

    if (isFinished() && mCallback.has_value()) {
        (*mCallback)();
    }
}
