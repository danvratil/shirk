#include "future.h"

#include <algorithm>

using namespace Shirk::Core;

Promise::Promise()
    : mState(std::make_shared<SharedState>())
{}

Future Promise::getFuture()
{
    Q_ASSERT(mState);
    return Future(mState);
}

void Promise::setError(const QString &error)
{
    Q_ASSERT(mState);
    mState->error = error;
    mState->hasResult = true;
    notify();
}

void Promise::setResult()
{
    Q_ASSERT(mState);
    mState->hasResult = true;
    notify();
}

void Promise::notify()
{
    // Make a copy of the vector: calling notify() might result in the future
    // removing itself from SharedState::futures vector and thus invalidating
    // iterators
    auto futures = mState->futures;
    std::for_each(futures.cbegin(), futures.cend(), std::mem_fn(&Future::notify));

    mState.reset();
}

void Promise::SharedState::addFuture(Future *future)
{
    futures.push_back(future);
}

void Promise::SharedState::removeFuture(Future *future)
{
    std::erase_if(futures, [future](auto *f) { return f == future; });
}


Future::Future(std::shared_ptr<Promise::SharedState> state)
    : mState(std::move(state))
{
    mState->addFuture(this);
}

Future::Future(Future &&other)
{
    *this = std::move(other);
}

Future &Future::operator=(Future &&other)
{
    if (other.mState) {
        other.mState->removeFuture(&other);
    }
    std::swap(mState, other.mState);
    std::swap(mContinuation, other.mContinuation);
    std::swap(mWatcher, other.mWatcher);
    if (mState) {
        mState->addFuture(this);
    }
    return *this;
}

Future::~Future()
{
    if (mState) {
        mState->removeFuture(this);
    }
}

std::optional<QString> Future::error() const
{
    return mState ? mState->error : std::nullopt;
}

bool Future::isFinished() const
{
    return mState && (mState->error.has_value() || mState->hasResult);
}

Future Future::then(ContinuationCb &&cont)
{
    auto nextPromise = std::make_shared<Promise>();
    mContinuation.emplace([promise = nextPromise, cont = std::move(cont)]() {
        cont();
        promise->setResult();
    });

    return nextPromise->getFuture();
}

void Future::notify()
{
    if (mContinuation.has_value()) {
        (*mContinuation)();
    }

    if (mWatcher != nullptr) {
        mWatcher->notify(this);
    }
}

FutureWatcher::FutureWatcher(Callback &&cb)
    : mCallback(std::move(cb))
{}

FutureWatcher::FutureWatcher(Future &&future)
{
    Q_ASSERT_X(!isFinished(), __FUNCTION__, "Adding future to a finished FutureWatcher is not alloed");
    future.mWatcher = this;
    mFutures.push_back(std::move(future));
}

FutureWatcher::FutureWatcher(Future &&future, Callback &&cb)
    : mCallback(std::move(cb))
{
    Q_ASSERT_X(!isFinished(), __FUNCTION__, "Adding future to a finished FutureWatcher is not allowed");
    future.mWatcher = this;
    mFutures.push_back(std::move(future));
}

void FutureWatcher::operator()(Future &&future)
{
    Q_ASSERT_X(!isFinished(), __FUNCTION__, "Adding future to a finished FutureWatcher");
    future.mWatcher = this;
    mFutures.push_back(std::move(future));
}

bool FutureWatcher::isFinished() const
{
    return mErrors.size() + mFinished == mFutures.size();
}

void FutureWatcher::notify(Future *future)
{
    if (const auto error = future->error(); error.has_value()) {
        mErrors.push_back(error.value());
    } else {
        ++mFinished;
    }

    if (isFinished() && mCallback.has_value()) {
        (*mCallback)();
    }
}
