#ifndef HEADER_INCLUSION_GUARD_9a884ff5_aafd_4bda_a669_fe6022312ba1
#define HEADER_INCLUSION_GUARD_9a884ff5_aafd_4bda_a669_fe6022312ba1

// Written for C++14 compilers and above

#include <cassert>          // assert
#include <cstddef>          // size_t
#include <cstring>          // strlen
#include <bit>              // bit_cast
#include <type_traits>      // conditional, is_same,
                            // is_constant_evaluated (c++20)

#ifdef __cpp_lib_string_view
#   include <string_view>   // c++17
#endif

#ifdef __TI_COMPILER_VERSION__
#    define constexprfunc
#    define constexprvar  const
#    define noexcept
#else
#    include <cstdint>      // uint_fast32_t
#    define constexprfunc constexpr
#    define constexprvar  constexpr
#endif

/* Number of bits in inttype_MAX, or in any (1<<b)-1 where 0 <= b < 3E+10 */
#define IMAX_BITS(m) ((m) /((m)%0x3fffffffL+1) /0x3fffffffL %0x3fffffffL *30 \
                      + (m)%0x3fffffffL /((m)%31+1)/31%31*5 + 4-12/((m)%31+3))

namespace FactoryUUID {

    using std::size_t;

    typedef char Byte;
    template<typename T> struct my_make_unsigned;
    template<> struct my_make_unsigned<char>               { typedef unsigned char  type; };
    template<> struct my_make_unsigned<signed char>        { typedef unsigned char  type; };
    template<> struct my_make_unsigned<short>              { typedef unsigned short type; };
    template<> struct my_make_unsigned<int>                { typedef unsigned int   type; };
    template<> struct my_make_unsigned<long>               { typedef unsigned long  type; };
  //template<> struct my_make_unsigned<long long>          { typedef unsigned long long type; };
    template<> struct my_make_unsigned<unsigned char>      { typedef unsigned char  type; };
    template<> struct my_make_unsigned<unsigned short>     { typedef unsigned short type; };
    template<> struct my_make_unsigned<unsigned int>       { typedef unsigned int   type; };
    template<> struct my_make_unsigned<unsigned long>      { typedef unsigned long  type; };
  //template<> struct my_make_unsigned<unsigned long long> { typedef unsigned long long type; };
    typedef typename my_make_unsigned<Byte>::type ByteUnsigned;

#ifdef __TI_COMPILER_VERSION__
     typedef long unsigned UIntType;  // We need at least 32 bits
#else
     typedef typename std::conditional<
         std::is_same< std::uint_fast32_t,
                       decltype(std::uint_fast32_t() + std::uint_fast32_t())
                     >::value,
         std::uint_fast32_t,
         long unsigned
     >::type UIntType;
#endif

    static_assert( sizeof(UIntType) >= sizeof(Byte) );

    constexprvar UIntType  total_bits_per_byte = IMAX_BITS( static_cast<ByteUnsigned>(-1) );
    constexprvar UIntType viable_bits_per_byte = (total_bits_per_byte / 8u) * 8u;
    constexprvar UIntType wasted_bits_per_byte = total_bits_per_byte - viable_bits_per_byte;
    constexprvar UIntType       viable_bitmask = (UIntType)(ByteUnsigned)-1 >> wasted_bits_per_byte;

    template<typename T, size_t N>
    struct array {
        static constexprvar size_t count = N;
        T b[count];
        constexprfunc T       &operator[](size_t const i)       noexcept { return this->b[i]; }
        constexprfunc T const &operator[](size_t const i) const noexcept { return this->b[i]; }
        constexprfunc T            *begin(void)                 noexcept { return this->b; }
        constexprfunc T            *end  (void)                 noexcept { return this->b + N; }
        constexprfunc T const     *cbegin(void)           const noexcept { return this->b; }
        constexprfunc T const     *cend  (void)           const noexcept { return this->b + N; }
        constexprfunc size_t        size (void)           const noexcept { return N; }

#ifdef __TI_COMPILER_VERSION__
        array(T const arg0 = 0, T const arg1 = 0, T const arg2 = 0, T const arg3 = 0) noexcept
        {
            // This is required because TI compiler
            // cannot initialise member with {}
            this->b[0] = arg0;
            this->b[1] = arg1;
            this->b[2] = arg2;
            this->b[3] = arg3;
        }
#endif
    };

