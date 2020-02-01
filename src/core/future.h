#pragma once

#include <QString>
#include <QStringList>

#include <memory>
#include <functional>
#include <type_traits>

#include <function2/function2.hpp>

namespace Shirk::Core
{

template<typename T>
class Future;
template<typename T>
class Promise;
class FutureWatcher;

namespace detail
{

class SharedStateObserver;

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
    fu2::unique_function<void()> continuation;
    bool hasResult = false;
};

template<typename T>
struct SharedState : public SharedStateBase
{
    using ResultType = T;

    std::aligned_storage_t<sizeof(ResultType), alignof(ResultType)> result{};

    ResultType getResult() const {
        return *reinterpret_cast<ResultType *>(&result);
    }

    ResultType &&getResult() {
        return std::move(*reinterpret_cast<ResultType *>(&result));
    }

    void setResult(const ResultType &value) {
        new (reinterpret_cast<ResultType *>(&result)) ResultType(value);
        hasResult = true;
    }

    void setResult(ResultType &&value) {
        new (reinterpret_cast<ResultType *>(&result)) ResultType(std::move(value));
        hasResult = true;
    }

    template<typename U,
             typename = std::enable_if_t<std::is_convertible_v<U, ResultType>>>
    void setResult(const U &value) {
        new (reinterpret_cast<ResultType *>(&result)) ResultType(value);
        hasResult = true;
    }

    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, ResultType>>>
    void setResult(U &&value) {
        new (reinterpret_cast<ResultType *>(&result)) ResultType(std::move(value));
        hasResult = true;
    }

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

template<typename T>
struct future_type;

template<typename T>
struct future_type<Future<T>>
{
    using type = T;
};

template<typename F, typename ... Arg>
struct continuation_traits
{
    using future_type = typename future_type<std::invoke_result_t<F, Arg ...>>::type;
};

} // namespace detail

template<typename T>
class Future : public detail::SharedStateObserver
{
public:
    using ErrorType = typename detail::SharedState<T>::ErrorType;
    using ResultType = typename detail::SharedState<T>::ResultType;

    Future(const Future &) = delete;
    Future &operator=(const Future &) = delete;
    Future(Future &&other) {
        *this = std::move(other);
    }

    Future &operator=(Future &&other) {
        std::swap(mState, other.mState);
        mState->removeObserver(&other);
        mState->addObserver(this);
        return *this;
    }

    ~Future() = default;

    std::optional<QString> error() const {
        return mState->error;
    }

    bool isFinished() const {
        return mState->error.has_value() || mState->hasResult;
    }

    template<typename U = ResultType, typename = std::enable_if_t<!std::is_void_v<U>>>
    U result() const {
        return mState->getResult();
    }

    template<typename U = ResultType, typename = std::enable_if_t<!std::is_void_v<U>>>
    U &&result() {
        return mState->getResult();
    }

    struct voidT_t {};
    struct voidVoid_t {};
    struct futureUT_t {};
    struct futureUVoid_t {};

    // F is void(T)
    template<typename F, typename = std::enable_if_t<std::is_void_v<std::invoke_result_t<F, T>>>>
    Future<void> then(F &&cb, voidT_t = {});

    // F is void()
    template<typename F, typename = std::enable_if_t<std::is_void_v<std::invoke_result_t<F>>>>
    Future<void> then(F &&cb, voidVoid_t = {});

    // F is Future<U>(T)
    template<typename F, typename U = typename std::invoke_result_t<F, T>::ResultType>
    Future<U> then(F &&cb, futureUT_t = {});

    // F is Future<U>()
    template<typename F, typename U = typename std::invoke_result_t<F>::ResultType>
    Future<U> then(F &&cb, futureUVoid_t = {});

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

    Future<T> getFuture() {
        Q_ASSERT(mState);
        return Future<T>(mState);
    }

    void setError(const ErrorType &error) {
        Q_ASSERT(mState);
        Q_ASSERT(!mState->hasResult && !mState->error.has_value());
        mState->error = error;
        notify();
    }

    template<typename U, typename = std::enable_if_t<!std::is_void_v<U>>>
    void setResult(const U &result) {
        Q_ASSERT(mState);
        Q_ASSERT(!mState->hasResult && !mState->error.has_value());
        mState->setResult(result);
        notify();
    }

    template<typename U, typename = std::enable_if_t<!std::is_void_v<U>>>
    void setResult(U &&result) {
        Q_ASSERT(mState);
        Q_ASSERT(!mState->hasResult && !mState->error.has_value());
        mState->setResult(std::move(result));
        notify();
    }



    template<typename U = T, typename = std::enable_if_t<std::is_void_v<U>>>
    void setResult() {
        Q_ASSERT(mState);
        Q_ASSERT(!mState->hasResult && !mState->error.has_value());
        mState->hasResult = true;
        notify();
    }

private:
    void notify() {
        auto observers = mState->observers;
        std::for_each(observers.begin(), observers.end(),
                      std::bind(&detail::SharedStateObserver::notify, std::placeholders::_1, mState.get()));

        if (mState->continuation) {
            mState->continuation();
        }

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
    FutureWatcher(Future<U> &&future) {
        addSharedState(std::move(future.mState));
    }

    template<typename U>
    FutureWatcher(Future<U> &&future, Callback &&cb)
        : mCallback(std::move(cb)) {
        addSharedState(std::move(future.mState));
    }

    FutureWatcher(FutureWatcher &&) = default;
    FutureWatcher &operator=(FutureWatcher &&) = default;
    FutureWatcher(const FutureWatcher &) = delete;
    FutureWatcher &operator=(const FutureWatcher &) = delete;
    ~FutureWatcher();

    template<typename U>
    void operator()(Future<U> &&future) {
        addSharedState(std::move(future.mState));
    }

    template<typename F, typename ... Futures>
    void operator()(F &&f, Futures && ... futures) {
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


template<typename T>
template<typename F, typename>
Future<void> Future<T>::then(F &&cb, voidT_t)
{
    Promise<void> nextPromise;
    auto nextFuture = nextPromise.getFuture();
    mState->continuation = [cb = std::move(cb), promise = std::move(nextPromise),
                            state = std::weak_ptr<detail::SharedState<T>>(mState)]() mutable {
        cb(state.lock()->getResult());
        promise.setResult();
    };
    return nextFuture;
}

template<typename T>
template<typename F, typename>
Future<void> Future<T>::then(F &&cb, voidVoid_t)
{
    Promise<void> nextPromise;
    auto nextFuture = nextPromise.getFuture();
    mState->continuation = [cb = std::move(cb), promise = std::move(nextPromise)]() mutable {
        cb();
        promise.setResult();
    };
    return nextFuture;
}

template<typename T>
template<typename F, typename U>
Future<U> Future<T>::then(F &&cb, futureUT_t)
{
    Promise<U> nextPromise;
    auto nextFuture = nextPromise.getFuture();
    mState->continuation = [cb = std::move(cb), promise = std::move(nextPromise),
                            state = mState]() mutable {
        cb(state->getResult()).then([promise = std::move(promise)](U &&val) mutable {
            promise.setResult(std::move(val));
        });
        state.reset();
    };
    return nextFuture;
}

template<typename T>
template<typename F, typename U>
Future<U> Future<T>::then(F &&cb, futureUVoid_t)
{
    Promise<U> nextPromise;
    auto nextFuture = nextPromise.getFuture();
    mState->continuation = [cb = std::move(cb), promise = std::move(nextPromise)]() mutable {
        cb().then([promise = std::move(promise)](U &&val) mutable {
            promise.setResult(std::move(val));
        });
    };
    return nextFuture;
}


} // namespace
