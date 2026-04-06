// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_CONVERT_TO_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_CONVERT_TO_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"

namespace arene {
namespace base {
namespace algorithm_detail {

/// @brief function object that converts from one type to another
/// @tparam To type to convert to
template <class To>
class convert_to  //
{
 public:
  /// @brief convert from one type to another
  /// @tparam From type to convert from
  /// @param value value to convert
  /// @return <c> static_cast<To>(std::forward<From>(value)) </c>
  ///
  template <  //
      class From,
      constraints<std::enable_if_t<std::is_convertible<From&&, To>::value>> = nullptr>
  constexpr auto operator()(From&& value) const noexcept(  //
      noexcept(static_cast<To>(std::declval<From>()))      //
  )                                                        //
      -> To                                                //
  {
    return static_cast<To>(std::forward<From>(value));
  }
};

}  // namespace algorithm_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_CONVERT_TO_HPP_
