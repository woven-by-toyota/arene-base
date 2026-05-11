// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::tuple_detail::tuple_zip"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_TUPLE_ZIP_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_TUPLE_ZIP_HPP_

#include "arene/base/algorithm/all_of.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/functional/bind_back.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/equal_to.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/forward_as_tuple.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/stdlib_choice/tuple_element.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/tuple/detail/get.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace tuple_detail {

/// @brief function object implementing the zip algorithm for tuples
///
/// Constructs a @c std::tuple whose elements are @c std::tuple types formed by
/// grouping elements with the same index from each input tuple.
///
/// For example:
/// @code
/// tuple_zip((a0, a1), (b0, b1)) -> ((a0, b0), (a1, b1))
/// @endcode
class tuple_zip_fn {
  // parasoft-begin-suppress AUTOSAR-A15_4_5-a "False positive: nothing can be thrown by this function"
  // parasoft-begin-suppress AUTOSAR-A2_10_1-a "False positive: 'count' does not hide anything"
  /// @brief check that all tuple sizes are equal to a given value
  /// @param sizes list of tuple sizes
  /// @param count reference size to compare against
  /// @return true if all elements in @p sizes are equal to @c count
  static constexpr auto all_are_equal_to(std::initializer_list<std::size_t> const sizes, std::size_t count) noexcept
      -> bool {
    // arene::base::bind_back is currently implemented as a function
    // template so we fully qualify it in case the same identifier is defined at
    // global scope
    return all_of(sizes.begin(), sizes.end(), arene::base::bind_back(std::equal_to<>{}, count));
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-a
  // parasoft-end-suppress AUTOSAR-A15_4_5-a

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief helper to deduce the type of a single zipped element
  /// @tparam I index of the element across all input tuples
  /// @tparam Tuples input tuple types contained in the bundle
  /// @return @c std::tuple of the @c I-th element type from each input tuple
  template <std::size_t I, class... Tuples>
  static auto zipped_element_type(std::tuple<Tuples...>&&)
      -> std::tuple<std::tuple_element_t<I, std::remove_reference_t<Tuples>>...>;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  /// @brief zipped element type at a specific index
  /// @tparam I index of the element across all input tuples
  /// @tparam Bundle @c std::tuple containing all input tuples
  template <std::size_t I, class Bundle>
  using zipped_element_t = decltype(zipped_element_type<I>(std::declval<Bundle>()));

  /// @brief construct a single zipped element tuple from a bundle of tuples
  /// @tparam I index of the element to extract from each input tuple
  /// @tparam Js index sequence corresponding to the input tuples in the bundle
  /// @tparam Tuples... input tuple types
  /// @tparam Bundle deduced @c std::tuple containing all input tuples
  /// @tparam Result deduced result type for the zipped element tuple
  /// @param bundle @c std::tuple containing all input tuples
  /// @return @c std::tuple containing the @c I-th element from each input tuple
  ///
  /// Constructs the result using the type deduced by @c zipped_element_type_t,
  /// preserving the element types as determined by @c std::tuple_element_t.
  template <  //
      std::size_t I,
      std::size_t... Js,
      class... Tuples,
      class Bundle = std::tuple<Tuples...>,
      class Result = zipped_element_t<I, Bundle>>
  static constexpr auto zip_element_impl(std::index_sequence<Js...>, std::tuple<Tuples...>&& bundle) noexcept(  //
      noexcept(Result{get<I>(get<Js>(std::declval<Bundle>()))...})
  ) -> decltype(Result{get<I>(get<Js>(std::declval<Bundle>()))...}) {
    return Result{get<I>(get<Js>(std::move(bundle)))...};
  }

  /// @brief construct a single zipped element tuple from a bundle of tuples
  /// @tparam I index of the element to extract from each input tuple
  /// @tparam Tuples input tuple types contained in the bundle
  /// @param bundle @c std::tuple containing all input tuples
  /// @return @c std::tuple containing the @c I-th element from each input tuple
  ///
  /// @note We cannot use <c> arene::base::transform(tuple, get<I>) </c>, as it
  ///   will always decay references.
  template <std::size_t I, class... Tuples>
  static constexpr auto zip_element(std::tuple<Tuples...>&& bundle) noexcept(  //
      noexcept(zip_element_impl<I>(
          std::index_sequence_for<Tuples...>{},
          std::declval<std::tuple<Tuples...>>()
      ))
  ) -> decltype(zip_element_impl<I>(  //
      std::index_sequence_for<Tuples...>{},
      std::declval<std::tuple<Tuples...>>()
  )) {
    return zip_element_impl<I>(  //
        std::index_sequence_for<Tuples...>{},
        std::move(bundle)
    );
  }

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief helper to deduce the zipped result type
  /// @tparam Is index sequence corresponding to the elements of the input tuples
  /// @tparam Tuples... input tuple types
  /// @tparam Bundle deduced @c std::tuple containing all input tuples
  /// @return unevaluated; return type is a @c std::tuple of zipped element
  ///   tuples, one per index
  template <std::size_t... Is, class... Tuples, class Bundle = std::tuple<Tuples...>>
  static auto zipped_type(std::index_sequence<Is...>, std::tuple<Tuples...>&&)
      -> std::tuple<zipped_element_t<Is, Bundle>...>;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  /// @brief result type of zipping a bundle of tuples
  /// @tparam N number of elements in each input tuple
  /// @tparam Tuples... input tuple types
  //
  /// Computes the type of the @c std::tuple produced by zipping the input
  /// tuples contained in @c Bundle. Each element of the result is a
  /// @c std::tuple containing the corresponding element type from each input
  /// tuple, as determined by @c std::tuple_element_t.
  template <std::size_t N, class... Tuples>
  using zipped_t = decltype(zipped_type(std::make_index_sequence<N>{}, std::declval<std::tuple<Tuples...>>()));

  /// @brief implementation detail for zipping tuple types
  /// @tparam Is index sequence corresponding to the number of elements in each tuple
  /// @tparam Tuples... input tuple types
  /// @param bundle @c std::tuple of forwarded input tuples
  /// @return @c std::tuple whose elements are tuples formed from corresponding
  ///   elements of each input tuple
  template <std::size_t... Is, class... Tuples>
  static constexpr auto impl(std::index_sequence<Is...>, std::tuple<Tuples...>&& bundle) noexcept(  //
      noexcept(zipped_t<sizeof...(Is), Tuples...>{zip_element<Is>(std::declval<std::tuple<Tuples...>>())...})
  ) -> decltype(zipped_t<sizeof...(Is), Tuples...>{zip_element<Is>(std::declval<std::tuple<Tuples...>>())...}) {
    return zipped_t<sizeof...(Is), Tuples...>{zip_element<Is>(std::move(bundle))...};
  }

 public:
  // parasoft-begin-suppress AUTOSAR-A8_4_2-a "False positive: function does have a return"
  // parasoft-begin-suppress CERT_C-MSC37-a "False positive: function does have a return"
  // parasoft-begin-suppress CERT_CPP-MSC52-a "False positive: function does have a return"
  /// @brief zip tuple types
  /// @return @c std::tuple
  constexpr auto operator()() const noexcept -> std::tuple<> { return {}; }
  // parasoft-end-suppress AUTOSAR-A8_4_2-a
  // parasoft-end-suppress CERT_C-MSC37-a
  // parasoft-end-suppress CERT_CPP-MSC52-a

  /// @brief zip tuple types
  /// @tparam Head first tuple type
  /// @tparam Tail remaining tuple types
  /// @param head first tuple
  /// @param tail remaining tuples
  ///
  /// Groups elements at the same index from each input tuple into a new tuple.
  ///
  /// @return @c std::tuple of tuples, where each inner tuple contains elements
  ///   from the input tuples at the same index
  ///
  /// @note Constraints: <br>
  /// * All input tuples must have the same size.
  ///
  /// @note Reference handling: <br>
  /// * Uses @c tuple_element_t to preserve reference element types
  template <
      class Head,
      class... Tail,
      constraints<std::enable_if_t<all_are_equal_to(
          {std::tuple_size<std::remove_reference_t<Tail>>::value...},
          std::tuple_size<std::remove_reference_t<Head>>::value
      )>> = nullptr>
  constexpr auto operator()(Head&& head, Tail&&... tail) const noexcept(  //
      noexcept(impl(
          std::make_index_sequence<std::tuple_size<std::remove_reference_t<Head>>::value>{},
          std::declval<std::tuple<Head&&, Tail&&...>>()
      ))
  )
      -> decltype(impl(
          std::make_index_sequence<std::tuple_size<std::remove_reference_t<Head>>::value>{},
          std::declval<std::tuple<Head&&, Tail&&...>>()
      )) {
    return impl(  //
        std::make_index_sequence<std::tuple_size<std::remove_reference_t<Head>>::value>{},
        std::forward_as_tuple(  //
            std::forward<Head>(head),
            std::forward<Tail>(tail)...
        )
    );
  }
};

/// @def arene::base::tuple_detail::tuple_zip
/// @copydoc arene::base::tuple_detail::tuple_zip_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(tuple_detail::tuple_zip_fn, tuple_zip);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a
}  // namespace tuple_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_DETAIL_TUPLE_ZIP_HPP_