    typedef array< ByteUnsigned, (128u / viable_bits_per_byte) + !!(128u % viable_bits_per_byte) > Digest;

    namespace details {

        constexprvar UIntType constant_c = 64u;
        constexprvar UIntType constant_L = constant_c / 4u;

        constexprfunc size_t const_strlen(Byte const *str) noexcept
        {
#ifdef __cpp_lib_is_constant_evaluated
            if constexpr ( false == std::is_constant_evaluated() )
            {
                // This is just an optimisation for runtime use
                return std::strlen(str);
            }
            else
#endif
            {
                return ('\0' == *str) ? 0u : const_strlen(++str) + 1u;
            }
        }

        constexprfunc Digest make_digest(array<UIntType, 4u> const &input) noexcept
        {
            Digest digest{};
            for ( UIntType i = 0u; i < input.size(); ++i )
            {
                constexprvar UIntType x = 32u / viable_bits_per_byte;

                for ( UIntType j = 0u; j < x; ++j )
                {
                    digest[i * x + j] = (input[i] >> (viable_bits_per_byte*j)) & viable_bitmask;
                }
            }
            return digest;
        }

        constexprfunc UIntType f(UIntType const b, UIntType const c, UIntType const d) noexcept
        {
            return (b & c) | (~b & d);
        }

        constexprfunc UIntType g(UIntType const b, UIntType const c, UIntType const d) noexcept
        {
            return (b & d) | (c & ~d);
        }

        constexprfunc UIntType h(UIntType const b, UIntType const c, UIntType const d) noexcept
        {
            return b ^ c ^ d;
        }

        constexprfunc UIntType i(UIntType const b, UIntType const c, UIntType const d) noexcept
        {
            return c ^ (b | ~d);
        }

        typedef UIntType (*Fn)(UIntType, UIntType, UIntType);
        constexprvar Fn array_of_funcptrs[4u] = { f, g, h, i };

        constexprvar UIntType block_G[constant_c] = {
            0u, 1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u, 10u, 11u, 12u, 13u, 14u, 15u,
            1u, 6u, 11u,  0u,  5u, 10u, 15u,  4u,  9u, 14u,  3u,  8u, 13u,  2u,  7u, 12u,
            5u, 8u, 11u, 14u,  1u,  4u,  7u, 10u, 13u,  0u,  3u,  6u,  9u, 12u, 15u,  2u,
            0u, 7u, 14u,  5u, 12u, 3u,  10u,  1u,  8u, 15u,  6u, 13u,  4u, 11u,  2u,  9u
        };

        constexprvar UIntType block_K[constant_c] = {
            0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
            0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
            0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
            0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
            0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
            0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
            0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
            0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
            0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
            0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
            0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
            0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
            0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
            0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
            0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
            0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
        };

        constexprvar UIntType block_S[constant_L] = {
            7u, 12u, 17u, 22u,
            5u,  9u, 14u, 20u,
            4u, 11u, 16u, 23u,
            6u, 10u, 15u, 21u
        };

        constexprfunc UIntType rotate(UIntType x, UIntType const n) noexcept
        {
            x &= 0xfffffffful;  // The only change needed for 64-Bit UIntType
            return (x << n) | (x >> (32u - n));
        }

        constexprfunc UIntType t(Fn arg_func, UIntType a, UIntType b, UIntType c, UIntType d, UIntType x, UIntType s, UIntType ac) noexcept
        {
            return b + rotate(a + arg_func(b, c, d) + x + ac, s);
        }

        constexprfunc UIntType array_to_UIntType(ByteUnsigned const *const data) noexcept
        {
            return
                (static_cast<UIntType>(data[3] & 0xFF) << 24u) |
                (static_cast<UIntType>(data[2] & 0xFF) << 16u) |
                (static_cast<UIntType>(data[1] & 0xFF) << 8u ) |
                (static_cast<UIntType>(data[0] & 0xFF) << 0u );
        }

        static constexprvar char ascii[] =
            " !\"#$%&'()*+,-./0123456789:;<=>?@"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "[\\]^_`"
            "abcdefghijklmnopqrstuvwxyz"
            "{|}~";

