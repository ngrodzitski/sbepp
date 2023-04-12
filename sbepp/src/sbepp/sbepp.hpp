// SPDX-License-Identifier: MIT
// Copyright (c) 2023, Oleksandr Koval

/**
 * @file sbepp.hpp
 * @brief Contains various utilities and implementation details which are
 * required for generated schema headers
 */

#pragma once

#include <array>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <limits>
#include <type_traits>

// Clang generates tons of warnings about code like `return {N};` where `N` is
// calculated during generation or just a value from schema. Using braced
// initialization syntax provides additional safety in such cases.
#ifdef __clang__
#    define SBEPP_WARNINGS_OFF()          \
        _Pragma("clang diagnostic push"); \
        _Pragma("clang diagnostic ignored \"-Wbraced-scalar-init\"")
#    define SBEPP_WARNINGS_ON() _Pragma("clang diagnostic pop")
#else
#    define SBEPP_WARNINGS_OFF()
#    define SBEPP_WARNINGS_ON()
#endif

SBEPP_WARNINGS_OFF();

#ifdef __has_include
#    if __has_include(<version>)
#        include <version>
#    endif
#endif

/**
 * @addtogroup compiler-features Compiler features
 *
 *  The list of configuration macros, if necessary, their values can be enforced
 *  by client.
 *  @{
 */

//! @brief `1` if compiler supports three-way comparison, `0` otherwise
#if !defined(SBEPP_HAS_THREE_WAY_COMPARISON)    \
    && defined(__cpp_impl_three_way_comparison) \
    && defined(__cpp_lib_three_way_comparison)
#    if(__cpp_impl_three_way_comparison >= 201907L) \
        && (__cpp_lib_three_way_comparison >= 201907L)
#        define SBEPP_HAS_THREE_WAY_COMPARISON 1
#        include <compare>
#    endif
#endif
#ifndef SBEPP_HAS_THREE_WAY_COMPARISON
#    define SBEPP_HAS_THREE_WAY_COMPARISON 0
#endif

//! @brief `1` if compiler supports concepts, `0` otherwise
#if !defined(SBEPP_HAS_CONCEPTS) && defined(__cpp_concepts)
#    if(__cpp_concepts >= 201907L)
#        define SBEPP_HAS_CONCEPTS 1
#    endif
#endif
#ifndef SBEPP_HAS_CONCEPTS
#    define SBEPP_HAS_CONCEPTS 0
#endif

//! @brief `1` is compiler supports inline variables, `0` otherwise
#if !defined(SBEPP_HAS_INLINE_VARS) && defined(__cpp_inline_variables)
#    if(__cpp_inline_variables >= 201606L)
#        define SBEPP_HAS_INLINE_VARS 1
#        define SBEPP_CPP17_INLINE_VAR inline
#    endif
#endif
#ifndef SBEPP_CPP17_INLINE_VAR
#    define SBEPP_HAS_INLINE_VARS 0
#    define SBEPP_CPP17_INLINE_VAR
#endif

//! @brief `1` if compiler supports `std::endian`, `0` otherwise
#if !defined(SBEPP_HAS_ENDIAN) && defined(__cpp_lib_endian)
#    if(__cpp_lib_endian >= 201907L)
#        define SBEPP_HAS_ENDIAN 1
#        include <bit>
#    endif
#endif
#ifndef SBEPP_HAS_ENDIAN
#    define SBEPP_HAS_ENDIAN 0
#endif

//! @brief `1` if compiler supports `std::bitcast`, `0` otherwise
#if !defined(SBEPP_HAS_BITCAST) && defined(__cpp_lib_bit_cast)
#    if(__cpp_lib_bit_cast >= 201806L)
#        define SBEPP_HAS_BITCAST 1
#        include <bit>
#    endif
#endif
#ifndef SBEPP_HAS_BITCAST
#    define SBEPP_HAS_BITCAST 0
#endif

//! @brief `1` if compiler supports `std::byteswap`, `0` otherwise
#if !defined(SBEPP_HAS_BYTESWAP) && defined(__cpp_lib_byteswap)
#    if(__cpp_lib_byteswap >= 202110L)
#        define SBEPP_HAS_BYTESWAP 1
#        include <bit>
#    endif
#endif
#ifndef SBEPP_HAS_BYTESWAP
#    define SBEPP_HAS_BYTESWAP 0
#endif

//! @brief `1` if compiler supports constexpr `std` algorithms, `0` otherwise
#if !defined(SBEPP_HAS_CONSTEXPR_ALGORITHMS) \
    && defined(__cpp_lib_constexpr_algorithms)
#    if(__cpp_lib_constexpr_algorithms >= 201806L)
#        define SBEPP_HAS_CONSTEXPR_ALGORITHMS 1
#    endif
#endif
#ifndef SBEPP_HAS_CONSTEXPR_ALGORITHMS
#    define SBEPP_HAS_CONSTEXPR_ALGORITHMS 0
#endif

#ifdef __has_cpp_attribute
#    if __has_cpp_attribute(nodiscard)
#        define SBEPP_CPP17_NODISCARD [[nodiscard]]
#    endif
#endif
#ifndef SBEPP_CPP17_NODISCARD
#    define SBEPP_CPP17_NODISCARD
#endif

//! @brief `1` if constexpr accessors are supported, `0` otherwise
#if SBEPP_HAS_BITCAST && SBEPP_HAS_CONSTEXPR_ALGORITHMS
#    define SBEPP_HAS_CONSTEXPR_ACCESSORS 1
#else
#    define SBEPP_HAS_CONSTEXPR_ACCESSORS 0
#endif

#if SBEPP_HAS_CONSTEXPR_ACCESSORS
#    include <bit>
#    define SBEPP_CPP20_CONSTEXPR constexpr
#else
#    define SBEPP_CPP20_CONSTEXPR
#endif

#ifdef __cpp_constexpr
#    if(__cpp_constexpr >= 201304L)
#        define SBEPP_CPP14_CONSTEXPR constexpr
#    endif
#else
#    if(__cplusplus >= 201402L)
#        define SBEPP_CPP14_CONSTEXPR constexpr
#    endif
#endif
#ifndef SBEPP_CPP14_CONSTEXPR
#    define SBEPP_CPP14_CONSTEXPR
#endif

//! @brief `1` if compiler supports ranges, `0` otherwise
#if !defined(SBEPP_HAS_RANGES) && defined(__cpp_lib_ranges)
#    if(__cpp_lib_ranges >= 201911L)
#        define SBEPP_HAS_RANGES 1
#        include <ranges>
#    endif
#endif
#ifndef SBEPP_HAS_RANGES
#    define SBEPP_HAS_RANGES 0
#endif
//! @}

#ifdef SBEPP_DOXYGEN
//! @brief When defined, unconditionally disables assertions and size checks
#    define SBEPP_DISABLE_ASSERTS

/**
 * @brief When defined, uses client-defined `sbepp::assertion_failed` as an
 *  assertion handler. Whether assertions are enabled is controlled by  `NDEBUG`
 */
#    define SBEPP_ASSERT_HANDLER

/**
 * @brief When defined, unconditionally enables assertions. Requires
 *  `sbepp::assertion_failed` to be defined by the client.
 */
#    define SBEPP_ENABLE_ASSERTS_WITH_HANDLER

namespace sbepp
{
/**
 * @brief When #SBEPP_ASSERT_HANDLER or #SBEPP_ENABLE_ASSERTS_WITH_HANDLER is
 *  defined, this function is called for failed assertions. Must be defined by
 *  the client.
 *
 * @param expr expression that failed
 * @param function enclosing function name
 * @param file file name
 * @param line line number
 */
[[noreturn]] void assertion_failed(
    char const* expr, char const* function, char const* file, long line);
} // namespace sbepp
#endif

#ifdef SBEPP_DISABLE_ASSERTS
#    define SBEPP_SIZE_CHECKS_ENABLED 0
#    define SBEPP_ASSERT(expr) ((void)0)
#elif defined(SBEPP_ASSERT_HANDLER) \
    || defined(SBEPP_ENABLE_ASSERTS_WITH_HANDLER)
#    if !defined(NDEBUG) || defined(SBEPP_ENABLE_ASSERTS_WITH_HANDLER)
namespace sbepp
{
// must be defined by user
[[noreturn]] void assertion_failed(
    char const* expr, char const* function, char const* file, long line);
} // namespace sbepp

#        define SBEPP_SIZE_CHECKS_ENABLED 1
#        define SBEPP_ASSERT(expr)                                \
            (static_cast<bool>(expr) ? ((void)0)                  \
                                     : ::sbepp::assertion_failed( \
                                         #expr, __func__, __FILE__, __LINE__))
#    else
#        define SBEPP_SIZE_CHECKS_ENABLED 0
#        define SBEPP_ASSERT(expr) ((void)0)
#    endif
#else
#    include <cassert>
#    ifdef NDEBUG
#        define SBEPP_SIZE_CHECKS_ENABLED 0
#    else
#        define SBEPP_SIZE_CHECKS_ENABLED 1
#    endif
#    define SBEPP_ASSERT(expr) assert(expr)
#endif

#define SBEPP_SIZE_CHECK(begin, end, offset, size) \
    SBEPP_ASSERT(                                  \
        (begin)                                    \
        && (((offset) + (size)) <= static_cast<std::size_t>((end) - (begin))))

//! @brief The main `sbepp` namespace
namespace sbepp
{
//! @brief Represents `type_traits::length()` value type
using length_t = std::uint64_t;
// for unknown reasons, in the SBE standard `offset` is specified as an unsigned
// 32-bit integer which makes no sense since `length` has 64 bits
//! @brief Represents `offset` trait value type, e.g. `type_traits::offset()`
using offset_t = std::uint64_t;
//! @brief Represents `version` trait value type, e.g.
//!  `type_traits::since_version()`
using version_t = std::uint64_t;
//! @brief Represents `schema_traits::id()` value type
using schema_id_t = std::uint32_t;
//! @brief Represents `message_traits::id()` value type
using message_id_t = std::uint32_t;
//! @brief Represents `block_length` trait value type, e.g.
//!  `message_traits::block_length()`
using block_length_t = std::uint64_t;
//! @brief Represents `id` trait value type, e.g. `field_traits::id()`
using member_id_t = std::uint16_t;
//! @brief Represents `set_choice_traits::index()` value type
using choice_index_t = std::uint8_t;

//! @brief Represents `presence` trait value type, e.g.
//! `type_traits::presence()`
enum class field_presence
{
    //! field is required
    required,
    //! field is optional
    optional,
    //! field is constant
    constant
};

#ifdef SBEPP_DOXYGEN
/**
 * @brief Must be defined to either `little` or `big` when native endianness
 *  cannot be detected automatically.
 */
#    define SBEPP_BYTE_ORDER
#endif

/**
 * @brief represents `schema_traits::byte_order()` value type. When
 *  #SBEPP_HAS_ENDIAN is `1`, it's just an alias to `std::endian`. Requires
 *  #SBEPP_BYTE_ORDER to be defined if endianness is not detected automatically.
 */
#ifdef SBEPP_DOXYGEN
enum class endian
{
    //! little-endian
    little,
    //! big-endian
    big,
    //! current platform endianness
    native
};
#elif SBEPP_HAS_ENDIAN
using endian = std::endian;
#else
// https://en.cppreference.com/w/cpp/types/endian
enum class endian
{
#    if defined(_WIN32) || defined(WIN32)
    little = 0,
    big = 1,
    native = little
#    elif defined(__BYTE_ORDER__)
    little = __ORDER_LITTLE_ENDIAN__,
    big = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
#    elif defined(SBEPP_BYTE_ORDER)
    little = 0,
    big = 1,
    native = SBEPP_BYTE_ORDER
#    else
#        error "Byte order cannot be detected.\
 Define SBEPP_BYTE_ORDER to 'little' or 'big'"
#    endif
};
#endif

static_assert(
    (endian::native == endian::little) || (endian::native == endian::big),
    "Mixed-endian is not supported");

//! @brief Namespace for various implementation details. Should not be used
//!     directly
namespace detail
{
// modern C++ bits
template<bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template<typename T>
using remove_cv_t = typename std::remove_cv<T>::type;

template<typename T>
using remove_reference_t = typename std::remove_reference<T>::type;

template<typename ByteFrom, typename ByteTo>
using enable_if_convertible_t =
    enable_if_t<std::is_convertible<ByteFrom*, ByteTo*>::value>;

template<typename Byte, typename T = void>
using enable_if_writable_t = enable_if_t<!std::is_const<Byte>::value, T>;

template<typename...>
using void_t = void;

template<typename T>
constexpr typename std::make_unsigned<T>::type to_unsigned(T v) noexcept
{
    return static_cast<typename std::make_unsigned<T>::type>(v);
}

// intrinsics detection is taken from <boost/endian/detail/intrinsic.hpp>
#ifndef __has_builtin          // Optional of course
#    define __has_builtin(x) 0 // Compatibility with non-clang compilers
#endif

#if SBEPP_HAS_BYTESWAP

using std::byteswap;

#else

// because `if((E == endian::native) || (sizeof(T) == 1))` is not a constexpr-if
// this function has to be declared for single byte types even if it will never
// be used
template<typename T>
T byteswap(T) noexcept;

#    if defined(_MSC_VER) && (!defined(__clang__) || defined(__c2__))

#        include <cstdlib>

inline std::uint64_t byteswap(std::uint64_t v) noexcept
{
    return _byteswap_uint64(v);
}

inline std::uint32_t byteswap(std::uint32_t v) noexcept
{
    return _byteswap_ulong(v);
}

inline std::uint16_t byteswap(std::uint16_t v) noexcept
{
    return _byteswap_ushort(v);
}

#    elif(                                                     \
        defined(__clang__) && __has_builtin(__builtin_bswap32) \
        && __has_builtin(__builtin_bswap64))                   \
        || (defined(__GNUC__)                                  \
            && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)))

#        if(defined(__clang__) && __has_builtin(__builtin_bswap16)) \
            || (defined(__GNUC__)                                   \
                && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)))

inline std::uint16_t byteswap(std::uint16_t v) noexcept
{
    return __builtin_bswap16(v);
}

#        else

inline std::uint16_t byteswap(std::uint16_t v) noexcept
{
    return __builtin_bswap32(v) << 16;
}

#        endif

inline std::uint64_t byteswap(std::uint64_t v) noexcept
{
    return __builtin_bswap64(v);
}

inline std::uint32_t byteswap(std::uint32_t v) noexcept
{
    return __builtin_bswap32(v);
}

#    else

constexpr std::uint64_t byteswap(std::uint64_t v) noexcept
{
    return ((v & UINT64_C(0x00000000000000FF)) << 56)
           | ((v & UINT64_C(0x000000000000FF00)) << 40)
           | ((v & UINT64_C(0x0000000000FF0000)) << 24)
           | ((v & UINT64_C(0x00000000FF000000)) << 8)
           | ((v & UINT64_C(0x000000FF00000000)) >> 8)
           | ((v & UINT64_C(0x0000FF0000000000)) >> 24)
           | ((v & UINT64_C(0x00FF000000000000)) >> 40)
           | ((v & UINT64_C(0xFF00000000000000)) >> 56);
}

constexpr std::uint32_t byteswap(std::uint32_t v) noexcept
{
    return ((v & UINT32_C(0x000000FF)) << 24)
           | ((v & UINT32_C(0x0000FF00)) << 8)
           | ((v & UINT32_C(0x00FF0000)) >> 8)
           | ((v & UINT32_C(0xFF000000)) >> 24);
}

constexpr std::uint16_t byteswap(std::uint16_t v) noexcept
{
    return ((v & UINT16_C(0x00FF)) << 8) | ((v & UINT16_C(0xFF00)) >> 8);
}

#    endif
#endif

template<typename T, endian E, typename Byte>
SBEPP_CPP20_CONSTEXPR T get_primitive(const Byte* ptr)
{
#if SBEPP_HAS_BITCAST
    std::array<Byte, sizeof(T)> arr;
    if((E == endian::native) || (sizeof(T) == 1))
    {
        std::copy(ptr, ptr + sizeof(T), std::begin(arr));
    }
    else
    {
        std::reverse_copy(ptr, ptr + sizeof(T), std::begin(arr));
    }
    return std::bit_cast<T>(arr);
#else
    // old compilers don't optimize `std::copy` approach good enough, that's
    // why explicit `std::memcpy` call is required
    T res;
    std::memcpy(&res, ptr, sizeof(T));
    if((E == endian::native) || (sizeof(T) == 1))
    {
        return res;
    }
    else
    {
        return static_cast<T>(byteswap(to_unsigned(res)));
    }
#endif
}

template<endian E, typename T, typename Byte>
SBEPP_CPP20_CONSTEXPR void set_primitive(Byte* ptr, T value)
{
#if SBEPP_HAS_BITCAST
    auto arr = std::bit_cast<std::array<Byte, sizeof(T)>>(value);
    if((E == endian::native) || (sizeof(T) == 1))
    {
        std::copy(std::begin(arr), std::end(arr), ptr);
    }
    else
    {
        std::reverse_copy(std::begin(arr), std::end(arr), ptr);
    }
#else
    // old compilers don't optimize `std::copy` approach good enough, that's
    // why explicit `std::memcpy` call is required
    if((E != endian::native) && (sizeof(T) != 1))
    {
        value = static_cast<T>(byteswap(to_unsigned(value)));
    }
    std::memcpy(ptr, &value, sizeof(T));
#endif
}

struct fill_message_header_tag
{
    explicit fill_message_header_tag() = default;
};

struct fill_group_header_tag
{
    explicit fill_group_header_tag() = default;
};

struct size_bytes_tag
{
    explicit size_bytes_tag() = default;
};

struct addressof_tag
{
    explicit addressof_tag() = default;
};

struct end_ptr_tag
{
    explicit end_ptr_tag() = default;
};

