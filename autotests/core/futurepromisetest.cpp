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
    void testFinished() {
        Promise<int> promise;
        Future<int> future = promise.getFuture();
        QVERIFY(!future.isFinished());

        promise.setResult(42);

        QVERIFY(!future.error().has_value());
        QVERIFY(future.isFinished());
        QCOMPARE(future.result(), 42);
    }

    void testError() {
        Promise<int> promise;
        Future<int> future = promise.getFuture();
        QVERIFY(!future.isFinished());

        promise.setError(u"Error!"_qs);
        QVERIFY(future.isFinished());

        QVERIFY(future.error().has_value());
        QCOMPARE(future.error().value(), u"Error!"_qs);
    }

    void testVoid() {
        Promise<void> promise;
        Future<void> future = promise.getFuture();
        QVERIFY(!future.isFinished());

        promise.setResult();
        QVERIFY(!future.error().has_value());
        QVERIFY(future.isFinished());
    }

    void testVoidContinuation() {
        Promise<void> promise;
        bool continuationCalled = false;
        auto baseFuture = promise.getFuture();
        auto thenFuture = baseFuture.then([&continuationCalled]() mutable {
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

    void testTempVoidContinuation() {
        Promise<void> promise;
        bool continuationCalled = false;
        Future<void> future = promise.getFuture().then([&continuationCalled]() mutable {
            continuationCalled = true;
        });
        QVERIFY(!future.isFinished());
        QVERIFY(!continuationCalled);

        promise.setResult();

        QVERIFY(continuationCalled);
        QVERIFY(future.isFinished());
    }

    void testArgContinuation() {
        Promise<int> promise;
        int continuationValue = 0;
        auto baseFuture = promise.getFuture();
        auto thenFuture = baseFuture.then([&continuationValue](int value) mutable {
            continuationValue = value;
        });
        QVERIFY(!baseFuture.isFinished());
        QVERIFY(!thenFuture.isFinished());
        QCOMPARE(continuationValue, 0);

        promise.setResult(42);

        QVERIFY(baseFuture.isFinished());
        QVERIFY(thenFuture.isFinished());
        QCOMPARE(continuationValue, 42);
    }

    void testArgContinuationTemp() {
        Promise<int> promise;
        int continuationValue = 0;
        auto future = promise.getFuture().then([&continuationValue](int value) mutable {
            continuationValue = value;
        });
        QVERIFY(!future.isFinished());
        QCOMPARE(continuationValue, 0);

        promise.setResult(42);

        QVERIFY(future.isFinished());
        QCOMPARE(continuationValue, 42);
    }

    void testVoidContinuationReturningFuture() {
        Promise<void> promise;
        bool continuationCalled = false;
        auto baseFuture = promise.getFuture();
        Promise<int> thenPromise;
        auto thenFuture = baseFuture.then([&continuationCalled, &thenPromise]() mutable {
            continuationCalled = true;
            return thenPromise.getFuture();
        });

        QVERIFY(!baseFuture.isFinished());
        QVERIFY(!thenFuture.isFinished());
        QVERIFY(!continuationCalled);

        promise.setResult();

        QVERIFY(baseFuture.isFinished());
        QVERIFY(!thenFuture.isFinished());
        QVERIFY(continuationCalled);

        thenPromise.setResult(42);

        QVERIFY(thenFuture.isFinished());
        QCOMPARE(thenFuture.result(), 42);
    }

    void testVoidContinuationReturningFutureTemp() {
        Promise<void> promise;
        bool continuationCalled = false;
        Promise<int> thenPromise;

        auto future = promise.getFuture().then([&continuationCalled, &thenPromise]() mutable {
            continuationCalled = true;
            return thenPromise.getFuture();
        });

        QVERIFY(!future.isFinished());
        QVERIFY(!continuationCalled);

        promise.setResult();

        QVERIFY(!future.isFinished());
        QVERIFY(continuationCalled);

        thenPromise.setResult(42);

        QVERIFY(future.isFinished());
    }

    void testArgContinuationReturningFuture() {
        Promise<int> basePromise;
        int continuationValue = 0;
        Promise<QString> thenPromise;

        auto baseFuture = basePromise.getFuture();
        auto thenFuture = baseFuture.then([&continuationValue, &thenPromise](int value) mutable {
            continuationValue = value;
            return thenPromise.getFuture();
        });

        QVERIFY(!baseFuture.isFinished());
        QVERIFY(!thenFuture.isFinished());
        QCOMPARE(continuationValue, 0);

        basePromise.setResult(42);

        QVERIFY(baseFuture.isFinished());
        QVERIFY(!thenFuture.isFinished());
        QCOMPARE(continuationValue, 42);

        thenPromise.setResult(u"foo"_qs);

        QVERIFY(thenFuture.isFinished());
        QCOMPARE(thenFuture.result(), u"foo"_qs);
    }

    void testArgContinuationReturningFutureTmp() {
        Promise<int> basePromise;
        int continuationValue = 0;
        Promise<QString> thenPromise;

        auto thenFuture = basePromise.getFuture().then([&continuationValue, &thenPromise](int value) mutable {
            continuationValue = value;
            return thenPromise.getFuture();
        });

        QVERIFY(!thenFuture.isFinished());
        QCOMPARE(continuationValue, 0);

        basePromise.setResult(42);

        QVERIFY(!thenFuture.isFinished());
        QCOMPARE(continuationValue, 42);

        thenPromise.setResult(u"foo"_qs);

        QVERIFY(thenFuture.isFinished());
        QCOMPARE(thenFuture.result(), u"foo"_qs);
    }

    void testFutureWatcher() {
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

