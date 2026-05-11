// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::endian"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_ENDIAN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_ENDIAN_HPP_

// IWYU pragma: private, include "arene/base/endian.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax pmeritted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/copy.hpp"
#include "arene/base/byte/byte.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/is_arithmetic.hpp"
#include "arene/base/stdlib_choice/is_enum.hpp"
#include "arene/base/stdlib_choice/is_floating_point.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/underlying_type.hpp"
#include "arene/base/type_traits/type_identity.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

#if defined(_WIN32)
#include "arene/base/endian/detail/endian_windows_impl.hpp"  // IWYU pragma: export
#elif defined(__BYTE_ORDER__)
#include "arene/base/endian/detail/endian_specified_byte_order_impl.hpp"  // IWYU pragma: export
#else
#include "arene/base/endian/detail/endian_generic_impl.hpp"  // IWYU pragma: export
#endif

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-A0_4_2-a-2 "long double named as explicitly not supported"

///////////
//
// Utilities for reading/writing binary values in LITTLE-ENDIAN format.

/// @brief Read a value from a serialized binary representation stored in little-endian byte order.
///
/// @tparam T The type of the value to read.
/// @param bytes A view of the bytes to read the data from.
/// @return The read value
template <
    typename T,
    constraints<std::enable_if_t<!std::is_enum<T>::value>, std::enable_if_t<!std::is_arithmetic<T>::value>> = nullptr>
auto read_little_endian(span<byte const, sizeof(T)> bytes) noexcept -> T = delete;

/// @brief Read an integral value from a serialized binary representation stored in little-endian byte order.
///
/// @tparam T The type of the value to read.
/// @param bytes A view of the bytes to read the data from.
/// @return The read value
template <typename T, constraints<std::enable_if_t<std::is_integral<T>::value>> = nullptr>
constexpr auto read_little_endian(span<byte const, sizeof(T)> bytes) noexcept -> T;

/// @brief Read an enum value from a serialized binary representation stored in little-endian byte order.
///
/// @tparam T The type of the value to read.
/// @param bytes A view of the bytes to read the data from.
/// @return The read value
template <typename T, constraints<std::enable_if_t<std::is_enum<T>::value>> = nullptr>
constexpr auto read_little_endian(span<byte const, sizeof(T)> bytes) noexcept -> T;

/// @brief Read a floating point value from a serialized binary representation stored in little-endian byte order.
///
/// @tparam T The type of the value to read.
/// @param bytes A view of the bytes to read the data from.
/// @return The read value
template <
    typename T,
    constraints<
        std::enable_if_t<std::is_floating_point<T>::value>,
        std::enable_if_t<!std::is_same<T, long double>::value>> = nullptr>
auto read_little_endian(span<byte const, sizeof(T)> bytes) noexcept -> T;

/// @brief Write a value to a buffer in little-endian binary representation.
///
/// @tparam T The type of the value to write. This template parameter must be explicitly specified.
/// @param value The value to write.
/// @param bytes The buffer to write the little-endian binary representation to.
template <
    typename T,
    constraints<std::enable_if_t<!std::is_enum<T>::value>, std::enable_if_t<!std::is_arithmetic<T>::value>> = nullptr>
void write_little_endian(base::type_identity_t<T> value, span<byte, sizeof(T)> bytes) noexcept = delete;

/// @brief Write an integral value to a buffer in little-endian binary representation.
///
/// @tparam T The type of the value to write. This template parameter must be explicitly specified.
/// @param value The value to write.
/// @param bytes The buffer to write the little-endian binary representation to.
template <typename T, constraints<std::enable_if_t<std::is_integral<T>::value>> = nullptr>
constexpr void write_little_endian(base::type_identity_t<T> value, span<byte, sizeof(T)> bytes) noexcept;

/// @brief Write an enum value to a buffer in little-endian binary representation.
///
/// @tparam T The type of the value to write. This template parameter must be explicitly specified.
/// @param value The value to write.
/// @param bytes The buffer to write the little-endian binary representation to.
template <typename T, constraints<std::enable_if_t<std::is_enum<T>::value>> = nullptr>
constexpr void write_little_endian(base::type_identity_t<T> value, span<byte, sizeof(T)> bytes) noexcept;

