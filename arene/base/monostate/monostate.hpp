// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MONOSTATE_MONOSTATE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MONOSTATE_MONOSTATE_HPP_

// IWYU pragma: private, include "arene/base/monostate.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/hash.hpp"
#include "arene/base/stdlib_choice/is_void.hpp"
#include "arene/base/type_manipulation/static_if.hpp"
#include "arene/base/type_traits/give_cv_to.hpp"

// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {

/// @brief A monostate type that is a well-behaved empty value for variants.
struct monostate {
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: comparison operators are permitted"
  /// @brief All monostates are equal.
  /// @return Always returns @c true
  ARENE_NODISCARD friend constexpr auto operator==(monostate, monostate) noexcept -> bool { return true; }

  /// @brief All monostates are not not equal.
  /// @return Always returns @c false
  ARENE_NODISCARD friend constexpr auto operator!=(monostate, monostate) noexcept -> bool { return false; }

  /// @brief A monostate is never less than another monostate.
  /// @return Always returns @c false
  ARENE_NODISCARD friend constexpr auto operator<(monostate, monostate) noexcept -> bool { return false; }

  /// @brief A monostate is always less-equal than another monostate as all are equal.
  /// @return Always returns @c true
  ARENE_NODISCARD friend constexpr auto operator<=(monostate, monostate) noexcept -> bool { return true; }

  /// @brief A monostate is never greater than another monostate
  /// @return Always returns @c false
  ARENE_NODISCARD friend constexpr auto operator>(monostate, monostate) noexcept -> bool { return false; }

  /// @brief A monostate is always greater-equal than another monostate as all are equal
  /// @return Always returns @c true
  ARENE_NODISCARD friend constexpr auto operator>=(monostate, monostate) noexcept -> bool { return true; }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
};

/// @brief A metafunction that is an identity for all types except for void types.
/// @details In the case of a void type, this results in @c arene::base::monostate with the same cv-qualifiers.
/// @tparam T The parameter type.
template <class T>
using monostate_identity =
    typename static_if<std::is_void<T>::value>::template then_apply_else<give_cv_to_t, T, T, monostate>;

}  // namespace base
}  // namespace arene

// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "False positive: specialization of standard templates is permitted"
// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "False positive: specialization of standard templates is permitted"
namespace std {
// parasoft-begin-suppress AUTOSAR-A11_0_2-a-2 "False positive: must follow the primary template"
/// @brief Implementation of std::hash for arene::base::monostate.
template <>
struct hash<arene::base::monostate> {
  /// @brief The result type.
  using result_type = std::size_t;
  /// @brief The argument type.
  using argument_type = arene::base::monostate;

  /// @brief Compute the hash of a monostate instance.
  /// @return The hash of a monostate instance.
  constexpr auto operator()(arene::base::monostate const&) const noexcept -> std::size_t {
    constexpr std::size_t monostate_hash{0xEF0B0666};  // an arbitrary constant
    return monostate_hash;
  }
};
// parasoft-end-suppress AUTOSAR-A11_0_2-a-2
}  // namespace std
// parasoft-end-suppress CERT_CPP-DCL58-a-2
// parasoft-end-suppress AUTOSAR-A17_6_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MONOSTATE_MONOSTATE_HPP_
