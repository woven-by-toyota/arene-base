/// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_TUPLE_CAT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_TUPLE_CAT_HPP_

// IWYU pragma: private, include "arene/base/tuple.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/integer_sequences.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/forward_as_tuple.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/tuple/detail/as_type_list.hpp"
#include "arene/base/tuple/detail/get.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_manipulation/repeat_type.hpp"
#include "arene/base/type_traits/all_of.hpp"
#include "arene/base/type_traits/is_tuple_like.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace tuple_cat_detail {

/// @brief helper binding tuple_index positions to their corresponding tuple sizes
/// @tparam Is pack of tuple_index values (one per input tuple)
/// @tparam Sizes... sizes of the input tuples, in positional correspondence with @c Is
template <class Is, std::size_t... Sizes>
class tuple_index_seq_impl;

/// @brief tuple_index_seq_impl specialization that concatenates one run per input tuple
/// @tparam Is pack of tuple_index values (one per input tuple)
/// @tparam Sizes... sizes of the input tuples, in positional correspondence with @c Is
///
/// @note Prepending an empty @c std::index_sequence<> seed makes the empty-pack case
///   well-formed, as @c integer_sequence_cat rejects an empty pack.
template <std::size_t... Is, std::size_t... Sizes>
class tuple_index_seq_impl<std::index_sequence<Is...>, Sizes...> {
 public:
  /// @brief The resulting integer sequence
  using type = integer_sequence_cat<std::index_sequence<>, make_index_sequence_repeat_n<Is, Sizes>...>;
};

/// @brief Generate the @c tuple_index sequence, used as the first index into the @c bundle
/// @tparam Sizes... sizes of the input tuples, in order
///
/// For each input tuple at position @c k in the bundle (with size @c Sizes[k]), emit the
/// value @c k repeated @c Sizes[k] times; the resulting runs are concatenated into a single
/// flat sequence whose length equals the total number of output elements.
template <std::size_t... Sizes>
using tuple_index_seq_t = typename tuple_index_seq_impl<std::make_index_sequence<sizeof...(Sizes)>, Sizes...>::type;

/// @brief helper producing the flat element_index sequence
/// @tparam Sizes... sizes of the input tuples, in order
///
/// @note Prepending an empty @c std::index_sequence<> seed makes the empty-pack case
///   well-formed, as @c integer_sequence_cat rejects an empty pack.
template <std::size_t... Sizes>
class element_index_seq_impl {
 public:
  /// @brief The resulting integer sequence
  using type = integer_sequence_cat<std::index_sequence<>, std::make_index_sequence<Sizes>...>;
};

/// @brief Generate the @c element_index sequence, used as the second index into the @c bundle
/// @tparam Sizes... sizes of the input tuples, in order
///
/// For each input tuple at position @c k in the bundle (with size @c Sizes[k]), emit the sequence <c>[0, 1, ...,
/// Sizes[k] - 1]</c>; the resulting runs are concatenated into a single flat sequence whose length equals the total
/// number of output elements.
template <std::size_t... Sizes>
using element_index_seq_t = typename element_index_seq_impl<Sizes...>::type;

/// @brief result of concatenating the element types of tuple-likes @c Ts...
/// @tparam Ts... tuple-like types
template <class... Ts>
using tuple_cat_result_t = type_lists::concat_t<std::tuple<>, tuple_detail::as_type_list_t<Ts>...>;