struct get_header_tag
{
    explicit get_header_tag() = default;
};

struct get_block_length_tag
{
    explicit get_block_length_tag() = default;
};

struct get_bit_tag
{
    explicit get_bit_tag() = default;
};

struct set_bit_tag
{
    explicit set_bit_tag() = default;
};

struct get_level_tag
{
    explicit get_level_tag() = default;
};

struct visit_tag
{
    explicit visit_tag() = default;
};

struct visit_children_tag
{
    explicit visit_children_tag() = default;
};

struct enum_to_str_tag
{
    explicit enum_to_str_tag() = default;
};

struct visit_set_tag
{
    explicit visit_set_tag() = default;
};

template<typename T, typename U, endian E, typename View>
SBEPP_CPP20_CONSTEXPR T
    get_value(const View view, const std::size_t offset) noexcept
{
    SBEPP_SIZE_CHECK(
        view(addressof_tag{}), view(end_ptr_tag{}), offset, sizeof(U));
    return T{get_primitive<U, E>(view(addressof_tag{}) + offset)};
}

template<endian E, typename T, typename View>
SBEPP_CPP20_CONSTEXPR void
    set_value(const View view, const std::size_t offset, const T value) noexcept
{
    SBEPP_SIZE_CHECK(
        view(addressof_tag{}), view(end_ptr_tag{}), offset, sizeof(T));
    set_primitive<E>(view(addressof_tag{}) + offset, value);
}

template<typename Res, typename View>
SBEPP_CPP20_CONSTEXPR Res
    get_static_field_view(const View view, const std::size_t offset) noexcept
{
    SBEPP_SIZE_CHECK(view(addressof_tag{}), view(end_ptr_tag{}), offset, 0);
    return {view(addressof_tag{}) + offset, view(end_ptr_tag{})};
}

template<typename Group, typename View>
SBEPP_CPP20_CONSTEXPR Group
    get_first_dynamic_field_view(const View view) noexcept
{
    return {
        view(get_level_tag{}) + view(get_block_length_tag{}),
        view(end_ptr_tag{})};
}

template<typename Group, typename View, typename Prev>
SBEPP_CPP20_CONSTEXPR Group
    get_dynamic_field_view(const View view, const Prev prev) noexcept
{
    return {
        prev(addressof_tag{}) + prev(size_bytes_tag{}), view(end_ptr_tag{})};
}

//! @brief Base class for all reference semantics types
//! @tparam Byte byte type. Must have size equal to 1
template<typename Byte>
class byte_range
{
public:
    static_assert(sizeof(Byte) == 1, "Byte must represent a single byte");

    template<typename Byte2>
    friend class byte_range;

    //! @brief Initializes to `nullptr`
    byte_range() = default;

    //! @brief Constructs from a pair of pointers
    SBEPP_CPP14_CONSTEXPR byte_range(Byte* begin, Byte* end) noexcept
        : begin{begin}
#if SBEPP_SIZE_CHECKS_ENABLED
          ,
          end{end}
#endif
    {
        (void)end;
    }

    //! @brief Constructs from pointer and size
    constexpr byte_range(Byte* ptr, const std::size_t size) noexcept
        : byte_range{ptr, ptr + size}
    {
    }

    //! @brief Copy constructor. Available if `Byte2*` is convertible to `Byte*`
    template<typename Byte2, typename = enable_if_convertible_t<Byte2, Byte>>
    // NOLINTNEXTLINE: intentionally implicit
    constexpr byte_range(const byte_range<Byte2>& other) noexcept
        : begin{other.begin}
#if SBEPP_SIZE_CHECKS_ENABLED
          ,
          end{other.end}
#endif
    {
    }

    constexpr Byte* operator()(addressof_tag) const noexcept
    {
        return begin;
    }

    constexpr Byte* operator()(end_ptr_tag) const noexcept
    {
#if SBEPP_SIZE_CHECKS_ENABLED
        return end;
#else
        return nullptr;
#endif
    }

private:
    Byte* begin{};
#if SBEPP_SIZE_CHECKS_ENABLED
    Byte* end{};
#endif
};
} // namespace detail

/**
 * @brief Represents cursor which is used in cursor-based API. Clients should
 *  not use undocumented methods.
 *
 * @tparam Byte byte type
 */
template<typename Byte>
class cursor
{
public:
    //! @brief same as `Byte`
    using byte_type = Byte;

    // in accessors we have to use trailing return type because in "skip" mode
    // they should return `void`. In most cases it's not a problem but
    // `get_group_view` and `get_data_view` take lambdas which were not allowed
    // in unevaluated context (e.g. decltype) prior to C++20 so we need this
    // alias to specify return types for them. First attempt used
    // pointer-to-member instead of lambda but GCC doesn't support it so I
    // switched to simpler approach with lambda and trait.
    template<typename T>
    using result_type = T;

    //! @brief Construct a new cursor object initialized with `nullptr`
    cursor() = default;

    /**
     * @brief Construct from another cursor. Enabled only if `Byte2*` is
     *  convertible to `Byte*`.
     *
     * @tparam Byte2 `other`'s byte type
     * @param other cursor to construct from
     */
    template<
        typename Byte2,
        typename = detail::enable_if_convertible_t<Byte2, Byte>>
    // NOLINTNEXTLINE: implicit conversion is intentional
    constexpr cursor(cursor<Byte2> other) noexcept : ptr{other.ptr}
    {
    }

    /**
     * @brief Assign from another cursor. Enabled only if `Byte2*` is
     *  convertible to `Byte`
     *
     * @tparam Byte2 `other`'s byte type
     * @param other cursor to assign from
     * @return `*this`
     */
    template<
        typename Byte2,
        typename = detail::enable_if_convertible_t<Byte2, Byte>>
    SBEPP_CPP14_CONSTEXPR cursor& operator=(cursor<Byte2> other) noexcept
    {
        ptr = other.ptr;
        return *this;
    }

    /**
     * @brief access underlying pointer. Might be useful in rare cases to
     *  initialize cursor with a particular value.
     *
     * @return a reference to underlying pointer
     */
    SBEPP_CPP14_CONSTEXPR Byte*& pointer() noexcept
    {
        return ptr;
    }

    /**
     * @brief access underlying pointer
     *
     * @return underlying pointer
     */
    SBEPP_CPP14_CONSTEXPR Byte* pointer() const noexcept
    {
        return ptr;
    }

    template<typename T, typename U, endian E, typename View>
    SBEPP_CPP20_CONSTEXPR T get_value(
        const View view,
        const std::size_t offset,
        const std::size_t /*absolute_offset*/) noexcept
    {
        SBEPP_SIZE_CHECK(ptr, view(detail::end_ptr_tag{}), offset, sizeof(U));
        T res{detail::get_primitive<U, E>(ptr + offset)};
        ptr += offset + sizeof(U);
        return res;
    }

    template<endian E, typename T, typename View>
    SBEPP_CPP20_CONSTEXPR void set_value(
        const View view,
        const std::size_t offset,
        const std::size_t /*absolute_offset*/,
        const T value) noexcept
    {
        SBEPP_SIZE_CHECK(ptr, view(detail::end_ptr_tag{}), offset, sizeof(T));
        detail::set_primitive<E>(ptr + offset, value);
        ptr += offset + sizeof(T);
    }

    template<typename T, typename U, endian E, typename View>
    SBEPP_CPP20_CONSTEXPR T get_last_value(
        const View view,
        const std::size_t offset,
        const std::size_t /*absolute_offset*/) noexcept
    {
        SBEPP_SIZE_CHECK(ptr, view(detail::end_ptr_tag{}), offset, sizeof(U));
        auto res = T{detail::get_primitive<U, E>(ptr + offset)};
        ptr = view(detail::get_level_tag{})
              + view(detail::get_block_length_tag{});
        return res;
    }

    template<endian E, typename T, typename View>
    SBEPP_CPP20_CONSTEXPR void set_last_value(
        const View view,
        const std::size_t offset,
        const std::size_t /*absolute_offset*/,
        const T value) noexcept
    {
        SBEPP_SIZE_CHECK(ptr, view(detail::end_ptr_tag{}), offset, sizeof(T));
        detail::set_primitive<E>(ptr + offset, value);
        ptr = view(detail::get_level_tag{})
              + view(detail::get_block_length_tag{});
    }

    template<typename Res, typename View>
    SBEPP_CPP20_CONSTEXPR Res get_static_field_view(
        const View view,
        const std::size_t offset,
        const std::size_t /*absolute_offset*/) noexcept
    {
        SBEPP_SIZE_CHECK(ptr, view(detail::end_ptr_tag{}), offset, 0);
        Res res{ptr + offset, view(detail::end_ptr_tag{})};
        ptr += offset + res(detail::size_bytes_tag{});
        return res;
    }

    template<typename Res, typename View>
    SBEPP_CPP20_CONSTEXPR Res get_last_static_field_view(
        const View view,
        const std::size_t offset,
        const std::size_t /*absolute_offset*/) noexcept
    {
        SBEPP_SIZE_CHECK(ptr, view(detail::end_ptr_tag{}), offset, 0);
        Res res{ptr + offset, view(detail::end_ptr_tag{})};
        ptr = view(detail::get_level_tag{})
              + view(detail::get_block_length_tag{});
        return res;
    }

    template<typename ResView, typename View>
    SBEPP_CPP20_CONSTEXPR ResView get_first_group_view(const View view) noexcept
    {
        ptr = view(detail::get_level_tag{})
              + view(detail::get_block_length_tag{});
        ResView g{ptr, view(detail::end_ptr_tag{})};
        ptr += g(detail::get_header_tag{})(detail::size_bytes_tag{});

        return g;
    }

    template<typename ResView, typename View>
    SBEPP_CPP20_CONSTEXPR ResView get_first_data_view(const View view) noexcept
    {
        ptr = view(detail::get_level_tag{})
              + view(detail::get_block_length_tag{});
        ResView d{ptr, view(detail::end_ptr_tag{})};
        ptr += d(detail::size_bytes_tag{});

        return d;
    }

    template<typename ResView, typename View, typename Getter>
    SBEPP_CPP20_CONSTEXPR ResView
        get_group_view(const View view, Getter&& /*getter*/) noexcept
    {
        ResView res{ptr, view(detail::end_ptr_tag{})};
        auto header = res(detail::get_header_tag{});
        ptr += header(detail::size_bytes_tag{});
        return res;
    }

    template<typename ResView, typename View, typename Getter>
    SBEPP_CPP20_CONSTEXPR ResView
        get_data_view(const View view, Getter&& /*getter*/) noexcept
    {
        ResView res{ptr, view(detail::end_ptr_tag{})};
        ptr += res(detail::size_bytes_tag{});
        return res;
    }

private:
    template<typename T>
    friend class cursor;

    Byte* ptr{};
};

namespace detail
{
template<typename Byte>
class init_cursor_wrapper
{
public:
    using byte_type = Byte;

    template<typename T>
    using result_type = T;

    init_cursor_wrapper() = default;

    explicit constexpr init_cursor_wrapper(sbepp::cursor<Byte>& cursor)
        : cursor{&cursor}
    {
    }

    template<typename T, typename U, endian E, typename View>
    SBEPP_CPP20_CONSTEXPR T get_value(
        const View view,
        const std::size_t /*offset*/,
        const std::size_t absolute_offset) noexcept
    {
        SBEPP_SIZE_CHECK(
            view(addressof_tag{}),
            view(end_ptr_tag{}),
            absolute_offset,
            sizeof(U));
        T res{get_primitive<U, E>(view(addressof_tag{}) + absolute_offset)};
        cursor->pointer() = view(addressof_tag{}) + absolute_offset + sizeof(U);
        return res;
    }

    template<endian E, typename T, typename View>
    SBEPP_CPP20_CONSTEXPR void set_value(
        const View view,
        const std::size_t /*offset*/,
        const std::size_t absolute_offset,
        const T value) noexcept
    {
        SBEPP_SIZE_CHECK(
            view(addressof_tag{}),
            view(end_ptr_tag{}),
            absolute_offset,
            sizeof(T));
        set_primitive<E>(view(addressof_tag{}) + absolute_offset, value);
        cursor->pointer() = view(addressof_tag{}) + absolute_offset + sizeof(T);
    }

    template<typename T, typename U, endian E, typename View>
    SBEPP_CPP20_CONSTEXPR T get_last_value(
        const View view,
        const std::size_t /*offset*/,
        const std::size_t absolute_offset) noexcept
    {
        SBEPP_SIZE_CHECK(
            view(addressof_tag{}),
            view(end_ptr_tag{}),
            absolute_offset,
            sizeof(U));
        T res{get_primitive<U, E>(view(addressof_tag{}) + absolute_offset)};
        cursor->pointer() =
            view(get_level_tag{}) + view(get_block_length_tag{});
        return res;
    }

    template<endian E, typename T, typename View>
    SBEPP_CPP20_CONSTEXPR void set_last_value(
        const View view,
        const std::size_t /*offset*/,
        const std::size_t absolute_offset,
        const T value) noexcept
    {
        SBEPP_SIZE_CHECK(
            view(addressof_tag{}),
            view(end_ptr_tag{}),
            absolute_offset,
            sizeof(T));
        set_primitive<E>(view(addressof_tag{}) + absolute_offset, value);
        cursor->pointer() =
            view(get_level_tag{}) + view(get_block_length_tag{});
    }

    template<typename Res, typename View>
    SBEPP_CPP20_CONSTEXPR Res get_static_field_view(
        const View view,
        const std::size_t /*offset*/,
        const std::size_t absolute_offset) noexcept
    {
        SBEPP_SIZE_CHECK(
            view(addressof_tag{}), view(end_ptr_tag{}), absolute_offset, 0);
        Res res{view(addressof_tag{}) + absolute_offset, view(end_ptr_tag{})};
        cursor->pointer() = res(addressof_tag{}) + res(size_bytes_tag{});
        return res;
    }

    template<typename Res, typename View>
    SBEPP_CPP20_CONSTEXPR Res get_last_static_field_view(
        const View view,
        const std::size_t /*offset*/,
        const std::size_t absolute_offset) noexcept
    {
        SBEPP_SIZE_CHECK(
            view(addressof_tag{}), view(end_ptr_tag{}), absolute_offset, 0);
        Res res{view(addressof_tag{}) + absolute_offset, view(end_ptr_tag{})};
        cursor->pointer() =
            view(get_level_tag{}) + view(get_block_length_tag{});
        return res;
    }

    template<typename ResView, typename View>
    SBEPP_CPP20_CONSTEXPR ResView get_first_group_view(const View view) noexcept
    {
        return cursor->template get_first_group_view<ResView>(view);
    }

    template<typename ResView, typename View>
    SBEPP_CPP20_CONSTEXPR ResView get_first_data_view(const View view) noexcept
    {
        return cursor->template get_first_data_view<ResView>(view);
    }

    template<typename ResView, typename View, typename Getter>
    SBEPP_CPP20_CONSTEXPR ResView
        get_group_view(const View /*view*/, Getter&& getter) noexcept
    {
        auto res = getter();
        auto header = res(get_header_tag{});
        cursor->pointer() = res(addressof_tag{}) + header(size_bytes_tag{});
        return res;
    }

    template<typename ResView, typename View, typename Getter>
    SBEPP_CPP20_CONSTEXPR ResView
        get_data_view(const View /*view*/, Getter&& getter) noexcept
    {
        auto res = getter();
        cursor->pointer() = res(addressof_tag{}) + res(size_bytes_tag{});
        return res;
    }

private:
    sbepp::cursor<Byte>* cursor{};
};

template<typename Byte>
class init_dont_move_cursor_wrapper
{
public:
    using byte_type = Byte;

    template<typename T>
    using result_type = T;

    init_dont_move_cursor_wrapper() = default;

    explicit constexpr init_dont_move_cursor_wrapper(
        sbepp::cursor<Byte>& cursor)
        : cursor{&cursor}
    {
    }

    template<typename T, typename U, endian E, typename View>
    SBEPP_CPP20_CONSTEXPR T get_value(
        const View view,
        const std::size_t offset,
        const std::size_t absolute_offset) noexcept
    {
        SBEPP_SIZE_CHECK(
            view(addressof_tag{}),
            view(end_ptr_tag{}),
            absolute_offset,
            sizeof(U));
        cursor->pointer() = view(addressof_tag{}) + absolute_offset - offset;
        return T{get_primitive<U, E>(view(addressof_tag{}) + absolute_offset)};
    }

    template<endian E, typename T, typename View>
    SBEPP_CPP20_CONSTEXPR void set_value(
        const View view,
        const std::size_t offset,
        const std::size_t absolute_offset,
        const T value) noexcept
    {
        SBEPP_SIZE_CHECK(
            view(addressof_tag{}),
            view(end_ptr_tag{}),
            absolute_offset,
            sizeof(T));
        cursor->pointer() = view(addressof_tag{}) + absolute_offset - offset;
        set_primitive<E>(view(addressof_tag{}) + absolute_offset, value);
    }

    template<typename T, typename U, endian E, typename View>
    SBEPP_CPP20_CONSTEXPR T get_last_value(
        const View view,
        const std::size_t offset,
        const std::size_t absolute_offset) noexcept
    {
        return get_value<T, U, E>(view, offset, absolute_offset);
    }

    template<endian E, typename T, typename View>
    SBEPP_CPP20_CONSTEXPR void set_last_value(
        const View view,
        const std::size_t offset,
        const std::size_t absolute_offset,
        const T value) noexcept
    {
        return set_value<E>(view, offset, absolute_offset, value);
    }

    template<typename Res, typename View>
    SBEPP_CPP20_CONSTEXPR Res get_static_field_view(
        const View view,
        const std::size_t offset,
        const std::size_t absolute_offset) noexcept
    {
        SBEPP_SIZE_CHECK(
            view(addressof_tag{}), view(end_ptr_tag{}), absolute_offset, 0);
        cursor->pointer() = view(addressof_tag{}) + absolute_offset - offset;
        return {view(addressof_tag{}) + absolute_offset, view(end_ptr_tag{})};
    }

