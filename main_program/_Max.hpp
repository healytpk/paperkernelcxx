#ifndef HEADER_INCLUSION_GUARD_fbde6307_28ac_4082_904d_2984d8802179
#define HEADER_INCLUSION_GUARD_fbde6307_28ac_4082_904d_2984d8802179

#include <limits>                       // numeric_limits<T>::max()
#include <type_traits>                  // is_integral_v

struct _Max_t {
    template<typename T> requires std::is_integral_v<T> && std::is_unsigned_v<T>
    constexpr operator T(void) const
    {
        return std::numeric_limits<T>::max();
    }
    template<typename T> requires std::is_integral_v<T> && std::is_unsigned_v<T>
    constexpr bool operator==(T const n) const
    {
        return n == std::numeric_limits<T>::max();
    }
};

constexpr _Max_t _Max{};

#endif
