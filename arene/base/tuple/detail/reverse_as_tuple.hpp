// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_REVERSE_AS_TUPLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_REVERSE_AS_TUPLE_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/tuple/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/stdlib_choice/tuple_element.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/tuple/detail/get.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace tuple_detail {

namespace reverse_as_tuple_detail {

// parasoft-begin-suppress AUTOSAR-A2_10_1-a "False positive: tuple is not hiding any identifier"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: Parameter tuple *is* used"
/// @brief reverses the elements of a tuple-ish into a std::tuple
/// @tparam Tuple tuple-ish type
/// @tparam Is index pack
/// @param tuple input tuple-ish
/// @return a std::tuple with elements in reverse order
template <
    class Tuple,
    std::size_t... Is,
    class Bare = std::remove_cv_t<std::remove_reference_t<Tuple>>,
    class Result = std::tuple<std::tuple_element_t<sizeof...(Is) - 1U - Is, Bare>...>>
constexpr auto reverse_impl(Tuple&& tuple, std::index_sequence<Is...>) noexcept(noexcept(Result{
    tuple_detail::get<sizeof...(Is) - 1U - Is>(std::declval<Tuple&&>())...
})) -> Result {
  return Result{tuple_detail::get<sizeof...(Is) - 1U - Is>(std::forward<Tuple>(tuple))...};
}
// parasoft-end-suppress AUTOSAR-A0_1_4-a
// parasoft-end-suppress CERT_C-EXP37-a
// parasoft-end-suppress AUTOSAR-M2_10_1-a
// parasoft-end-suppress AUTOSAR-A2_10_1-a

}  // namespace reverse_as_tuple_detail

/// @brief function object implementing tuple reversal
class reverse_as_tuple_fn {
 public:
  // parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
  /// @brief reverses the elements of a tuple-ish, always returning a std::tuple
  /// @tparam Tuple tuple-ish type
  /// @param tuple input tuple-ish
  /// @return a std::tuple with elements in reverse order
  template <typename Tuple>
  constexpr auto operator()(Tuple&& tuple) const noexcept(noexcept(reverse_as_tuple_detail::reverse_impl(
      std::declval<Tuple&&>(),
      std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{}
  )))
      -> decltype(reverse_as_tuple_detail::reverse_impl(
          std::declval<Tuple&&>(),
          std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{}
      )) {
    return reverse_as_tuple_detail::reverse_impl(
        std::forward<Tuple>(tuple),
        std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{}
    );
  }
  // parasoft-end-suppress AUTOSAR-M2_10_1-a
};

/// @def arene::base::tuple_detail::reverse_as_tuple
/// @copydoc arene::base::tuple_detail::reverse_as_tuple_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(tuple_detail::reverse_as_tuple_fn, reverse_as_tuple);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace tuple_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_REVERSE_AS_TUPLE_HPP_
