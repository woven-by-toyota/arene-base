// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines
// arene::base::tuple_detail::tuple_transform_reduce"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_TUPLE_TRANSFORM_REDUCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_TUPLE_TRANSFORM_REDUCE_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/forward_as_tuple.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/tuple/detail/get.hpp"
#include "arene/base/tuple/detail/tuple_reduce.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace tuple_detail {

/// @brief function object implementing the transform reduce algorithm for tuples
class tuple_transform_reduce_fn  //
{
  /// @brief apply a binary transform, then reduce, on two tuple-like types
  /// @tparam Is index sequence
  /// @tparam Tuple1 first tuple type
  /// @tparam Tuple2 second tuple type
  /// @tparam T init type
  /// @tparam Reduce reduce type
  /// @tparam Transform transform type
  /// @param tuple1 first tuple
  /// @param tuple2 second tuple
  /// @param init initial value
  /// @param reduce reduce operation
  /// @param transform transform operation
  /// @return value after pair-wise transforming elements in @c tuple1 with
  ///   @c tuple2, then reducing
  template <std::size_t... Is, class Tuple1, class Tuple2, class T, class Reduce, class Transform>
  static constexpr auto impl(
      std::index_sequence<Is...>,
      Tuple1&& tuple1,
      Tuple2&& tuple2,
      T&& init,
      Reduce&& reduce,
      Transform& transform
  ) noexcept(false) -> T {
    return tuple_reduce(
        std::forward_as_tuple(transform(  //
            tuple_detail::get<Is>(std::forward<Tuple1>(tuple1)),
            tuple_detail::get<Is>(std::forward<Tuple2>(tuple2))
        )...),
        // parasoft-begin-suppress AUTOSAR-A18_9_2-a "'init' and 'reduce' are
        // always rvalue references by construction in this implementation
        // detail function"
        // NOLINTBEGIN(bugprone-move-forwarding-reference)
        std::move(init),
        std::move(reduce)
        // NOLINTEND(bugprone-move-forwarding-reference)
        // parasoft-end-suppress AUTOSAR-A18_9_2-a
    );
  }

 public:
  /// @brief apply a binary transform, then reduce, on two tuple-like types
  /// @tparam Tuple1 first tuple type
  /// @tparam Tuple2 second tuple type
  /// @tparam T init type
  /// @tparam Reduce reduce type
  /// @tparam Transform transform type
  /// @param tuple1 first tuple
  /// @param tuple2 second tuple
  /// @param init initial value
  /// @param reduce reduce operation
  /// @param transform transform operation
  /// @return value after pair-wise transforming elements in @c tuple1 with
  ///   @c tuple2, then reducing
  template <class Tuple1, class Tuple2, class T, class Reduce, class Transform>
  constexpr auto operator()(Tuple1&& tuple1, Tuple2&& tuple2, T init, Reduce reduce, Transform transform) const
      noexcept(false) -> T {
    static_assert(
        std::tuple_size<std::remove_reference_t<Tuple1>>::value ==
            std::tuple_size<std::remove_reference_t<Tuple2>>::value,
        "'Tuple1' and 'Tuple2' must have the same size."
    );

    return impl(
        std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple1>>::value>{},
        std::forward<Tuple1>(tuple1),
        std::forward<Tuple2>(tuple2),
        std::move(init),
        std::move(reduce),
        transform
    );
  }
};

/// @def arene::base::tuple_detail::tuple_transform_reduce
/// @copydoc arene::base::tuple_detail::tuple_transform_reduce_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(tuple_detail::tuple_transform_reduce_fn, tuple_transform_reduce);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace tuple_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_TUPLE_TRANSFORM_REDUCE_HPP_
