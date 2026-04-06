// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_SORT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_SORT_HPP_

// IWYU pragma: private, include "arene/base/type_list.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/type_traits/always_false.hpp"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/type_list/apply_all.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/type_list.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace type_lists {

/// @brief Sort a type list. @c ComparisonPredicate<T1,T2>::value should be a @c constexpr @c bool that is @c true if @c
/// T1 is ordered before @c T2, and @c false otherwise, for any pair of types @c T1 and @c T2 from the type list. The @c
/// type member is an alias to the resulting type list, with the elements sorted according to the comparator, such that
/// if @c ComparisonPredicate<T1,T2>::value is @c true, then @c T1 is before @c T2 in the resulting list. If neither @c
/// ComparisonPredicate<T1,T2>::value nor @c ComparisonPredicate<T2,T1>::value is @c true then the whichever is first in
/// @c TypeList appears first in the result.
///
/// @tparam TypeList The type list to sort
/// @tparam ComparisonPredicate A template to compare two types
template <typename TypeList, template <typename, typename> class ComparisonPredicate>
struct sort {
  // parasoft-begin-suppress CERT_CPP-DCL56-a-3 "False positive: variable is initialized"
  // parasoft-begin-suppress AUTOSAR-A3_3_2-a-2 "False positive: initializer is constant"
  /// @brief Always-false value to indicate that @c TypeList is not a type list
  static constexpr bool tl_is_type_list{always_false_v<TypeList>};
  // parasoft-end-suppress AUTOSAR-A3_3_2-a-2
  // parasoft-end-suppress CERT_CPP-DCL56-a-3
  static_assert(
      tl_is_type_list,
      "The first parameter must be an instantiation of something that meets the type list concept"
  );
};

/// Internal namespace for the type list sort algorithm
namespace sort_detail {

/// @brief Internal class template to merge two sorted type lists into one. If an element from the first list is
/// compared to an element from the second list, and neither is before the other when compared with the @c
/// ComparisonPredicate, then the element from the first list appears first in the output.
///
/// @tparam ComparisonPredicate The metafunction to compare two types
/// @tparam FirstList The first type list
/// @tparam SecondList The second type list
template <template <typename, typename> class ComparisonPredicate, typename FirstList, typename SecondList>
struct merge_sorted_lists_impl;

/// @brief Internal class template to merge two sorted type lists into one, and prepend the specified additional
/// element.
///
/// This is a helper to avoid instantiating the recursive merge if not required.
///
/// @tparam ComparisonPredicate The metafunction to compare two types
/// @tparam PrependFirst The type to use as the first element before the merged elements
/// @tparam FirstList The first type list
/// @tparam SecondList The second type list
template <
    template <typename, typename>
    class ComparisonPredicate,
    typename PrependFirst,
    typename FirstList,
    typename SecondList>
struct merge_sorted_lists_helper {
  /// @brief The type of the resulting list, with @c PrependFirst before the result of merging @c FirstList and @c
  /// SecondList
  using type = concat_t<
      type_list<PrependFirst>,
      typename merge_sorted_lists_impl<ComparisonPredicate, FirstList, SecondList>::type>;
};

/// @brief Internal class template to merge two sorted type lists into one.
///
/// This specialization handles the case that each list has at least one element
///
/// @tparam ComparisonPredicate The metafunction to compare two types
/// @tparam FirstHead The first element of the first type list
/// @tparam RemainingFirst The remaining elements of the first type list
/// @tparam SecondHead The first element of the second type list
/// @tparam RemainingSecond The remaining elements of the second type list
template <
    template <typename, typename>
    class ComparisonPredicate,
    typename FirstHead,
    typename... RemainingFirst,
    typename SecondHead,
    typename... RemainingSecond>
struct merge_sorted_lists_impl<
    ComparisonPredicate,
    type_list<FirstHead, RemainingFirst...>,
    type_list<SecondHead, RemainingSecond...>> {
  /// @brief The merged list
  using type = typename std::conditional_t<
      !ComparisonPredicate<SecondHead, FirstHead>::value,
      merge_sorted_lists_helper<
          ComparisonPredicate,
          FirstHead,
          type_list<RemainingFirst...>,
          type_list<SecondHead, RemainingSecond...>>,
      merge_sorted_lists_helper<
          ComparisonPredicate,
          SecondHead,
          type_list<FirstHead, RemainingFirst...>,
          type_list<RemainingSecond...>>>::type;
};

/// @brief Internal class template to merge two sorted type lists into one.
///
/// This specialization handles the case that the second list is empty
///
/// @tparam ComparisonPredicate The metafunction to compare two types
/// @tparam FirstList The first list to merge
template <template <typename, typename> class ComparisonPredicate, typename FirstList>
struct merge_sorted_lists_impl<ComparisonPredicate, FirstList, type_list<>> {
  /// @brief The merged list
  using type = FirstList;
};

/// @brief Internal class template to merge two sorted type lists into one.
///
/// This specialization handles the case that the first list is empty
///
/// @tparam ComparisonPredicate The metafunction to compare two types
/// @tparam SecondList The second list to merge
template <template <typename, typename> class ComparisonPredicate, typename SecondList>
struct merge_sorted_lists_impl<ComparisonPredicate, type_list<>, SecondList> {
  /// @brief The merged list
  using type = SecondList;
};

/// @brief Internal implementation class for sorting a list of types
/// @tparam ComparisonPredicate A template to compare two types
/// @tparam Types The types to sort
template <template <typename, typename> class ComparisonPredicate, typename... Types>
struct sort_impl;

/// @brief Internal implementation class for sorting an empty list of types
/// @tparam ComparisonPredicate A template to compare two types
template <template <typename, typename> class ComparisonPredicate>
struct sort_impl<ComparisonPredicate> {
  /// @brief The sorted list
  using type = type_list<>;
};

/// @brief Internal implementation class for sorting a list of types with one element
/// @tparam ComparisonPredicate A template to compare two types
/// @tparam First The single type in the list
template <template <typename, typename> class ComparisonPredicate, typename First>
struct sort_impl<ComparisonPredicate, First> {
  /// @brief The sorted list
  using type = type_list<First>;
};

/// @brief Internal implementation class for sorting a list of types with two elements
/// @tparam ComparisonPredicate A template to compare two types
/// @tparam First The first type in the list
/// @tparam Second The second type in the list
template <template <typename, typename> class ComparisonPredicate, typename First, typename Second>
struct sort_impl<ComparisonPredicate, First, Second> {
  /// @brief @c true if the second element sorts first, @c false otherwise
  static constexpr bool second_before_first{ComparisonPredicate<Second, First>::value};
  /// @brief The sorted list
  using type = type_list<
      std::conditional_t<second_before_first, Second, First>,
      std::conditional_t<second_before_first, First, Second>>;
};

/// @brief Internal implementation class for sorting a list of types with at least three elements.  Sorts the first 3
/// elements by direct comparisons, sorts the remainder of the elements by a recursive instantiation, and then merges
/// the results.
///
/// @tparam ComparisonPredicate A template to compare two types
/// @tparam First The first type in the list
/// @tparam Second The second type in the list
/// @tparam Third The third type in the list
/// @tparam RemainingTypes The remaining types in the list
template <
    template <typename, typename>
    class ComparisonPredicate,
    typename First,
    typename Second,
    typename Third,
    typename... RemainingTypes>
struct sort_impl<ComparisonPredicate, First, Second, Third, RemainingTypes...> {
  /// @brief @c true if the second element is ordered before the first, @c false otherwise
  static constexpr bool second_before_first{ComparisonPredicate<Second, First>::value};
  /// @brief @c true if the third element is ordered before the first, @c false otherwise
  static constexpr bool third_before_first{ComparisonPredicate<Third, First>::value};
  /// @brief @c true if the third element is ordered before the second, @c false otherwise
  static constexpr bool third_before_second{ComparisonPredicate<Third, Second>::value};

