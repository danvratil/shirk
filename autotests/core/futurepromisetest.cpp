#include "core/future.h"
#include "utils/stringliterals.h"

#include <QTest>
#include <QObject>

using namespace Shirk::Core;
using namespace Shirk::StringLiterals;

class FuturePromiseTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testFinished()
    {
        Promise promise;
        Future future = promise.getFuture();
        QVERIFY(!future.isFinished());

        promise.setResult();

        QVERIFY(future.isFinished());
    }

    void testError()
    {
        Promise promise;
        Future future = promise.getFuture();
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
        Future future = promise.getFuture().then([&continuationCalled]() mutable {
            continuationCalled = true;
        });
        QVERIFY(!future.isFinished());
        QVERIFY(!continuationCalled);

        promise.setResult();

        QVERIFY(future.isFinished());
        QVERIFY(continuationCalled);
    }

    void testFutureWatcher()
    {
        Promise promise1;
        Promise promise2;
        bool doneCalled = false;

        FutureWatcher watcher([&doneCalled]() mutable { doneCalled = true; });
        watcher(promise1.getFuture(), promise2.getFuture());

        QVERIFY(!watcher.isFinished());
        QVERIFY(!doneCalled);

        promise1.setResult();
        QVERIFY(!watcher.isFinished());
        QVERIFY(!doneCalled);

        promise2.setResult();
        QVERIFY(watcher.isFinished());
        QVERIFY(doneCalled);
    }
};

QTEST_GUILESS_MAIN(FuturePromiseTest)

#include "futurepromisetest.moc"

