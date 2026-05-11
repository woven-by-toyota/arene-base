// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::transform"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TRANSFORM_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TRANSFORM_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/decay.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/make_tuple.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/tuple/detail/get.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
#include "arene/base/utility/forward_like.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace transform_detail {

/// @brief Transform each element of a tuple-like object with the specified
/// function. The tuple-like object should support std::get and
/// std::tuple_size.
/// @tparam F The transformation function type.
/// @tparam TupleType The tuple type.
/// @tparam Is The index sequence.
/// @param func The transformation function.
/// @param tuple The tuple.
/// @return A tuple with elements defined by @c f(std::get<I>(tuple)) for each I.
template <class F, typename TupleType, std::size_t... Is>
constexpr auto transform_tuple_impl(F&& func, TupleType&& tuple, std::index_sequence<Is...>) -> decltype(auto) {
  return std::make_tuple(func(std::get<Is>(std::forward<TupleType>(tuple)))...);
}

/// @brief Transform each element of an array with the specified function.
/// @tparam F The transformation function type.
/// @tparam ArrayType The array type.
/// @tparam N The array size.
/// @tparam Is The index sequence.
/// @param func The transformation function.
/// @param array The array.
/// @return An array with elements defined by @c f(std::get<I>(array)) for each I.
template <template <typename U, std::size_t N> class ToType, typename F, typename FromArrayType>
constexpr auto transform_array_impl(F&& func, FromArrayType&& array)
    -> ToType<std::decay_t<decltype(func(*array.begin()))>, 0U> {
  return ToType<std::decay_t<decltype(func(*array.begin()))>, 0U>{};
}

/// @brief Transform each element of an array with the specified function.
/// @tparam F The transformation function type.
/// @tparam ArrayType The array type.
/// @tparam N The array size.
/// @tparam Is The index sequence.
/// @param func The transformation function.
/// @param array The array.
/// @return An array with elements defined by @c f(tuple_detail::get<I>(array)) for each I.
template <template <typename U, std::size_t N> class ToType, typename F, typename FromArrayType, std::size_t... Is>
constexpr auto transform_array_impl(F&& func, FromArrayType&& array, std::index_sequence<Is...>)
    -> ToType<std::decay_t<decltype(func(*array.begin()))>, sizeof...(Is)> {
  return ToType<std::decay_t<decltype(func(*array.begin()))>, sizeof...(Is)>{
      func(tuple_detail::get<Is>(std::forward<FromArrayType>(array)))...
  };
}

///
/// @brief Helper for implementing transform as a Niebloid
///
class transform_fn {
 public:
  ///
  /// @brief Backport of C++20's unary op constexpr @c std::transform .
  ///
  /// @tparam StartItr type of the iterator for the sequence to transform. Must model @c InputIterator .
  /// @tparam EndItr type of the iterator to compare to for the sequence to transform. Must satisfy
  ///         @c is_equality_comparable<StartItr,EndItr> .
  /// @tparam OutputItr type of iterator for the output sequence. Must model @c OutputIterator .
  /// @tparam UnaryOperation type of the function to invoke to transform elements.
  /// @param start Iterator to the start of the sequence to transform.
  /// @param end Iterator to the end of the sequence to transform.
  /// @param out Iterator to the start of the sequence to output the transformed values to.
  /// @param transform_op Callable that will be invoked on each element in the range @c [start,end) exactly once.
  /// @return OutputItr iterator in the output sequence pointing to the position after the last element transformed.
  ///
  template <
      typename StartItr,
      typename EndItr,
      typename OutputItr,
      typename UnaryOperation,
      constraints<
          std::enable_if_t<is_input_iterator_v<StartItr>>,
          std::enable_if_t<is_output_iterator_v<OutputItr>>,
          std::enable_if_t<is_inequality_comparable_v<StartItr, EndItr>>> = nullptr>
  constexpr auto operator()(StartItr start, EndItr end, OutputItr out, UnaryOperation transform_op) const noexcept(  //
      arene::base::is_nothrow_inequality_comparable_v<StartItr&, EndItr&>&&                                          //
      noexcept(*std::declval<OutputItr&>()++ = transform_op(*std::declval<StartItr&>()++))
  ) -> OutputItr {
    while (start != end) {
      // parasoft-begin-suppress AUTOSAR-M5_2_10-a "idiomatic iterator operations permitted by M5-2-10 Permit #1"
      // parasoft-begin-suppress AUTOSAR-M5_0_15-a "These are iterator types, so incrementing is ok"
      *out++ = transform_op(*start++);
      // parasoft-end-suppress AUTOSAR-M5_0_15-a
      // parasoft-end-suppress AUTOSAR-M5_2_10-a
    }
    return out;
  }