/// @brief Write a floating point value to a buffer in little-endian binary representation.
///
/// @tparam T The type of the value to write. This template parameter must be explicitly specified.
/// @param value The value to write.
/// @param bytes The buffer to write the little-endian binary representation to.
template <
    typename T,
    constraints<
        std::enable_if_t<std::is_floating_point<T>::value>,
        std::enable_if_t<!std::is_same<T, long double>::value>> = nullptr>
void write_little_endian(base::type_identity_t<T> value, span<byte, sizeof(T)> bytes) noexcept;

///////////
//
// Utilities for reading/writing binary values in BIG-ENDIAN format.

/// @brief Read a value from a serialized binary representation stored in big-endian byte order.
///
/// @tparam T The type of the value to read.
/// @param bytes A view of the bytes to read the data from.
/// @return The read value
template <
    typename T,
    constraints<std::enable_if_t<!std::is_enum<T>::value>, std::enable_if_t<!std::is_arithmetic<T>::value>> = nullptr>
auto read_big_endian(span<byte const, sizeof(T)> bytes) noexcept -> T = delete;

/// @brief Read a value from a serialized binary representation stored in big-endian byte order.
///
/// @tparam T The type of the value to read.
/// @param bytes A view of the bytes to read the data from.
/// @return The read value
template <typename T, constraints<std::enable_if_t<std::is_integral<T>::value>> = nullptr>
constexpr auto read_big_endian(span<byte const, sizeof(T)> bytes) noexcept -> T;

/// @brief Read a value from a serialized binary representation stored in big-endian byte order.
///
/// @tparam T The type of the value to read.
/// @param bytes A view of the bytes to read the data from.
/// @return The read value
template <typename T, constraints<std::enable_if_t<std::is_enum<T>::value>> = nullptr>
constexpr auto read_big_endian(span<byte const, sizeof(T)> bytes) noexcept -> T;

/// @brief Read a value from a serialized binary representation stored in big-endian byte order.
///
/// @tparam T The type of the value to read.
/// @param bytes A view of the bytes to read the data from.
/// @return The read value
template <
    typename T,
    constraints<
        std::enable_if_t<std::is_floating_point<T>::value>,
        std::enable_if_t<!std::is_same<T, long double>::value>> = nullptr>
auto read_big_endian(span<byte const, sizeof(T)> bytes) noexcept -> T;

/// @brief Write a value to a buffer in big-endian binary representation.
///
/// @tparam T The type of the value to write. This template parameter must be explicitly specified.
/// @param value The value to write.
/// @param bytes The buffer to write the big-endian binary representation to.
template <
    typename T,
    constraints<std::enable_if_t<!std::is_enum<T>::value>, std::enable_if_t<!std::is_arithmetic<T>::value>> = nullptr>
void write_big_endian(base::type_identity_t<T> value, span<byte, sizeof(T)> bytes) noexcept = delete;

/// @brief Write a value to a buffer in big-endian binary representation.
///
/// @tparam T The type of the value to write. This template parameter must be explicitly specified.
/// @param value The value to write.
/// @param bytes The buffer to write the big-endian binary representation to.
template <typename T, constraints<std::enable_if_t<std::is_integral<T>::value>> = nullptr>
constexpr void write_big_endian(base::type_identity_t<T> value, span<byte, sizeof(T)> bytes) noexcept;

/// @brief Write a value to a buffer in big-endian binary representation.
///
/// @tparam T The type of the value to write. This template parameter must be explicitly specified.
/// @param value The value to write.
/// @param bytes The buffer to write the big-endian binary representation to.
template <typename T, constraints<std::enable_if_t<std::is_enum<T>::value>> = nullptr>
constexpr void write_big_endian(base::type_identity_t<T> value, span<byte, sizeof(T)> bytes) noexcept;

/// @brief Write a value to a buffer in big-endian binary representation.
///
/// @tparam T The type of the value to write. This template parameter must be explicitly specified.
/// @param value The value to write.
/// @param bytes The buffer to write the big-endian binary representation to.
template <
    typename T,
    constraints<
        std::enable_if_t<std::is_floating_point<T>::value>,
        std::enable_if_t<!std::is_same<T, long double>::value>> = nullptr>
