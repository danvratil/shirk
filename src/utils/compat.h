#pragma once

#include <QString>
#include <QHash>

#include <memory>


#if QT_VERSION < QT_VERSION_CHECK(5, 13, 0)
    #define Q_DISABLE_MOVE(Class) \
        Class(Class &&) = delete; \
        Class &operator=(Class &&) = delete;

    #define Q_DISABLE_COPY_MOVE(Class) \
        Q_DISABLE_COPY(Class) \
        Q_DISABLE_MOVE(Class)
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
namespace std {
template<>
struct hash<QString> {
    std::size_t operator()(const QString &str) const noexcept {
        return qHash(str);
    }
};
}
#endif

class QObject;
struct DeleteLater {
    template<typename T>
    void operator()(T *obj) {
        static_assert(std::is_base_of_v<QObject, T>, "DeleteLater can only be used with QObject-derived types");
        obj->deleteLater();
    }
};

template<typename T, typename ... Args>
auto make_unique_qobject(Args && ... args)
{
    return std::unique_ptr<T, DeleteLater>(new T(std::forward<Args>(args) ...));
}
