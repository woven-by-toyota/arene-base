// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::tuple_detail::tuple_reduce"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_TUPLE_REDUCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_TUPLE_REDUCE_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/tuple/detail/get.hpp"
#include "arene/base/type_manipulation/consume_values.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"
// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {
namespace tuple_detail {

/// @brief function object implementing the reduce algorithm for tuples
class tuple_reduce_fn  //
{
  /// @brief apply a reduce on elements of a tuple
  /// @tparam Is index sequence
  /// @tparam Tuple tuple type
  /// @tparam T init type
  /// @tparam Reduce reduce type
  /// @param tuple tuple value
  /// @param init initial value
  /// @param reduce reduce operation
  /// @return value reducing elements in @c tuple with @ reduce
  template <std::size_t... Is, class Tuple, class T, class Reduce>
  static constexpr auto impl(std::index_sequence<Is...>, Tuple&& tuple, T&& init, Reduce& reduce) noexcept(false)
      -> decltype((
          consume_values({(  //
              std::declval<T&>() =
                  std::declval<Reduce&>()(std::declval<T&&>(), tuple_detail::get<Is>(std::declval<Tuple&&>())),
              true
          )...}),
          T{std::declval<T&&>()}
      )) {
    // parasoft-begin-suppress AUTOSAR-M5_18_1-a "Comma operator avoids an expensive recursive iteration"
    // parasoft-begin-suppress AUTOSAR-A18_9_2-a AUTOSAR-A8_4_6-a AUTOSAR-A8_4_5_a AUTOSAR-A12_8_4_a
    // "'init' is always an rvalue reference by construction in this implementation detail function"
    // NOLINTBEGIN(bugprone-move-forwarding-reference)
    consume_values({(  //
        init = reduce(std::move(init), tuple_detail::get<Is>(std::forward<Tuple>(tuple))),
        true
    )...});
    // NOLINTEND(bugprone-move-forwarding-reference)
    // parasoft-end-suppress AUTOSAR-A18_9_2-a AUTOSAR-A8_4_6-a AUTOSAR-A8_4_5_a AUTOSAR-A12_8_4_a
    // parasoft-end-suppress AUTOSAR-M5_18_1-a

    return init;
  }

 public:
  /// @brief apply a reduce on elements of a tuple
  /// @tparam Tuple tuple type
  /// @tparam T init type
  /// @tparam Reduce reduce type
  /// @param tuple tuple value
  /// @param init initial value
  /// @param reduce reduce operation
  /// @return value reducing elements in @c tuple with @ reduce
  template <class Tuple, class T, class Reduce>
  constexpr auto operator()(Tuple&& tuple, T init, Reduce reduce) const noexcept(false) -> decltype(impl(
      std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{},
      std::declval<Tuple&&>(),
      std::declval<T&&>(),
      std::declval<Reduce&>()
  )) {
    return impl(
        std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{},
        std::forward<Tuple>(tuple),
        std::move(init),
        reduce
    );
  }
};

/// @def arene::base::tuple_detail::tuple_reduce
/// @copydoc arene::base::tuple_detail::tuple_reduce_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(tuple_detail::tuple_reduce_fn, tuple_reduce);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace tuple_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_TUPLE_REDUCE_HPP_