void write_big_endian(base::type_identity_t<T> value, span<byte, sizeof(T)> bytes) noexcept;

}  // namespace base
}  // namespace arene

////////////////////////////////////////////
// Implementations

namespace arene {
namespace base {

namespace endian_detail {
/// @brief constants for bit offsets
/// @{
constexpr std::size_t bits_8{8U};
constexpr std::size_t bits_16{16U};
constexpr std::size_t bits_24{24U};
constexpr std::size_t bits_32{32U};
constexpr std::size_t bits_40{40U};
constexpr std::size_t bits_48{48U};
constexpr std::size_t bits_56{56U};
/// @}

/// @brief indexes for byte words
/// @{
constexpr std::size_t byte_0{0U};
constexpr std::size_t byte_1{1U};
constexpr std::size_t byte_2{2U};
constexpr std::size_t byte_3{3U};
constexpr std::size_t byte_4{4U};
constexpr std::size_t byte_5{5U};
constexpr std::size_t byte_6{6U};
constexpr std::size_t byte_7{7U};
/// @}

/// @brief Implementation template specialized for each size of type we want to handle
/// @tparam Size The number of bytes in the representation
template <std::size_t Size>
class endian_impl;

/// @brief Implementation template for an 8-bit type
template <>
class endian_impl<sizeof(std::uint8_t)> {
 public:
  /// @brief The value type
  using type = std::uint8_t;

  /// @brief Read the value
  /// @param bytes The source
  /// @return The read value
  static constexpr auto read_little_endian(span<byte const, sizeof(type)> const bytes) noexcept -> type {
    return ::arene::base::to_integer<type>(bytes[byte_0]);
  }

  /// @brief Write the value
  /// @param value The value to write
  /// @param bytes The target storage
  static constexpr void write_little_endian(type const value, span<byte, sizeof(type)> const bytes) noexcept {
    bytes[byte_0] = to_byte(value);
  }

  /// @brief Read the value
  /// @param bytes The source
  /// @return The read value
  static constexpr auto read_big_endian(span<byte const, sizeof(type)> const bytes) noexcept -> type {
    return ::arene::base::to_integer<type>(bytes[byte_0]);
  }

  /// @brief Write the value
  /// @param value The value to write
  /// @param bytes The target storage
  static constexpr void write_big_endian(type const value, span<byte, sizeof(type)> const bytes) noexcept {
    bytes[byte_0] = to_byte(value);
  }
};

/// @brief Implementation template for a 16-bit type
template <>
class endian_impl<sizeof(std::uint16_t)> {
 public:
  /// @brief The value type
  using type = std::uint16_t;

  /// @brief Read the value
  /// @param bytes The source
  /// @return The read value
  static constexpr auto read_little_endian(span<byte const, sizeof(type)> const bytes) noexcept -> type {
    // Note: GCC/Clang can usually recognise this pattern and compile this down
    // to a single load, optionally with the target architecture's byte-swap
    // instruction if byte-swapping is required.
    // However, when we know for sure that we're not being called as part of
    // a constant expression we dispatch to a memcpy/byte_swap-based
    // implementation above which the compiler is generally more reliable at
    // optimising.
    return ::arene::base::to_integer<std::uint16_t>(bytes[byte_0]) |
           static_cast<std::uint16_t>(::arene::base::to_integer<std::uint16_t>(bytes[byte_1]) << bits_8);
  }

  /// @brief Write the value
  /// @param value The value to write
  /// @param bytes The target storage
  static constexpr void write_little_endian(type const value, span<byte, sizeof(type)> const bytes) noexcept {
    // Note: GCC/Clang can usually recognise this pattern and compile this down
    // to a single store, optionally with the target architecture's byte-swap
    // instruction if byte-swapping is required.
    // However, when we know for sure that we're not being called as part of
    // a constant expression we dispatch to a memcpy/byte_swap-based
    // implementation above which the compiler is generally more reliable at
    // optimising.

    bytes[byte_0] = to_byte(value);
    bytes[byte_1] = to_byte(static_cast<std::uint8_t>(value >> bits_8));
  }

