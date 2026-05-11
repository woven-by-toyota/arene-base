// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines
// arene::base::mdspan_detail::representable_cast"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_REPRESENTABLE_CAST_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_REPRESENTABLE_CAST_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/mdspan/detail/is_representable_as.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief function object implementing representable_cast
/// @tparam To integral type to convert to
template <class To>
class representable_cast_fn  //
{
 public:
  static_assert(  //
      std::is_integral<To>::value,
      "this function is only designed to convert to integral types."
  );

  /// @brief convert a value to an integral type
  /// @tparam From type of the value to convert from
  /// @param value value to convert from
  /// @pre @c value can be represented in @c To
  ///
  /// Converts @c value to type @c To, checking that @c value is representable in
  /// @c To if possible with @c is_representable_as. Equivalent to <c>
  /// static_cast<To>(value) </c>.
  ///
  /// @return @c value converted to type @c To
  ///
  template <
      class From,
      constraints<std::enable_if_t<is_invocable_v<decltype(is_representable_as<To>), From const&>>> = nullptr>
  constexpr auto operator()(From const& value) const                    //
      noexcept(noexcept(static_cast<To>(std::declval<From const&>())))  //
      -> To                                                             //
  {
    ARENE_PRECONDITION(is_representable_as<To>(value));
    return static_cast<To>(value);
  }
};

/// @def arene::base::mdspan_detail::representable_cast
/// @tparam To integral type to convert to
/// @copydoc arene::base::mdspan_detail::representable_cast_fn::operator()
template <class To>
extern constexpr auto representable_cast = representable_cast_fn<To>{};

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_REPRESENTABLE_CAST_HPP_