    template<typename Res, typename View>
    SBEPP_CPP20_CONSTEXPR Res get_last_static_field_view(
        const View view,
        const std::size_t offset,
        const std::size_t absolute_offset) noexcept
    {
        return get_static_field_view<Res>(view, offset, absolute_offset);
    }

    template<typename ResView, typename View>
    SBEPP_CPP20_CONSTEXPR ResView get_first_group_view(const View view) noexcept
    {
        cursor->pointer() =
            view(get_level_tag{}) + view(get_block_length_tag{});
        ResView g{cursor->pointer(), view(end_ptr_tag{})};

        return g;
    }

    template<typename ResView, typename View>
    SBEPP_CPP20_CONSTEXPR ResView get_first_data_view(const View view) noexcept
    {
        cursor->pointer() =
            view(get_level_tag{}) + view(get_block_length_tag{});
        ResView d{cursor->pointer(), view(end_ptr_tag{})};

        return d;
    }

    template<typename ResView, typename View, typename Getter>
    SBEPP_CPP20_CONSTEXPR ResView
        get_group_view(const View /*view*/, Getter&& getter) noexcept
    {
        auto res = getter();
        cursor->pointer() = res(addressof_tag{});
        return res;
    }

    template<typename ResView, typename View, typename Getter>
    SBEPP_CPP20_CONSTEXPR ResView
        get_data_view(const View /*view*/, Getter&& getter) noexcept
    {
        auto res = getter();
        cursor->pointer() = res(addressof_tag{});
        return res;
    }

private:
    sbepp::cursor<Byte>* cursor{};
};

template<typename Byte>
class dont_move_cursor_wrapper
{
public:
    using byte_type = Byte;

    template<typename T>
    using result_type = T;

    dont_move_cursor_wrapper() = default;

    explicit constexpr dont_move_cursor_wrapper(sbepp::cursor<Byte>& cursor)
        : cursor{&cursor}
    {
    }

    template<typename T, typename U, endian E, typename View>
    SBEPP_CPP20_CONSTEXPR T get_value(
        const View view,
        const std::size_t offset,
        const std::size_t /*absolute_offset*/) noexcept
    {
        SBEPP_SIZE_CHECK(
            cursor->pointer(), view(end_ptr_tag{}), offset, sizeof(U));
        return T{get_primitive<U, E>(cursor->pointer() + offset)};
    }

    template<endian E, typename T, typename View>
    SBEPP_CPP20_CONSTEXPR void set_value(
        const View view,
        const std::size_t offset,
        const std::size_t /*absolute_offset*/,
        const T value) noexcept
    {
        SBEPP_SIZE_CHECK(
            cursor->pointer(), view(end_ptr_tag{}), offset, sizeof(T));
        set_primitive<E>(cursor->pointer() + offset, value);
    }

    template<typename T, typename U, endian E, typename View>
    SBEPP_CPP20_CONSTEXPR T get_last_value(
        const View view,
        const std::size_t offset,
        const std::size_t /*absolute_offset*/) noexcept
    {
        SBEPP_SIZE_CHECK(
            cursor->pointer(), view(end_ptr_tag{}), offset, sizeof(U));
        return T{get_primitive<U, E>(cursor->pointer() + offset)};
    }

    template<endian E, typename T, typename View>
    SBEPP_CPP20_CONSTEXPR void set_last_value(
        const View view,
        const std::size_t offset,
        const std::size_t /*absolute_offset*/,
        const T value) noexcept
    {
        SBEPP_SIZE_CHECK(
            cursor->pointer(), view(end_ptr_tag{}), offset, sizeof(T));
        set_primitive<E>(cursor->pointer() + offset, value);
    }

    template<typename Res, typename View>
    SBEPP_CPP20_CONSTEXPR Res get_static_field_view(
        const View view,
        const std::size_t offset,
        const std::size_t /*absolute_offset*/) noexcept
    {
        SBEPP_SIZE_CHECK(cursor->pointer(), view(end_ptr_tag{}), offset, 0);
        return {cursor->pointer() + offset, view(end_ptr_tag{})};
    }

    template<typename Res, typename View>
    SBEPP_CPP20_CONSTEXPR Res get_last_static_field_view(
        const View view,
        const std::size_t offset,
        const std::size_t /*absolute_offset*/) noexcept
    {
        SBEPP_SIZE_CHECK(cursor->pointer(), view(end_ptr_tag{}), offset, 0);
        return {cursor->pointer() + offset, view(end_ptr_tag{})};
    }

    template<typename ResView, typename View>
    SBEPP_CPP20_CONSTEXPR ResView get_first_group_view(const View view) noexcept
    {
        cursor->pointer() =
            view(get_level_tag{}) + view(get_block_length_tag{});
        ResView g{cursor->pointer(), view(end_ptr_tag{})};

        return g;
    }

    template<typename ResView, typename View>
    SBEPP_CPP20_CONSTEXPR ResView get_first_data_view(const View view) noexcept
    {
        cursor->pointer() =
            view(get_level_tag{}) + view(get_block_length_tag{});
        ResView d{cursor->pointer(), view(end_ptr_tag{})};

        return d;
    }

    template<typename ResView, typename View, typename Getter>
    SBEPP_CPP20_CONSTEXPR ResView
        get_group_view(const View view, Getter&& /*getter*/) noexcept
    {
        return {cursor->pointer(), view(end_ptr_tag{})};
    }

    template<typename ResView, typename View, typename Getter>
    SBEPP_CPP20_CONSTEXPR ResView
        get_data_view(const View view, Getter&& /*getter*/) noexcept
    {
        return {cursor->pointer(), view(end_ptr_tag{})};
    }

private:
    sbepp::cursor<Byte>* cursor{};
};

template<typename Byte>
class skip_cursor_wrapper
{
public:
    using byte_type = Byte;

    template<typename T>
    using result_type = void;

    skip_cursor_wrapper() = default;

    explicit constexpr skip_cursor_wrapper(sbepp::cursor<Byte>& cursor)
        : cursor{&cursor}
    {
    }

    template<typename T, typename U, endian E, typename View>
    SBEPP_CPP20_CONSTEXPR void get_value(
        const View view,
        const std::size_t offset,
        const std::size_t /*absolute_offset*/) noexcept
    {
        SBEPP_SIZE_CHECK(
            cursor->pointer(), view(end_ptr_tag{}), offset, sizeof(U));
        cursor->pointer() += offset + sizeof(U);
    }

    template<typename T, typename U, endian E, typename View>
    SBEPP_CPP20_CONSTEXPR void get_last_value(
        const View view,
        const std::size_t offset,
        const std::size_t /*absolute_offset*/) noexcept
    {
        SBEPP_SIZE_CHECK(
            cursor->pointer(), view(end_ptr_tag{}), offset, sizeof(U));
        cursor->pointer() =
            view(get_level_tag{}) + view(get_block_length_tag{});
    }

    template<typename Res, typename View>
    SBEPP_CPP20_CONSTEXPR void get_static_field_view(
        const View view,
        const std::size_t offset,
        const std::size_t /*absolute_offset*/) noexcept
    {
        SBEPP_SIZE_CHECK(cursor->pointer(), view(end_ptr_tag{}), offset, 0);
        Res res{cursor->pointer(), view(end_ptr_tag{})};
        cursor->pointer() += offset + res(size_bytes_tag{});
    }

    template<typename Res, typename View>
    SBEPP_CPP20_CONSTEXPR void get_last_static_field_view(
        const View view,
        const std::size_t offset,
        const std::size_t /*absolute_offset*/) noexcept
    {
        SBEPP_SIZE_CHECK(cursor->pointer(), view(end_ptr_tag{}), offset, 0);
        cursor->pointer() =
            view(get_level_tag{}) + view(get_block_length_tag{});
    }

    template<typename ResView, typename View>
    SBEPP_CPP20_CONSTEXPR void get_first_group_view(const View view) noexcept
    {
        cursor->pointer() =
            view(get_level_tag{}) + view(get_block_length_tag{});
        ResView g{cursor->pointer(), view(end_ptr_tag{})};
        cursor->pointer() += g(size_bytes_tag{});
    }

    template<typename ResView, typename View>
    SBEPP_CPP20_CONSTEXPR void get_first_data_view(const View view) noexcept
    {
        cursor->pointer() =
            view(get_level_tag{}) + view(get_block_length_tag{});
        ResView d{cursor->pointer(), view(end_ptr_tag{})};
        cursor->pointer() += d(size_bytes_tag{});
    }

    template<typename ResView, typename View, typename Getter>
    SBEPP_CPP20_CONSTEXPR void
        get_group_view(const View view, Getter&& /*getter*/) noexcept
    {
        ResView res{cursor->pointer(), view(end_ptr_tag{})};
        cursor->pointer() += res(size_bytes_tag{});
    }

    template<typename ResView, typename View, typename Getter>
    SBEPP_CPP20_CONSTEXPR void
        get_data_view(const View view, Getter&& /*getter*/) noexcept
    {
        ResView res{cursor->pointer(), view(end_ptr_tag{})};
        cursor->pointer() += res(size_bytes_tag{});
    }

private:
    sbepp::cursor<Byte>* cursor{};
};

template<typename Cursor, typename T>
using cursor_result_type_t =
    typename remove_reference_t<Cursor>::template result_type<T>;

template<typename Cursor>
using cursor_byte_type_t = typename remove_reference_t<Cursor>::byte_type;

template<typename MessageByte, typename CursorByte>
using enable_if_cursor_compatible_t =
    enable_if_convertible_t<MessageByte, CursorByte>;

template<typename MessageByte, typename CursorByte>
using enable_if_cursor_writeable_t = enable_if_t<
    std::is_convertible<MessageByte*, CursorByte*>::value
    && !std::is_const<MessageByte>::value && !std::is_const<CursorByte>::value>;
} // namespace detail

/**
 * @brief Returns the size of the underlying data represented by
 *  message/group/entry/data/composite view, including headers.
 *
 *  In general case, for message/group/entry, the complexity is O(a^n)
 *  where `n` is the depth of the level. For composites and data it's O(1).
 *
 * @param v view object
 * @return size in bytes
 */
template<typename T>
constexpr std::size_t size_bytes(T v) noexcept
{
    return v(detail::size_bytes_tag{});
}

/**
 * @brief Similar to `sbepp::size_bytes` but has constant complexity because it
 *  expects `c` to be set at the end of `v`. Useful to read the whole message
 *  and then get its size in constant time. Not applicable to composites.
 *
 * @param v view object
 * @param c cursor
 * @return size in bytes
 */
template<typename T, typename Byte>
constexpr std::size_t size_bytes(T v, cursor<Byte> c) noexcept
{
    return v(detail::size_bytes_tag{}, c);
}

/**
 * @brief Returns the header of a message/group
 *
 * @param v message/group view
 * @return header composite view
 */
template<typename T>
constexpr auto get_header(T v) noexcept -> decltype(v(detail::get_header_tag{}))
{
    return v(detail::get_header_tag{});
}

/**
 * @brief Returns pointer to the underlying data referenced by a view
 *
 * @param v view
 * @return pointer to the underlying data
 */
template<typename T>
constexpr auto addressof(T v) noexcept -> decltype(v(detail::addressof_tag{}))
{
    return v(detail::addressof_tag{});
}

/**
 * @brief Trait to get view's byte type
 *
 * @tparam View view type
 */
template<typename View>
struct byte_type
{
    //! @brief holds `View`'s byte type
    using type = typename std::remove_pointer<decltype(addressof(
        std::declval<View>()))>::type;
};

//! @brief Shortcut for `byte_type<T>::type`
template<typename View>
using byte_type_t = typename byte_type<View>::type;

/**
 * @brief Contains cursor wrappers which allow more precise control over its
 *  position
 */
namespace cursor_ops
{
/**
 * @brief Returns a wrapper which will initialize the cursor when it's used and
 *  advance after the usage.
 *
 * Allows to start using cursor API from any field/group/data.
 * Example:
 * ```cpp
 * schema::messages::msg1<char> m{ptr, size};
 * sbepp::cursor<char> c;   // Note: not initialized
 * // initialize, use, advance
 * auto first = m.firstField(sbepp::cursor_ops::init(c));
 * auto second = m.secondField(c); // continue as usual
 * ```
 *
 * @param c original cursor
 * @return unspecified cursor wrapper
 */
template<typename Byte>
constexpr detail::init_cursor_wrapper<Byte> init(cursor<Byte>& c) noexcept
{
    return detail::init_cursor_wrapper<Byte>{c};
}

/**
 * @brief Returns a wrapper which doesn't advance the cursor when it's used.
 *
 * Allows to access field/group/data more than once using cursor API. Useful to
 * read and write the same field or to write `data` member.
 * Example:
 * ```cpp
 * schema::messages::msg1<char> m{ptr, size};
 * auto c = sbepp::init_cursor(m);
 * // use but don't advance, otherwise we can't access `field` again
 * auto value = m.field(sbepp::cursor_ops::dont_move(c));
 * m.field(*value + 1, c);
 * // since data has unknown size, without `dont_move` cursor will be moved
 * // to a random position
 * auto d = m.data(sbepp::cursor_ops::dont_move(c));
 * d.resize(1); // init data somehow
 * m.data(c);   // advance the cursor
 * ```
 *
 * @param c original cursor
 * @return unspecified cursor wrapper
 */
template<typename Byte>
constexpr detail::dont_move_cursor_wrapper<Byte>
    dont_move(cursor<Byte>& c) noexcept
{
    return detail::dont_move_cursor_wrapper<Byte>{c};
}

/**
 * @brief Returns a wrapper which initializes the cursor but doesn't move it.
 *  Behaves like a combination of `init()` and `dont_move()`.
 *
 * @param c original cursor
 * @return unspecified cursor wrapper
 */
template<typename Byte>
constexpr detail::init_dont_move_cursor_wrapper<Byte>
    init_dont_move(cursor<Byte>& c) noexcept
{
    return detail::init_dont_move_cursor_wrapper<Byte>{c};
}

/**
 * @brief Returns a wrapper which moves the cursor to the end of
 *  field/group/data without returning the accessed value.
 *
 * Allows to skip uninteresting members. Example:
 * ```cpp
 * schema::messages::msg1<char> m{ptr, size};
 * auto c = sbepp::init_cursor(m);
 * m.group(sbepp::cursor_ops::skip(c)); // skip the whole group
 * auto d = m.data(c);  // access next member
 * ```
 * @param c original cursor
 * @return unspecified cursor wrapper
 */
template<typename Byte>
constexpr detail::skip_cursor_wrapper<Byte> skip(cursor<Byte>& c) noexcept
{
    return detail::skip_cursor_wrapper<Byte>{c};
}
} // namespace cursor_ops

namespace detail
{
// the only purpose of this class is to implement `is_composite` trait
//! @brief Base class for composites
template<typename Byte>
class composite_base : public byte_range<Byte>
{
public:
    using byte_range<Byte>::byte_range;
    using byte_range<Byte>::operator();
};

//! @brief Base class for messages
template<typename Byte, typename Header>
class message_base : public byte_range<Byte>
{
public:
    using byte_range<Byte>::byte_range;
    using byte_range<Byte>::operator();

    SBEPP_CPP14_CONSTEXPR Header operator()(get_header_tag) const noexcept
    {
        Header header{(*this)(addressof_tag{}), (*this)(end_ptr_tag{})};
        SBEPP_SIZE_CHECK(
            (*this)(addressof_tag{}),
            (*this)(end_ptr_tag{}),
            0,
            size_bytes(header));
        return header;
    }

    SBEPP_CPP14_CONSTEXPR Byte* operator()(get_level_tag) const noexcept
    {
        auto header = (*this)(get_header_tag{});
        return header(addressof_tag{}) + header(size_bytes_tag{});
    }

    constexpr typename std::decay<
        decltype(std::declval<Header>().blockLength().value())>::type
        operator()(get_block_length_tag) const noexcept
    {
        return operator()(get_header_tag{}).blockLength().value();
    }

    template<
        typename Byte2,
        typename = ::sbepp::detail::enable_if_cursor_compatible_t<Byte, Byte2>>
    SBEPP_CPP20_CONSTEXPR std::size_t operator()(
        ::sbepp::detail::size_bytes_tag,
        ::sbepp::cursor<Byte2>& c) const noexcept
    {
        return c.pointer() - (*this)(addressof_tag{});
    }
};

//! @brief Base class for group entries
template<typename Byte, typename BlockLengthType>
class entry_base : public byte_range<Byte>
{
public:
    using byte_range<Byte>::operator();

    template<typename Byte2, typename BlockLengthType2>
    friend class entry_base;

    //! @brief Constructs using `nullptr`
    entry_base() = default;

    //! @brief Constructs from two pointers
    constexpr entry_base(
        Byte* ptr, Byte* end, BlockLengthType block_length) noexcept
        : byte_range<Byte>{ptr, end}, block_length{block_length}
    {
    }

    //! @brief Constructs from pointer and size
    constexpr entry_base(
        Byte* ptr,
        const std::size_t size,
        const BlockLengthType block_length) noexcept
        : entry_base{ptr, ptr + size, block_length}
    {
    }

    //! @brief Constructs from cursor
    template<typename Byte2, typename = enable_if_convertible_t<Byte2, Byte>>
    constexpr entry_base(
        cursor<Byte2>& c, Byte* end_ptr, BlockLengthType block_length) noexcept
        : entry_base{c.pointer(), end_ptr, block_length}
    {
        // forwards to the above one for non-empty entries. Empty entries have
        // implementation in the derived class which advances cursor up to
        // `block_length`.
    }

