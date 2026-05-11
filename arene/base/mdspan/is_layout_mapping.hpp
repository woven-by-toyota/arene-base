// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_IS_LAYOUT_MAPPING_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_IS_LAYOUT_MAPPING_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_copyable.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace is_layout_mapping_detail {

/// @brief Helper trait to indicate if the given type is invocable with the number of index parameters equal to the rank
/// of @c T::extents_type returning an instance of @c T::index_type
///
/// The value is @c true if @c T is invocable in this way, @c false otherwise
/// @tparam T The type being checked
template <
    typename T,
    typename = std::make_integer_sequence<typename T::index_type, T::extents_type::rank()>,
    typename = constraints<>>
extern constexpr bool is_invocable_with_extents_indices_v = false;

/// @brief Helper trait to indicate if the given type is invocable with the number of index parameters equal to the rank
/// of @c T::extents_type returning an instance of @c T::index_type
///
/// The value is @c true if @c T is invocable in this way, @c false otherwise
/// @tparam T The type being checked
/// @tparam IndexType the index type of the extents
/// @tparam Indices A list of @c T::extents_type::rank() sample values of the @c IndexType
template <typename T, typename IndexType, IndexType... Indices>
extern constexpr bool is_invocable_with_extents_indices_v<
    T,
    std::integer_sequence<IndexType, Indices...>,
    constraints<std::enable_if_t<is_invocable_v<T const&, decltype(Indices)...>>>> =
    std::is_same<invoke_result_t<T const&, decltype(Indices)...>, IndexType>::value;

/// @brief Helper trait to indicate if the given type has all the basic properties required of a layout mapping:
/// - It must be copy constructible, and copy assignable
/// - It must be nothrow-move-constructible and nothrow-move-assignable
/// - It must be nothrow-swappable
/// - It must be equality comparable
/// - It must have the type aliases:
///   - @c T::extents_type must be an instance of @c extents
///   - @c T::index_type must be an alias for @c T::extents_type::index_type
///   - @c rank_type must be an alias for @c T::extents_type::rank_type
///   - @c layout_type must exist
/// - It must have the @c const member functions:
///   - @c extents() must return an instance of @c T::extents_type
///   - @c required_span_size() must return an instance of @c T::index_type
///   - @c is_unique() must return @c bool
///   - @c is_exhaustive() must return @c bool
///   - @c is_strided() must return @c bool
/// - It must have the @c static member functions:
///   - @c is_always_unique() must return @c bool
///   - @c is_always_exhaustive() must return @c bool
///   - @c is_always_strided() must return @c bool
/// - It must be invocable with @c T::extents_type::rank() indices of type @c T::index_type returning an instance of @c
///   T::index_type
///
/// The value is @c true if @c T has all these properties, @c false otherwise
/// @tparam T The type being checked
template <typename T, typename = constraints<>>
extern constexpr bool is_basic_layout_mapping_v = false;

