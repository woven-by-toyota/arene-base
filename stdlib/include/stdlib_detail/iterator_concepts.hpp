// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ITERATOR_CONCEPTS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ITERATOR_CONCEPTS_HPP_

#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "stdlib/include/stdlib_detail/add_rvalue_reference.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/is_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_convertible.hpp"
#include "stdlib/include/stdlib_detail/is_copy_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_default_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_same.hpp"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// Implementations of iterator concepts used to constrain standard algorithms.
// TODO: This could be replaced by the arene::base defined iterator concepts.

namespace std {
namespace internal {

/// @brief Internal type trait to check if a type supports the necessary input iterator operations. @c true if it does,
/// @c false otherwise
/// @tparam InputIterator The type to check
template <typename InputIterator>
extern constexpr bool has_basic_input_iterator_operations_v{arene::base::is_input_iterator_v<InputIterator>};

/// @brief Internal type trait to check if a type supports the necessary input iterator operations without throwing. @c
/// true if it does, @c false otherwise
/// @tparam InputIterator The type to check
template <typename InputIterator, typename = arene::base::constraints<>>
extern constexpr bool has_nothrow_basic_input_iterator_operations_v{false};

/// @brief Internal type trait to check if a type supports the necessary input iterator operations without throwing. @c
/// true if it does, @c false otherwise
/// @tparam InputIterator The type to check
template <typename InputIterator>
extern constexpr bool has_nothrow_basic_input_iterator_operations_v<
    InputIterator,
    arene::base::constraints<enable_if_t<has_basic_input_iterator_operations_v<InputIterator>>>> =
    noexcept(++declval<InputIterator&>())&& noexcept(*declval<InputIterator&>()
    )&& noexcept(declval<InputIterator&>() != declval<InputIterator const&>()) &&
    is_nothrow_copy_constructible_v<InputIterator>;

/// @brief Internal type trait to check if a type supports the necessary output iterator operations. @c  true if it
/// does, @c false otherwise
/// @tparam OutputIterator The type to check
template <typename OutputIterator>
extern constexpr bool has_basic_output_iterator_operations_v{arene::base::is_output_iterator_v<OutputIterator>};

/// @brief Internal type trait to check if a type supports the necessary output iterator operations without throwing. @c
/// true if it does, @c false otherwise
/// @tparam OutputIterator The type to check
template <typename OutputIterator, typename = arene::base::constraints<>>
extern constexpr bool has_nothrow_basic_output_iterator_operations_v{false};

/// @brief Internal type trait to check if a type supports the necessary output iterator operations without throwing. @c
/// true if it does, @c false otherwise
/// @tparam OutputIterator The type to check
template <typename OutputIterator>
extern constexpr bool has_nothrow_basic_output_iterator_operations_v<
    OutputIterator,
    arene::base::constraints<enable_if_t<has_basic_output_iterator_operations_v<OutputIterator>>>> =
    noexcept(++declval<OutputIterator&>())&& noexcept(*declval<OutputIterator&>()) &&
    is_nothrow_copy_constructible_v<OutputIterator>;

/// @brief Internal type trait to check if a type supports the necessary bidirectional iterator operations without
/// throwing. @c true if it does, @c false otherwise
/// @tparam BidirIt The type to check
template <typename BidirIt, typename = arene::base::constraints<>>
extern constexpr bool has_nothrow_basic_bidirectional_iterator_operations_v{false};

/// @brief Internal type trait to check if a type supports the necessary bidirectional iterator operations without
/// throwing. @c true if it does, @c false otherwise
/// @tparam BidirIt The type to check
template <typename BidirIt>
extern constexpr bool has_nothrow_basic_bidirectional_iterator_operations_v<
    BidirIt,
    arene::base::constraints<enable_if_t<has_nothrow_basic_input_iterator_operations_v<BidirIt>>>> =
    noexcept(--declval<BidirIt&>());

/// @brief Internal type trait to check if the values from a source iterator can be move-assigned to the destination
/// iterator. @c true if possible, @c false otherwise
/// @tparam SourceIterator The type of the source iterator
/// @tparam DestinationIterator The type of the target iterator
template <typename SourceIterator, typename DestinationIterator, typename = arene::base::constraints<>>
extern constexpr bool iterator_move_assignable_v{false};

/// @brief Internal type trait to check if the values from an input iterator can be move-assigned to the destination
/// iterator. @c true if possible, @c false otherwise
/// @tparam SourceIterator The type of the source iterator
/// @tparam DestinationIterator The type of the target iterator
template <typename SourceIterator, typename DestinationIterator, typename = arene::base::constraints<>>
extern constexpr bool is_indirectly_move_assignable_v{false};

/// @brief Internal type trait to check if the values from an input iterator can be move-assigned to the destination
/// iterator. @c true if possible, @c false otherwise
/// @tparam SourceIterator The type of the source iterator
/// @tparam DestinationIterator The type of the target iterator
template <typename SourceIterator, typename DestinationIterator>
extern constexpr bool is_indirectly_move_assignable_v<
    SourceIterator,
    DestinationIterator,
    arene::base::constraints<enable_if_t<is_assignable_v<
        arene::base::algorithm_detail::iter_reference_t<DestinationIterator>,
        add_rvalue_reference_t<arene::base::algorithm_detail::iter_reference_t<SourceIterator>>>>>> = true;

/// @brief Internal type trait to check if the values from an input iterator can be copy-assigned to the destination
/// iterator. @c true if possible, @c false otherwise
/// @tparam SourceIterator The type of the source iterator
/// @tparam DestinationIterator The type of the target iterator
template <typename SourceIterator, typename DestinationIterator, typename = arene::base::constraints<>>
extern constexpr bool is_indirectly_copy_assignable_v{false};

/// @brief Internal type trait to check if the values from an input iterator can be copy-assigned to the destination
/// iterator. @c true if possible, @c false otherwise
/// @tparam SourceIterator The type of the source iterator
/// @tparam DestinationIterator The type of the target iterator
template <typename SourceIterator, typename DestinationIterator>
extern constexpr bool is_indirectly_copy_assignable_v<
    SourceIterator,
    DestinationIterator,
    arene::base::constraints<enable_if_t<is_assignable_v<
        arene::base::algorithm_detail::iter_reference_t<DestinationIterator>,
        arene::base::algorithm_detail::iter_reference_t<SourceIterator>>>>> = true;

/// @brief Internal type trait to check if the values from an input iterator can be copy-assigned to the destination
/// iterator without throwing. @c true if possible, @c false otherwise
/// @tparam SourceIterator The type of the source iterator
/// @tparam DestinationIterator The type of the target iterator
template <typename SourceIterator, typename DestinationIterator, typename = arene::base::constraints<>>
extern constexpr bool is_indirectly_nothrow_copy_assignable_v{false};

/// @brief Internal type trait to check if the values from an input iterator can be copy-assigned to the destination
/// iterator without throwing. @c true if possible, @c false otherwise
/// @tparam SourceIterator The type of the source iterator
/// @tparam DestinationIterator The type of the target iterator
template <typename SourceIterator, typename DestinationIterator>
extern constexpr bool is_indirectly_nothrow_copy_assignable_v<
    SourceIterator,
    DestinationIterator,
    arene::base::constraints<enable_if_t<is_nothrow_assignable_v<
        arene::base::algorithm_detail::iter_reference_t<DestinationIterator>,
        arene::base::algorithm_detail::iter_reference_t<SourceIterator>>>>> = true;

/// @brief Internal type trait to check if a type supports the necessary forward iterator operations. @c true if it
/// does,
/// @c false otherwise
/// @tparam ForwardIterator The type to check
template <typename ForwardIterator>
extern constexpr bool has_basic_forward_iterator_operations_v{arene::base::is_forward_iterator_v<ForwardIterator>};

/// @brief Internal type trait to check if a type supports the necessary forward iterator operations. @c true if it
/// does,
/// @c false otherwise
/// @tparam ForwardIterator The type to check
template <typename ForwardIterator, typename = arene::base::constraints<>>
extern constexpr bool has_nothrow_basic_forward_iterator_operations_v{false};

/// @brief Internal type trait to check if a type supports the necessary forward iterator operations. @c true if it
/// does,
/// @c false otherwise
/// @tparam ForwardIterator The type to check
template <typename ForwardIterator>
extern constexpr bool has_nothrow_basic_forward_iterator_operations_v<
    ForwardIterator,
    arene::base::constraints<enable_if_t<has_basic_forward_iterator_operations_v<ForwardIterator>>>> =
    has_nothrow_basic_input_iterator_operations_v<ForwardIterator>&& noexcept(*declval<ForwardIterator&>()++) &&
    is_nothrow_default_constructible_v<ForwardIterator>;

}  // namespace internal
}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ITERATOR_CONCEPTS_HPP_
