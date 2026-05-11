// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines std::inner_product"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_BIT_BIT_CAST_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_BIT_BIT_CAST_HPP_

// IWYU pragma: private, include "arene/base/bit.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_trivially_copyable.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// NOLINTBEGIN(cppcoreguidelines-macro-usage) Selecting between constexpr and runtime implementations
// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

#if ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)
#define ARENE_BIT_CAST_CONSTEXPR constexpr
#include "arene/base/bit/detail/bit_cast_builtin.hpp"  // IWYU pragma: export
#else
#define ARENE_BIT_CAST_CONSTEXPR
#include "arene/base/bit/detail/bit_cast_memcpy.hpp"  // IWYU pragma: export
#endif

// NOLINTEND(cppcoreguidelines-macro-usage)
// parasoft-end-suppress AUTOSAR-A16_0_1-d-2
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

namespace arene {
namespace base {
namespace bit_cast_detail {

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"

/// @brief Function object implementing @c arene::base::bit_cast for a given destination type.
/// @tparam To type to reinterpret as
template <class To>
class do_bit_cast {
 public:
  /// @brief Reinterpret the object representation of @c From as that of @c To.
  /// @tparam From type to reinterpret from
  /// @param from the source of bits for the return value
  /// @return An object of type @c To whose value representation is copied from @c from.
  template <
      class From,
      constraints<
          std::enable_if_t<sizeof(From) == sizeof(To)>,
          std::enable_if_t<std::is_trivially_copyable<From>::value>,
          std::enable_if_t<std::is_trivially_copyable<To>::value && (sizeof(From*) != 0)>> = nullptr>
  ARENE_BIT_CAST_CONSTEXPR auto operator()(From const& from) const noexcept -> To {
    return ::arene::base::bit_cast_detail::bit_cast<To>(from);
  }
};

// parasoft-end-suppress AUTOSAR-M2_10_1-a

}  // namespace bit_cast_detail

/// @brief Reinterpret the object representation of one type as that of another
/// @tparam To type to reinterpret as
/// @tparam From type to reinterpret from
/// @param from the source of bits for the return value
///
/// Obtain a value of type @c To by reinterpreting the object representation of
/// @c From. Every bit in the value representation of the returned @c To object is
/// equal to the corresponding bit in the object representation of @c from. The
/// values of padding bits in the returned @c To object are unspecified.
///
/// If there is no value of type @c To corresponding to the value representation
/// produced, the behavior is undefined. If there are multiple such values,
/// which value is produced is unspecified.
///
/// A bit in the value representation of the result is indeterminate if it does
/// not correspond to a bit in the value representation of @c From (i.e. it
/// corresponds to a padding bit), or corresponds to a bit of an object that is
/// not within its lifetime, or has an indeterminate value. For each bit in the
/// value representation of the result that is indeterminate, the smallest
/// object containing that bit has an indeterminate value; the behavior is
/// undefined unless that object is of unsigned char type. The result does not
/// otherwise contain any indeterminate values.
///
/// @return An object of type @c To whose value representation is as described
/// above.
///
/// @note Requires:
/// * <c> sizeof(To) == sizeof(From) </c> is @c true
/// * <c> std::is_trivially_copyable<To>::value </c> is @c true
/// * <c> std::is_trivially_copyable<From>::value </c> is @c true
///
/// @note @c bit_cast is usable in a constant expression only when the compiler
/// provides @c __builtin_bit_cast . Query this via
/// @c ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST) . On toolchains without the
/// builtin (e.g. GCC 8), @c bit_cast compiles but cannot appear in a
/// @c constexpr context.
template <class To>
extern constexpr bit_cast_detail::do_bit_cast<To> bit_cast{};

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A7_1_5-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_BIT_BIT_CAST_HPP_
