#pragma once

#include <QString>
#include <QStringList>

#include <memory>
#include <functional>
#include <type_traits>

namespace Shirk::Core
{

template<typename T>
class Promise;
class FutureWatcher;

namespace detail
{

class SharedStateObserver;

struct VoidType {};

struct SharedStateBase : std::enable_shared_from_this<SharedStateBase>
{
    using ErrorType = QString;

    SharedStateBase() = default;
    SharedStateBase(const SharedStateBase &) = delete;
    SharedStateBase(SharedStateBase &&) noexcept = delete;
    SharedStateBase &operator=(const SharedStateBase &) = delete;
    SharedStateBase &operator=(SharedStateBase &&) noexcept = delete;

    void addObserver(SharedStateObserver *observer);
    void removeObserver(SharedStateObserver *observer);

    std::vector<SharedStateObserver *> observers;
    std::optional<ErrorType> error;
    bool hasResult = false;
};

template<typename T>
struct SharedState : public SharedStateBase
{
    using ResultType = T;

    std::aligned_storage_t<sizeof(ResultType), alignof(ResultType)> result{};
};

template<>
struct SharedState<void> : public SharedStateBase
{
    using ResultType = void;
};

class SharedStateObserver {
    template<typename T> friend class Promise;
protected:
    virtual void notify(detail::SharedStateBase *) {};
};

} // namespace detail

template<typename T>
class Future : public detail::SharedStateObserver
{
public:
    using ErrorType = typename detail::SharedState<T>::ErrorType;
    using ResultType = typename detail::SharedState<T>::ResultType;

    explicit Future() = default;
    Future(const Future &) = delete;
    Future &operator=(const Future &) = delete;
    Future(Future &&other) = default;
    Future &operator=(Future &&other) = default;

    ~Future() = default;

    std::optional<QString> error() const
    {
        return mState->error;
    }

    bool isFinished() const
    {
        return mState->error.has_value() || mState->hasResult;
    }

    template<typename U = ResultType, typename = std::enable_if_t<!std::is_void_v<U>>>
    U result() const
    {
        return *reinterpret_cast<U *>(&mState->result);
    }

    template<typename U = ResultType, typename = std::enable_if_t<!std::is_void_v<U>>>
    U &&result()
    {
        return std::move(*reinterpret_cast<U *>(&mState->result));
    }

private:
    template<typename U> friend class Promise;
    friend class FutureWatcher;

    Future(std::shared_ptr<detail::SharedState<T>> state)
        : mState(state)
    {}

private:
    std::shared_ptr<detail::SharedState<T>> mState;
};


template<typename T>
class Promise
{
public:
    using ErrorType = typename detail::SharedState<T>::ErrorType;
    using ResultType = typename detail::SharedState<T>::ResultType;

    explicit Promise()
        : mState(std::make_shared<detail::SharedState<T>>())
    {}

    Promise(const Promise &) = delete;
    Promise &operator=(const Promise &) = delete;
    Promise(Promise &&) = default;
    Promise &operator=(Promise &&) = default;
    ~Promise() = default;

    Future<T> getFuture()
    {
        return Future<T>(mState);
    }

    void setError(const ErrorType &error)
    {
        Q_ASSERT(mState);
        Q_ASSERT(!mState->hasResult && !mState->error.has_value());
        mState->error = error;
        notify();
    }

    template<typename U = T, typename = std::enable_if_t<!std::is_void_v<U>>>
    void setResult(const U &result)
    {
        Q_ASSERT(mState);
        Q_ASSERT(!mState->hasResult && !mState->error.has_value());
        new (reinterpret_cast<U *>(&mState->result)) U(result);
        mState->hasResult = true;
        notify();
    }

    template<typename U = T, typename = std::enable_if_t<!std::is_void_v<U>>>
    void setResult(U &&result)
    {
        Q_ASSERT(mState);
        Q_ASSERT(!mState->hasResult && !mState->error.has_value());
        new (reinterpret_cast<U *>(&mState->result)) U(std::move(result));
        mState->hasResult = true;
        notify();
    }

    template<typename U = T, typename = std::enable_if_t<std::is_void_v<U>>>
    void setResult()
    {
        Q_ASSERT(mState);
        Q_ASSERT(!mState->hasResult && !mState->error.has_value());
        mState->hasResult = true;
        notify();
    }

private:
    void notify()
    {
        auto observers = mState->observers;
        std::for_each(observers.begin(), observers.end(),
                      std::bind(&detail::SharedStateObserver::notify, std::placeholders::_1, mState.get()));
        mState.reset();
    }

private:
    friend class FutureWatcher;
    std::shared_ptr<detail::SharedState<T>> mState;
};


class FutureWatcher : public detail::SharedStateObserver
{
public:
    using Callback = std::function<void()>;

    explicit FutureWatcher() = default;
    explicit FutureWatcher(Callback &&cb);
    template<typename U>
    FutureWatcher(Future<U> &&future)
    {
        addSharedState(std::move(future.mState));
    }

    template<typename U>
    FutureWatcher(Future<U> &&future, Callback &&cb)
        : mCallback(std::move(cb))
    {
        addSharedState(std::move(future.mState));
    }

    FutureWatcher(FutureWatcher &&) = default;
    FutureWatcher &operator=(FutureWatcher &&) = default;
    FutureWatcher(const FutureWatcher &) = delete;
    FutureWatcher &operator=(const FutureWatcher &) = delete;
    ~FutureWatcher();

    template<typename U>
    void operator()(Future<U> &&future)
    {
        addSharedState(std::move(future.mState));
    }

    template<typename F, typename ... Futures>
    void operator()(F &&f, Futures && ... futures)
    {
        addSharedState(std::move(f.mState));
        (*this)(std::forward<Futures>(futures) ...);
    }

    bool isFinished() const;

private:
    void addSharedState(std::shared_ptr<detail::SharedStateBase> state);
    void notify(detail::SharedStateBase *state) override;

    std::vector<std::shared_ptr<detail::SharedStateBase>> mStates;
    std::optional<Callback> mCallback;
    QStringList mErrors;
    int mFinished = 0;
};

} // namespace
