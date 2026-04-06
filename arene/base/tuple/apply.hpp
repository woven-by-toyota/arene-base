// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::apply"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_APPLY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_APPLY_HPP_

// IWYU pragma: private, include "arene/base/tuple.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/functional/invoke.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/decay.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/tuple/detail/get.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {

namespace apply_detail {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: parameter 'tuple' is used"
/// @brief Helper function to call provided invocable with arguments from provided tuple.
/// @tparam F Provided callable type (with arguments of @c Tuple)
/// @tparam Tuple type implementing the tuple protocol
/// @tparam Is The parameter pack of indices.
/// @param callable A callable object.
/// @param tuple reference to a tuple-ish
/// @return Result of applying the callable to argument from tuple like a function call f(args...)
template <typename F, typename Tuple, std::size_t... Is>
constexpr auto apply_impl(F&& callable, Tuple&& tuple, std::index_sequence<Is...>) noexcept(
    noexcept(arene::base::invoke(std::forward<F>(callable), tuple_detail::get<Is>(std::forward<Tuple>(tuple))...))
) -> decltype(auto) {
  return arene::base::invoke(std::forward<F>(callable), tuple_detail::get<Is>(std::forward<Tuple>(tuple))...);
}
// parasoft-end-suppress AUTOSAR-A0_1_4-a
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief Function object class providing the implementation of @c apply
class do_apply {
 public:
  /// @brief Calls provided invocable with arguments from provided tuple.
  /// @tparam F Provided callable type (with arguments of @c Tuple)
  /// @tparam Tuple type implementing the tuple protocol
  /// @param callable A callable object.
  /// @param tuple A tuple-ish object.
  /// @return Result of applying the callable to argument from tuple like a function call f(args...)
  template <typename F, typename Tuple>
  constexpr auto operator()(F&& callable, Tuple&& tuple) const noexcept(noexcept(apply_impl(
      std::forward<F>(callable),
      std::forward<Tuple>(tuple),
      std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{}
  ))) -> decltype(auto) {
    return apply_impl(
        std::forward<F>(callable),
        std::forward<Tuple>(tuple),
        std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{}
    );
  }
};

}  // namespace apply_detail

/// @def arene::base::apply
/// @brief Calls provided invocable with arguments from provided tuple.
/// @tparam F Provided callable type (with arguments of @c Tuple)
/// @tparam Tuple type implementing the tuple protocol.
/// @param callable A callable object.
/// @param tuple reference to a tuple-ish
/// @return Result of applying the callable with arguments specified by @c tuple
///
/// @note A type implements the tuple protocol if it
///   * provides a specialization of @c std::tuple_size
///   * provides a specialization of @c std::tuple_element
///   * provides a function to get an element by index @c I, which is specified
///     as:
///     ** <c> std::forward<Tuple>(tuple).get<I>()  </c> if valid
///     ** otherwise, <c> get<I>(std::forward<Tuple>(tuple))  </c> if valid,
///        where @c get is looked up by ADL only
/// Note that the tuple-like concept defines types that satisfy the tuple
/// protocol but are restricted to a list of types defined in the @c std
/// namespace (@c std::array, @c std::pair, @c std::tuple in C++14). The term
/// tuple-ish is used to define types that satisfy the tuple protocol without
/// the restriction that the type is define in the @c std namespace -- which
/// includes user-defined types.
///
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(apply_detail::do_apply, apply);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_APPLY_HPP_
