// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_REMOVE_DUPLICATES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_REMOVE_DUPLICATES_HPP_

// IWYU pragma: private, include "arene/base/type_list.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

#include "arene/base/algorithm/copy_if.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/functional/bind_back.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/not_equal_to.hpp"
#include "arene/base/type_list/at.hpp"
#include "arene/base/type_list/size.hpp"
#include "arene/base/type_traits/index_of.hpp"
#include "arene/base/type_traits/type_identity.hpp"

namespace arene {
namespace base {

namespace type_lists {

/// @cond INTERNAL

namespace remove_duplicates_detail {

/// @brief collapse L0 (aka TypeList0<T...>) to TypeList0<U...>, where U... does
/// not contain any duplicates of the same type
template <class L0>
struct remove_duplicates_impl;

// The overall approach of this algorithm is:
// Starting from a list of types Ts...
//
// * create a masked index array where the value is either:
// ** I if T is the *last* occurence of T in Ts...
// ** dynamic_extent if T is *not* the last occurence of T in Ts...
//
// * create a unique index array by dropping all the dynamic_extent entries
//
// * create a new type list by mapping the indices back to the type in Ts...

/// @brief return the number of elements in the range matching a value
/// @tparam Range range type
/// @param range range of elements to examine
/// @param value value to count
/// @return number of elements in @c range equal to @c value
///
/// @note arene::base::count is not defined
///
template <class Range>
constexpr auto count(Range const& range, std::size_t value)  //
    -> std::size_t                                           //
{
  std::size_t num{};
  // parasoft-begin-suppress AUTOSAR-A7_1_5-a "False Positive: 'element' has return type of range element, which may
  // be non-fundamental"
  for (auto const& elem : range) {
    if (elem == value) {
      ++num;
    }
  }
  // parasoft-end-suppress AUTOSAR-A7_1_5-a
  return num;
}

// parasoft-begin-suppress AUTOSAR-A11_0_2-a "It is consistent with developer
// expectations that a type should be defined as a struct if it has no
// invariants to maintain"

/// @brief an array of indices
/// @tparam N array size
///
/// Simple wrapper around a C array.
///
/// @note This avoids using 'arene::base::array' to reduce dependencies.
///
template <std::size_t N>
struct index_array {
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: this identifier does not hide anything"
  /// @brief value_type
  using value_type = std::size_t;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  // parasoft-begin-suppress AUTOSAR-A18_1_1-a "This must work in contexts where std::array is not available."
  /// @brief array values
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays,misc-non-private-member-variables-in-classes)
  value_type values[N]{};
  // parasoft-end-suppress AUTOSAR-A18_1_1-a

  /// @brief iterator to the beginning of the range
  /// @return iterator to the beginning of the range
  constexpr auto begin() noexcept -> value_type* { return values; }
  /// @brief iterator to the beginning of the range
  /// @return iterator to the beginning of the range
  constexpr auto begin() const noexcept -> value_type const* { return values; }