/// @brief Helper trait to indicate if the given type has all the basic properties required of a layout mapping:
/// - It must be copy constructible, and copy assignable
/// - It must be nothrow-move-constructible and nothrow-move-assignable
/// - It must be nothrow-swappable
/// - It must be equality comparable
/// - It must have the type aliases:
///   - @c T::extents_type must be an instance of @c extents
///   - @c T::index_type must be an alias for @c T::extents_type::index_type
///   - @c rank_type must be an alias for @c T::extents_type::rank_type
///   - @c layout_type must exist
/// - It must have the @c const member functions:
///   - @c extents() must return an instance of @c T::extents_type
///   - @c required_span_size() must return an instance of @c T::index_type
///   - @c is_unique() must return @c bool
///   - @c is_exhaustive() must return @c bool
///   - @c is_strided() must return @c bool
/// - It must have the @c static member functions:
///   - @c is_always_unique() must be a constant expression returning @c bool
///   - @c is_always_exhaustive() must be a constant expression returning @c bool
///   - @c is_always_strided() must be a constant expression returning @c bool
/// - It must be invocable with @c T::extents_type::rank() indices of type @c T::index_type returning an instance of @c
///   T::index_type
///
/// The value is @c true if @c T has all these properties, @c false otherwise
/// @tparam T The type being checked
template <typename T>
extern constexpr bool is_basic_layout_mapping_v<
    T,
    constraints<
        typename T::extents_type,
        typename T::index_type,
        typename T::rank_type,
        typename T::layout_type,
        decltype(std::declval<T const&>().extents()),
        decltype(std::declval<T const&>().required_span_size()),
        decltype(std::declval<T const&>().is_unique()),
        decltype(std::declval<T const&>().is_exhaustive()),
        decltype(std::declval<T const&>().is_strided()),
        decltype(T::is_always_unique()),
        decltype(T::is_always_strided()),
        decltype(T::is_always_exhaustive()),
        std::enable_if_t<(static_cast<void>(T::is_always_unique()), true)>,
        std::enable_if_t<(static_cast<void>(T::is_always_strided()), true)>,
        std::enable_if_t<(static_cast<void>(T::is_always_exhaustive()), true)>,
        std::enable_if_t<is_extents_v<typename T::extents_type>>,
        std::enable_if_t<is_copyable_v<T>>,
        std::enable_if_t<std::is_nothrow_move_constructible<T>::value>,
        std::enable_if_t<std::is_nothrow_move_assignable<T>::value>,
        std::enable_if_t<is_nothrow_swappable_v<T>>,
        std::enable_if_t<is_equality_comparable_v<T>>>> =
    std::is_same<typename T::index_type, typename T::extents_type::index_type>::value &&
    std::is_same<typename T::rank_type, typename T::extents_type::rank_type>::value &&
    std::is_same<decltype(std::declval<T const&>().extents()), typename T::extents_type const&>::value &&
    std::is_same<decltype(std::declval<T const&>().required_span_size()), typename T::index_type>::value &&
    std::is_same<decltype(std::declval<T const&>().is_unique()), bool>::value &&
    std::is_same<decltype(std::declval<T const&>().is_exhaustive()), bool>::value &&
    std::is_same<decltype(std::declval<T const&>().is_strided()), bool>::value &&
    std::is_same<decltype(T::is_always_unique()), bool>::value &&
    std::is_same<decltype(T::is_always_exhaustive()), bool>::value &&
    std::is_same<decltype(T::is_always_strided()), bool>::value && is_invocable_with_extents_indices_v<T>;

/// @brief Sample extents type for checking layout mapping policy
// NOLINTNEXTLINE(readability-magic-numbers)
using sample_extents_1 = dextents<std::int32_t, 5>;
/// @brief Sample extents type for checking layout mapping policy
using sample_extents_2 = extents<std::uint16_t, 1, 2, 3, 4>;

/// @brief Helper trait to check if @c T is a valid layout mapping policy
///
/// A valid layout mapping policy must have a @c template member @c mapping such that @c T::mapping<some_extent> is a
/// valid layout mapping, such that @c T::mapping<some_extent>::extents_type is @c some_extent and @c
/// T::mapping<some_extent>::layout_type is @c T
///
/// The value is @c true if @c T has all these properties, @c false otherwise
/// @tparam T The type being checked
template <typename T, typename = constraints<>>
extern constexpr bool is_layout_mapping_policy_v = false;

/// @brief Helper trait to check if @c T is a valid layout mapping policy
///
/// A valid layout mapping policy must have a @c template member @c mapping such that @c T::mapping<some_extent> is a
/// valid layout mapping, such that @c T::mapping<some_extent>::extents_type is @c some_extent and @c
/// T::mapping<some_extent>::layout_type is @c T
///
/// The value is @c true if @c T has all these properties, @c false otherwise
/// @tparam T The type being checked
template <typename T>
extern constexpr bool is_layout_mapping_policy_v<
    T,
    constraints<
        std::enable_if_t<is_basic_layout_mapping_v<typename T::template mapping<sample_extents_1>>>,
        std::enable_if_t<
            std::is_same<typename T::template mapping<sample_extents_1>::extents_type, sample_extents_1>::value>,
        std::enable_if_t<std::is_same<typename T::template mapping<sample_extents_1>::layout_type, T>::value>,
        std::enable_if_t<is_basic_layout_mapping_v<typename T::template mapping<sample_extents_2>>>,
        std::enable_if_t<
            std::is_same<typename T::template mapping<sample_extents_2>::extents_type, sample_extents_2>::value>,
        std::enable_if_t<std::is_same<typename T::template mapping<sample_extents_2>::layout_type, T>::value>>> = true;