  /// @brief Read the value
  /// @param bytes The source
  /// @return The read value
  static constexpr auto read_big_endian(span<byte const, sizeof(type)> const bytes) noexcept -> type {
    // Note: GCC/Clang can usually recognise this pattern and compile this down
    // to a single load, optionally with the target architecture's byte-swap
    // instruction if byte-swapping is required.
    // However, when we know for sure that we're not being called as part of
    // a constant expression we dispatch to a memcpy/byte_swap-based
    // implementation above which the compiler is generally more reliable at
    // optimising.

    return ::arene::base::to_integer<std::uint16_t>(bytes[byte_1]) |
           static_cast<std::uint16_t>(::arene::base::to_integer<std::uint16_t>(bytes[byte_0]) << bits_8);
  }

  /// @brief Write the value
  /// @param value The value to write
  /// @param bytes The target storage
  static constexpr void write_big_endian(type const value, span<byte, sizeof(type)> const bytes) noexcept {
    // Note: GCC/Clang can usually recognise this pattern and compile this down
    // to a single store, optionally with the target architecture's byte-swap
    // instruction if byte-swapping is required.
    // However, when we know for sure that we're not being called as part of
    // a constant expression we dispatch to a memcpy/byte_swap-based
    // implementation above which the compiler is generally more reliable at
    // optimising.

    bytes[byte_1] = to_byte(value);
    bytes[byte_0] = to_byte(static_cast<std::uint8_t>(value >> bits_8));
  }
};

/// @brief Implementation template for a 32-bit type
template <>
class endian_impl<sizeof(std::uint32_t)> {
 public:
  /// @brief The value type
  using type = std::uint32_t;
  /// @brief Read the value
  /// @param bytes The source
  /// @return The read value
  static constexpr auto read_little_endian(span<byte const, sizeof(type)> const bytes) noexcept -> type {
    // Note: GCC/Clang can usually recognise this pattern and compile this down
    // to a single load, optionally with the target architecture's byte-swap
    // instruction if byte-swapping is required.
    // However, when we know for sure that we're not being called as part of
    // a constant expression we dispatch to a memcpy/byte_swap-based
    // implementation above which the compiler is generally more reliable at
    // optimising.

    return ::arene::base::to_integer<std::uint32_t>(bytes[byte_0]) |

           (::arene::base::to_integer<std::uint32_t>(bytes[byte_1]) << bits_8) |

           (::arene::base::to_integer<std::uint32_t>(bytes[byte_2]) << bits_16) |

           (::arene::base::to_integer<std::uint32_t>(bytes[byte_3]) << bits_24);
  }

  /// @brief Write the value
  /// @param value The value to write
  /// @param bytes The target storage
  static constexpr void write_little_endian(type const value, span<byte, sizeof(type)> const bytes) noexcept {
    // Note: GCC/Clang can usually recognise this pattern and compile this down
    // to a single store, optionally with the target architecture's byte-swap
    // instruction if byte-swapping is required.
    // However, when we know for sure that we're not being called as part of
    // a constant expression we dispatch to a memcpy/byte_swap-based
    // implementation above which the compiler is generally more reliable at
    // optimising.
    bytes[byte_0] = to_byte(value);
    bytes[byte_1] = to_byte(value >> bits_8);
    bytes[byte_2] = to_byte(value >> bits_16);
    bytes[byte_3] = to_byte(value >> bits_24);
  }

  /// @brief Read the value
  /// @param bytes The source
  /// @return The read value
  static constexpr auto read_big_endian(span<byte const, sizeof(type)> const bytes) noexcept -> type {
    // Note: GCC/Clang can usually recognise this pattern and compile this down
    // to a single load, optionally with the target architecture's byte-swap
    // instruction if byte-swapping is required.
    // However, when we know for sure that we're not being called as part of
    // a constant expression we dispatch to a memcpy/byte_swap-based
    // implementation above which the compiler is generally more reliable at
    // optimising.

    return ::arene::base::to_integer<std::uint32_t>(bytes[byte_3]) |
           (::arene::base::to_integer<std::uint32_t>(bytes[byte_2]) << bits_8) |
           (::arene::base::to_integer<std::uint32_t>(bytes[byte_1]) << bits_16) |
           (::arene::base::to_integer<std::uint32_t>(bytes[byte_0]) << bits_24);
  }

