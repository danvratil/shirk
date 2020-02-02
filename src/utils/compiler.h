#pragma once


#define __pragma_impl(str) _Pragma(#str)

#if defined(__clang__)
    #define __pragma(pragma) __pragma_impl(clang pragma)
#elif defined(__GNUC__)
    #define __pragma(pragma) __pragma_impl(GCC pragma)
#else
    #define __pragma(...)
#endif

#define compiler_suppress_warning(w) \
    __pragma(diagnostic push) \
    __pragma(diagnostic ignored w)

#define compiler_restore_warning(w) \
    __pragma(diagnostic pop)