    //! @brief Constructs from `entry_base` of compatible byte type. Available
    //!  if `Byte2*` is convertible to `Byte*`
    template<typename Byte2, typename = enable_if_convertible_t<Byte2, Byte>>
    constexpr entry_base(
        const entry_base<Byte2, BlockLengthType>& other) noexcept
        : byte_range<Byte>{other}, block_length{other.block_length}
    {
    }

    constexpr BlockLengthType operator()(get_block_length_tag) const noexcept
    {
        return block_length;
    }

    constexpr Byte* operator()(get_level_tag) const noexcept
    {
        return (*this)(addressof_tag{});
    }

private:
    BlockLengthType block_length{};
};

template<
    typename Byte,
    typename ValueType,
    typename IndexType,
    typename DifferenceType,
    typename BlockLengthType>
class forward_iterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = ValueType;
    using reference = value_type;
    using difference_type = DifferenceType;
    // for compatibility with `std::iterator_traits`
    using pointer = void;

    forward_iterator() = default;

    SBEPP_CPP14_CONSTEXPR forward_iterator(
        Byte* ptr,
        const IndexType index,
        const BlockLengthType block_length,
        Byte* end) noexcept
        : ptr{ptr},
          index{index},
          block_length{block_length}
#if SBEPP_SIZE_CHECKS_ENABLED
          ,
          end{end}
#endif
    {
        (void)end;
    }

    constexpr reference operator*() const noexcept
    {
#if SBEPP_SIZE_CHECKS_ENABLED
        return {ptr, end, block_length};
#else
        return {ptr, nullptr, block_length};
#endif
    }

    SBEPP_CPP14_CONSTEXPR forward_iterator& operator++() noexcept
    {
        SBEPP_SIZE_CHECK(ptr, end, 0, ::sbepp::size_bytes(operator*()));
        ptr += ::sbepp::size_bytes(operator*());
        index++;
        return *this;
    }

    SBEPP_CPP14_CONSTEXPR forward_iterator operator++(int) noexcept
    {
        auto old = *this;
        operator++();
        return old;
    }

    friend constexpr bool operator==(
        const forward_iterator& lhs, const forward_iterator& rhs) noexcept
    {
        return lhs.index == rhs.index;
    }

    friend constexpr bool operator!=(
        const forward_iterator& lhs, const forward_iterator& rhs) noexcept
    {
        return lhs.index != rhs.index;
    }

private:
    Byte* ptr{};
    IndexType index{};
    BlockLengthType block_length{};
#if SBEPP_SIZE_CHECKS_ENABLED
    Byte* end{};
#endif
};

template<
    typename Byte,
    typename ValueType,
    typename BlockLengthType,
    typename DifferenceType,
    typename IndexType>
class random_access_iterator
{
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = ValueType;
    using reference = value_type;
    using difference_type = DifferenceType;
    // for compatibility with `std::iterator_traits`
    using pointer = void;

    random_access_iterator() = default;

    SBEPP_CPP14_CONSTEXPR random_access_iterator(
        Byte* ptr,
        const BlockLengthType block_length,
        const IndexType index,
        Byte* end) noexcept
        : ptr{ptr},
          block_length{block_length},
          index{index}
#if SBEPP_SIZE_CHECKS_ENABLED
          ,
          end{end}
#endif
    {
        (void)end;
    }

    constexpr reference operator*() const noexcept
    {
#if SBEPP_SIZE_CHECKS_ENABLED
        return {ptr, end, block_length};
#else
        return {ptr, nullptr, block_length};
#endif
    }

    SBEPP_CPP14_CONSTEXPR random_access_iterator& operator++() noexcept
    {
        SBEPP_SIZE_CHECK(ptr, end, 0, block_length);
        ptr += block_length;
        index++;
        return *this;
    }

    SBEPP_CPP14_CONSTEXPR random_access_iterator operator++(int) noexcept
    {
        auto old = *this;
        operator++();
        return old;
    }

    SBEPP_CPP14_CONSTEXPR random_access_iterator& operator--() noexcept
    {
        ptr -= block_length;
        index--;
        return *this;
    }

    SBEPP_CPP14_CONSTEXPR random_access_iterator operator--(int) noexcept
    {
        auto old = *this;
        operator--();
        return old;
    }

    SBEPP_CPP14_CONSTEXPR random_access_iterator&
        operator+=(difference_type n) noexcept
    {
        ptr += n * block_length;
        index += n;
        return *this;
    }

    SBEPP_CPP14_CONSTEXPR random_access_iterator
        operator+(difference_type n) const noexcept
    {
        auto tmp = *this;
        return tmp += n;
    }

    friend constexpr random_access_iterator
        operator+(difference_type n, const random_access_iterator& it) noexcept
    {
        return it + n;
    }

    SBEPP_CPP14_CONSTEXPR random_access_iterator&
        operator-=(difference_type n) noexcept
    {
        return *this += -n;
    }

    SBEPP_CPP14_CONSTEXPR random_access_iterator
        operator-(difference_type n) const noexcept
    {
        auto tmp = *this;
        return tmp -= n;
    }

    constexpr difference_type
        operator-(const random_access_iterator& rhs) const noexcept
    {
        return index - rhs.index;
    }

    constexpr reference operator[](difference_type n) const noexcept
    {
        return *(*this + n);
    }

    friend constexpr bool operator==(
        const random_access_iterator& lhs,
        const random_access_iterator& rhs) noexcept
    {
        return lhs.index == rhs.index;
    }

    friend constexpr bool operator!=(
        const random_access_iterator& lhs,
        const random_access_iterator& rhs) noexcept
    {
        return lhs.index != rhs.index;
    }

    friend constexpr bool operator<(
        const random_access_iterator& lhs,
        const random_access_iterator& rhs) noexcept
    {
        return lhs.index < rhs.index;
    }

    friend constexpr bool operator<=(
        const random_access_iterator& lhs,
        const random_access_iterator& rhs) noexcept
    {
        return lhs.index <= rhs.index;
    }

    friend constexpr bool operator>(
        const random_access_iterator& lhs,
        const random_access_iterator& rhs) noexcept
    {
        return lhs.index > rhs.index;
    }

    friend constexpr bool operator>=(
        const random_access_iterator& lhs,
        const random_access_iterator& rhs) noexcept
    {
        return lhs.index >= rhs.index;
    }

private:
    Byte* ptr{};
    BlockLengthType block_length{};
    // iterator should be index-based instead of just pointer-based to support
    // groups with empty entries, i.e. when `block_length == 0`
    IndexType index{};
#if SBEPP_SIZE_CHECKS_ENABLED
    Byte* end{};
#endif
};

template<
    typename ValueType,
    typename IndexType,
    typename CursorType,
    typename BlockLengthType,
    typename Byte>
class input_iterator
{
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = ValueType;
    using reference = value_type;
    using difference_type = typename std::make_signed<IndexType>::type;
    // for compatibility with `std::iterator_traits`
    using pointer = void;

    input_iterator() = default;

    SBEPP_CPP14_CONSTEXPR input_iterator(
        const IndexType index,
        CursorType* cursor,
        BlockLengthType block_length,
        Byte* end) noexcept
        : index{index},
          cursor{cursor},
          block_length{block_length}
#if SBEPP_SIZE_CHECKS_ENABLED
          ,
          end{end}
#endif
    {
        (void)end;
    }

    constexpr reference operator*() const noexcept
    {
#if SBEPP_SIZE_CHECKS_ENABLED
        return {*cursor, end, block_length};
#else
        return {*cursor, nullptr, block_length};
#endif
    }

    SBEPP_CPP14_CONSTEXPR input_iterator& operator++() noexcept
    {
        index++;
        return *this;
    }

    SBEPP_CPP14_CONSTEXPR input_iterator operator++(int) noexcept
    {
        auto old = *this;
        operator++();
        return old;
    }

    friend constexpr bool operator==(
        const input_iterator& lhs, const input_iterator& rhs) noexcept
    {
        return lhs.index == rhs.index;
    }

    friend constexpr bool operator!=(
        const input_iterator& lhs, const input_iterator& rhs) noexcept
    {
        return lhs.index != rhs.index;
    }

private:
    IndexType index{};
    CursorType* cursor{};
    BlockLengthType block_length{};
#if SBEPP_SIZE_CHECKS_ENABLED
    Byte* end{};
#endif
};

template<
    typename ValueType,
    typename IndexType,
    typename CursorType,
    typename BlockLengthType,
    typename Byte>
class cursor_range
{
public:
    SBEPP_CPP14_CONSTEXPR cursor_range(
        CursorType& cursor,
        BlockLengthType block_length,
        Byte* end,
        IndexType start_pos,
        IndexType length) noexcept
        : cursor{&cursor},
          block_length{block_length},
          start_pos{start_pos},
#if SBEPP_SIZE_CHECKS_ENABLED
          end_ptr{end},
#endif
          length{length}
    {
        (void)end;
    }

    using iterator =
        input_iterator<ValueType, IndexType, CursorType, BlockLengthType, Byte>;

    constexpr iterator begin() const noexcept
    {
#if SBEPP_SIZE_CHECKS_ENABLED
        return {start_pos, cursor, block_length, end_ptr};
#else
        return {start_pos, cursor, block_length, nullptr};
#endif
    }

    constexpr iterator end() const noexcept
    {
#if SBEPP_SIZE_CHECKS_ENABLED
        return {
            static_cast<IndexType>(start_pos + size()),
            cursor,
            block_length,
            end_ptr};
#else
        return {
            static_cast<IndexType>(start_pos + size()),
            cursor,
            block_length,
            nullptr};
#endif
    }

    constexpr IndexType size() const noexcept
    {
        return length;
    }

private:
    CursorType* cursor{};
    BlockLengthType block_length{};
    IndexType start_pos{};
#if SBEPP_SIZE_CHECKS_ENABLED
    Byte* end_ptr{};
#endif
    IndexType length{};
};

//! @brief Base class for a flat group
template<typename Byte, typename Entry, typename Dimension>
class flat_group_base : public byte_range<Byte>
{
public:
    //! @brief Entry type
    using value_type = Entry;
    //! @brief `value_type`
    using reference = value_type;
    //! @brief `numInGroup` value type
    using sbe_size_type = typename std::decay<
        decltype(std::declval<Dimension>().numInGroup())>::type;
    //! @brief Raw size type
    using size_type = typename sbe_size_type::value_type;
    //! @brief signed `size_type`
    using difference_type = typename std::make_signed<size_type>::type;
    //! @brief Random access iterator to `value_type`. Satisfies
    //!     `std::random_access_iterator`
    using iterator = random_access_iterator<
        Byte,
        Entry,
        typename std::decay<
            decltype(std::declval<Dimension>().blockLength().value())>::type,
        difference_type,
        size_type>;

    using byte_range<Byte>::byte_range;
    using byte_range<Byte>::operator();

    SBEPP_CPP14_CONSTEXPR Dimension operator()(get_header_tag) const noexcept
    {
        Dimension header{(*this)(addressof_tag{}), (*this)(end_ptr_tag{})};
        SBEPP_SIZE_CHECK(
            (*this)(addressof_tag{}),
            (*this)(end_ptr_tag{}),
            0,
            size_bytes(header));
        return header;
    }

    SBEPP_CPP20_CONSTEXPR std::size_t operator()(size_bytes_tag) const noexcept
    {
        auto dimension = (*this)(get_header_tag{});
        return size_bytes(dimension)
               + dimension.numInGroup().value()
                     * dimension.blockLength().value();
    }

    //! @brief Returns header's `numInGroup`
    SBEPP_CPP20_CONSTEXPR sbe_size_type sbe_size() const noexcept
    {
        return (*this)(get_header_tag{}).numInGroup();
    }

    //! @brief Returns raw size
    SBEPP_CPP20_CONSTEXPR size_type size() const noexcept
    {
        return sbe_size().value();
    }

    //! @brief Sets `numInGroup` to `count`
    SBEPP_CPP20_CONSTEXPR void resize(const size_type count) const noexcept
    {
        (*this)(get_header_tag{}).numInGroup(count);
    }

    //! @brief Checks if `size() == 0`
    SBEPP_CPP17_NODISCARD SBEPP_CPP20_CONSTEXPR bool empty() const noexcept
    {
        return !size();
    }

    //! @brief Returns `numInGroup`'s `maxValue`
    constexpr static size_type max_size() noexcept
    {
        return sbe_size_type::max_value();
    }

    //! @brief Returns an iterator to the beginning
    SBEPP_CPP14_CONSTEXPR iterator begin() const noexcept
    {
        auto dimension = (*this)(get_header_tag{});
        return iterator{
            (*this)(addressof_tag{}) + size_bytes(dimension),
            dimension.blockLength().value(),
            0,
            (*this)(end_ptr_tag{})};
    }

    //! @brief Returns an iterator to the end
    constexpr iterator end() const noexcept
    {
        return iterator{
            (*this)(addressof_tag{}) + (*this)(size_bytes_tag{}),
            (*this)(get_header_tag{}).blockLength().value(),
            size(),
            (*this)(end_ptr_tag{})};
    }

    //! @brief Access group entry at `pos`
    //! @pre `pos < size()`
    SBEPP_CPP14_CONSTEXPR reference operator[](size_type pos) const noexcept
    {
        SBEPP_ASSERT(pos < size());
        return *(begin() + pos);
    }

    //! @brief Returns the first entry
    //! @pre `!empty()`
    SBEPP_CPP14_CONSTEXPR reference front() const noexcept
    {
        SBEPP_ASSERT(!empty());
        return *begin();
    }

    //! @brief Returns the last entry
    //! @pre `!empty()`
    SBEPP_CPP14_CONSTEXPR reference back() const noexcept
    {
        SBEPP_ASSERT(!empty());
        return *(--end());
    }

    //! @brief Resizes to `0`
    //! @post `size() == 0`
    SBEPP_CPP14_CONSTEXPR void clear() const noexcept
    {
        resize(0);
    }

    //! @brief Type of a cursor range. Satisfies `std::ranges::input_range`
    template<typename Byte2>
    using cursor_range_t = ::sbepp::detail::cursor_range<
        value_type,
        size_type,
        cursor<Byte2>,
        typename std::decay<
            decltype(std::declval<Dimension>().blockLength().value())>::type,
        Byte>;

    //! @brief Returns cursor range to all group entries
    template<
        typename Byte2,
        typename = enable_if_cursor_compatible_t<Byte, Byte2>>
    SBEPP_CPP20_CONSTEXPR cursor_range_t<Byte2>
        cursor_range(cursor<Byte2>& c) const noexcept
    {
        return {
            c,
            (*this)(get_header_tag{}).blockLength().value(),
            (*this)(end_ptr_tag{}),
            0,
            size()};
    }

    //! @brief Returns cursor range to [pos; size()) entries
    //! @pre `pos < size()`
    template<
        typename Byte2,
        typename = enable_if_cursor_compatible_t<Byte, Byte2>>
    SBEPP_CPP20_CONSTEXPR cursor_range_t<Byte2>
        cursor_subrange(cursor<Byte2>& c, const size_type pos) const noexcept
    {
        SBEPP_ASSERT(pos < size());

        return {
            c,
            (*this)(get_header_tag{}).blockLength().value(),
            (*this)(end_ptr_tag{}),
            pos,
            static_cast<size_type>(size() - pos)};
    }

    //! @brief Returns cursor range to [pos; pos+count) entries
    //! @pre `pos < size()`
    //! @pre `count <= size() - pos`
    template<
        typename Byte2,
        typename = enable_if_cursor_compatible_t<Byte, Byte2>>
    SBEPP_CPP20_CONSTEXPR cursor_range_t<Byte2> cursor_subrange(
        cursor<Byte2>& c,
        const size_type pos,
        const size_type count) const noexcept
    {
        SBEPP_ASSERT(pos < size());
        SBEPP_ASSERT(count <= (size() - pos));

        return {
            c,
            (*this)(get_header_tag{}).blockLength().value(),
            (*this)(end_ptr_tag{}),
            pos,
            count};
    }

    //! @brief `cursor_range_t::iterator`. Satisfies `std::input_iterator`
    template<typename Byte2>
    using cursor_iterator = typename cursor_range_t<Byte2>::iterator;

    //! @brief Returns cursor iterator to the beginning
    template<
        typename Byte2,
        typename = enable_if_cursor_compatible_t<Byte, Byte2>>
    SBEPP_CPP20_CONSTEXPR cursor_iterator<Byte2>
        cursor_begin(cursor<Byte2>& c) const noexcept
    {
        return cursor_range(c).begin();
    }

    //! @brief Returns cursor iterator to the end
    template<
        typename Byte2,
        typename = enable_if_cursor_compatible_t<Byte, Byte2>>
    SBEPP_CPP20_CONSTEXPR cursor_iterator<Byte2>
        cursor_end(cursor<Byte2>& c) const noexcept
    {
        return cursor_range(c).end();
    }

    template<typename Visitor, typename Cursor>
    SBEPP_CPP14_CONSTEXPR bool
        operator()(visit_children_tag, Visitor& v, Cursor& c)
    {
        for(const auto entry : this->cursor_range(c))
        {
            if(v.template on_entry(entry, c))
            {
                return true;
            }
        }
        return false;
    }
};

//! @brief Base class for a nested group
template<typename Byte, typename Entry, typename Dimension>
class nested_group_base : public byte_range<Byte>
{
public:
    //! @brief entry type
    using value_type = Entry;
    //! @brief `value_type`
    using reference = value_type;
    //! @brief `numInGroup` value type
    using sbe_size_type = typename std::decay<
        decltype(std::declval<Dimension>().numInGroup())>::type;
    //! @brief raw size type
    using size_type = typename sbe_size_type::value_type;
    //! @brief signed `size_type`
    using difference_type = typename std::make_signed<size_type>::type;

    //! @brief Forward iterator to `value_type`. Satisfies
    //!     `std::forward_iterator`
    using iterator = forward_iterator<
        Byte,
        Entry,
        size_type,
        difference_type,
        typename std::decay<
            decltype(std::declval<Dimension>().blockLength().value())>::type>;