  /// @brief Write the value
  /// @param value The value to write
  /// @param bytes The target storage
  static constexpr void write_big_endian(type const value, span<byte, sizeof(type)> const bytes) noexcept {
    // Note: GCC/Clang can usually recognise this pattern and compile this down
    // to a single store, optionally with the target architecture's byte-swap
    // instruction if byte-swapping is required.
    // However, when we know for sure that we're not being called as part of
    // a constant expression we dispatch to a memcpy/byte_swap-based
    // implementation above which the compiler is generally more reliable at
    // optimising.

    bytes[byte_3] = to_byte(value);
    bytes[byte_2] = to_byte(value >> bits_8);
    bytes[byte_1] = to_byte(value >> bits_16);
    bytes[byte_0] = to_byte(value >> bits_24);
  }
};

/// @brief Implementation template for a 64-bit type
template <>
class endian_impl<sizeof(std::uint64_t)> {
 public:
  /// @brief The value type
  using type = std::uint64_t;
  /// @brief Read the value
  /// @param bytes The source
  /// @return The read value
  static constexpr auto read_little_endian(span<byte const, sizeof(type)> const bytes) noexcept -> type {
    // Note: GCC/Clang can usually recognise this pattern and compile this down
    // to a single load, optionally with the target architecture's byte-swap
    // instruction if byte-swapping is required.
    // However, when we know for sure that we're not being called as part of
    // a constant expression we dispatch to a memcpy/byte_swap-based
    // implementation above which the compiler is generally more reliable at
    // optimising.

    return ::arene::base::to_integer<std::uint64_t>(bytes[byte_0]) |
           (::arene::base::to_integer<std::uint64_t>(bytes[byte_1]) << bits_8) |
           (::arene::base::to_integer<std::uint64_t>(bytes[byte_2]) << bits_16) |
           (::arene::base::to_integer<std::uint64_t>(bytes[byte_3]) << bits_24) |
           (::arene::base::to_integer<std::uint64_t>(bytes[byte_4]) << bits_32) |
           (::arene::base::to_integer<std::uint64_t>(bytes[byte_5]) << bits_40) |
           (::arene::base::to_integer<std::uint64_t>(bytes[byte_6]) << bits_48) |
           (::arene::base::to_integer<std::uint64_t>(bytes[byte_7]) << bits_56);
  }

  /// @brief Write the value
  /// @param value The value to write
  /// @param bytes The target storage
  static constexpr void write_little_endian(type const value, span<byte, sizeof(type)> const bytes) noexcept {
    // Note: GCC/Clang can usually recognise this pattern and compile this down
    // to a single store, optionally with the target architecture's byte-swap
    // instruction if byte-swapping is required.
    // However, when we know for sure that we're not being called as part of
    // a constant expression we dispatch to a memcpy/byte_swap-based
    // implementation above which the compiler is generally more reliable at
    // optimising.

    bytes[byte_0] = to_byte(value);
    bytes[byte_1] = to_byte(value >> bits_8);
    bytes[byte_2] = to_byte(value >> bits_16);
    bytes[byte_3] = to_byte(value >> bits_24);
    bytes[byte_4] = to_byte(value >> bits_32);
    bytes[byte_5] = to_byte(value >> bits_40);
    bytes[byte_6] = to_byte(value >> bits_48);
    bytes[byte_7] = to_byte(value >> bits_56);
  }

  /// @brief Read the value
  /// @param bytes The source
  /// @return The read value
  static constexpr auto read_big_endian(span<byte const, sizeof(type)> const bytes) noexcept -> type {
    // Note: GCC/Clang can usually recognise this pattern and compile this down
    // to a single load, optionally with the target architecture's byte-swap
    // instruction if byte-swapping is required.
    // However, when we know for sure that we're not being called as part of
    // a constant expression we dispatch to a memcpy/byte_swap-based
    // implementation above which the compiler is generally more reliable at
    // optimising.

    return ::arene::base::to_integer<std::uint64_t>(bytes[byte_7]) |
           (::arene::base::to_integer<std::uint64_t>(bytes[byte_6]) << bits_8) |
           (::arene::base::to_integer<std::uint64_t>(bytes[byte_5]) << bits_16) |
           (::arene::base::to_integer<std::uint64_t>(bytes[byte_4]) << bits_24) |
           (::arene::base::to_integer<std::uint64_t>(bytes[byte_3]) << bits_32) |
           (::arene::base::to_integer<std::uint64_t>(bytes[byte_2]) << bits_40) |
           (::arene::base::to_integer<std::uint64_t>(bytes[byte_1]) << bits_48) |
           (::arene::base::to_integer<std::uint64_t>(bytes[byte_0]) << bits_56);
  }

