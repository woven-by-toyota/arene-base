// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::type_lists::cartesian_product"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_CARTESIAN_PRODUCT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_CARTESIAN_PRODUCT_HPP_

// IWYU pragma: private, include "arene/base/type_list.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/copy.hpp"
#include "arene/base/algorithm/iter_swap.hpp"
#include "arene/base/algorithm/reduce.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/detail/index_permutation_range.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/multiplies.hpp"
#include "arene/base/type_list/at.hpp"
#include "arene/base/type_list/size.hpp"
#include "arene/base/type_list/type_list.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

namespace arene {
namespace base {

namespace type_lists {

namespace cartesian_product_detail {

/// @brief The size of the space of all permutations of indices fitting within the given @c Sizes
/// @tparam Sizes The sizes within which the indices of a permutation must fit
/// @note This is essentially just a C++17 fold expression over multiplication but dim 0 is taken to be size 0
template <std::size_t... Sizes>
constexpr std::size_t permutation_space_size_v{
    sizeof...(Sizes) == 0U  //
        ? 0U                //
        : reduce(           //
              std::initializer_list<std::size_t>{Sizes...},
              std::size_t{1},
              std::multiplies<>{}
          )
};

/// @brief constexpr reverse algorithm
/// @tparam BidirectionalIterator iterator type
/// @param first beginning of the range
/// @param last end of the range
template <class BidirectionalIterator>
constexpr auto reverse(BidirectionalIterator first, BidirectionalIterator last) noexcept -> void {
  while (first != last && first != --last) {
    arene::base::iter_swap(first, last);
    ++first;
  }
}

/// @brief Make an array of all permutations of indices fitting within the given @c Sizes
/// @tparam Sizes A pack of sizes which all indices (of the same dimension) must be less than
/// @return An array of all possible permutations of "one in-bounds index for each size in <c>Sizes</c>"
template <std::size_t... Sizes>
constexpr auto all_index_permutations_impl() noexcept
    -> arene::base::array<arene::base::array<std::size_t, sizeof...(Sizes)>, permutation_space_size_v<Sizes...>> {
  using permutation = arene::base::array<std::size_t, sizeof...(Sizes)>;

  // parasoft-begin-suppress AUTOSAR-M8_5_2-a "False positive: correct initialization"
  auto reversed_values = arene::base::array<std::size_t, sizeof...(Sizes)>{Sizes...};
  // parasoft-end-suppress AUTOSAR-M8_5_2-a
  reverse(reversed_values.begin(), reversed_values.end());

  arene::base::array<permutation, permutation_space_size_v<Sizes...>> permutations{};
  auto perm_itr = permutations.begin();
  for (auto& perm : arene::base::detail::make_index_permutation_range(reversed_values)) {
    std::ignore = arene::base::copy(perm.crbegin(), perm.crend(), perm_itr->begin());
    ++perm_itr;
  }
  return permutations;
}

/// @brief As a static variable, an array of all permutations of indices fitting within the given @c Sizes
/// @tparam Sizes A pack of sizes which all indices (of the same dimension) must be less than
template <std::size_t... Sizes>
constexpr arene::base::array<arene::base::array<std::size_t, sizeof...(Sizes)>, permutation_space_size_v<Sizes...>>
    all_index_permutations_v{all_index_permutations_impl<Sizes...>()};

/// @brief Get the n'th permutation of the types in the type-list pack @c Ln and emit it wrapped in @c InnerTemplate
/// @tparam InnerTemplate The template to apply to the permuted type-list
/// @tparam PermIndex The index of this particular permutation
/// @tparam Ln The pack of purported type lists to permute
template <template <class...> class InnerTemplate, std::size_t PermIndex, class... Ln>
class nth_type_permutation {
  // parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: constexpr variables are always initialized in
  // constexpr"
  // parasoft-begin-suppress AUTOSAR-A3_3_2-a "False positive: constexpr variables are always initialized
  // in constexpr"
  /// @brief The *index* permutation used by this type permutation
  static constexpr arene::base::array<std::size_t, sizeof...(Ln)> const& index_permutation{
      all_index_permutations_v<size_v<Ln>...>[PermIndex]
  };
  // parasoft-end-suppress CERT_CPP-DCL56-a
  // parasoft-end-suppress AUTOSAR-A3_3_2-a

  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: rule does not mention naming all parameters"
  /// @brief Generate a pack of types from some permutation of the lists in @c Ln and return it as @c InnerTemplate
  /// @tparam TypeListIndices a pack of indices of the type *lists* in @c Ln (not the permuted indices)
  /// @return Notionally a value of type @c InnerTemplate with the pack of permuted types; only used for @c decltype
  template <std::size_t... TypeListIndices>
  static constexpr auto deduce_type_from_permutation(std::index_sequence<TypeListIndices...>)
      -> InnerTemplate<at_t<Ln, index_permutation[TypeListIndices]>...>;
  // parasoft-end-suppress CERT_C-EXP37-a

