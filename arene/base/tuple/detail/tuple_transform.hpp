// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines
// arene::base::tuple_detail::tuple_transform"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_TUPLE_TRANSFORM_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_TUPLE_TRANSFORM_HPP_

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/functional/bind_front.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/forward_as_tuple.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_void.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/tuple/apply.hpp"
#include "arene/base/tuple/detail/get.hpp"
#include "arene/base/tuple/detail/tuple_zip.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {
namespace tuple_detail {

namespace tuple_transform_detail {

/// @brief specifies an array result with an explicit value type
/// @tparam T array value type
///
/// Specifies the result type as @c arene::base::array<T, N> for a sequence of values,
/// where @c T is the explicit array value type.
template <class T, class = constraints<>>
class to_array_result {
 public:
  /// @brief result type
  template <class... Ts>
  using result_type = arene::base::array<T, sizeof...(Ts)>;
};

/// @brief specifies an array result with a deduced value type
/// @tparam T array value type, constrained to cv @c void
///
/// Specifies an @c arene::base::array by deducing the value type from a
/// sequence of values using the deduction mechanics of
/// @c arene::base::to_array.
///
/// @note This default specialization for cv @c void is analogous to how
/// standard arithmetic function objects such as @c std::plus<void> deduce
/// their operand and result types.
template <class T>
class to_array_result<T, constraints<std::enable_if_t<std::is_void<T>::value>>> {
 public:
  /// @brief result type, ill-formed if sizeof...(Ts) is zero
  template <class... Ts>
  using result_type = decltype(arene::base::to_array({std::declval<Ts>()...}));
};

/// @brief specifies a @c std::tuple result of the provided types
/// @note Does not decay tuple element types.
class to_tuple_result {
 public:
  /// @brief result type
  template <class... Ts>
  using result_type = std::tuple<Ts...>;
};

/// @brief function object implementing tuple-based transform algorithms
/// @tparam ResultPolicy result construction policy
///
/// Applies a unary or n-ary transform to elements of one or more tuples and
/// constructs the result using a result construction policy.
///
/// For unary transforms, the transform is applied element-wise to the input
/// tuple. For n-ary transforms, the input tuples are first zipped element-wise,
/// and then transform is applied to each grouped element.
template <class ResultPolicy>
class tuple_transform_to_fn {
  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: parameters 'tuple' and 'transform' are used"
  /// @brief implementation for transforming a tuple
  /// @tparam Is index sequence corresponding to the elements of the input tuple
  /// @tparam Tuple input tuple type
  /// @tparam Transform transform operation type
  /// @tparam Result tuple result type, deduced from the @c ResultPolicy
  /// @param tuple input tuple
  /// @param transform transform operation
  /// @return result of invoking @c ResultPolicy with the transformed elements
  template <  //
      std::size_t... Is,
      class Tuple,
      class Transform,
      class Result = typename ResultPolicy::template result_type<                  //
          decltype(std::declval<Transform&>()(get<Is>(std::declval<Tuple>())))...  //
          >                                                                        //
      >
  static constexpr auto impl(std::index_sequence<Is...>, Tuple&& tuple, Transform& transform) noexcept(  //
      noexcept(Result{std::declval<Transform&>()(get<Is>(std::declval<Tuple>()))...})
  ) -> decltype(Result{std::declval<Transform&>()(get<Is>(std::declval<Tuple>()))...}) {
    // parasoft-begin-suppress AUTOSAR-M8_5_2-a-2 "False positive: correct initialization"
    return Result{transform(get<Is>(std::forward<Tuple>(tuple)))...};
    // parasoft-end-suppress AUTOSAR-M8_5_2-a-2
  }
  // parasoft-end-suppress AUTOSAR-A0_1_4-a

