// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_IMPLICIT_CONSTRUCTOR_BASE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_IMPLICIT_CONSTRUCTOR_BASE_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/stdlib_choice/is_constructible.hpp"

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-A12_0_1-a-2 "implicit_constructor_base has no content so doesn't need assignment
// operators"
// parasoft-begin-suppress AUTOSAR-A1_1_1-b-2 "False positive: This is all conforming code"

namespace arene {
namespace base {

/// @brief A class that has the same implicit constructors as another type.
///
/// It is intended for use as a base class for other types that define the
/// constructors with @c =default, so they remain trivial when present,
/// but are deleted when @c T does not provide them.
/// @tparam T The type to copy the properties from
template <
    typename T,
    bool = std::is_constructible<T, T const&>::value,
    bool = std::is_constructible<T, T&>::value,
    bool = std::is_constructible<T, T const&&>::value,
    bool = std::is_constructible<T, T&&>::value>
class implicit_constructor_base {};

// @cond INTERNAL
/// @brief A class that has the same implicit constructors as another type.
///
/// It is intended for use as a base class for other types that define the
/// constructors with @c =default, so they remain trivial when present,
/// but are deleted when @c T does not provide them.
/// @tparam T The type to copy the properties from
template <typename T>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class implicit_constructor_base<T, false, false, false, false> {
 protected:
  /// @brief Defaulted constructor
  implicit_constructor_base() noexcept = default;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base const&) = delete;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base&) = delete;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base const&&) = delete;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base&&) = delete;
};

/// @brief A class that has the same implicit constructors as another type.
///
/// It is intended for use as a base class for other types that define the
/// constructors with @c =default, so they remain trivial when present,
/// but are deleted when @c T does not provide them.
/// @tparam T The type to copy the properties from
template <typename T>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class implicit_constructor_base<T, true, false, false, false> {
 protected:
  /// @brief Defaulted constructor
  implicit_constructor_base() noexcept = default;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base const&) noexcept = default;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base&) = delete;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base const&&) = delete;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base&&) = delete;
};

/// @brief A class that has the same implicit constructors as another type.
///
/// It is intended for use as a base class for other types that define the
/// constructors with @c =default, so they remain trivial when present,
/// but are deleted when @c T does not provide them.
/// @tparam T The type to copy the properties from
template <typename T>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class implicit_constructor_base<T, false, true, false, false> {
 protected:
  /// @brief Defaulted constructor
  implicit_constructor_base() noexcept = default;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base const&) = delete;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base&) noexcept = default;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base const&&) = delete;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base&&) = delete;
};

/// @brief A class that has the same implicit constructors as another type.
///
/// It is intended for use as a base class for other types that define the
/// constructors with @c =default, so they remain trivial when present,
/// but are deleted when @c T does not provide them.
/// @tparam T The type to copy the properties from
template <typename T>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class implicit_constructor_base<T, true, true, false, false> {
 protected:
  /// @brief Defaulted constructor
  implicit_constructor_base() noexcept = default;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base const&) noexcept = default;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base&) noexcept = default;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base const&&) = delete;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base&&) = delete;
};

/// @brief A class that has the same implicit constructors as another type.
///
/// It is intended for use as a base class for other types that define the
/// constructors with @c =default, so they remain trivial when present,
/// but are deleted when @c T does not provide them.
/// @tparam T The type to copy the properties from
template <typename T>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class implicit_constructor_base<T, false, false, true, false> {
 protected:
  /// @brief Defaulted constructor
  implicit_constructor_base() noexcept = default;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base const&) = delete;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base&) = delete;
  /// @brief Do-nothing constructor
  implicit_constructor_base(implicit_constructor_base const&&) noexcept {}
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base&&) = delete;
};

/// @brief A class that has the same implicit constructors as another type.
///
/// It is intended for use as a base class for other types that define the
/// constructors with @c =default, so they remain trivial when present,
/// but are deleted when @c T does not provide them.
/// @tparam T The type to copy the properties from
template <typename T>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class implicit_constructor_base<T, true, false, true, false> {
 protected:
  /// @brief Defaulted constructor
  implicit_constructor_base() noexcept = default;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base const&) noexcept = default;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base&) = delete;
  /// @brief Do-nothing constructor
  implicit_constructor_base(implicit_constructor_base const&&) noexcept {}
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base&&) = delete;
};

/// @brief A class that has the same implicit constructors as another type.
///
/// It is intended for use as a base class for other types that define the
/// constructors with @c =default, so they remain trivial when present,
/// but are deleted when @c T does not provide them.
/// @tparam T The type to copy the properties from
template <typename T>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class implicit_constructor_base<T, false, true, true, false> {
 protected:
  /// @brief Defaulted constructor
  implicit_constructor_base() noexcept = default;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base const&) = delete;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base&) noexcept = default;
  /// @brief do-nothing constructor
  implicit_constructor_base(implicit_constructor_base const&&) noexcept {}
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base&&) = delete;
};