        struct Context {
            ByteUnsigned buffer[constant_c];
            array<UIntType, 4u> state;
            UIntType nl, nh;

            constexprfunc Context(void) noexcept
                : buffer()
#ifdef __TI_COMPILER_VERSION__
                , state( 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476 )
#else
                , state{ 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476 }
#endif
                , nl(0u)
                , nh(0u)
            {}

            static constexprfunc Byte MakeASCII(Byte const c) noexcept
            {
                char const *p = ascii;
                for (; /* ever */ ;)
                {
                    assert( '\0' != *p );
                    if ( '\0' == *p ) return 0x00;
                    if ( c == (Byte)*p++ ) break;
                }
                return (Byte)(p - ascii - 1) + 32u;
            }

            constexprfunc void append_padding(size_t const arg_len) noexcept
            {
                UIntType input[constant_L] = {};
                UIntType k = (nl >> 3u) & 0x3f;
                UIntType const len = static_cast<UIntType>(arg_len);
                nl += len << 3u;
                if ( nl < (len << 3u) ) ++nh;
                nh += len >> 29u;
                ByteUnsigned value = 0x80;
                for ( unsigned i = 0u; i < len; ++i )
                {
                    buffer[k++] = value;
                    value = 0x00;
                    if ( 0x40 != k ) continue;
                    for ( UIntType j = 0; j < constant_L; ++j )
                    {
                        input[j] = array_to_UIntType( &buffer[j * 4u] );
                    }
                    transform(input);
                    k = 0u;
                }
            }

            constexprfunc void append(Byte const *const data, size_t const len, bool make_ascii = false) noexcept
            {
                UIntType input[constant_L] = {};
                UIntType k = (nl >> 3u) & 0x3f;
                UIntType const length = static_cast<UIntType>(len);
                nl += length << 3u;
                if ( nl < (length << 3u) ) ++nh;
                nh += length >> 29u;
                for ( Byte const *p = data; p != (data + len); ++p )
                {
                  //buffer[k++] = static_cast<char unsigned>(static_cast<int16_t>(*ptr) + UCHAR_MAX + 1);
                    assert( 0u == (*p >> 8u) );
                    //buffer[k++] = 0xFF & (ByteUnsigned)(make_ascii ? MakeASCII(*p) : *p);
                    if ( 0x40 != k ) continue;
                    for ( UIntType i = 0; i < constant_L; ++i )
                    {
                        input[i] = array_to_UIntType( &buffer[i * 4u] );
                    }
                    transform(input);
                    k = 0u;
                }
            }

            template <size_t N>
            constexprfunc Context &operator<<( Byte const (&data)[N] ) noexcept
            {
                this->append( data, N - 1u, true );
                return *this;
            }

            constexprfunc Context &operator<<( Byte const *const data ) noexcept
            {
                this->append( data, const_strlen(data), true );
                return *this;
            }

#ifdef __cpp_lib_string_view
            constexprfunc Context &operator<<(std::string_view const s) noexcept
            {
                this->append( s.data(), s.size(), true );
                return *this;
            }
#endif

            constexprfunc void transform(UIntType const (&input)[constant_L]) noexcept
            {
                UIntType a = state[0], b = state[1], c = state[2], d = state[3];
                for ( UIntType r = 0u; r < 4u; ++r )
                {
                    UIntType const *const pG = &block_G[r * constant_L];
                    UIntType const *const pS = &block_S[r * 4u];
                    UIntType const *const pK = &block_K[r * constant_L];

                    for ( UIntType i = 0u; i < constant_L; ++i )
                    {
                        UIntType const new_b = t(array_of_funcptrs[r], a, b, c, d, input[ pG[i] ], pS[i % 4u], pK[i]);
                        a = d;
                        d = c;
                        c = b;
                        b = new_b;
                    }
                }
                state[0] += a;
                state[1] += b;
                state[2] += c;
                state[3] += d;
            }

