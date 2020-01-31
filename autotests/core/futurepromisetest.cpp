#include "core/future.h"
#include "utils/stringliterals.h"

#include <QTest>
#include <QObject>
#include <QString>

using namespace Shirk::Core;
using namespace Shirk::StringLiterals;

class FuturePromiseTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testFinished()
    {
        Promise<int> promise;
        Future<int> future = promise.getFuture();
        QVERIFY(!future.isFinished());

        promise.setResult(42);

        QVERIFY(!future.error().has_value());
        QVERIFY(future.isFinished());
        QCOMPARE(future.result(), 42);
    }

    void testError()
    {
        Promise<int> promise;
        Future<int> future = promise.getFuture();
        QVERIFY(!future.isFinished());

        promise.setError(u"Error!"_qs);
        QVERIFY(future.isFinished());

        QVERIFY(future.error().has_value());
        QCOMPARE(future.error().value(), u"Error!"_qs);
    }

    void testContinuation()
    {
        Promise promise;
        bool continuationCalled = false;
        Future baseFuture = promise.getFuture();
        Future thenFuture = baseFuture.then([&continuationCalled]() mutable {
                continuationCalled = true;
        });
        QVERIFY(!baseFuture.isFinished());
        QVERIFY(!thenFuture.isFinished());
        QVERIFY(!continuationCalled);

        promise.setResult();

        QVERIFY(baseFuture.isFinished());
        QVERIFY(thenFuture.isFinished());
        QVERIFY(continuationCalled);
    }

    void testContinuationTemp()
    {
        Promise promise;
        bool continuationCalled = false;
        Future future = promise.getFuture().then([&continuationCalled]() mutable {
                continuationCalled = true;
        });
        QVERIFY(!future.isFinished());
        QVERIFY(!continuationCalled);

        promise.setResult();

        QVERIFY(continuationCalled);
        QVERIFY(future.isFinished());
    }

    void testFutureWatcher()
    {
        Promise<int> promise1;
        Promise<QString> promise2;
        bool doneCalled = false;

        FutureWatcher watcher([&doneCalled]() mutable { doneCalled = true; });
        watcher(promise1.getFuture(), promise2.getFuture());

        QVERIFY(!watcher.isFinished());
        QVERIFY(!doneCalled);

        promise1.setResult(42);
        QVERIFY(!watcher.isFinished());
        QVERIFY(!doneCalled);

        promise2.setResult(u"Foo"_qs);
        QVERIFY(watcher.isFinished());
        QVERIFY(doneCalled);
    }
};

QTEST_GUILESS_MAIN(FuturePromiseTest)

#include "futurepromisetest.moc"