    using byte_range<Byte>::byte_range;
    using byte_range<Byte>::operator();

    SBEPP_CPP14_CONSTEXPR Dimension operator()(get_header_tag) const noexcept
    {
        Dimension header{(*this)(addressof_tag{}), (*this)(end_ptr_tag{})};
        SBEPP_SIZE_CHECK(
            (*this)(addressof_tag{}),
            (*this)(end_ptr_tag{}),
            0,
            size_bytes(header));
        return header;
    }

    SBEPP_CPP20_CONSTEXPR std::size_t operator()(size_bytes_tag) const noexcept
    {
        std::size_t size{size_bytes((*this)(get_header_tag{}))};
        for(const auto entry : *this)
        {
            size += size_bytes(entry);
        }

        return size;
    }

    //! @brief Returns header's `numInGroup`
    SBEPP_CPP20_CONSTEXPR sbe_size_type sbe_size() const noexcept
    {
        return (*this)(get_header_tag{}).numInGroup();
    }

    //! @brief Returns raw size
    SBEPP_CPP20_CONSTEXPR size_type size() const noexcept
    {
        return sbe_size().value();
    }

    //! @brief Sets `numInGroup` to `count`
    SBEPP_CPP20_CONSTEXPR void resize(const size_type count) const noexcept
    {
        (*this)(get_header_tag{}).numInGroup(count);
    }

    //! @brief Checks if `size() == 0`
    SBEPP_CPP17_NODISCARD SBEPP_CPP20_CONSTEXPR bool empty() const noexcept
    {
        return !size();
    }

    //! @brief Returns `numInGroup`'s `maxValue`
    constexpr static size_type max_size() noexcept
    {
        return sbe_size_type::max_value();
    }

    //! @brief Returns an iterator to the beginning
    SBEPP_CPP14_CONSTEXPR iterator begin() const noexcept
    {
        auto dimension = (*this)(get_header_tag{});
        return iterator{
            (*this)(addressof_tag{}) + size_bytes(dimension),
            0,
            dimension.blockLength().value(),
            (*this)(end_ptr_tag{})};
    }

    //! @brief Returns an iterator to the end
    constexpr iterator end() const noexcept
    {
        return iterator{
            nullptr,
            (*this)(get_header_tag{}).numInGroup().value(),
            (*this)(get_header_tag{}).blockLength().value(),
            (*this)(end_ptr_tag{})};
    }

    //! @brief Returns the first element
    //! @pre `!empty()`
    SBEPP_CPP14_CONSTEXPR reference front() const noexcept
    {
        SBEPP_ASSERT(!empty());
        return *begin();
    }

    //! @brief Resizes to `0`
    //! @post `size() == 0`
    SBEPP_CPP14_CONSTEXPR void clear() const noexcept
    {
        resize(0);
    }

    //! @brief Type of a cursor range. Satisfies `std::ranges::input_range`
    template<typename Byte2>
    using cursor_range_t = ::sbepp::detail::cursor_range<
        value_type,
        size_type,
        cursor<Byte2>,
        typename std::decay<
            decltype(std::declval<Dimension>().blockLength().value())>::type,
        Byte>;

    //! @brief Returns cursor range to all group entries
    template<
        typename Byte2,
        typename = enable_if_cursor_compatible_t<Byte, Byte2>>
    SBEPP_CPP20_CONSTEXPR cursor_range_t<Byte2>
        cursor_range(cursor<Byte2>& c) const noexcept
    {
        return {
            c,
            (*this)(get_header_tag{}).blockLength().value(),
            (*this)(end_ptr_tag{}),
            0,
            size()};
    }

    //! @brief Returns cursor range to `[pos; size())` entries
    //! @pre `pos < size()`
    template<
        typename Byte2,
        typename = enable_if_cursor_compatible_t<Byte, Byte2>>
    SBEPP_CPP20_CONSTEXPR cursor_range_t<Byte2>
        cursor_subrange(cursor<Byte2>& c, const size_type pos) const noexcept
    {
        SBEPP_ASSERT(pos < size());

        return {
            c,
            (*this)(get_header_tag{}).blockLength().value(),
            (*this)(end_ptr_tag{}),
            pos,
            static_cast<size_type>(size() - pos)};
    }

    //! @brief Returns cursor range to `[pos; pos+count)` entries
    //! @pre `pos < size()`
    //! @pre `count <= size() - pos`
    template<
        typename Byte2,
        typename = enable_if_cursor_compatible_t<Byte, Byte2>>
    SBEPP_CPP20_CONSTEXPR cursor_range_t<Byte2> cursor_subrange(
        cursor<Byte2>& c,
        const size_type pos,
        const size_type count) const noexcept
    {
        SBEPP_ASSERT(pos < size());
        SBEPP_ASSERT(count <= (size() - pos));

        return {
            c,
            (*this)(get_header_tag{}).blockLength().value(),
            (*this)(end_ptr_tag{}),
            pos,
            count};
    }

    //! @brief `cursor_range_t::iterator`. Satisfies `std::input_iterator`
    template<typename Byte2>
    using cursor_iterator = typename cursor_range_t<Byte2>::iterator;

    //! @brief Returns cursor iterator to the beginning
    template<
        typename Byte2,
        typename = enable_if_cursor_compatible_t<Byte, Byte2>>
    SBEPP_CPP20_CONSTEXPR cursor_iterator<Byte2>
        cursor_begin(cursor<Byte2>& c) const noexcept
    {
        return cursor_range(c).begin();
    }

    //! @brief Returns cursor iterator to the end
    template<
        typename Byte2,
        typename = enable_if_cursor_compatible_t<Byte, Byte2>>
    SBEPP_CPP20_CONSTEXPR cursor_iterator<Byte2>
        cursor_end(cursor<Byte2>& c) const noexcept
    {
        return cursor_range(c).end();
    }

    template<typename Visitor, typename Cursor>
    SBEPP_CPP14_CONSTEXPR bool
        operator()(visit_children_tag, Visitor& v, Cursor& c)
    {
        for(const auto entry : this->cursor_range(c))
        {
            if(v.template on_entry(entry, c))
            {
                return true;
            }
        }
        return false;
    }
};

//! @brief Base class for bitsets
//! @tparam T underlying value type
template<typename T>
class bitset_base
{
public:
    //! @brief Default constructs underlying value to `0`
    bitset_base() = default;

    //! @brief Constructs from given value
    explicit constexpr bitset_base(T value) noexcept : bits{value}
    {
    }

    //! @brief Returns reference to underlying value
    SBEPP_CPP14_CONSTEXPR T& operator*() noexcept
    {
        return bits;
    }

    //! @brief Returns underlying value
    constexpr T operator*() const noexcept
    {
        return bits;
    }

    constexpr bool
        operator()(get_bit_tag, const choice_index_t n) const noexcept
    {
        return bits & (1 << n);
    }

    SBEPP_CPP14_CONSTEXPR void
        operator()(set_bit_tag, const choice_index_t n, const bool b) noexcept
    {
        bits = ((bits & ~(1 << n)) | (b << n));
    }

    //! @name Comparisons
    //! @brief Compare underlying values
    //! @{
    constexpr friend bool
        operator==(const bitset_base& lhs, const bitset_base& rhs) noexcept
    {
        return *lhs == *rhs;
    }

    constexpr friend bool
        operator!=(const bitset_base& lhs, const bitset_base& rhs) noexcept
    {
        return *lhs != *rhs;
    }

    //! @}

private:
    T bits{};
};

template<typename View, typename = void_t<>>
struct has_get_header : std::false_type
{
};

template<typename View>
struct has_get_header<View, void_t<decltype(get_header(std::declval<View>()))>>
    : std::true_type
{
};

template<
    typename View,
    typename = detail::enable_if_t<detail::has_get_header<View>::value>>
constexpr std::size_t get_header_size(View view) noexcept
{
    return sbepp::size_bytes(get_header(view));
}

template<
    typename View,
    typename = detail::enable_if_t<!detail::has_get_header<View>::value>,
    typename = int>
constexpr std::size_t get_header_size(View) noexcept
{
    return 0;
}
} // namespace detail

/**
 * @brief Converts an enumeration to its underlying type. Equivalent to C++23
 *  `std::to_underlying()`
 *
 * @param e enumerator
 * @return enumerator's underlying value
 */
template<typename Enum>
constexpr typename std::underlying_type<Enum>::type
    to_underlying(Enum e) noexcept
{
    return static_cast<typename std::underlying_type<Enum>::type>(e);
}

/**
 * @brief Initializes cursor from a message/group view with the same byte type.
 *
 * Example:
 * ```cpp
 * schema::messages::msg1<char> {ptr, size};
 * auto c = sbepp::init_cursor(m);
 * auto value = m.field(c);
 * ```
 *
 * @param view message or group view
 * @return initialized cursor
 */
template<typename View>
SBEPP_CPP14_CONSTEXPR cursor<byte_type_t<View>> init_cursor(View view) noexcept
{
    cursor<byte_type_t<View>> c;
    c.pointer() = addressof(view) + detail::get_header_size(view);
    return c;
}

/**
 * @brief Initializes cursor from a message/group view with `const` byte type.
 *
 * Example:
 * ```cpp
 * schema::messages::msg1<char> {ptr, size};
 * auto c = sbepp::init_const_cursor(m);
 * auto value = m.field(c);
 * m.field(1, c);   // error: cursor has const-qualified byte type
 * ```
 *
 * @param view message or group view
 * @return initialized cursor
 */
template<typename View>
SBEPP_CPP14_CONSTEXPR cursor<typename std::add_const<byte_type_t<View>>::type>
    init_const_cursor(View view) noexcept
{
    cursor<typename std::add_const<byte_type_t<View>>::type> c;
    c.pointer() = addressof(view) + detail::get_header_size(view);
    return c;
}

/**
 * @brief tag for `dynamic_array_ref::resize()`. Used to skip value
 * initialization.
 */
struct default_init_t
{
    explicit default_init_t() = default;
};

/**
 * @brief helper to pass `default_init_t` to `dynamic_array_ref::resize()`.
 *
 * Example: `ref.resize(n, sbepp::default_init);`.
 */
SBEPP_CPP17_INLINE_VAR constexpr default_init_t default_init{};

namespace detail
{
template<typename From, typename To>
struct copy_cv_qualifiers
{
    using copy_const_t = typename std::
        conditional<std::is_const<From>::value, const To, To>::type;

    using type = typename std::conditional<
        std::is_volatile<From>::value,
        volatile copy_const_t,
        copy_const_t>::type;
};

template<typename From, typename To>
using apply_cv_qualifiers_t = typename copy_cv_qualifiers<From, To>::type;

//! @brief Represents reference to fixed-size array
//! @details The general precondition for most functions is that current buffer
//!     should be able to hold all elements
//! @tparam Byte byte type
//! @tparam Value array element type from schema
//! @tparam N array length
template<typename Byte, typename Value, std::size_t N>
class static_array_ref : public detail::byte_range<Byte>
{
public:
    //! @brief final element type. `value_type` with the same cv-qualifiers as
    //!     `Byte`
    using element_type = detail::apply_cv_qualifiers_t<Byte, Value>;
    //! @brief same as `Value`
    using value_type = Value;
    //! @brief `std::size_t`
    using size_type = std::size_t;
    //! @brief `std::ptrdiff_t`
    using difference_type = std::ptrdiff_t;
    //! @brief element reference type
    using reference = element_type&;
    //! @brief element pointer type
    using pointer = element_type*;
    //! @brief iterator type. Satisfies `std::random_access_iterator`
    using iterator = pointer;
    //! @brief reverse iterator type
    using reverse_iterator = std::reverse_iterator<iterator>;

    using detail::byte_range<Byte>::byte_range;
    using detail::byte_range<Byte>::operator();

    constexpr std::size_t operator()(detail::size_bytes_tag) const noexcept
    {
        return size();
    }

    //! @brief Access element at `pos`
    //! @pre `pos < size()`
    SBEPP_CPP14_CONSTEXPR reference operator[](size_type pos) const noexcept
    {
        SBEPP_ASSERT(pos < size());
        return data()[pos];
    }

    //! @brief Access the first element
    constexpr reference front() const noexcept
    {
        return *data();
    }

    //! @brief Access the last element
    constexpr reference back() const noexcept
    {
        return data()[size() - 1];
    }

    //! @brief Direct access to the underlying array
    SBEPP_CPP14_CONSTEXPR pointer data() const noexcept
    {
        // it would be nice to use `reinterpret_cast` here but it's not allowed
        // in constexpr context
        SBEPP_SIZE_CHECK(
            (*this)(detail::addressof_tag{}),
            (*this)(detail::end_ptr_tag{}),
            0,
            N);
        return (pointer)(*this)(detail::addressof_tag{}); // NOLINT
    }

    //! @brief Checks if `size() != 0`
    SBEPP_CPP17_NODISCARD static constexpr bool empty() noexcept
    {
        return !size();
    }

    //! @brief Returns `N`
    static constexpr size_type size() noexcept
    {
        return N;
    }

    //! @brief Returns `size()`
    static constexpr size_type max_size() noexcept
    {
        return size();
    }

    //! @brief Returns an iterator to the beginning
    constexpr iterator begin() const noexcept
    {
        return data();
    }

    //! @brief Returns an iterator to the end
    constexpr iterator end() const noexcept
    {
        return data() + size();
    }

    //! @brief Returns a reverse iterator to the beginning
    constexpr reverse_iterator rbegin() const noexcept
    {
        return reverse_iterator{end()};
    }

    //! @brief Returns a reverse iterator to the end
    constexpr reverse_iterator rend() const noexcept
    {
        return reverse_iterator{begin()};
    }

    /**
     * @brief Returns `static_array_ref<Byte, Byte, N>`.
     *
     * Useful in constexpr context to modify an array which has different `Byte`
     * and `Value` types. Example:
     * ```cpp
     * static_array_ref<std::byte, char, 1> a1;
     * a1[0] = 'a';    // error: cannot convert `char` to `std::byte`
     * a1.raw()[0] = std::byte{'a'};   // OK
     * ```
     */
    constexpr static_array_ref<Byte, detail::remove_cv_t<Byte>, N>
        raw() const noexcept
    {
        return static_array_ref<Byte, detail::remove_cv_t<Byte>, N>{
            (*this)(detail::addressof_tag{}), (*this)(detail::end_ptr_tag{})};
    }
};

//! @brief Represents reference to dynamic arrays used for `<data>` elements
//! @details The general precondition for most functions is that current buffer
//!     should be able to hold `size()` number of elements
//! @tparam Byte byte type
//! @tparam Value array element type
//! @tparam Length SBE length type
//! @tparam E schema endianness
template<typename Byte, typename Value, typename Length, endian E>
class dynamic_array_ref : public detail::byte_range<Byte>
{
public:
    static_assert(
        std::is_unsigned<typename Length::value_type>::value,
        "Length must be unsigned");

    //! @brief final element type. `value_type` with the same cv-qualifiers as
    //!     `Byte`
    using element_type = detail::apply_cv_qualifiers_t<Byte, Value>;
    //! @brief same as `Value`
    using value_type = Value;
    //! @brief `length` SBE representation of data's encoding
    using sbe_size_type = Length;
    //! @brief raw size type
    using size_type = typename sbe_size_type::value_type;
    //! @brief `std::ptrdiff_t`
    using difference_type = std::ptrdiff_t;
    //! @brief element reference type
    using reference = element_type&;
    //! @brief element pointer type
    using pointer = element_type*;
    //! @brief iterator type. Satisfies `std::random_access_iterator`
    using iterator = pointer;
    //! @brief reverse iterator type
    using reverse_iterator = std::reverse_iterator<iterator>;

    using detail::byte_range<Byte>::byte_range;
    using detail::byte_range<Byte>::operator();

    //! @brief Returns an iterator to the beginning
    constexpr iterator begin() const noexcept
    {
        return data_checked();
    }

    //! @brief Returns an iterator to the end
    SBEPP_CPP20_CONSTEXPR iterator end() const noexcept
    {
        return begin() + size();
    }

    //! @brief Returns a reverse iterator to the beginning
    SBEPP_CPP20_CONSTEXPR reverse_iterator rbegin() const noexcept
    {
        return reverse_iterator{end()};
    }

    //! @brief Returns a reverse iterator to the end
    constexpr reverse_iterator rend() const noexcept
    {
        return reverse_iterator{begin()};
    }

    //! @brief Access the first element
    //! @pre `!empty()`
    constexpr reference front() const noexcept
    {
        return *data();
    }

    //! @brief Access the last element
    //! @pre `!empty()`
    SBEPP_CPP20_CONSTEXPR reference back() const noexcept
    {
        return *(data() + size() - 1);
    }

    //! @brief Direct access to the underlying array
    //! @pre `!empty()`
    SBEPP_CPP14_CONSTEXPR pointer data() const noexcept
    {
        SBEPP_ASSERT(!empty());
        return data_checked();
    }

    //! @brief Access element at `pos`
    //! @pre `pos < size()`
    SBEPP_CPP14_CONSTEXPR reference operator[](size_type pos) const noexcept
    {
        SBEPP_ASSERT(pos < size());
        return *(data() + pos);
    }

    //! @brief Returns SBE size representation
    SBEPP_CPP20_CONSTEXPR sbe_size_type sbe_size() const noexcept
    {
        return detail::get_value<size_type, size_type, E>(*this, 0);
    }

    //! @brief Returns raw size
    SBEPP_CPP20_CONSTEXPR size_type size() const noexcept
    {
        return sbe_size().value();
    }

    //! @brief Checks if `size() != 0`
    SBEPP_CPP17_NODISCARD SBEPP_CPP20_CONSTEXPR bool empty() const noexcept
    {
        return !size();
    }

    //! @brief Returns max value of SBE length representation
    constexpr static size_type max_size() noexcept
    {
        return sbe_size_type::max_value();
    }

