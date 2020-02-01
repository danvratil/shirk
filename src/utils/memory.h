#pragma once

#include <memory>

class QObject;

namespace Shirk
{

template<typename T>
struct UniquePtrCompare
{
    UniquePtrCompare(T *ptr)
        : m_ptr(ptr)
    {}

    bool operator()(const std::unique_ptr<std::decay_t<T>> &ptr) const {
        return ptr.get() == m_ptr;
    }

private:
    T *m_ptr = nullptr;
};

template<typename T>
UniquePtrCompare(T) -> UniquePtrCompare<T>;


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

template<typename T>
using UniqueQObjectPtr = std::unique_ptr<T, DeleteLater>;

} // namespace

template<typename T>
bool operator==(const Shirk::UniquePtrCompare<T> &cmp, const std::unique_ptr<T> &ptr) {
    return cmp(ptr);
}

template<typename T>
bool operator==(const std::unique_ptr<std::decay_t<T>> &ptr, const Shirk::UniquePtrCompare<T> &cmp) {
    return cmp(ptr);
}