/// @brief Helper trait to indicate if the given type has all the basic properties required of a layout mapping:
/// - It must be copy constructible, and copy assignable
/// - It must be nothrow-move-constructible and nothrow-move-assignable
/// - It must be nothrow-swappable
/// - It must be equality comparable
/// - It must have the type aliases
///   - @c T::extents_type must be an instance of @c extents
///   - @c T::index_type must be an alias for @c T::extents_type::index_type
///   - @c T::rank_type must be an alias for @c T::extents_type::rank_type
///   - @c T::layout_type must be a valid layout mapping policy for @c T such that @c
///     T::layout_type::mapping<T::extents_type> is @c T
/// - It must have the @c const member functions:
///   - @c extents() must return an instance of @c T::extents_type
///   - @c required_span_size() must return an instance of @c T::index_type
///   - @c is_unique() must return @c bool
///   - @c is_exhaustive() must return @c bool
///   - @c is_strided() must return @c bool
/// - It must have the @c static member functions:
///   - @c is_always_unique() must return @c bool
///   - @c is_always_exhaustive() must return @c bool
///   - @c is_always_strided() must return @c bool
/// - It must be invocable with @c T::extents_type::rank() indices of type @c T::index_type returning an instance of @c
///   T::index_type
///
/// The value is @c true if @c T has all these properties, @c false otherwise
/// @tparam T The type being checked
template <typename T, typename = constraints<>>
extern constexpr bool is_layout_mapping_v = false;

/// @brief Helper trait to indicate if the given type has all the basic properties required of a layout mapping:
/// - It must be copy constructible, and copy assignable
/// - It must be nothrow-move-constructible and nothrow-move-assignable
/// - It must be nothrow-swappable
/// - It must be equality comparable
/// - It must have the type aliases:
///   - @c T::extents_type must be an instance of @c extents
///   - @c T::index_type must be an alias for @c T::extents_type::index_type
///   - @c T::rank_type must be an alias for @c T::extents_type::rank_type
///   - @c T::layout_type must be a valid layout mapping policy for @c T such that @c
///     T::layout_type::mapping<T::extents_type> is @c T
/// - It must have the @c const member functions:
///   - @c extents() must return an instance of @c T::extents_type
///   - @c required_span_size() must return an instance of @c T::index_type
///   - @c is_unique() must return @c bool
///   - @c is_exhaustive() must return @c bool
///   - @c is_strided() must return @c bool
/// - It must have the @c static member functions:
///   - @c is_always_unique() must return @c bool
///   - @c is_always_exhaustive() must return @c bool
///   - @c is_always_strided() must return @c bool
/// - It must be invocable with @c T::extents_type::rank() indices of type @c T::index_type returning an instance of @c
///   T::index_type
///
/// The value is @c true if @c T has all these properties, @c false otherwise
/// @tparam T The type being checked
template <typename T>
extern constexpr bool is_layout_mapping_v<
    T,
    constraints<
        std::enable_if_t<is_basic_layout_mapping_v<T>>,
        std::enable_if_t<
            std::is_same<typename T::layout_type::template mapping<typename T::extents_type>, T>::value>>> =
    is_layout_mapping_policy_v<typename T::layout_type>;

}  // namespace is_layout_mapping_detail

/// @brief Helper trait to indicate if the given type has all the basic properties required of a layout mapping:
/// - It must be copy constructible, and copy assignable
/// - It must be nothrow-move-constructible and nothrow-move-assignable
/// - It must be nothrow-swappable
/// - It must be equality comparable
/// - It must have the type aliases:
///   - @c T::extents_type must be an instance of @c extents
///   - @c T::index_type must be an alias for @c T::extents_type::index_type
///   - @c T::rank_type must be an alias for @c T::extents_type::rank_type
///   - @c T::layout_type must be a valid layout mapping policy for @c T such that @c
///     T::layout_type::mapping<T::extents_type> is @c T
/// - It must have the @c const member functions:
///   - @c extents() must return an instance of @c T::extents_type
///   - @c required_span_size() must return an instance of @c T::index_type
///   - @c is_unique() must return @c bool
///   - @c is_exhaustive() must return @c bool
///   - @c is_strided() must return @c bool
/// - It must have the @c static member functions:
///   - @c is_always_unique() must return @c bool
///   - @c is_always_exhaustive() must return @c bool
///   - @c is_always_strided() must return @c bool
/// - It must be invocable with @c T::extents_type::rank() indices of type @c T::index_type returning an instance of @c
///   T::index_type
///
/// The value is @c true if @c T has all these properties, @c false otherwise
/// @tparam T The type being checked
template <typename T>
extern constexpr bool is_layout_mapping_v = is_layout_mapping_detail::is_layout_mapping_v<T>;

/// @brief Helper trait to check if @c T is a valid layout mapping policy
///
/// A valid layout mapping policy must have a @c template member @c mapping such that @c T::mapping<some_extent> is a
/// valid layout mapping, such that @c T::mapping<some_extent>::extents_type is @c some_extent and @c
/// T::mapping<some_extent>::layout_type is @c T
///
/// The value is @c true if @c T has all these properties, @c false otherwise
/// @tparam T The type being checked
template <typename T>
extern constexpr bool is_layout_mapping_policy_v = is_layout_mapping_detail::is_layout_mapping_policy_v<T>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_IS_LAYOUT_MAPPING_HPP_