    //! @brief Sets size to 0
    template<
        typename T = void,
        typename = detail::enable_if_writable_t<Byte, T>>
    SBEPP_CPP20_CONSTEXPR void clear() const noexcept
    {
        resize(0, default_init);
    }

    //! @brief Sets size to `count`, value initializes new elements
    template<
        typename T = void,
        typename = detail::enable_if_writable_t<Byte, T>>
    SBEPP_CPP20_CONSTEXPR void resize(size_type count) const noexcept
    {
        const auto old_size = size();
        resize(count, default_init);
        if(count > old_size)
        {
            for(auto i = old_size; i != count; i++)
            {
                operator[](i) = {};
            }
        }
    }

    //! @brief Sets size to `count`, initializes new elements with `value`
    template<
        typename T = void,
        typename = detail::enable_if_writable_t<Byte, T>>
    SBEPP_CPP20_CONSTEXPR void
        resize(size_type count, value_type value) const noexcept
    {
        const auto old_size = size();
        resize(count, default_init);
        if(count > old_size)
        {
            for(auto i = old_size; i != count; i++)
            {
                operator[](i) = value;
            }
        }
    }

    //! @brief Sets size to `count`, default initializes new elements
    template<
        typename T = void,
        typename = detail::enable_if_writable_t<Byte, T>>
    SBEPP_CPP20_CONSTEXPR void
        resize(size_type count, sbepp::default_init_t) const noexcept
    {
        // can't use `detail::set_value()` here because its size check checks
        // only `sizeof(T)`, here we need `sizeof(size_type) + count`
        SBEPP_SIZE_CHECK(
            (*this)(detail::addressof_tag{}),
            (*this)(detail::end_ptr_tag{}),
            0,
            sizeof(size_type) + count);
        detail::set_primitive<E>((*this)(detail::addressof_tag{}), count);
    }

    //! @brief Adds new element to the end
    template<
        typename T = void,
        typename = detail::enable_if_writable_t<Byte, T>>
    SBEPP_CPP20_CONSTEXPR void push_back(value_type value) const noexcept
    {
        const auto current_size = size();
        resize(current_size + 1, default_init);
        (*this)[current_size] = value;
    }

    //! @brief Removes the last element
    template<
        typename T = void,
        typename = detail::enable_if_writable_t<Byte, T>>
    SBEPP_CPP20_CONSTEXPR void pop_back() const noexcept
    {
        SBEPP_ASSERT(!empty());
        resize(size() - 1, default_init);
    }

    //! @brief Erases element at `pos`
    template<
        typename T = void,
        typename = detail::enable_if_writable_t<Byte, T>>
    SBEPP_CPP20_CONSTEXPR iterator erase(iterator pos) const noexcept
    {
        SBEPP_ASSERT(pos >= begin() && pos < end());
        std::copy(pos + 1, end(), pos);
        resize(size() - 1, default_init);
        return pos;
    }

    //! @brief Erases elements in `[first; last)` range
    template<
        typename T = void,
        typename = detail::enable_if_writable_t<Byte, T>>
    SBEPP_CPP20_CONSTEXPR iterator
        erase(iterator first, iterator last) const noexcept
    {
        SBEPP_ASSERT(first >= begin() && last < end());
        std::copy(last, end(), first);
        resize(size() - (last - first), default_init);
        return first;
    }

    //! @brief Inserts `value` before `pos`
    template<
        typename T = void,
        typename = detail::enable_if_writable_t<Byte, T>>
    SBEPP_CPP20_CONSTEXPR iterator
        insert(iterator pos, const value_type value) const noexcept
    {
        SBEPP_ASSERT(pos >= begin() && pos <= end());
        const auto old_end = end();
        resize(size() + 1, default_init);
        std::copy_backward(pos, old_end, end());
        *pos = value;
        return pos;
    }

    //! @brief Inserts `count` copies of `value` before `pos`
    template<
        typename T = void,
        typename = detail::enable_if_writable_t<Byte, T>>
    SBEPP_CPP20_CONSTEXPR iterator insert(
        iterator pos, size_type count, const value_type value) const noexcept
    {
        SBEPP_ASSERT(pos >= begin() && pos <= end());
        const auto old_end = end();
        resize(size() + count, default_init);
        std::copy_backward(pos, old_end, end());
        std::fill_n(pos, count, value);
        return pos;
    }

    //! @brief Inserts elements from `[first; last)` range before `pos`
    template<
        typename InputIt,
        typename = detail::enable_if_t<
            !std::is_const<Byte>::value
            && std::is_convertible<
                typename std::iterator_traits<InputIt>::iterator_category,
                std::input_iterator_tag>::value>>
    SBEPP_CPP14_CONSTEXPR iterator
        insert(iterator pos, InputIt first, InputIt last) const
    {
        SBEPP_ASSERT(pos >= begin() && pos <= end());
        using category_t =
            typename std::iterator_traits<InputIt>::iterator_category;
        return insert_impl(pos, first, last, category_t{});
    }

    //! @brief Inserts elements from `ilist` before `pos`
    template<
        typename T = void,
        typename = detail::enable_if_writable_t<Byte, T>>
    constexpr iterator insert(
        iterator pos, std::initializer_list<element_type> ilist) const noexcept
    {
        return insert(pos, std::begin(ilist), std::end(ilist));
    }

    //! @brief Replaces the contents of the container with `count` copies of
    //!     `value`
    template<
        typename T = void,
        typename = detail::enable_if_writable_t<Byte, T>>
    SBEPP_CPP20_CONSTEXPR void
        assign(size_type count, const value_type value) const noexcept
    {
        resize(count, default_init);
        std::fill_n(begin(), count, value);
    }

    //! @brief Replaces the contents of the container with the elements from
    //!     `[first; last)` range
    template<
        typename InputIt,
        typename T = void,
        typename = detail::enable_if_writable_t<Byte, T>>
    SBEPP_CPP20_CONSTEXPR void assign(InputIt first, InputIt last) const
    {
        size_type size{};
        auto out = data_unchecked();
        for(; first != last; ++first, ++out, ++size)
        {
            SBEPP_SIZE_CHECK(
                (*this)(detail::addressof_tag{}),
                (*this)(detail::end_ptr_tag{}),
                0,
                sizeof(size_type) + size);
            *out = *first;
        }
        resize(size, default_init);
    }

    //! @brief Replaces the contents of the container with the elements from
    //!     `ilist`
    template<
        typename T = void,
        typename = detail::enable_if_writable_t<Byte, T>>
    SBEPP_CPP20_CONSTEXPR void
        assign(std::initializer_list<value_type> ilist) const noexcept
    {
        assign(std::begin(ilist), std::end(ilist));
    }

    /**
     * @brief Returns `dynamic_array_ref<Byte, Byte, Length, E>`.
     *
     * Useful in constexpr context to modify an array which has different `Byte`
     * and `Value` types. For example see `static_array_ref::raw()`
     */
    constexpr dynamic_array_ref<Byte, detail::remove_cv_t<Byte>, Length, E>
        raw() const noexcept
    {
        return dynamic_array_ref<Byte, detail::remove_cv_t<Byte>, Length, E>{
            (*this)(detail::addressof_tag{}), (*this)(detail::end_ptr_tag{})};
    }

    SBEPP_CPP20_CONSTEXPR std::size_t
        operator()(detail::size_bytes_tag) const noexcept
    {
        return sizeof(size_type) + size();
    }

private:
    SBEPP_CPP14_CONSTEXPR pointer data_checked() const noexcept
    {
        SBEPP_SIZE_CHECK(
            (*this)(detail::addressof_tag{}),
            (*this)(detail::end_ptr_tag{}),
            0,
            sizeof(size_type) + size());
        return data_unchecked();
    }

    constexpr pointer data_unchecked() const noexcept
    {
        // cast is conditionally required here when `Byte` is different type
        // from `Value`. `reinterpret_cast` is not allowed in constexpr context
        // even when it's an identity cast. On the other hand, C-style cast
        // is allowed in constexpr context when it renders to an identity cast
        // which effectively makes this and other functions conditionally
        // `constexpr` when `Byte` is equal to `Value`.
        // NOLINTNEXTLINE: see above
        return (pointer)((*this)(detail::addressof_tag{}) + sizeof(size_type));
    }

    template<typename It>
    SBEPP_CPP14_CONSTEXPR iterator insert_impl(
        iterator pos, It first, It last, std::input_iterator_tag) const
    {
        auto out = pos;
        for(; first != last; ++first, ++out)
        {
            insert(out, *first);
        }

        return pos;
    }

    template<typename It>
    SBEPP_CPP20_CONSTEXPR iterator insert_impl(
        iterator pos, It first, It last, std::forward_iterator_tag) const
    {
        const auto in_size = std::distance(first, last);
        auto old_end = end();
        resize(size() + in_size, default_init);
        std::copy_backward(pos, old_end, end());
        std::copy(first, last, pos);
        return pos;
    }
};
} // namespace detail

//! @brief Tag type used to initialize optional types with null value
struct nullopt_t
{
    explicit constexpr nullopt_t(int)
    {
    }
};

/**
 * @brief Helper constant used to initialize optional types with null value
 *
 * Example: `msg.optionalField(sbepp::nullopt);`
 */
SBEPP_CPP17_INLINE_VAR constexpr nullopt_t nullopt{0};

namespace detail
{
// see `optional_base` note about explicit `alignas`
//! @brief Base class for required types
//! @note `in_range()` check is not enforced implicitly, it's up to client to
//!  apply it
template<typename T, typename Derived>
class alignas(T) required_base
{
public:
    //! @brief Underlying type
    using value_type = T;

    //! @brief Default constructor. Constructs value-initialized object
    required_base() = default;

    //! @brief Constructs from given value
    // NOLINTNEXTLINE: it should be implicit
    constexpr required_base(value_type val) noexcept : val{val}
    {
    }

    //! @brief Returns underlying value
    constexpr value_type value() const noexcept
    {
        return **this;
    }

    //! @brief Returns reference to underlying value
    SBEPP_CPP14_CONSTEXPR value_type& operator*() noexcept
    {
        return val;
    }

    //! @brief Returns underlying value
    constexpr value_type operator*() const noexcept
    {
        return val;
    }

    //! @brief Checks if value is in
    //!     `[Derived::min_value(); Derived::max_value()]` range
    constexpr bool in_range() const noexcept
    {
        return (Derived::min_value() <= val) && (val <= Derived::max_value());
    }

    //! @name Comparisons
    //! Compare underlying values
    //! @{
#ifdef SBEPP_DOXYGEN
    //! @brief Available only if #SBEPP_HAS_THREE_WAY_COMPARISON == 1
    friend auto
        operator<=>(const required_base&, const required_base&) = default;
#endif

#if SBEPP_HAS_THREE_WAY_COMPARISON
    friend auto
        operator<=>(const required_base&, const required_base&) = default;
#else
    constexpr friend bool
        operator==(const required_base& lhs, const required_base& rhs) noexcept
    {
        return *lhs == *rhs;
    }

    constexpr friend bool
        operator!=(const required_base& lhs, const required_base& rhs) noexcept
    {
        return *lhs != *rhs;
    }

    constexpr friend bool
        operator<(const required_base& lhs, const required_base& rhs) noexcept
    {
        return *lhs < *rhs;
    }

    constexpr friend bool
        operator<=(const required_base& lhs, const required_base& rhs) noexcept
    {
        return *lhs <= *rhs;
    }

    constexpr friend bool
        operator>(const required_base& lhs, const required_base& rhs) noexcept
    {
        return *lhs > *rhs;
    }

    constexpr friend bool
        operator>=(const required_base& lhs, const required_base& rhs) noexcept
    {
        return *lhs >= *rhs;
    }
#endif
    //! @}

private:
    value_type val{};
};

// old compilers might generate wrong alignment for classes which contain
// `double` member. To overcome this, explicit alignment is provided.
//! @brief Base class for optional types
//! @note `in_range()` and `has_value()` checks are not enforced implicitly,
//!  it's up to client to apply it.
template<typename T, typename Derived>
class alignas(T) optional_base
{
public:
    //! @brief Underlying type
    using value_type = T;

    //! @brief Constructs null object
    optional_base() = default;

    //! @brief Constructs null object
    // NOLINTNEXTLINE: it's intentionally implicit
    constexpr optional_base(nullopt_t) noexcept : optional_base{}
    {
    }

    //! @brief Constructs object from given value
    // NOLINTNEXTLINE: it's intentionally implicit
    constexpr optional_base(value_type val) noexcept : val{val}
    {
    }

    //! @brief Returns underlying value
    constexpr value_type value() const noexcept
    {
        return **this;
    }

    //! @brief Returns reference to underlying value
    SBEPP_CPP14_CONSTEXPR value_type& operator*() noexcept
    {
        return val;
    }

    //! @brief Returns underlying value
    constexpr value_type operator*() const noexcept
    {
        return val;
    }

    //! @brief Checks if value is in
    //!     `[Derived::min_value(); Derived::max_value()]` range
    constexpr bool in_range() const noexcept
    {
        return (Derived::min_value() <= val) && (val <= Derived::max_value());
    }

    //! @brief Returns value if not null, `default_value` otherwise
    SBEPP_CPP14_CONSTEXPR value_type value_or(T default_value) const noexcept
    {
        if(*this)
        {
            return value();
        }
        return default_value;
    }

    //! @brief Checks if has value
    constexpr bool has_value() const noexcept
    {
        return (val != Derived::null_value());
    }

    //! @brief Checks if has value
    constexpr explicit operator bool() const noexcept
    {
        return has_value();
    }

    //! @name Comparisons
    //! The contained values are compared only if both `lhs` and `rhs`
    //!  are not null. Otherwise:
    //!  - `lhs` is *equal to* `rhs` iff both are null
    //!  - `lhs` is *less than* `rhs` iff `rhs` is not null and `lhs` is null
    //! @{
    constexpr friend bool
        operator==(const optional_base& lhs, const optional_base& rhs) noexcept
    {
        return *lhs == *rhs;
    }

#ifdef SBEPP_DOXYGEN
    //! @brief Available only if #SBEPP_HAS_THREE_WAY_COMPARISON == 1
    constexpr friend std::strong_ordering operator<=>(
        const optional_base& lhs, const optional_base& rhs) noexcept;
#endif

#if SBEPP_HAS_THREE_WAY_COMPARISON
    constexpr friend std::strong_ordering
        operator<=>(const optional_base& lhs, const optional_base& rhs) noexcept
    {
        if(lhs && rhs)
        {
            return *lhs <=> *rhs;
        }
        return lhs.has_value() <=> rhs.has_value();
    }
#else
    constexpr friend bool
        operator!=(const optional_base& lhs, const optional_base& rhs) noexcept
    {
        return *lhs != *rhs;
    }

    constexpr friend bool
        operator<(const optional_base& lhs, const optional_base& rhs) noexcept
    {
        return rhs && (!lhs || (*lhs < *rhs));
    }

    constexpr friend bool
        operator<=(const optional_base& lhs, const optional_base& rhs) noexcept
    {
        return !lhs || (rhs && (*lhs <= *rhs));
    }

    constexpr friend bool
        operator>(const optional_base& lhs, const optional_base& rhs) noexcept
    {
        return lhs && (!rhs || (*lhs > *rhs));
    }

    constexpr friend bool
        operator>=(const optional_base& lhs, const optional_base& rhs) noexcept
    {
        return !rhs || (lhs && (*lhs >= *rhs));
    }
#endif
    //! @}

private:
    value_type val{Derived::null_value()};
};
} // namespace detail

/**
 * @brief Fill message header
 *
 * Automatically fills these fields in message header:
 * - `schemaId`
 * - `templateId`
 * - `version`
 * - `blockLength`
 * - `numGroups` (if exists in header)
 * - `numVarDataFields` (if exists in header)
 *
 * @param m message
 * @return message header
 */
template<typename Message>
constexpr auto fill_message_header(Message m) noexcept
    -> decltype(m(detail::fill_message_header_tag{}))
{
    return m(detail::fill_message_header_tag{});
}

/**
 * @brief Fill group header
 *
 * Automatically fills these fields in group header:
 * - `blockLength`
 * - `numInGroup`
 * - `numGroups` (if exists in header)
 * - `numVarDataFields` (if exists in header)
 *
 * @param g group
 * @param num_in_group number if entries in group
 * @return group header
 */
template<typename Group, typename Size>
constexpr auto fill_group_header(Group g, Size num_in_group) noexcept
    -> decltype(g(detail::fill_group_header_tag{}, num_in_group))
{
    return g(detail::fill_group_header_tag{}, num_in_group);
}

/** @addtogroup traits-list Traits list
 *
 *  The list of available traits. For more details see @ref traits
 *  @{
 */
/**
 * @brief Provides various traits and attributes of a `<type>` element.
 *
 * For example:
 * `sbepp::type_traits<some_schema::schema::types::some_type>::name()`;
 *
 * @tparam T type tag
 */
template<typename T>
class type_traits;

#ifdef SBEPP_DOXYGEN
template<typename T>
class type_traits
{
public:
    //! @brief Underlying type
    using primitive_type = PrimitiveType;

    //! @brief Representation type
    using value_type = ValueType;

    /**
     * @brief Representation type for non-constant arrays
     *
     * @tparam Byte byte type
     */
    template<typename Byte>
    using value_type = ArrayType<Byte>;

    //! @brief Returns `name` attribute
    static constexpr const char* name() noexcept;

    //! @brief Returns `description` attribute
    static constexpr const char* description() noexcept;

    //! @brief Returns presence
    static constexpr field_presence presence() noexcept;

    /**
     * @brief Returns `minValue`. Available only if `length() == 1` and
     *  `presence() != field_presence::constant`
     */
    static constexpr primitive_type min_value() noexcept;

