#pragma once

#include <QString>
#include <QStringList>

#include <optional>
#include <memory>
#include <functional>

namespace Shirk::Core
{

class Future;
class FutureWatcher;

class Promise
{
public:
    Promise();
    Promise(const Promise &) = delete;
    Promise &operator=(const Promise &) = delete;
    Promise(Promise &&) = default;
    Promise &operator=(Promise &&) = default;
    ~Promise() = default;

    Future getFuture();

    void setError(const QString &error);
    void setResult();

private:
    void notify();

    friend class Future;
    struct SharedState {
        void addFuture(Future *future);
        void removeFuture(Future *future);

        std::optional<QString> error;
        std::vector<Future *> futures;
        bool hasResult = false;
    };

    std::shared_ptr<SharedState> mState;
};



class Future
{
public:
    Future() = default;
    Future(const Future &) = delete;
    Future &operator=(const Future &) = delete;
    Future(Future &&);
    Future &operator=(Future &&);

    ~Future();

    std::optional<QString> error() const;

    bool isFinished() const;

    using ContinuationCb = std::function<void()>;
    Future then(ContinuationCb &&cont);

private:
    friend class Promise;
    friend class FutureWatcher;

    Future(std::shared_ptr<Promise::SharedState> state);
    void notify();

private:
    std::shared_ptr<Promise::SharedState> mState;
    std::optional<ContinuationCb> mContinuation;
    FutureWatcher *mWatcher = nullptr;
};


class FutureWatcher
{
public:
    using Callback = std::function<void()>;

    FutureWatcher() = default;
    FutureWatcher(Callback &&cb);
    FutureWatcher(Future &&future);
    FutureWatcher(Future &&future, Callback &&cb);
    FutureWatcher(FutureWatcher &&) = default;
    FutureWatcher &operator=(FutureWatcher &&) = default;
    FutureWatcher(const FutureWatcher &) = delete;
    FutureWatcher &operator=(const FutureWatcher &) = delete;
    ~FutureWatcher() = default;

    void operator()(Future &&future);

    template<typename F, typename ... Futures>
    void operator()(F &&f, Futures && ... futures)
    {
        (*this)(std::forward<F>(f));
        (*this)(std::forward<Futures>(futures) ...);
    }

    bool isFinished() const;

private:
    friend class Future;

    void notify(Future *future);

    std::vector<Future> mFutures;
    std::optional<Callback> mCallback;
    QStringList mErrors;
    int mFinished = 0;
};

}