  /// @brief implementation for n-ary tuple transform
  /// @tparam Is index sequence corresponding to the elements of the input
  ///   tuples, does *not* include the transform argument
  /// @tparam Args parameter pack containing references of input tuples followed
  ///   by a reference to the transform
  /// @param args @c std::tuple of references of the input tuples and a
  ///   reference to the transform
  /// @return result from applying the adapted transform element-wise across
  ///   input tuples
  template <std::size_t... Is, class... Args>
  static constexpr auto nary_transform(std::index_sequence<Is...>, std::tuple<Args...>&& args) noexcept(  //
      noexcept(std::declval<tuple_transform_to_fn>()(
          tuple_zip(std::get<Is>(std::declval<std::tuple<Args&&...>&&>())...),
          arene::base::bind_front(apply, std::get<sizeof...(Is)>(std::declval<std::tuple<Args...>&&>()))
      ))
  )
      -> decltype(std::declval<tuple_transform_to_fn>()(
          tuple_zip(std::get<Is>(std::declval<std::tuple<Args&&...>&&>())...),
          arene::base::bind_front(apply, std::get<sizeof...(Is)>(std::declval<std::tuple<Args...>&&>()))
      )) {
    return tuple_transform_to_fn{}(  //
        tuple_zip(std::get<Is>(std::move(args))...),
        arene::base::bind_front(apply, std::get<sizeof...(Is)>(std::move(args)))
    );
  }

 public:
  /// @brief apply a transform to a tuple
  /// @tparam Tuple input tuple type
  /// @tparam Transform transform operation type
  /// @param tuple input tuple
  /// @param transform transform operation
  /// @return result constructed from applying the transform element-wise
  template <class Tuple, class Transform>
  constexpr auto operator()(Tuple&& tuple, Transform transform) const noexcept(  //
      noexcept(impl(
          std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{},
          std::declval<Tuple&&>(),
          std::declval<Transform&>()
      ))
  )
      -> decltype(impl(
          std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{},
          std::declval<Tuple&&>(),
          std::declval<Transform&>()
      )) {
    return impl(
        std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{},
        std::forward<Tuple>(tuple),
        transform
    );
  }

  /// @brief apply an n-ary transform to n tuples
  /// @tparam TuplesAndThenTransform input tuple types followed by the transform
  /// @param args input tuples followed by the transform
  /// @return result constructed by applying the transform to associated elements
  ///   across the input tuples
  ///
  /// @note Constraints <br>
  /// * <c> sizeof...(TuplesAndThenTransform) > 2 </c> is @c true
  ///
  /// @note The transform (the last argument) is accepted as a forwarding
  /// reference rather than by value. This is a departure from the convention
  /// used by standard algorithms such as @c std::transform, where function
  /// objects are taken by value. Internally the transform is decay-copied,
  /// which requires the transform to be move or copy constructible. This may
  /// result in one additional move or copy construction compared to a standard
  /// algorithm that accepts the transform by value directly.
  template <  //
      class... TuplesAndThenTransform,
      constraints<std::enable_if_t<(sizeof...(TuplesAndThenTransform) > 2U)>> = nullptr>
  constexpr auto operator()(TuplesAndThenTransform&&... args) const noexcept(  //
      noexcept(nary_transform(
          std::make_index_sequence<sizeof...(TuplesAndThenTransform) - 1U>{},
          std::declval<std::tuple<TuplesAndThenTransform&&...>>()
      ))
  )
      -> decltype(nary_transform(
          std::make_index_sequence<sizeof...(TuplesAndThenTransform) - 1U>{},
          std::declval<std::tuple<TuplesAndThenTransform&&...>>()
      )) {
    return nary_transform(
        std::make_index_sequence<sizeof...(TuplesAndThenTransform) - 1U>{},
        std::forward_as_tuple(std::forward<TuplesAndThenTransform&&>(args)...)
    );
  }
};

}  // namespace tuple_transform_detail

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."

/// @brief function object implementing tuple-based transform algorithms
///
/// Provides n-ary tuple-based transform operations returning a @c std::tuple.
ARENE_CPP14_INLINE_VARIABLE(
    tuple_transform_detail::tuple_transform_to_fn<tuple_transform_detail::to_tuple_result>,
    tuple_transform
);

/// @brief function object implementing tuple-based transform algorithms
/// @tparam T optional array value type
///
/// Provides n-ary tuple-based transform operations returning an @c
/// arene::base::array. When constructing an array result, all invocations of
/// the transform must produce the same value type.
///
/// If @c T is @c void, the array value type is deduced. The array value type
/// cannot be deduced for a zero-element tuple.
template <class T = void>
extern constexpr auto tuple_transform_to_array =
    tuple_transform_detail::tuple_transform_to_fn<tuple_transform_detail::to_array_result<T>>{};

// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace tuple_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_TUPLE_TRANSFORM_HPP_