/// @brief A class that has the same implicit constructors as another type.
///
/// It is intended for use as a base class for other types that define the
/// constructors with @c =default, so they remain trivial when present,
/// but are deleted when @c T does not provide them.
/// @tparam T The type to copy the properties from
template <typename T>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class implicit_constructor_base<T, true, true, true, false> {
 protected:
  /// @brief Defaulted constructor
  implicit_constructor_base() noexcept = default;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base const&) noexcept = default;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base&) noexcept = default;
  /// @brief do-nothing constructor
  implicit_constructor_base(implicit_constructor_base const&&) noexcept {}
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base&&) = delete;
};

/// @brief A class that has the same implicit constructors as another type.
///
/// It is intended for use as a base class for other types that define the
/// constructors with @c =default, so they remain trivial when present,
/// but are deleted when @c T does not provide them.
/// @tparam T The type to copy the properties from
template <typename T>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class implicit_constructor_base<T, false, false, false, true> {
 protected:
  /// @brief Defaulted constructor
  implicit_constructor_base() noexcept = default;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base const&) = delete;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base&) = delete;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base const&&) = delete;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base&&) noexcept = default;
};

/// @brief A class that has the same implicit constructors as another type.
///
/// It is intended for use as a base class for other types that define the
/// constructors with @c =default, so they remain trivial when present,
/// but are deleted when @c T does not provide them.
/// @tparam T The type to copy the properties from
template <typename T>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class implicit_constructor_base<T, true, false, false, true> {
 protected:
  /// @brief Defaulted constructor
  implicit_constructor_base() noexcept = default;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base const&) noexcept = default;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base&) = delete;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base const&&) = delete;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base&&) noexcept = default;
};

/// @brief A class that has the same implicit constructors as another type.
///
/// It is intended for use as a base class for other types that define the
/// constructors with @c =default, so they remain trivial when present,
/// but are deleted when @c T does not provide them.
/// @tparam T The type to copy the properties from
template <typename T>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class implicit_constructor_base<T, false, true, false, true> {
 protected:
  /// @brief Defaulted constructor
  implicit_constructor_base() noexcept = default;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base const&) = delete;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base&) noexcept = default;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base const&&) = delete;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base&&) noexcept = default;
};

/// @brief A class that has the same implicit constructors as another type.
///
/// It is intended for use as a base class for other types that define the
/// constructors with @c =default, so they remain trivial when present,
/// but are deleted when @c T does not provide them.
/// @tparam T The type to copy the properties from
template <typename T>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class implicit_constructor_base<T, true, true, false, true> {
 protected:
  /// @brief Defaulted constructor
  implicit_constructor_base() noexcept = default;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base const&) noexcept = default;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base&) noexcept = default;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base const&&) = delete;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base&&) noexcept = default;
};

/// @brief A class that has the same implicit constructors as another type.
///
/// It is intended for use as a base class for other types that define the
/// constructors with @c =default, so they remain trivial when present,
/// but are deleted when @c T does not provide them.
/// @tparam T The type to copy the properties from
template <typename T>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class implicit_constructor_base<T, false, false, true, true> {
 protected:
  /// @brief Defaulted constructor
  implicit_constructor_base() noexcept = default;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base const&) = delete;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base&) = delete;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base&&) noexcept = default;
};

/// @brief A class that has the same implicit constructors as another type.
///
/// It is intended for use as a base class for other types that define the
/// constructors with @c =default, so they remain trivial when present,
/// but are deleted when @c T does not provide them.
/// @tparam T The type to copy the properties from
template <typename T>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class implicit_constructor_base<T, true, false, true, true> {
 protected:
  /// @brief Defaulted constructor
  implicit_constructor_base() noexcept = default;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base const&) noexcept = default;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base&) = delete;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base&&) noexcept = default;
};

/// @brief A class that has the same implicit constructors as another type.
///
/// It is intended for use as a base class for other types that define the
/// constructors with @c =default, so they remain trivial when present,
/// but are deleted when @c T does not provide them.
/// @tparam T The type to copy the properties from
template <typename T>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class implicit_constructor_base<T, false, true, true, true> {
 protected:
  /// @brief Defaulted constructor
  implicit_constructor_base() noexcept = default;
  /// @brief Deleted constructor
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  implicit_constructor_base(implicit_constructor_base const&) = delete;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base&) noexcept = default;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base&&) noexcept = default;
};

/// @brief A class that has the same implicit constructors as another type.
///
/// It is intended for use as a base class for other types that define the
/// constructors with @c =default, so they remain trivial when present,
/// but are deleted when @c T does not provide them.
/// @tparam T The type to copy the properties from
template <typename T>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class implicit_constructor_base<T, true, true, true, true> {
 protected:
  /// @brief Defaulted constructor
  implicit_constructor_base() noexcept = default;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base const&) noexcept = default;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base&) noexcept = default;
  /// @brief Defaulted constructor
  implicit_constructor_base(implicit_constructor_base&&) noexcept = default;
};
// @endcond

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_IMPLICIT_CONSTRUCTOR_BASE_HPP_