  ///
  /// @brief Backport of C++20's binary op constexpr @c std::transform.
  ///
  /// @tparam LhsStartItr type of the iterator for the left hand side sequence to transform. Must model
  ///         @c InputIterator.
  /// @tparam LhsEndItr type of the iterator to compare to for the left hand side sequence to transform. Must satisfy
  ///         @c is_equality_comparable<StartItr,EndItr>.
  /// @tparam RhsIter type of the iterator to the right hand side sequence to transform. Must model @c InputIterator.
  /// @tparam OutputItr type of iterator for the output sequence. Must model @c OutputIterator.
  /// @tparam BinaryOperation type of the function to invoke to transform elements.
  /// @param lhs_start Iterator to the start of the left hand side sequence to transform.
  /// @param lhs_end Iterator to the end of the left hand side sequence to transform.
  /// @param rhs_start Iterator to the start of the right hand side sequence.
  /// @param out Iterator to the start of the sequence to output the transformed values to.
  /// @param transform_op Callable that will be invoked exactly once on each element in the range @c [lhs_start,lhs_end)
  ///        and the range starting at @c rhs_start of length @c distance(lhs_start,lhs_end).
  /// @return OutputItr iterator in the output sequence pointing to the position after the last element transformed.
  ///
  template <
      typename LhsStartItr,
      typename LhsEndItr,
      typename RhsItr,
      typename OutputItr,
      typename BinaryOperation,
      constraints<
          std::enable_if_t<is_input_iterator_v<LhsStartItr>>,
          std::enable_if_t<is_input_iterator_v<RhsItr>>,
          std::enable_if_t<is_output_iterator_v<OutputItr>>,
          std::enable_if_t<is_inequality_comparable_v<LhsStartItr, LhsEndItr>>> = nullptr>
  constexpr auto operator()(
      LhsStartItr lhs_start,
      LhsEndItr lhs_end,
      RhsItr rhs_start,
      OutputItr out,
      BinaryOperation transform_op
  ) const noexcept(                                                                //
      arene::base::is_nothrow_inequality_comparable_v<LhsStartItr&, LhsEndItr&>&&  //
      noexcept(
          *std::declval<OutputItr&>()++ = transform_op(*std::declval<LhsStartItr&>()++, *std::declval<RhsItr&>()++)
      )
  ) -> OutputItr {
    while (lhs_start != lhs_end) {
      // parasoft-begin-suppress AUTOSAR-M5_2_10-a "idiomatic iterator operations permitted by M5-2-10 Permit #1"
      // parasoft-begin-suppress AUTOSAR-M5_0_15-a "These are iterator types, so incrementing is ok"
      *out++ = transform_op(*lhs_start++, *rhs_start++);
      // parasoft-end-suppress AUTOSAR-M5_0_15-a
      // parasoft-end-suppress AUTOSAR-M5_2_10-a
    }
    return out;
  }

  /// @brief Transform each element of a std::pair with the specified function.
  /// @tparam F The transformation function type.
  /// @param func The transformation function.
  /// @param pair The pair.
  /// @return A pair with elements defined by f(pair.first, pair.second).
  template <
      typename F,
      typename PairType,
      constraints<std::enable_if_t<is_instantiation_of_v<remove_cvref_t<PairType>, std::pair>>> = nullptr>
  constexpr auto operator()(F&& func, PairType&& pair) const {
    return std::make_pair(func(forward_like<PairType>(pair.first)), func(forward_like<PairType>(pair.second)));
  }

  /// @brief Transform each element of a @c std::tuple
  /// @tparam F The transformation function type.
  /// @tparam TupleType The tuple-like object type.
  /// @param func The transformation function.
  /// @param tuple The tuple-like object.
  /// @return A @c std::tuple with elements defined by f(std::get<I>(tuple)) for each I.
  template <
      class F,
      class TupleType,
      constraints<std::enable_if_t<is_instantiation_of_v<remove_cvref_t<TupleType>, std::tuple>>> = nullptr>
  constexpr auto operator()(F&& func, TupleType&& tuple) const {  // CODEQLFP(DCL51-CPP)
    return transform_tuple_impl(
        std::forward<F>(func),
        std::forward<TupleType>(tuple),
        std::make_index_sequence<std::tuple_size<remove_cvref_t<TupleType>>::value>{}
    );
  }

  /// @brief Transform each element of a std::array with the specified function.
  /// @tparam F The transformation function type.
  /// @tparam T The array element type.
  /// @tparam N The array size.
  /// @param func The transformation function.
  /// @param array The array.
  /// @return An array with elements defined by f(std::get<I>(array)) for each I.
  template <
      class F,
      template <typename T, std::size_t N>
      class ArrayLikeType,
      typename T,
      std::size_t N,
      constraints<std::enable_if_t<N != 0>, std::enable_if_t<std::tuple_size<ArrayLikeType<T, N>>::value == N>> =
          nullptr>
  constexpr auto operator()(F&& func, ArrayLikeType<T, N> const& array) const {
    return transform_array_impl<ArrayLikeType>(std::forward<F>(func), array, std::make_index_sequence<N>{});
  }
  /// @brief Transform each element of a std::array with the specified function.
  /// @tparam F The transformation function type.
  /// @tparam T The array element type.
  /// @tparam N The array size.
  /// @param func The transformation function.
  /// @param array The array.
  /// @return An array with elements defined by f(std::get<I>(array)) for each I.
  template <
      class F,
      template <typename T, std::size_t N>
      class ArrayLikeType,
      typename T,
      std::size_t N,
      constraints<std::enable_if_t<N != 0>, std::enable_if_t<std::tuple_size<ArrayLikeType<T, N>>::value == N>> =
          nullptr>
  constexpr auto operator()(F&& func, ArrayLikeType<T, N>&& array) const {
    return transform_array_impl<ArrayLikeType>(std::forward<F>(func), std::move(array), std::make_index_sequence<N>{});
  }
  /// @brief Transform each element of a std::array with the specified function.
  /// @tparam F The transformation function type.
  /// @tparam T The array element type.
  /// @param func The transformation function.
  /// @param array The array.
  /// @return An array with elements defined by f(std::get<I>(array)) for each I.
  template <
      class F,
      template <typename T, std::size_t N>
      class ArrayLikeType,
      typename T,
      constraints<std::enable_if_t<std::tuple_size<ArrayLikeType<T, 0U>>::value == 0>> = nullptr>
  constexpr auto operator()(F&& func, ArrayLikeType<T, 0U> const& array) const {
    return transform_array_impl<ArrayLikeType>(std::forward<F>(func), array);
  }
};

}  // namespace transform_detail

/// @def arene::base::transform
/// @copydoc arene::base::transform_detail::transform_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(transform_detail::transform_fn, transform);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A2_8_1-a-2
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TRANSFORM_HPP_
