#pragma once

#include "utils/stringliterals.h"

#include <QJsonArray>

namespace Core::API
{

inline QString bool2str(bool val)
{
    using namespace Core::StringLiterals;

    return val ? u"true"_qs : u"false"_qs;
}

inline QStringList toStringList(const QJsonValue &val)
{
    Q_ASSERT(val.isArray());
    const auto arr = val.toArray();
    QStringList rv;
    rv.reserve(arr.size());
    std::transform(arr.cbegin(), arr.cend(), std::back_inserter(rv), [](const auto &val) { return val.toString(); });
    return rv;
}

#ifdef API_EXTRACT_STR
#undef API_EXTRACT_STR
#endif
#define API_EXTRACT_STR(resp, val, name) \
    resp.name = val[#name ## _ql1].toString()
#ifdef API_EXTRACT_BOOL
#undef API_EXTRACT_BOOL
#endif
#define API_EXTRACT_BOOL(resp, val, name) \
    resp.name = val[#name ## _ql1].toBool(false)
#ifdef API_EXTRACT_INT
#undef API_EXTRACT_INT
#endif
#define API_EXTRACT_INT(resp, val, name) \
    resp.name = val[#name ## _ql1].toInt()
#ifdef API_EXTRACT_STRLIST
#undef API_EXTRACT_STRLIST
#endif
#define API_EXTRACT_STRLIST(resp, val, name) \
    resp.name = toStringList(val[#name ## _ql1])

} // namespace