    /**
     * @brief Returns `maxValue`. Available only if `length() == 1` and
     *  `presence() != field_presence::constant`
     */
    static constexpr primitive_type max_value() noexcept;

    /**
     * @brief Returns `nullValue`. Available only if `length() == 1` and
     *  `presence() == field_presence::optional`
     */
    static constexpr primitive_type null_value() noexcept;

    //! @brief Returns `length` attribute
    static constexpr length_t length() noexcept;

    /**
     * @brief Returns type offset. Available only if offset is static, i.e. not
     *  available for public types whose offset is different on each usage.
     */
    static constexpr offset_t offset() noexcept;

    //! @brief Returns `semanticType` attribute
    static constexpr const char* semantic_type() noexcept;

    //! @brief Returns `sinceVersion` attribute
    static constexpr version_t since_version() noexcept;

    //! @brief Returns `deprecated` attribute. Available only if provided in
    //! schema
    static constexpr version_t deprecated() noexcept;

    //! @brief Returns `characterEncoding` attribute
    static constexpr const char* character_encoding() noexcept;
};
#endif

/**
 * @brief Provides various traits/attributes of a `<messageSchema>` element.
 *
 * Schema tags are represented using `schema_name::schema` type. For example:
 * `sbepp::schema_traits<market_schema::schema>::id();`
 *
 * @tparam T schema tag
 */
template<typename T>
class schema_traits;

#ifdef SBEPP_DOXYGEN
template<typename T>
class schema_traits
{
public:
    //! @brief Returns `package` attribute
    static constexpr const char* package() noexcept;
    //! @brief Returns `id` attribute
    static constexpr schema_id_t id() noexcept;
    //! @brief Returns `version` attribute
    static constexpr version_t version() noexcept;
    //! @brief Returns `semanticVersion` attribute
    static constexpr const char* semantic_version() noexcept;
    //! @brief Returns `byteOrder` attribute
    static constexpr endian byte_order() noexcept;
    //! @brief Returns `description` attribute
    static constexpr const char* description() noexcept;
    /**
     * @brief Message header composite type
     *
     * @tparam Byte byte type
     */
    template<typename Byte>
    using header_type = HeaderComposite<Byte>;
    //! @brief Message header composite tag. Can be used to access its traits.
    using header_type_tag = HeaderTypeTag;
};
#endif

/**
 * @brief Provides various traits/attributes of an `<enum>` element.
 *
 * For example:
 * `sbepp::enum_traits<market_schema::schema::types::enum_name>::name();`
 *
 * @tparam T enum tag
 */
template<typename T>
class enum_traits;

#ifdef SBEPP_DOXYGEN
template<typename T>
class enum_traits
{
public:
    //! @brief Returns `name` attribute
    static constexpr const char* name() noexcept;
    //! @brief Returns `description` attribute
    static constexpr const char* description() noexcept;
    //! @brief Underlying type
    using encoding_type = EncodingType;
    /**
     * @brief Returns enum offset. Available only if offset is static, i.e. not
     *  available for public enums whose offset is different on each usage.
     */
    static constexpr offset_t offset() noexcept;
    //! @brief Returns `sinceVersion` attribute
    static constexpr version_t since_version() noexcept;
    //! @brief Returns `deprecated` attribute. Available only if provided in
    //! schema
    static constexpr version_t deprecated() noexcept;
    //! @brief Representation type
    using value_type = ScopedEnumType;
};
#endif

/**
 * @brief Provides various traits/attributes of a `<validValue>` element.
 *
 * For example:
 * `sbepp::enum_value_traits<EnumTag::value_a>::since_version();`
 *
 * @tparam T enumerator tag
 */
template<typename T>
class enum_value_traits;

#ifdef SBEPP_DOXYGEN
template<typename T>
class enum_value_traits
{
public:
    //! @brief Returns `name` attribute
    static constexpr const char* name() noexcept;
    //! @brief Returns `description` attribute
    static constexpr const char* description() noexcept;
    //! @brief Returns `addedSince` attribute
    static constexpr version_t since_version() noexcept;
    //! @brief Returns `deprecated` attribute. Available only if provided in
    //! schema
    static constexpr version_t deprecated() noexcept;
    //! @brief Returns enumerator value
    static constexpr ScopedEnumType value() noexcept;
};
#endif

/**
 * @brief Provides various traits/attributes of a `<set>` element.
 *
 * For example:
 * `sbepp::set_traits<market_schema::schema::types::set_name>::since_version();`
 *
 * @tparam T set tag
 */
template<typename T>
class set_traits;

#ifdef SBEPP_DOXYGEN
template<typename T>
class set_traits
{
public:
    //! @brief Returns `name` attribute
    static constexpr const char* name() noexcept;
    //! @brief Returns `description` attribute
    static constexpr const char* description() noexcept;
    //! @brief Returns `addedSince` attribute
    static constexpr version_t since_version() noexcept;
    //! @brief Returns `deprecated` attribute. Available only if provided in
    //! schema
    static constexpr version_t deprecated() noexcept;
    //! @brief Underlying type
    using encoding_type = EncodingType;
    /**
     * @brief Returns type offset. Available only if offset is static, i.e. not
     *  available for public sets whose offset is different on each usage.
     */
    static constexpr offset_t offset() noexcept;
    //! @brief Representation type
    using value_type = SetType;
};
#endif

/**
 * @brief Provides various traits/attributes of a `<choice>` element.
 *
 * For example:
 * `sbepp::set_choice_traits<SetTag::choice>::since_version();`
 *
 * @tparam T set choice tag
 */
template<typename T>
class set_choice_traits;

#ifdef SBEPP_DOXYGEN
template<typename T>
class set_choice_traits
{
public:
    //! @brief Returns `name` attribute
    static constexpr const char* name() noexcept;
    //! @brief Returns `description` attribute
    static constexpr const char* description() noexcept;
    //! @brief Returns `addedSince` attribute
    static constexpr version_t since_version() noexcept;
    //! @brief Returns `deprecated` attribute. Available only if provided in
    //! schema
    static constexpr version_t deprecated() noexcept;
    //! @brief Returns choice bit index
    static constexpr choice_index_t index() noexcept;
};
#endif

/**
 * @brief Provides various traits/attributes of a `<composite>` element.
 *
 * For example:
 * `sbepp::composite_traits<SchemaTag::types::composite>::since_version();`
 *
 * @tparam T composite tag
 */
template<typename T>
class composite_traits;

#ifdef SBEPP_DOXYGEN
template<typename T>
class composite_traits
{
public:
    //! @brief Returns `name` attribute
    static constexpr const char* name() noexcept;
    //! @brief Returns `description` attribute
    static constexpr const char* description() noexcept;
    /**
     * @brief Returns type offset. Available only if offset is static, i.e. not
     *  available for public composites whose offset is different on each usage.
     */
    static constexpr offset_t offset() noexcept;
    //! @brief Returns `semanticType` attribute
    static constexpr const char* semantic_type() noexcept;
    //! @brief Returns `addedSince` attribute
    static constexpr version_t since_version() noexcept;
    //! @brief Returns `deprecated` attribute. Available only if provided in
    // !schema
    static constexpr version_t deprecated() noexcept;
    /**
     * @brief Representation type
     *
     * @tparam Byte byte type
     */
    template<typename Byte>
    using value_type = CompositeType<Byte>;
    //! @brief Size of the composite in bytes
    static constexpr std::size_t size_bytes() noexcept;
};
#endif

/**
 * @brief Provides various traits/attributes of a `<message>` element.
 *
 * For example:
 * `sbepp::message_traits<market_schema::schema::messages::msg1>::name();`
 *
 * @tparam T message tag
 */
template<typename T>
class message_traits;

#ifdef SBEPP_DOXYGEN
template<typename T>
class message_traits
{
public:
    //! @brief Returns `name` attribute
    static constexpr const char* name() noexcept;
    //! @brief Returns `description` attribute
    static constexpr const char* description() noexcept;
    //! @brief Returns `id` attribute
    static constexpr message_id_t id() noexcept;
    //! @brief Returns `blockLength` attribute
    static constexpr block_length_t block_length() noexcept;
    //! @brief Returns `semanticType` attribute
    static constexpr const char* semantic_type() noexcept;
    //! @brief Returns `addedSince` attribute
    static constexpr version_t since_version() noexcept;
    //! @brief Returns `deprecated` attribute. Available only if provided in
    //! schema
    static constexpr version_t deprecated() noexcept;
    /**
     * @brief Representation type
     *
     * @tparam Byte byte type
     */
    template<typename Byte>
    using value_type = MessageType<Byte>;
};
#endif

/**
 * @brief Provides various traits/attributes of a `<field>` element.
 *
 * For example:
 * `sbepp::message_traits<message_tag::field_name>::since_version();`
 *
 * @tparam T field tag
 */
template<typename T>
class field_traits;

#ifdef SBEPP_DOXYGEN
template<typename T>
class field_traits
{
public:
    //! @brief Returns `name` attribute
    static constexpr const char* name() noexcept;
    //! @brief Returns `id` attribute
    static constexpr member_id_t id() noexcept;
    //! @brief Returns `description` attribute
    static constexpr const char* description() noexcept;
    /**
     * @brief Returns the actual presence. Note that it can be different from
     *  the one provided in schema, for example if field type is `required` but
     *  the field itself is specified as `optional`.
     */
    static constexpr field_presence presence() noexcept;
    //! Returns actual offset
    static constexpr offset_t offset() noexcept;
    //! @brief Returns `addedSince` attribute
    static constexpr version_t since_version() noexcept;
    //! @brief Returns `deprecated` attribute. Available only if provided in
    //!  schema
    static constexpr version_t deprecated() noexcept;
    //! @brief Representation type
    using value_type = ValueType;
    /**
     * @brief Representation type for non-constant arrays or composites
     *
     * @tparam Byte byte type
     */
    template<typename Byte>
    using value_type = Type<Byte>;
    //!  @brief `value_type`'s tag. Not available for constants of numeric types
    using value_type_tag = TypeTag;
};
#endif

/**
 * @brief Provides various traits/attributes of a `<group>` element.
 *
 * For example:
 * `sbepp::group_traits<message_tag::group_name>::since_version();`
 *
 * @tparam T group tag
 */
template<typename T>
class group_traits;

#ifdef SBEPP_DOXYGEN
template<typename T>
class group_traits
{
public:
    //! @brief Returns `name` attribute
    static constexpr const char* name() noexcept;
    //! @brief Returns `description` attribute
    static constexpr const char* description() noexcept;
    //! @brief Returns `id` attribute
    static constexpr member_id_t id() noexcept;
    //! @brief Returns `blockLength` attribute
    static constexpr block_length_t block_length() noexcept;
    //! @brief Returns `semanticType` attribute
    static constexpr const char* semantic_type() noexcept;
    //! @brief Returns `addedSince` attribute
    static constexpr version_t since_version() noexcept;
    //! @brief Returns `deprecated` attribute. Available only if provided in
    //! schema
    static constexpr version_t deprecated() noexcept;
    /**
     * @brief Representation type
     *
     * @tparam Byte byte type
     */
    template<typename Byte>
    using value_type = GroupType<Byte>;
    /**
     * @brief Group dimension composite type
     *
     * @tparam Byte byte type
     */
    template<typename Byte>
    using dimension_type = HeaderType<Byte>;
    //! @brief Dimension composite tag
    using dimension_type_tag = HeaderTag;
    /**
     * @brief Group entry type
     *
     * @tparam Byte byte type
     */
    template<typename Byte>
    using entry_type = EntryType<Byte>;
};
#endif

/**
 * @brief Provides various traits/attributes of a `<data>` element.
 *
 * For example:
 * `sbepp::group_traits<message_tag::data_name>::since_version();`
 *
 * @tparam T data tag
 */
template<typename T>
class data_traits;

#ifdef SBEPP_DOXYGEN
template<typename T>
class data_traits
{
public:
    //! @brief Returns `name` attribute
    static constexpr const char* name() noexcept;
    //! @brief Returns `id` attribute
    static constexpr member_id_t id() noexcept;
    //! @brief Returns `description` attribute
    static constexpr const char* description() noexcept;
    //! @brief Returns `addedSince` attribute
    static constexpr version_t since_version() noexcept;
    //! @brief Returns `deprecated` attribute. Available only if provided in
    //!  schema
    static constexpr version_t deprecated() noexcept;
    /**
     * @brief Representation type
     *
     * @tparam Byte byte type
     */
    template<typename Byte>
    using value_type = DataType;
    //! @brief Length type
    using length_type = LengthType;
    //! @brief `length_type` tag
    using length_type_tag = LengthTypeTag;
};
#endif
/** @} */

// NOLINTNEXTLINE: macro is required here
#define SBEPP_BUILT_IN_IMPL(NAME, TYPE, MIN, MAX, NULL)               \
    /** @brief Built-in `NAME` required type */                       \
    /** Also works as a tag for its traits */                         \
    class NAME##_t : public detail::required_base<TYPE, NAME##_t>     \
    {                                                                 \
    public:                                                           \
        using detail::required_base<TYPE, NAME##_t>::required_base;   \
                                                                      \
        /** @brief Returns `minValue` attribute */                    \
        static constexpr value_type min_value() noexcept              \
        {                                                             \
            return {MIN};                                             \
        }                                                             \
                                                                      \
        /** @brief Returns `maxValue` attribute */                    \
        static constexpr value_type max_value() noexcept              \
        {                                                             \
            return {MAX};                                             \
        }                                                             \
    };                                                                \
                                                                      \
    /** @brief Built-in `NAME` optional type */                       \
    /** Also works as a tag for its traits */                         \
    class NAME##_opt_t : public detail::optional_base<TYPE, NAME##_t> \
    {                                                                 \
    public:                                                           \
        using detail::optional_base<TYPE, NAME##_t>::optional_base;   \
                                                                      \
        /** @brief Returns `minValue` attribute */                    \
        static constexpr value_type min_value() noexcept              \
        {                                                             \
            return {MIN};                                             \
        }                                                             \
                                                                      \
        /** @brief Returns `maxValue` attribute */                    \
        static constexpr value_type max_value() noexcept              \
        {                                                             \
            return {MAX};                                             \
        }                                                             \
                                                                      \
        /** @brief Returns `nullValue` attribute */                   \
        static constexpr value_type null_value() noexcept             \
        {                                                             \
            return {NULL};                                            \
        }                                                             \
    };                                                                \
                                                                      \
    template<>                                                        \
    class type_traits<NAME##_t>                                       \
    {                                                                 \
    public:                                                           \
        static constexpr const char* name() noexcept                  \
        {                                                             \
            return #NAME;                                             \
        }                                                             \
                                                                      \
        static constexpr const char* description() noexcept           \
        {                                                             \
            return "";                                                \
        }                                                             \
                                                                      \
        static constexpr field_presence presence() noexcept           \
        {                                                             \
            return field_presence::required;                          \
        }                                                             \
                                                                      \
        static constexpr TYPE min_value() noexcept                    \
        {                                                             \
            return NAME##_t::min_value();                             \
        }                                                             \
                                                                      \
        static constexpr TYPE max_value() noexcept                    \
        {                                                             \
            return NAME##_t::max_value();                             \
        }                                                             \
                                                                      \
        static constexpr length_t length() noexcept                   \
        {                                                             \
            return 1;                                                 \
        }                                                             \
                                                                      \
        static constexpr const char* semantic_type() noexcept         \
        {                                                             \
            return "";                                                \
        }                                                             \
                                                                      \
        static constexpr version_t since_version() noexcept           \
        {                                                             \
            return 0;                                                 \
        }                                                             \
                                                                      \
        using value_type = NAME##_t;                                  \
        using primitive_type = value_type::value_type;                \
    };                                                                \
                                                                      \
    template<>                                                        \
    class type_traits<NAME##_opt_t>                                   \
    {                                                                 \
    public:                                                           \
        static constexpr const char* name() noexcept                  \
        {                                                             \
            return #NAME;                                             \
        }                                                             \
                                                                      \
        static constexpr const char* description() noexcept           \
        {                                                             \
            return "";                                                \
        }                                                             \
                                                                      \
        static constexpr field_presence presence() noexcept           \
        {                                                             \
            return field_presence::optional;                          \
        }                                                             \
                                                                      \
        static constexpr TYPE min_value() noexcept                    \
        {                                                             \
            return NAME##_opt_t::min_value();                         \
        }                                                             \
                                                                      \
        static constexpr TYPE max_value() noexcept                    \
        {                                                             \
            return NAME##_opt_t::max_value();                         \
        }                                                             \
                                                                      \
        static constexpr TYPE null_value() noexcept                   \
        {                                                             \
            return NAME##_opt_t::null_value();                        \
        }                                                             \
                                                                      \
        static constexpr length_t length() noexcept                   \
        {                                                             \
            return 1;                                                 \
        }                                                             \
                                                                      \
        static constexpr const char* semantic_type() noexcept         \
        {                                                             \
            return "";                                                \
        }                                                             \
                                                                      \
        static constexpr version_t since_version() noexcept           \
        {                                                             \
            return 0;                                                 \
        }                                                             \
                                                                      \
        using value_type = NAME##_opt_t;                              \
        using primitive_type = value_type::value_type;                \
    }

SBEPP_BUILT_IN_IMPL(char, char, 0x20, 0x7E, 0);
SBEPP_BUILT_IN_IMPL(
    int8,
    std::int8_t,
    std::numeric_limits<std::int8_t>::min() + 1,
    std::numeric_limits<std::int8_t>::max(),
    std::numeric_limits<std::int8_t>::min());
SBEPP_BUILT_IN_IMPL(
    uint8,
    std::uint8_t,
    std::numeric_limits<std::uint8_t>::min(),
    std::numeric_limits<std::uint8_t>::max() - 1,
    std::numeric_limits<std::uint8_t>::max());