            constexprfunc Digest final(void) noexcept
            {
                UIntType input[constant_L] = {};
                UIntType const k = (nl >> 3u) & 0x3f;
                input[14] = nl;
                input[15] = nh;

                if ( k < 56u )
                {
                    this->append_padding(56u - k);
                }
                else
                {
                    this->append_padding(120u - k);
                }

                for ( UIntType i = 0u; i < 14u; ++i )
                {
                    input[i] = array_to_UIntType(&buffer[i*4u]);
                }

                transform(input);
                return make_digest(state);
            }
        };
    }  // close namespace 'details'

    template <size_t N>
    constexprfunc Digest compute( Byte const (&s)[N] ) noexcept
    {
        return (details::Context() << s).final();
    }

    constexprfunc Digest compute(Byte const *const s) noexcept
    {
        return (details::Context() << s).final();
    }

#ifdef __cpp_lib_string_view
    constexprfunc Digest compute(std::string_view const s) noexcept
    {
        return (details::Context() << s).final();
    }
#endif //__cpp_lib_string_view

    constexprfunc Digest rand128_to_UUIDv3(Digest arg) noexcept
    {
        // Set to version 4 and IETF variant
        //arg &= ((__uint128_t)0xffffffffffff003f << 64u) | 0xff0fffffffffffff;
        //arg |= ((__uint128_t)0x0000000000000080 << 64u) | 0x0040000000000000;
        return arg;
    }

#ifdef __cpp_consteval
    consteval  // c++(20)
#else
    constexprfunc
#endif
    Digest uuid(Byte const *const name) noexcept
    {
        Byte const salt[] = {'T','h','i','s',' ','i','s',' ','m','y',' ','s','a','l','t','!','\0'};
        Digest const digest = (details::Context() << name << salt).final();
#ifdef UUID_ONLY_122_BITS
        return rand128_to_UUIDv3(digest);
#endif
        return digest;
    }

    constexprfunc void uuid_to_cstr(Digest const &d, Byte *p) noexcept
    {
        *p++ = '{';
        for ( UIntType i = 0u; i < Digest::count; ++i )
        {
            constexprvar UIntType multiplier = 2u * (viable_bits_per_byte / 8u);

            Byte val = 0u;
            for ( UIntType n = 0; n < (viable_bits_per_byte / 8u); ++n )
            {
                UIntType const n_inverse = (viable_bits_per_byte / 8u) - 1u - n;
                val |= ((d[i] >> 8u*n_inverse) & 0xFF) << (8u*n);
            }

            for ( UIntType j = 0u; j < multiplier; ++j )
            {
                UIntType const j_inverted = multiplier - 1u - j;
                UIntType const index      = multiplier * i + j;
                UIntType const shifter    = 4u * j_inverted;
                p[index] = "0123456789abcdef"[ 0xF & (val >> shifter) ];
                switch ( index )
                {
                case  7u:
                case 11u:
                case 15u:
                case 19u:
                    ++p;
                    p[index] = '-';
                }
            }
        }
        p[32] = '}';
        p[33] = '\0';
    }

}  // close namespace 'FactoryUUID'

#include <iostream>

#include <openssl/md5.h>

constexpr long long unsigned GetHash(std::string_view name) noexcept
{
    //std::cout << "\n[[[[[Getting the hash of '" << name << "']]]]]\n";

    char unsigned digest[16];
#if 1
    MD5( (char unsigned const*)name.data(), name.size(), digest );
    return (uint64_t)digest[0] |
           (uint64_t)digest[1] << 8 |
           (uint64_t)digest[2] << 16 |
           (uint64_t)digest[3] << 24 |
           (uint64_t)digest[4] << 32 |
           (uint64_t)digest[5] << 40 |
           (uint64_t)digest[6] << 48 |
           (uint64_t)digest[7] << 56;
#else
    FactoryUUID::Digest const digest = (FactoryUUID::details::Context() << name).final();
#ifdef UUID_ONLY_122_BITS
    force compiler error
    return rand128_to_UUIDv3(digest);
#endif
    return (uint64_t)digest[0] |
           (uint64_t)digest[1] << 8 |
           (uint64_t)digest[2] << 16 |
           (uint64_t)digest[3] << 24 |
           (uint64_t)digest[4] << 32 |
           (uint64_t)digest[5] << 40 |
           (uint64_t)digest[6] << 48 |
           (uint64_t)digest[7] << 56;
#endif
}

#endif  // HEADER_INCLUSION_GUARD