  // parasoft-begin-suppress AUTOSAR-M5_0_15-a "The incremented pointer does point to an array"
  /// @brief iterator past the end of the range
  /// @return iterator past the end of the range
  constexpr auto end() noexcept -> value_type* { return begin() + N; }
  /// @brief iterator past the end of the range
  /// @return iterator past the end of the range
  constexpr auto end() const noexcept -> value_type const* { return begin() + N; }
  // parasoft-end-suppress AUTOSAR-M5_0_15-a
};

// parasoft-end-suppress AUTOSAR-A11_0_2-a

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief returns indices of the last occurence of each type in a parameter pack
/// @tparam Ts types
/// @tparam Is indices
/// @return a masked array of indices
///
/// Returns an array of indices. For each type @c T in @c Ts..., if @c T is the
/// *last* occurence, the element in the returned array is equal to its index in
/// @c Ts... . Otherwise the element is equal to @c dynamic_extent.
///
/// For example, the returned array for type sequence 'int, double, double', is
/// '[0, dynamic_extent, 2]'.
///
template <class... Ts, std::size_t... Is>
constexpr auto masked_indices_of_impl(std::index_sequence<Is...>)  //
    -> index_array<sizeof...(Ts)>                                  //
{                                                                  //
   // parasoft-begin-suppress AUTOSAR-A5_16_1-a "Alternatives to ?: make the code less readable and harder to maintain"
   // parasoft-begin-suppress AUTOSAR-M8_5_2-a "False positive: there is no subobject here that can be initialized"
  return {                                                  //
          ((Is == arene::base::last_index_of_v<Ts, Ts...>)  //
               ? Is
               : dynamic_extent)...
  };
  // parasoft-end-suppress AUTOSAR-M8_5_2-a
  // parasoft-end-suppress AUTOSAR-A5_16_1-a
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief masked array containing indices of the last occurence of each type in a type list
/// @note The primary template is a dummy, 'remove_duplicates' will always use
///   specialization below
///
template <class>
extern constexpr auto masked_indices_of = nullptr;

/// @brief masked array containing indices of the last occurence of each type in a type list
/// @tparam List type list
/// @tparam Ts types
///
template <template <class...> class List, class... Ts>
extern constexpr auto masked_indices_of<List<Ts...>> =  //
    masked_indices_of_impl<Ts...>(std::index_sequence_for<Ts...>{});

/// @brief obtains the unique number of types in a type list
/// @tparam List type list
///
template <class List>
extern constexpr auto unique_count_v = type_lists::size_v<List> - count(masked_indices_of<List>, dynamic_extent);

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: cannot apply static here"
/// @brief returns indices of the last occurence of each type in a type list
/// @tparam Ts types
/// @tparam Is indices
/// @return an array of indices
///
/// Returns an array of indices. Each index is the last occurence of a type @c T
/// in @c Ts... .
///
/// For example, the returned array for type list '<int, double, double>', is
/// '[0, 2]'.
///
template <class... Ts>
constexpr auto unique_indices_impl() -> index_array<unique_count_v<Ts...>>  //
{
  auto filtered = index_array<unique_count_v<Ts...>>{};
  std::ignore = arene::base::copy_if(  //
      masked_indices_of<Ts...>.begin(),
      masked_indices_of<Ts...>.end(),
      filtered.begin(),
      arene::base::bind_back(std::not_equal_to<>{}, dynamic_extent)
  );
  return filtered;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief array containing indices of the last occurence of each type in a type list
/// @tparam Ts types
///
template <class... Ts>
extern constexpr auto unique_indices = unique_indices_impl<Ts...>();

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: cannot apply static here"
/// @brief implementation function for @c remove_duplicates
/// @tparam List type list
/// @tparam Ts types
/// @tparam Js unique indices
/// @return type list with duplicates removed
///
template <template <class...> class List, class... Ts, std::size_t... Js>
auto remove_duplicates_from_impl(std::index_sequence<Js...>)  //
    -> List<                                                  //
        type_lists::at_t<                                     //
            List<Ts...>,
            unique_indices<List<Ts...>>.begin()[Js]>...>;
// parasoft-end-suppress AUTOSAR-M3_3_2-a

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: cannot apply static here"
/// @brief implementation function for @c remove_duplicates
/// @tparam List type list
/// @tparam T0 first type
/// @tparam Tn remaining types
/// @return type list with duplicates removed
///
template <template <class...> class List, class T0, class... Tn>
auto remove_duplicates_from(type_identity<List<T0, Tn...>>)              //
    -> decltype(                                                         //
        remove_duplicates_from_impl<List, T0, Tn...>(                    //
            std::make_index_sequence<unique_count_v<List<T0, Tn...>>>{}  //
        )
    );
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief implementation function for @c remove_duplicates
/// @tparam List type list
/// @return empty type list
/// @note overload for an empty type list
///
template <template <class...> class List>
auto remove_duplicates_from(type_identity<List<>>) -> List<>;

/// @brief implementation type for @c remove_duplicates
/// @tparam List type list
/// @tparam Ts types
///
template <template <class...> class List, class... Ts>
struct remove_duplicates_impl<List<Ts...>> {
  static_assert(
      sizeof...(Ts) != dynamic_extent,  //
      "cannot handle 'std::size_t(-1)' number of types"
  );

  /// @brief type list with duplicates removed
  using type = decltype(remove_duplicates_from(type_identity<List<Ts...>>{}));
};

}  // namespace remove_duplicates_detail

/// @endcond

/// @brief Filter a type-list to remove duplicates of elements.
/// @details Only keeps the *last* instance of a type in the type list.
///
/// @tparam L0 An instantiation of a type-list that holds the types to filter
/// @pre @c L0 is a type-list
/// @return TypeList<T...>
template <class L0>
using remove_duplicates = arene::base::type_lists::remove_duplicates_detail::remove_duplicates_impl<L0>;

/// @brief Filter a type-list to remove duplicates of elements.
/// @details Only keeps the *last* instance of a type in the type list.
///
/// @tparam L0 An instantiation of a type-list that holds the types to filter
/// @pre @c L0 is a type-list
/// @return TypeList<T...>
template <class L0>
using remove_duplicates_t = typename remove_duplicates<L0>::type;

}  // namespace type_lists

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_REMOVE_DUPLICATES_HPP_
