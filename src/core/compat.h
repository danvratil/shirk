#pragma once

#include <qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 13, 0)
    #define Q_DISABLE_MOVE(Class) \
        Class(Class &&) = delete; \
        Class &operator=(Class &&) = delete;

    #define Q_DISABLE_COPY_MOVE(Class) \
        Q_DISABLE_COPY(Class) \
        Q_DISABLE_MOVE(Class)
#endif

struct DeleteLater {
    template<typename T>
    void operator()(T *obj) {
        static_assert(std::is_base_of_v<QObject, T>, "DeleteLater can only be used with QObject-derived types");
        obj->deleteLater();
    }
};