 public:
  /// @brief The permuted types, wrapped in @c InnerTemplate
  using type = decltype(deduce_type_from_permutation(std::index_sequence_for<Ln...>{}));
};

/// @brief Get the n'th permutation of the types in the type-list pack @c Ln and emit it wrapped in @c InnerTemplate
/// @tparam InnerTemplate The template to apply to the permuted type-list
/// @tparam PermIndex The index of this particular permutation
/// @tparam Ln The pack of purported type lists to permute
template <template <class...> class InnerTemplate, std::size_t PermIndex, class... Ln>
using nth_type_permutation_t = typename nth_type_permutation<InnerTemplate, PermIndex, Ln...>::type;

/// @brief Get a type-list of all permutations of the entries of the input type-lists @c Ln
/// @tparam OuterTemplate The outer type-list template in which to wrap the pack of permuted type-lists
/// @tparam InnerTemplate The template to apply to each individual permuted type-list within the list of permutations
/// @tparam PermIndexList An index-list, one for each permutation to include in the emitted type-list
/// @tparam Ln The pack of purported type lists to permute
template <
    template <class...>
    class OuterTemplate,
    template <class...>
    class InnerTemplate,
    class PermIndexList,
    class... Ln>
struct all_type_permutations;

/// @brief Get a type-list of all permutations of the entries of the input type-lists @c Ln
/// @tparam OuterTemplate The outer type-list template in which to wrap the pack of permuted type-lists
/// @tparam InnerTemplate The template to apply to each individual permuted type-list within the list of permutations
/// @tparam PermIndices A pack of indices, one for each permutation to include in the emitted type-list
/// @tparam Ln The pack of purported type lists to permute
template <
    template <class...>
    class OuterTemplate,
    template <class...>
    class InnerTemplate,
    std::size_t... PermIndices,
    class... Ln>
struct all_type_permutations<OuterTemplate, InnerTemplate, std::index_sequence<PermIndices...>, Ln...> {
  /// @brief A pack of all permutations of the types in <c>Ln</c>, wrapped in @c OuterTemplate
  using type = OuterTemplate<nth_type_permutation_t<InnerTemplate, PermIndices, Ln...>...>;
};

/// @brief Get the first type-list template used in the given pack, or fall back to @c arene::base::type_list if none
/// @tparam Ln The pack of purported type lists to check
template <class... Ln>
struct get_first_list_template {
  /// @brief Fallback for the case where no first list template can be extracted: use @c arene::base::type_list
  /// @tparam Types A pack of types to hold
  template <class... Types>
  using type = arene::base::type_list<Types...>;
};

/// @brief Get the first type-list template used in the given pack
/// @tparam FirstInnerTemplate The type-list template to emit
/// @tparam Ts The types on which @c FirstInnerTemplate is templated; only used for deduction
/// @tparam RemainingLists Any other type lists which may remain in the pack after picking out the first one
template <template <class...> class FirstInnerTemplate, class... Ts, class... RemainingLists>
struct get_first_list_template<FirstInnerTemplate<Ts...>, RemainingLists...> {
  /// @brief For the case where the first list template can be extracted, return it
  /// @tparam Types A pack of types to hold
  template <class... Types>
  using type = FirstInnerTemplate<Types...>;
};

}  // namespace cartesian_product_detail

/// @brief Produce the cartesian product of multiple type-lists
///
/// @tparam Ln A pack of type-lists that holds the types to process.
/// @return A type list of type lists, where each inner list holds one combination of one type from each @c Ln
/// @note The type of the resulting type-list, as well as all of the individual permuted type-lists within it, will be
/// the type of the first type-list within <c>Ln</c>. It will not necessarily be <c>arene::base::type_list</c>.
/// @note Permutations within the resulting product are ordered lexicographically with respect to the indices of the
/// types they refer to within the original lists. The first is (0, 0, ... , 0), followed by (0, 0, ... , 1), and so on.
template <class... Ln>
using cartesian_product = cartesian_product_detail::all_type_permutations<
    cartesian_product_detail::get_first_list_template<Ln...>::template type,
    cartesian_product_detail::get_first_list_template<Ln...>::template type,
    std::make_index_sequence<cartesian_product_detail::permutation_space_size_v<size_v<Ln>...>>,
    Ln...>;

/// @brief Produce the cartesian product of multiple type-lists
///
/// @tparam Ln A pack of type-lists that holds the types to process.
/// @return A type list of type lists, where each inner list holds one combination of one type from each @c Ln
/// @note The type of the resulting type-list, as well as all of the individual permuted type-lists within it, will be
/// the type of the first type-list within <c>Ln</c>. It will not necessarily be <c>arene::base::type_list</c>.
/// @note Permutations within the resulting product are ordered lexicographically with respect to the indices of the
/// types they refer to within the original lists. The first is (0, 0, ... , 0), followed by (0, 0, ... , 1), and so on.
template <class... Ln>
using cartesian_product_t = typename cartesian_product<Ln...>::type;

/// @brief The cartesian product of multiple type-lists, with each combination of types bound to @c DesiredInnerTemplate
///
/// @tparam DesiredInnerTemplate A template taking only types, to be used as the inner template of the final list
/// @tparam Ln A pack of type-lists that holds the types to process.
/// @return A type list of <c>DesiredInnerTemplate</c>; each entry uses one combination of one type from each @c Ln
/// @note The type of the resulting type-list will be the type of the first type-list within <c>Ln</c>, and the type of
/// each permuted element within it will be <c>DesiredInnerTemplate</c>. Neither will necessarily be
/// <c>arene::base::type_list</c>.
/// @note Permutations within the resulting product are ordered lexicographically with respect to the indices of the
/// types they refer to within the original lists. The first is (0, 0, ... , 0), followed by (0, 0, ... , 1), and so on.
template <template <class...> class DesiredInnerTemplate, class... Ln>
using cartesian_product_as = cartesian_product_detail::all_type_permutations<
    cartesian_product_detail::get_first_list_template<Ln...>::template type,
    DesiredInnerTemplate,
    std::make_index_sequence<cartesian_product_detail::permutation_space_size_v<size_v<Ln>...>>,
    Ln...>;

/// @brief The cartesian product of multiple type-lists, with each combination of types bound to @c DesiredInnerTemplate
///
/// @tparam DesiredInnerTemplate A template taking only types, to be used as the inner template of the final list
/// @tparam Ln A pack of type-lists that holds the types to process.
/// @return A type list of <c>DesiredInnerTemplate</c>; each entry uses one combination of one type from each @c Ln
/// @note The type of the resulting type-list will be the type of the first type-list within <c>Ln</c>, and the type of
/// each permuted element within it will be <c>DesiredInnerTemplate</c>. Neither will necessarily be
/// <c>arene::base::type_list</c>.
/// @note Permutations within the resulting product are ordered lexicographically with respect to the indices of the
/// types they refer to within the original lists. The first is (0, 0, ... , 0), followed by (0, 0, ... , 1), and so on.
template <template <class...> class DesiredInnerTemplate, class... Ln>
using cartesian_product_as_t = typename cartesian_product_as<DesiredInnerTemplate, Ln...>::type;

}  // namespace type_lists
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_CARTESIAN_PRODUCT_HPP_