  /// @brief Write the value
  /// @param value The value to write
  /// @param bytes The target storage
  static constexpr void write_big_endian(type const value, span<byte, sizeof(type)> const bytes) noexcept {
    // Note: GCC/Clang can usually recognise this pattern and compile this down
    // to a single store, optionally with the target architecture's byte-swap
    // instruction if byte-swapping is required.
    // However, when we know for sure that we're not being called as part of
    // a constant expression we dispatch to a memcpy/byte_swap-based
    // implementation above which the compiler is generally more reliable at
    // optimising.

    bytes[byte_7] = to_byte(value);
    bytes[byte_6] = to_byte(value >> bits_8);
    bytes[byte_5] = to_byte(value >> bits_16);
    bytes[byte_4] = to_byte(value >> bits_24);
    bytes[byte_3] = to_byte(value >> bits_32);
    bytes[byte_2] = to_byte(value >> bits_40);
    bytes[byte_1] = to_byte(value >> bits_48);
    bytes[byte_0] = to_byte(value >> bits_56);
  }
};

}  // namespace endian_detail

// parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: Documented on predeclaration"
// parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: Documented on predeclaration"

template <typename T, constraints<std::enable_if_t<std::is_integral<T>::value>>>
constexpr auto read_little_endian(span<byte const, sizeof(T)> const bytes) noexcept -> T {
  return static_cast<T>(endian_detail::endian_impl<sizeof(T)>::read_little_endian(bytes));
}

// parasoft-begin-suppress AUTOSAR-A7_2_1-a-2 "Value must be checked after reading, this is generic code"
// parasoft-begin-suppress CERT_CPP-INT50-a-3 "Value must be checked after reading, this is generic code"
template <typename T, constraints<std::enable_if_t<std::is_enum<T>::value>>>
constexpr auto read_little_endian(span<byte const, sizeof(T)> const bytes) noexcept -> T {
  return static_cast<T>(::arene::base::read_little_endian<typename std::underlying_type<T>::type>(bytes));
}
// parasoft-end-suppress AUTOSAR-A7_2_1-a-2
// parasoft-end-suppress CERT_CPP-INT50-a-3

template <typename T, constraints<std::enable_if_t<std::is_integral<T>::value>>>
constexpr void write_little_endian(base::type_identity_t<T> value, span<byte, sizeof(T)> const bytes) noexcept {
  endian_detail::endian_impl<sizeof(T)>::write_little_endian(
      static_cast<typename endian_detail::endian_impl<sizeof(T)>::type>(value),
      bytes
  );
}

template <typename T, constraints<std::enable_if_t<std::is_enum<T>::value>>>
constexpr void write_little_endian(base::type_identity_t<T> value, span<byte, sizeof(T)> const bytes) noexcept {
  ::arene::base::write_little_endian<typename std::underlying_type<T>::type>(
      static_cast<typename std::underlying_type<T>::type>(value),
      bytes
  );
}

template <typename T, constraints<std::enable_if_t<std::is_integral<T>::value>>>
constexpr auto read_big_endian(span<byte const, sizeof(T)> const bytes) noexcept -> T {
  return static_cast<T>(endian_detail::endian_impl<sizeof(T)>::read_big_endian(bytes));
}

// parasoft-begin-suppress AUTOSAR-A7_2_1-a-2 "Value must be checked after reading, this is generic code"
// parasoft-begin-suppress CERT_CPP-INT50-a-3 "Value must be checked after reading, this is generic code"
template <typename T, constraints<std::enable_if_t<std::is_enum<T>::value>>>
constexpr auto read_big_endian(span<byte const, sizeof(T)> const bytes) noexcept -> T {
  return static_cast<T>(::arene::base::read_big_endian<typename std::underlying_type<T>::type>(bytes));
}
// parasoft-end-suppress AUTOSAR-A7_2_1-a-2
// parasoft-end-suppress CERT_CPP-INT50-a-3

template <typename T, constraints<std::enable_if_t<std::is_integral<T>::value>>>
constexpr void write_big_endian(base::type_identity_t<T> value, span<byte, sizeof(T)> const bytes) noexcept {
  endian_detail::endian_impl<sizeof(T)>::write_big_endian(
      static_cast<typename endian_detail::endian_impl<sizeof(T)>::type>(value),
      bytes
  );
}

template <typename T, constraints<std::enable_if_t<std::is_enum<T>::value>>>
constexpr void write_big_endian(base::type_identity_t<T> value, span<byte, sizeof(T)> const bytes) noexcept {
  ::arene::base::write_big_endian<typename std::underlying_type<T>::type>(
      static_cast<typename std::underlying_type<T>::type>(value),
      bytes
  );
}

template <
    typename T,
    constraints<
        std::enable_if_t<std::is_floating_point<T>::value>,
        std::enable_if_t<!std::is_same<T, long double>::value>>>
void write_big_endian(base::type_identity_t<T> value, span<byte, sizeof(T)> const bytes) noexcept {
  static_assert((endian::native == endian::big) || (endian::native == endian::little), "Must have fixed endian");
  auto const raw_bytes = as_bytes(span<T, 1U>(&value, 1U));
  switch (endian::native) {
    case endian::little:
      std::ignore = ::arene::base::copy(raw_bytes.rbegin(), raw_bytes.rend(), bytes.begin());
      break;
    case endian::big:
      std::ignore = ::arene::base::copy(raw_bytes.begin(), raw_bytes.end(), bytes.begin());
      break;
    default:
      break;
  }
}

template <
    typename T,
    constraints<
        std::enable_if_t<std::is_floating_point<T>::value>,
        std::enable_if_t<!std::is_same<T, long double>::value>>>
void write_little_endian(base::type_identity_t<T> value, span<byte, sizeof(T)> const bytes) noexcept {
  static_assert((endian::native == endian::big) || (endian::native == endian::little), "Must have fixed endian");
  auto const raw_bytes = ::arene::base::as_bytes(span<T, 1U>(&value, 1U));
  switch (endian::native) {
    case endian::little:
      std::ignore = ::arene::base::copy(raw_bytes.begin(), raw_bytes.end(), bytes.begin());
      break;
    case endian::big:
      std::ignore = ::arene::base::copy(raw_bytes.rbegin(), raw_bytes.rend(), bytes.begin());
      break;
    default:
      break;
  }
}

template <
    typename T,
    constraints<
        std::enable_if_t<std::is_floating_point<T>::value>,
        std::enable_if_t<!std::is_same<T, long double>::value>>>
auto read_big_endian(span<byte const, sizeof(T)> const bytes) noexcept -> T {
  static_assert((endian::native == endian::big) || (endian::native == endian::little), "Must have fixed endian");
  T res{};
  auto raw_bytes = ::arene::base::as_writable_bytes(span<T, 1U>(&res, 1U));
  switch (endian::native) {
    case endian::little:
      std::ignore = ::arene::base::copy(bytes.rbegin(), bytes.rend(), raw_bytes.begin());
      break;
    case endian::big:
      std::ignore = ::arene::base::copy(bytes.begin(), bytes.end(), raw_bytes.begin());
      break;
    default:
      break;
  }
  return res;
}

template <
    typename T,
    constraints<
        std::enable_if_t<std::is_floating_point<T>::value>,
        std::enable_if_t<!std::is_same<T, long double>::value>>>
auto read_little_endian(span<byte const, sizeof(T)> const bytes) noexcept -> T {
  static_assert((endian::native == endian::big) || (endian::native == endian::little), "Must have fixed endian");
  T res{};
  auto raw_bytes = ::arene::base::as_writable_bytes(span<T, 1U>(&res, 1U));
  switch (endian::native) {
    case endian::little:
      std::ignore = ::arene::base::copy(bytes.begin(), bytes.end(), raw_bytes.begin());
      break;
    case endian::big:
      std::ignore = ::arene::base::copy(bytes.rbegin(), bytes.rend(), raw_bytes.begin());
      break;
    default:
      break;
  }
  return res;
}

// parasoft-end-suppress AUTOSAR-A0_4_2-a-2

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_ENDIAN_HPP_
