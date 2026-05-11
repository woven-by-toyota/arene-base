// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines
// arene::base::tuple_detail::tuple_transform_reduce"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_TUPLE_TRANSFORM_REDUCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_TUPLE_TRANSFORM_REDUCE_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/functional/bind_front.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/forward_as_tuple.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/tuple/apply.hpp"
#include "arene/base/tuple/detail/get.hpp"
#include "arene/base/tuple/detail/tuple_reduce.hpp"
#include "arene/base/tuple/detail/tuple_zip.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"
// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {
namespace tuple_detail {

/// @brief function object implementing the transform reduce algorithm for tuples
class tuple_transform_reduce_fn  //
{
  /// @brief apply a unary transform, then reduce, on a single tuple-like type
  /// @tparam Is index sequence
  /// @tparam Tuple tuple type
  /// @tparam T init type
  /// @tparam Reduce reduce type
  /// @tparam Transform transform type
  /// @param tuple input tuple
  /// @param init initial value
  /// @param reduce reduce operation
  /// @param transform transform operation
  /// @return value after transforming each element in @c tuple with
  ///   @c transform, then reducing with @c reduce
  template <std::size_t... Is, class Tuple, class T, class Reduce, class Transform>
  static constexpr auto
  impl(std::index_sequence<Is...>, Tuple&& tuple, T&& init, Reduce&& reduce, Transform& transform) noexcept(  //
      noexcept(tuple_reduce(
          std::forward_as_tuple(std::declval<Transform&>()(tuple_detail::get<Is>(std::declval<Tuple&&>()))...),
          std::declval<T&&>(),
          std::declval<Reduce&&>()
      ))
  )
      -> decltype(tuple_reduce(
          std::forward_as_tuple(std::declval<Transform&>()(tuple_detail::get<Is>(std::declval<Tuple&&>()))...),
          std::declval<T&&>(),
          std::declval<Reduce&&>()
      )) {
    return tuple_reduce(
        std::forward_as_tuple(transform(tuple_detail::get<Is>(std::forward<Tuple>(tuple)))...),
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

  /// @brief implementation for n-ary tuple transform reduce
  /// @tparam Is index sequence corresponding to the input tuples, does *not*
  ///   include the init, reduce, or transform arguments
  /// @tparam Args parameter pack containing references of input tuples
  ///   followed by T&&, Reduce&&, and Transform&&
  /// @param args @c std::tuple of references of the input tuples, init,
  ///   reduce, and transform
  /// @return result from applying the adapted transform element-wise across
  ///   input tuples and then reducing
  ///
  /// The last three arguments in the bundle are interpreted as (init, reduce,
  /// transform). All preceding arguments are input tuples.
  template <std::size_t... Is, class... Args>
  static constexpr auto nary_impl(std::index_sequence<Is...>, std::tuple<Args...>&& args) noexcept(  //
      noexcept(std::declval<tuple_transform_reduce_fn>()(                                            //
          tuple_zip(std::get<Is>(std::declval<std::tuple<Args&&...>&&>())...),
          std::get<sizeof...(Is)>(std::declval<std::tuple<Args&&...>&&>()),
          std::get<sizeof...(Is) + 1U>(std::declval<std::tuple<Args&&...>&&>()),
          arene::base::bind_front(apply, std::get<sizeof...(Is) + 2U>(std::declval<std::tuple<Args&&...>&&>()))
      ))
  ) -> decltype(std::declval<tuple_transform_reduce_fn>()(  //
      tuple_zip(std::get<Is>(std::declval<std::tuple<Args&&...>&&>())...),
      std::get<sizeof...(Is)>(std::declval<std::tuple<Args&&...>&&>()),
      std::get<sizeof...(Is) + 1U>(std::declval<std::tuple<Args&&...>&&>()),
      arene::base::bind_front(apply, std::get<sizeof...(Is) + 2U>(std::declval<std::tuple<Args&&...>&&>()))
  )) {
    return tuple_transform_reduce_fn{}(  //
        tuple_zip(std::get<Is>(std::move(args))...),
        std::get<sizeof...(Is)>(std::move(args)),
        std::get<sizeof...(Is) + 1U>(std::move(args)),
        arene::base::bind_front(apply, std::get<sizeof...(Is) + 2U>(std::move(args)))
    );
  }

 public:
  /// @brief apply a unary transform, then reduce, on a tuple
  /// @tparam Tuple input tuple type
  /// @tparam T init type
  /// @tparam Reduce reduce type
  /// @tparam Transform transform type
  /// @param tuple input tuple
  /// @param init initial value
  /// @param reduce reduce operation
  /// @param transform transform operation
  /// @return value after transforming each element in @c tuple with
  ///   @c transform, then reducing with @c reduce
  template <class Tuple, class T, class Reduce, class Transform>
  constexpr auto operator()(Tuple&& tuple, T init, Reduce reduce, Transform transform) const noexcept(  //
      noexcept(impl(
          std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{},
          std::declval<Tuple&&>(),
          std::declval<T&&>(),
          std::declval<Reduce&&>(),
          std::declval<Transform&>()
      ))
  )
      -> decltype(impl(
          std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{},
          std::declval<Tuple&&>(),
          std::declval<T&&>(),
          std::declval<Reduce&&>(),
          std::declval<Transform&>()
      )) {
    return impl(
        std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{},
        std::forward<Tuple>(tuple),
        std::move(init),
        std::move(reduce),
        transform
    );
  }

  /// @brief apply an n-ary transform, then reduce, on n tuples
  /// @tparam TuplesInitReduceAndTransform input tuple types followed by the
  ///   init value type, the reduce type, and the transform type
  /// @param args input tuples followed by the init value, the reduce
  ///   operation, and the transform operation
  /// @return value after n-ary transforming corresponding elements across
  ///   input tuples, then reducing with the reduce operation
  ///
  /// @note Constraints <br>
  /// * <c> sizeof...(TuplesInitReduceAndTransform) > 4 </c> is @c true
  ///
  /// The last three arguments are interpreted as (init, reduce, transform).
  /// All preceding arguments are input tuples. The transform is accepted as
  /// a forwarding reference rather than by value; internally, the initial value and transform are decay-copied,
  /// which requires the types to be move or copy constructible. This may result
  /// in one additional move or copy construction compared to a standard
  /// algorithm that accepts the initial value and transform by value directly.
  template <
      class... TuplesInitReduceAndTransform,
      constraints<std::enable_if_t<(sizeof...(TuplesInitReduceAndTransform) > 4U)>> = nullptr>
  constexpr auto operator()(TuplesInitReduceAndTransform&&... args) const noexcept(  //
      noexcept(nary_impl(
          std::make_index_sequence<sizeof...(TuplesInitReduceAndTransform) - 3U>{},
          std::declval<std::tuple<TuplesInitReduceAndTransform&&...>>()
      ))
  )
      -> decltype(nary_impl(
          std::make_index_sequence<sizeof...(TuplesInitReduceAndTransform) - 3U>{},
          std::declval<std::tuple<TuplesInitReduceAndTransform&&...>>()
      )) {
    return nary_impl(
        std::make_index_sequence<sizeof...(TuplesInitReduceAndTransform) - 3U>{},
        std::forward_as_tuple(std::forward<TuplesInitReduceAndTransform>(args)...)
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