/// @brief function object implementing tuple concatenation
///
/// Rather than a recursive approach, this implementation builds the resulting tuple in a single pack expansion.
///
/// Start by creating a single tuple-of-tuples (the @c bundle) from the list of input tuples. This bundle can be treated
/// as a jagged 2D array (an array where each row can have different length), which can be indexed into with two
/// coordinates. By generating a list of coordinates which covers each item in the bundle, each element in the bundle
/// can be fetched and used to build the resulting tuple in a single expansion.
///
/// Two parallel <c>std::index_sequence</c>s can be used to represent the 2d coordinates. Both sequences must be of
/// equal length @c K (where @c K is the total number of output elements). The first @c std::index_sequence holds the
/// @c tuple_index - the first index into the @c bundle. The second @c std::index_sequence holds the @c element_index -
/// the index into the particular tuple at @c tuple_index of the bundle.
///
/// The @c tuple_index @c std::index_sequence is generated by repeating the tuple's position in the bundle a number of
/// times equal to that tuple's size. The @c element_index sequence is generated by concatenating the result of @c
/// std::index_sequence_for for each tuple in the bundle.
///
/// For example, for <c>tuple_cat(tuple<A,B>, tuple<C>, tuple<D,E,F>)</c>:
///
/// @code
///   output index k    : 0   1   2   3   4   5
///   tuple_index[k]    : 0,  0,  1,  2,  2,  2
///   element_index[k]  : 0,  1,  0,  0,  1,  2
/// @endcode
///
/// Once both index sequences are created, they can be simultaneously expanded to generate the final tuple. To continue
/// the example:
/// @code
///   (0,0) -> A
///   (0,1) -> B
///   (1,0) -> C
///   (2,0) -> D
///   (2,1) -> E
///   (2,2) -> F
/// @endcode
///
/// Credit for this algorithm goes to Stephen T. Lavavej in the implementation of @c std::tuple_cat for MSVC.
/// This particular implementation is inspired by Eric Niebler's implemetation
/// @see https://ericniebler.com/2014/11/13/tiny-metaprogramming-library/
// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
class tuple_cat_fn {
 private:
  /// @brief concatenation implementation over a bundle of forwarded tuples
  /// @tparam Result @c std::tuple type carrying the final element types
  /// @tparam Bundle @c std::tuple holding forwarded input tuples
  /// @tparam TupleIndices value pack: for each output position, which input tuple to read from
  /// @tparam ElementIndices value pack: for each output position, which slot within that tuple
  /// @param bundle bundle of tuples
  /// @return concatenated @c std::tuple
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: The parameter bundles is used"
  template <class Result, class Bundle, std::size_t... TupleIndices, std::size_t... ElementIndices>
  static constexpr auto
  impl(Bundle&& bundle, std::index_sequence<TupleIndices...>, std::index_sequence<ElementIndices...>) noexcept(
      noexcept(Result{tuple_detail::get<ElementIndices>(tuple_detail::get<TupleIndices>(std::declval<Bundle>()))...})
  ) -> Result {
    return Result{tuple_detail::get<ElementIndices>(tuple_detail::get<TupleIndices>(std::forward<Bundle>(bundle)))...};
  }
  // parasoft-end-suppress AUTOSAR-A0_1_4-a
  // parasoft-end-suppress CERT_C-EXP37-a

 public:
  /// @brief concatenate zero or more tuple-ish inputs
  /// @tparam Ts tuple-ish input types
  /// @param args tuple-ish arguments
  /// @return @c std::tuple formed by concatenating the elements of each input in order;
  ///   an empty @c std::tuple<> when called with no arguments
  ///
  /// @note Constraints: <br>
  /// * Every input must satisfy @c arene::base::is_tuple_like_v
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
  template <
      class... Ts,
      constraints<std::enable_if_t<all_of_v<is_tuple_like_v<Ts>...>>> = nullptr,
      class Result = tuple_cat_result_t<remove_cvref_t<Ts>...>,
      class TupleIndices = tuple_index_seq_t<std::tuple_size<remove_cvref_t<Ts>>::value...>,
      class ElementIndices = element_index_seq_t<std::tuple_size<remove_cvref_t<Ts>>::value...>>
  constexpr auto operator()(Ts&&... args) const
      noexcept(noexcept(impl<Result>(std::declval<std::tuple<Ts&&...>>(), TupleIndices{}, ElementIndices{})))
          -> Result {
    return impl<Result>(std::forward_as_tuple(std::forward<Ts>(args)...), TupleIndices{}, ElementIndices{});
  }
};
// parasoft-end-suppress AUTOSAR-M2_10_1-a

}  // namespace tuple_cat_detail

/// @def arene::base::tuple_cat
/// @copydoc arene::base::tuple_cat_detail::tuple_cat_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(tuple_cat_detail::tuple_cat_fn, tuple_cat);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_TUPLE_CAT_HPP_
