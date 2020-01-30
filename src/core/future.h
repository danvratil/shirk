#pragma once

#include <QString>
#include <QStringList>

#include <variant>
#include <memory>
#include <functional>

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

    void addObserver(SharedStateObserver *observer);
    void removeFuture(SharedStateObserver *observer);

    std::vector<SharedStateObserver *> observers;
};

template<typename T>
struct SharedState : public SharedStateBase
{
    using ResultType = T;
    using ResultValueType = std::conditional_t<std::is_void_v<T>, VoidType, ResultType>;

    std::variant<std::monostate, ResultValueType, ErrorType> result;
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
    using ResultValueType = typename detail::SharedState<T>::ResultValueType;
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
        if (std::holds_alternative<ErrorType>(mState->result)) {
            return std::get<ErrorType>(mState->result);
        }
        return std::nullopt;
    }

    bool isFinished() const
    {
        return std::holds_alternative<ErrorType>(mState->result)
                || std::holds_alternative<ResultValueType>(mState->result);
    }

    template<typename U = ResultType, typename = std::enable_if_t<!std::is_void_v<U>>>
    U result() const
    {
        return std::get<ResultValueType>(mState->result);
    }

    template<typename U = ResultType, typename = std::enable_if_t<!std::is_void_v<U>>>
    U &&result()
    {
        return std::move(std::get<ResultValueType>(mState->result));
    }

private:
    template<typename U> friend class Promise;

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
        Q_ASSERT(std::holds_alternative<std::monostate>(mState->result));
        mState->result = error;
        notify();
    }

    template<typename U = T, typename = std::enable_if_t<!std::is_void_v<U>>>
    void setResult(const U &result)
    {
        Q_ASSERT(mState);
        Q_ASSERT(std::holds_alternative<std::monostate>(mState->result));
        mState->result = result;
        notify();
    }

    template<typename U = T, typename = std::enable_if_t<!std::is_void_v<U>>>
    void setResult(U &&result)
    {
        Q_ASSERT(mState);
        Q_ASSERT(std::holds_alternative<std::monostate>(mState->result));
        mState->result = std::move(result);
        notify();
    }

    template<typename U = T, typename = std::enable_if_t<std::is_void_v<U>>>
    void setResult()
    {
        Q_ASSERT(mState);
        Q_ASSERT(std::holds_alternative<std::monostate>(mState->result));
        mState->result = typename detail::SharedState<U>::ResultValueType{};
        notify();
    }

private:
    void notify()
    {
        auto observers = mState->observers;
        std::for_each(observers.begin(), observers.end(), [this](detail::SharedStateObserver *o) {
            o->notify(this);
        });
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
    FutureWatcher(Callback &&cb);
    template<typename U>
    FutureWatcher(Future<U> &&future)
    {
        mStates.push_back(std::move(future.mState));
        mStates.back()->addObserver(this);
    }
    template<typename U>
    FutureWatcher(Future<U> &&future, Callback &&cb)
        : mCallback(std::move(cb))
    {
        mStates.push_back(std::move(future.mState));
        mStates.back()->addObserver(this);
    }

    FutureWatcher(FutureWatcher &&) = default;
    FutureWatcher &operator=(FutureWatcher &&) = default;
    FutureWatcher(const FutureWatcher &) = delete;
    FutureWatcher &operator=(const FutureWatcher &) = delete;
    ~FutureWatcher() = default;

    template<typename U>
    void operator()(Future<U> &&future);

    template<typename F, typename ... Futures>
    void operator()(F &&f, Futures && ... futures)
    {
        (*this)(std::forward<F>(f));
        (*this)(std::forward<Futures>(futures) ...);
    }

    bool isFinished() const;

private:
    void notify(detail::SharedStateBase *state) override;

    std::vector<std::shared_ptr<detail::SharedStateBase>> mStates;
    std::optional<Callback> mCallback;
    QStringList mErrors;
    int mFinished = 0;
};

}
