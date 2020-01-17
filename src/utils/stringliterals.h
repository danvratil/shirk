#pragma once

#include <QLatin1String>

namespace Shirk::StringLiterals {

constexpr QLatin1String operator"" _ql1(const char *str, std::size_t len) noexcept
{
    return QLatin1String(str, len);
}

constexpr QStringView operator"" _qsv(const char16_t *str, std::size_t len) noexcept
{
    return QStringView(str, len);
}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
#endif

template<typename CharT, CharT ... str>
QString operator""_qs() noexcept
{
    static_assert(std::is_same_v<CharT, char16_t>, "The _qs literal operator can only be used for Unicode string literals");
    static constexpr const QStaticStringData<sizeof...(str)> qstring_literal {
        Q_STATIC_STRING_DATA_HEADER_INITIALIZER(sizeof...(str)),
        str ...
    };
    return QString{QStringDataPtr{qstring_literal.data_ptr()}};
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

} // namespace
