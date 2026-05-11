// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::mdspan_detail::is_nonnegative"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_NONNEGATIVE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_NONNEGATIVE_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief function object implementing is_nonnegative
/// @note This is not yet defined in arene/base/math as it is unclear what
///   concept @c Numeric must satisfy
class is_nonnegative_fn  //
{
 public:
  /// @brief determine if a value is non-negative
  /// @tparam Numeric numeric type
  /// @param value value to check
  /// @return @c true if @c value is non-negative, otherwise @c false
  /// @note currently assumes the zero value of @c Numeric can be spelled as
  ///   @c Numeric{}
  ///
  template <class Numeric>
  constexpr auto operator()(Numeric const& value) const                          //
      noexcept(                                                                  //
          std::is_nothrow_default_constructible<Numeric>::value&&                //
          noexcept(std::declval<Numeric const&>() >= std::declval<Numeric&&>())  //
      ) -> bool                                                                  //
  {
    // NOTE: a type may be usable with this function without having a partial
    // order (e.g. complex numbers)
    return value >= Numeric{};
  }
};

/// @def arene::base::mdspan_detail::is_nonnegative
/// @copydoc arene::base::mdspan_detail::is_nonnegative_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(mdspan_detail::is_nonnegative_fn, is_nonnegative);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_NONNEGATIVE_HPP_