SBEPP_BUILT_IN_IMPL(
    int16,
    std::int16_t,
    std::numeric_limits<std::int16_t>::min() + 1,
    std::numeric_limits<std::int16_t>::max(),
    std::numeric_limits<std::int16_t>::min());
SBEPP_BUILT_IN_IMPL(
    uint16,
    std::uint16_t,
    std::numeric_limits<std::uint16_t>::min(),
    std::numeric_limits<std::uint16_t>::max() - 1,
    std::numeric_limits<std::uint16_t>::max());
SBEPP_BUILT_IN_IMPL(
    int32,
    std::int32_t,
    std::numeric_limits<std::int32_t>::min() + 1,
    std::numeric_limits<std::int32_t>::max(),
    std::numeric_limits<std::int32_t>::min());
SBEPP_BUILT_IN_IMPL(
    uint32,
    std::uint32_t,
    std::numeric_limits<std::uint32_t>::min(),
    std::numeric_limits<std::uint32_t>::max() - 1,
    std::numeric_limits<std::uint32_t>::max());
SBEPP_BUILT_IN_IMPL(
    int64,
    std::int64_t,
    std::numeric_limits<std::int64_t>::min() + 1,
    std::numeric_limits<std::int64_t>::max(),
    std::numeric_limits<std::int64_t>::min());
SBEPP_BUILT_IN_IMPL(
    uint64,
    std::uint64_t,
    std::numeric_limits<std::uint64_t>::min(),
    std::numeric_limits<std::uint64_t>::max() - 1,
    std::numeric_limits<std::uint64_t>::max());
SBEPP_BUILT_IN_IMPL(
    float,
    float,
    std::numeric_limits<float>::min(),
    std::numeric_limits<float>::max(),
    std::numeric_limits<float>::quiet_NaN());
SBEPP_BUILT_IN_IMPL(
    double,
    double,
    std::numeric_limits<double>::min(),
    std::numeric_limits<double>::max(),
    std::numeric_limits<double>::quiet_NaN());

#undef SBEPP_BUILT_IN_IMPL

/**
 * @brief Construct view from memory buffer.
 *
 * View will have the same byte type as provided buffer.
 * Example:
 * ```cpp
 * std::array<char, N> buf;
 * auto m = sbepp::make_view<some_schema::messages::msg1>(
 *      buf.data(), buf.size()); // creates some_schema::messages::msg1<char>
 * ```
 *
 * @tparam View view template
 * @tparam Byte byte type
 * @param ptr buffer start
 * @param size buffer size
 * @return constructed view
 */
template<template<typename> class View, typename Byte>
constexpr View<Byte> make_view(Byte* ptr, const std::size_t size) noexcept
{
    return {ptr, size};
}

/**
 * @brief Construct read-only view from memory buffer.
 *
 * View's byte type will be a const-qualified buffer byte type.
 * Example:
 * ```cpp
 * std::array<char, N> buf;
 * // creates some_schema::messages::msg1<const char>
 * auto m = sbepp::make_view<some_schema::messages::msg1>(
 *      buf.data(), buf.size());
 * ```
 *
 * @tparam View view template
 * @tparam Byte byte type
 * @param ptr buffer start
 * @param size buffer size
 * @return constructed view
 */
template<template<typename> class View, typename Byte>
constexpr View<typename std::add_const<Byte>::type>
    make_const_view(Byte* ptr, const std::size_t size) noexcept
{
    return {ptr, size};
}

namespace detail
{
// taken from https://stackoverflow.com/a/34672753
template<template<typename...> class Base, typename Derived>
struct is_base_of_tmp_impl
{
    static constexpr std::false_type test(...);

    template<typename... Ts>
    static constexpr std::true_type test(Base<Ts...>*);

    using type = decltype(test(std::declval<Derived*>()));
};

template<template<typename...> class Base, typename Derived>
using is_base_of_tmp = typename is_base_of_tmp_impl<Base, Derived>::type;

#if SBEPP_HAS_CONCEPTS
template<typename Derived, template<typename...> class Base>
concept derived_from_tmp = is_base_of_tmp<Base, Derived>::value;
#endif
} // namespace detail

/**
 * @brief Visit a view using given cursor
 *
 * @tparam Visitor visitor type
 * @param view view to visit
 * @param c cursor
 * @param visitor visitor
 * @return forwarded reference to `visitor`
 */
template<typename Visitor, typename View, typename Cursor>
SBEPP_CPP14_CONSTEXPR Visitor&&
    visit(View view, Cursor& c, Visitor&& visitor = {})
{
    view(detail::visit_tag{}, visitor, c);
    return std::forward<Visitor>(visitor);
}

/**
 * @brief Visits a view
 *
 * @tparam Visitor visitor
 * @param view view to visit
 * @param visitor visitor
 * @return forwarded reference to `visitor`
 */
template<typename Visitor, typename View>
SBEPP_CPP14_CONSTEXPR Visitor&& visit(View view, Visitor&& visitor = {})
{
    auto c = init_cursor(view);
    return visit(view, c, std::forward<Visitor>(visitor));
}

/**
 * @brief Visit view's children using provided cursor
 *
 * @tparam Visitor visitor type
 * @param view view to visit
 * @param c cursor
 * @param visitor visitor
 * @return forwarded reference to `visitor`
 */
template<typename Visitor, typename View, typename Cursor>
SBEPP_CPP14_CONSTEXPR Visitor&&
    visit_children(View view, Cursor& c, Visitor&& visitor = {})
{
    view(detail::visit_children_tag{}, visitor, c);
    return std::forward<Visitor>(visitor);
}

/**
 * @brief Visit view's children
 *
 * @tparam Visitor visitor type
 * @param view view to visit
 * @param visitor visitor
 * @return forwarded reference to `visitor`
 */
template<typename Visitor, typename View>
SBEPP_CPP14_CONSTEXPR Visitor&&
    visit_children(View view, Visitor&& visitor = {})
{
    auto c = init_cursor(view);
    return visit_children(view, c, std::forward<Visitor>(visitor));
}

namespace detail
{
class size_bytes_checked_visitor
{
public:
    constexpr explicit size_bytes_checked_visitor(
        const std::size_t size) noexcept
        : size{size}
    {
    }

    template<typename T, typename Cursor, typename Tag>
    SBEPP_CPP14_CONSTEXPR void on_message(T m, Cursor& c, Tag) noexcept
    {
        const auto header = get_header(m);
        const auto header_size = size_bytes(header);
        if(!validate_and_subtract(header_size))
        {
            return;
        }

        if(!validate_and_subtract(*header.blockLength()))
        {
            return;
        }

        sbepp::visit_children(m, c, *this);
    }

    template<typename T, typename Cursor, typename Tag>
    SBEPP_CPP14_CONSTEXPR bool on_group(T g, Cursor& c, Tag) noexcept
    {
        const auto header = get_header(g);
        const auto header_size = size_bytes(header);
        if(!validate_and_subtract(header_size))
        {
            return true;
        }

        const auto prev_block_length =
            set_group_block_length(*header.blockLength());
        visit_children(g, c, *this);
        set_group_block_length(prev_block_length);

        return !is_valid();
    }

    template<typename T, typename Cursor>
    SBEPP_CPP14_CONSTEXPR bool on_entry(T e, Cursor& c) noexcept
    {
        if(!validate_and_subtract(group_block_length))
        {
            return true;
        }

        return !visit_children(e, c, *this).is_valid();
    }

    template<typename T, typename Tag>
    SBEPP_CPP14_CONSTEXPR bool on_data(T d, Tag) noexcept
    {
        return !validate_and_subtract(size_bytes(d));
    }

    // ignore them all because we validate `blockLength`
    template<typename T, typename Tag>
    constexpr bool on_field(T, Tag) const noexcept
    {
        return {};
    }

    constexpr bool is_valid() const noexcept
    {
        return valid;
    }

    // returns previous value
    SBEPP_CPP14_CONSTEXPR std::size_t
        set_group_block_length(const std::size_t block_length) noexcept
    {
        auto prev = group_block_length;
        group_block_length = block_length;
        return prev;
    }

    constexpr std::size_t get_size() const noexcept
    {
        return size;
    }

private:
    std::size_t size;
    bool valid{true};
    // current group's blockLength, used to validate entry
    std::size_t group_block_length{};

    SBEPP_CPP14_CONSTEXPR bool
        validate_and_subtract(const std::size_t n) noexcept
    {
        if(size < n)
        {
            valid = false;
        }
        else
        {
            size -= n;
        }

        return valid;
    }
};
} // namespace detail

//! @brief Result type of `size_bytes_checked`
struct size_bytes_checked_result
{
    //! Denotes whether `size` is valid
    bool valid;
    //! Calculated size, valid only if `valid == true`
    std::size_t size;
};

// can be used with message/group
/**
 * @brief Calculate `view` size with additional safety checks.
 *
 * Similar to `size_bytes()` but stops if `view` cannot fit into the given
 * `size`. Useful to check that incoming message is fully contained within given
 * buffer.
 *
 * @param view message or group view
 * @param size buffer size
 */
template<typename View>
SBEPP_CPP20_CONSTEXPR size_bytes_checked_result
    size_bytes_checked(View view, std::size_t size) noexcept
{
    // `init_cursor` skips header, we need to ensure there's enough space for it
    if(!addressof(view) || (size < detail::get_header_size(view)))
    {
        return {};
    }

    detail::size_bytes_checked_visitor visitor{size};
    auto c = init_cursor(view);
    visit(view, c, visitor);
    if(visitor.is_valid())
    {
        return {true, size - visitor.get_size()};
    }
    return {};
}

/**
 * @brief Converts enum to string
 *
 * @param e enum to convert
 * @param only_enumerator if `false`, returns full name in form `Enum::Value`,
 *      otherwise returns only `Value`
 * @returns pointer to a null-terminated string representing an enumerator's
 *  name or `nullptr` if `e` holds unknown value
 */
template<typename E>
constexpr auto enum_to_string(const E e) noexcept
    -> decltype(tag_invoke(detail::enum_to_str_tag{}, e))
{
    return tag_invoke(detail::enum_to_str_tag{}, e);
}

/**
 * @brief Visits set choices in order of their declaration
 *
 * @param s set to visit
 * @param visitor visitor. Must have signature
 *      `void (bool choice_value, const char* choice_name)`
 * @return forwarded reference to `visitor`
 */
template<typename Set, typename Visitor>
constexpr auto visit_set(const Set s, Visitor&& visitor) noexcept
    -> decltype(s(detail::visit_set_tag{}, std::forward<Visitor>(visitor)))
{
    return s(detail::visit_set_tag{}, std::forward<Visitor>(visitor));
}

namespace detail
{
template<typename Derived>
struct is_array_type_impl
{
    static constexpr std::false_type test(...);

    template<typename T1, typename T2, std::size_t N>
    static constexpr std::true_type test(detail::static_array_ref<T1, T2, N>*);

    using type = decltype(test(std::declval<Derived*>()));
};
} // namespace detail

//! @brief Checks is `T` is an array type
template<typename T>
using is_array_type = typename detail::is_array_type_impl<T>::type;

//! @brief Checks if `T` is a non-array required type
template<typename T>
using is_required_type = detail::is_base_of_tmp<detail::required_base, T>;

//! @brief Checks if `T` is a non-array optional type
template<typename T>
using is_optional_type = detail::is_base_of_tmp<detail::optional_base, T>;

//! @brief Checks if `T` is a non-array type
template<typename T>
using is_non_array_type = std::integral_constant<
    bool,
    is_required_type<T>::value || is_optional_type<T>::value>;

//! @brief Checks if `T` is a type of any kind
template<typename T>
using is_type = std::integral_constant<
    bool,
    is_required_type<T>::value || is_optional_type<T>::value
        || is_array_type<T>::value>;

//! @brief Checks if `T` is an enumeration
template<typename T, typename = void>
struct is_enum : std::false_type
{
};

template<typename T>
struct is_enum<
    T,
    detail::void_t<decltype(sbepp::enum_to_string(std::declval<T>()))>>
    : std::true_type
{
};

//! @brief Checks if `T` is a set
template<typename T>
using is_set = detail::is_base_of_tmp<detail::bitset_base, T>;

//! @brief Checks if `T` is a composite
template<typename T>
using is_composite = detail::is_base_of_tmp<detail::composite_base, T>;

//! @brief Checks if `T` is a message
template<typename T>
using is_message = detail::is_base_of_tmp<detail::message_base, T>;

//! @brief Checks if `T` is a flat group
template<typename T>
using is_flat_group = detail::is_base_of_tmp<detail::flat_group_base, T>;

//! @brief Checks if `T` is a nested group
template<typename T>
using is_nested_group = detail::is_base_of_tmp<detail::nested_group_base, T>;

//! @brief Checks if `T` is a group of any kind
template<typename T>
using is_group = std::integral_constant<
    bool,
    is_flat_group<T>::value || is_nested_group<T>::value>;

namespace detail
{
template<typename Derived>
struct is_data_impl
{
    static constexpr std::false_type test(...);

    template<typename T1, typename T2, typename T3, endian E>
    static constexpr std::true_type
        test(detail::dynamic_array_ref<T1, T2, T3, E>*);

    using type = decltype(test(std::declval<Derived*>()));
};
} // namespace detail

//! @brief Checks if `T` is a data
template<typename T>
using is_data = typename detail::is_data_impl<T>::type;

#if SBEPP_HAS_INLINE_VARS
//! @brief Shorthand for `sbepp::is_array_type<T>::value`
template<typename T>
inline constexpr auto is_array_type_v = is_array_type<T>::value;

//! @brief Shorthand for `sbepp::is_required_type<T>::value`
template<typename T>
inline constexpr auto is_required_type_v = is_required_type<T>::value;

//! @brief Shorthand for `sbepp::is_optional_type<T>::value`
template<typename T>
inline constexpr auto is_optional_type_v = is_optional_type<T>::value;

//! @brief Shorthand for `sbepp::is_non_array_type<T>::value`
template<typename T>
inline constexpr auto is_non_array_type_v = is_non_array_type<T>::value;

//! @brief Shorthand for `sbepp::is_type<T>::value`
template<typename T>
inline constexpr auto is_type_v = is_type<T>::value;

//! @brief Shorthand for `sbepp::is_enum<T>::value`
template<typename T>
inline constexpr auto is_enum_v = is_enum<T>::value;

//! @brief Shorthand for `sbepp::is_set<T>::value`
template<typename T>
inline constexpr auto is_set_v = is_set<T>::value;

//! @brief Shorthand for `sbepp::is_composite<T>::value`
template<typename T>
inline constexpr auto is_composite_v = is_composite<T>::value;

//! @brief Shorthand for `sbepp::is_message<T>::value`
template<typename T>
inline constexpr auto is_message_v = is_message<T>::value;

//! @brief Shorthand for `sbepp::is_flat_group<T>::value`
template<typename T>
inline constexpr auto is_flat_group_v = is_flat_group<T>::value;

//! @brief Shorthand for `sbepp::is_nested_group<T>::value`
template<typename T>
inline constexpr auto is_nested_group_v = is_nested_group<T>::value;

//! @brief Shorthand for `sbepp::is_group<T>::value`
template<typename T>
inline constexpr auto is_group_v = is_group<T>::value;

//! @brief Shorthand for `sbepp::is_data<T>::value`
template<typename T>
inline constexpr auto is_data_v = is_data<T>::value;
#endif

#if SBEPP_HAS_CONCEPTS
//! @brief Concept for `sbepp::is_array_type<T>::value`
template<typename T>
concept array_type = is_array_type_v<T>;

//! @brief Concept for `sbepp::is_required_type<T>::value`
template<typename T>
concept required_type = is_required_type_v<T>;

//! @brief Concept for `sbepp::is_optional_type<T>::value`
template<typename T>
concept optional_type = is_optional_type_v<T>;

//! @brief Concept for `sbepp::is_non_array_type<T>::value`
template<typename T>
concept non_array_type = is_non_array_type_v<T>;

//! @brief Concept for `sbepp::is_type<T>::value`
template<typename T>
concept type = is_type_v<T>;

//! @brief Concept for `sbepp::is_enum<T>::value`
template<typename T>
concept enumeration = is_enum_v<T>;

//! @brief Concept for `sbepp::is_set<T>::value`
template<typename T>
concept set = is_set_v<T>;

//! @brief Concept for `sbepp::is_composite<T>::value`
template<typename T>
concept composite = is_composite_v<T>;

//! @brief Concept for `sbepp::is_message<T>::value`
template<typename T>
concept message = is_message_v<T>;

//! @brief Concept for `sbepp::is_flat_group<T>::value`
template<typename T>
concept flat_group = is_flat_group_v<T>;

//! @brief Concept for `sbepp::is_nested_group<T>::value`
template<typename T>
concept nested_group = is_nested_group_v<T>;

//! @brief Concept for `sbepp::is_group<T>::value`
template<typename T>
concept group = is_group_v<T>;

//! @brief Concept for `sbepp::is_data<T>::value`
template<typename T>
concept data = is_data_v<T>;
#endif
} // namespace sbepp

#if SBEPP_HAS_RANGES && SBEPP_HAS_CONCEPTS

template<typename Byte, typename Value, std::size_t N>
inline constexpr bool std::ranges::enable_borrowed_range<
    sbepp::detail::static_array_ref<Byte, Value, N>> = true;

template<typename Byte, typename Value, typename Length, sbepp::endian E>
inline constexpr bool std::ranges::enable_borrowed_range<
    sbepp::detail::dynamic_array_ref<Byte, Value, Length, E>> = true;

template<sbepp::detail::derived_from_tmp<sbepp::detail::flat_group_base> T>
inline constexpr bool std::ranges::enable_borrowed_range<T> = true;

template<sbepp::detail::derived_from_tmp<sbepp::detail::nested_group_base> T>
inline constexpr bool std::ranges::enable_borrowed_range<T> = true;
#endif

#undef SBEPP_CPP17_INLINE_VAR

SBEPP_WARNINGS_ON();