  /// @brief The first element in the first 3
  using first_element = typename std::conditional_t<         //
      second_before_first,                                   //
      std::conditional<third_before_second, Third, Second>,  //
      std::conditional<third_before_first, Third, First>>::type;

  /// @brief The second element in the first 3
  using second_element = typename std::conditional_t<
      second_before_first,
      std::conditional<third_before_second, Second, std::conditional_t<third_before_first, Third, First>>,
      std::conditional<third_before_first, First, std::conditional_t<third_before_second, Third, Second>>>::type;

  /// @brief The third element in the first 3
  using third_element = typename std::conditional_t<         //
      third_before_first,                                    //
      std::conditional<second_before_first, First, Second>,  //
      std::conditional<third_before_second, Second, Third>>::type;

  /// @brief The result of sorting the first 3 elements
  using first_three_sorted = type_list<first_element, second_element, third_element>;

  /// @brief The result of sorting the remainder of the elements
  using sorted_remainder = typename sort_impl<ComparisonPredicate, RemainingTypes...>::type;

  /// @brief The final sorted list
  using type = typename merge_sorted_lists_impl<ComparisonPredicate, first_three_sorted, sorted_remainder>::type;
};

}  // namespace sort_detail

/// @brief Sort a type list.
/// The @c type member type is an alias for an instantiation of @c TypeList holding the types in @c Types sorted
/// according to
/// @c ComparisonPredicate
/// @tparam TypeList The type list template holding the types
/// @tparam Types The types to sort
/// @tparam ComparisonPredicate A template to compare two types
template <
    template <typename...>
    class TypeList,
    typename... Types,
    template <typename, typename>
    class ComparisonPredicate>
struct sort<TypeList<Types...>, ComparisonPredicate> {
  /// @brief An alias for an instantiation of @c TypeList holding the sorted list of types
  using type = apply_all_t<typename sort_detail::sort_impl<ComparisonPredicate, Types...>::type, TypeList>;
};

/// @brief A type alias for a type list holding the elements of @c TypeList sorted by @c ComparisonPredicate . See @c
/// arene::base::type_lists::sort for details.
///
/// @tparam TypeList The type list to sort
/// @tparam ComparisonPredicate A template to compare two types
template <typename TypeList, template <typename, typename> class ComparisonPredicate>
using sort_t = typename sort<TypeList, ComparisonPredicate>::type;

}  // namespace type_lists
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_SORT_HPP_
