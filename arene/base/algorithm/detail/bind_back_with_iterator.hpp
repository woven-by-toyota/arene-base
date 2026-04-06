// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_BIND_BACK_WITH_ITERATOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_BIND_BACK_WITH_ITERATOR_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/algorithm/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/functional/invoke.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/iterator_traits.hpp"

namespace arene {
namespace base {
namespace algorithm_detail {

/// @brief helper function object for algorithms
/// @tparam F function to partially apply to
/// @tparam I indirect argument to bind
///
/// Adapts a binary function object to allow an algorithm to invoke *other*
/// algorithms. The bound argument is captured by the iterator instead of the
/// dereferenced value.
///
/// @note The lifetime of the returned value must be bound by lifetime of
///   @c func.
///
template <class F, class I>
class bind_back_with_iterator {
  /// @brief function to partially apply an argument to
  F& func_;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

  /// @brief bound argument
  I iter_;

 public:
  // parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"
  /// @brief construct the partially applied closure
  /// @param func the function to partially apply
  /// @param iter iterator argument to bind back
  constexpr bind_back_with_iterator(F& func, I const& iter)  //
      noexcept(std::is_nothrow_copy_constructible<I>::value)
      : func_{func},
        iter_{iter} {}
  // parasoft-end-suppress AUTOSAR-M2_10_1-a

  /// @brief complete the function invocation
  /// @param value first argument
  /// @return return value of <c> func(value, *iter) </c>
  constexpr auto operator()(iter_reference_t<I> value) const                          //
      noexcept(is_nothrow_invocable_v<F&, iter_reference_t<I>, iter_reference_t<I>>)  //
      -> invoke_result_t<F&, iter_reference_t<I>, iter_reference_t<I>>                //
  {
    // value comes from deferencing an iterator so propagate the reference
    // qualification correctly
    return arene::base::invoke(func_, static_cast<iter_reference_t<I>>(value), *iter_);
  }
};

}  // namespace algorithm_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_BIND_BACK_WITH_ITERATOR_HPP_
