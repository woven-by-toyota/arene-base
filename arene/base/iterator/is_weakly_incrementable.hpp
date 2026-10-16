// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_IS_WEAKLY_INCREMENTABLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_IS_WEAKLY_INCREMENTABLE_HPP_

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/iterator/iter_difference_t.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_traits/is_movable.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

namespace arene {
namespace base {

namespace weakly_incrementable_detail {

/// @brief Computes the type resulting from the post-increment operation on the given iterator type.
/// @tparam Itr The iterator type for which to compute the post-increment result type.
template <typename Itr>
using post_increment_t = decltype(std::declval<Itr&>()++);

/// @brief Checks if the given iterator type has a valid post-increment operation.
/// @tparam Itr The iterator type to be checked.
template <typename Itr>
constexpr bool has_post_increment_v = substitution_succeeds<post_increment_t, Itr>;

/// @brief Computes the type resulting from the pre-increment operation on the given iterator type.
/// @tparam Itr The iterator type for which to compute the pre-increment result type.
template <typename Itr>
using pre_increment_t = decltype(++std::declval<Itr&>());

/// @brief Implementation helper for determining if an iterator type models the weakly incrementable concept.
/// @tparam Itr The iterator type to be checked.
/// @note The default specialization is false
/// @see ::arene::base::is_weakly_incrementable
template <typename Itr, typename = constraints<>>
class weakly_incrementable_impl : public std::false_type {};

/// @brief Specialization of weakly_incrementable_impl for iterators that satisfy all weakly incrementable requirements.
/// @tparam Itr The iterator type to be checked.
/// @see ::arene::base::is_weakly_incrementable<Itr>
template <typename Itr>
class weakly_incrementable_impl<
    Itr,
    constraints<
        std::enable_if_t<is_movable_v<Itr>>,
        std::enable_if_t<std::is_same<pre_increment_t<Itr>, Itr&>::value>,
        std::enable_if_t<has_post_increment_v<Itr>>,
        std::enable_if_t<std::is_integral<iter_difference_t<Itr>>::value>,
        std::enable_if_t<std::is_signed<iter_difference_t<Itr>>::value>>> : public std::true_type {};

}  // namespace weakly_incrementable_detail

/// @brief A trait which validates if an iterator models pre/post-increment operations without requiring equality
/// preservation.
///
/// @tparam Itr The type of the iterator to be checked.
///
/// An iterator models the weakly incrementable concept if all of the following are true:
/// 1. The iterator is movable.
/// 2. The iterator supports pre-increment (++itr) returning a reference to itself.
/// 3. The iterator supports post-increment (itr++).
/// 4. The iterator has a valid difference type (iter_difference_t<Itr>) and it is a signed integral type.
///
/// @note Condition 4 is a strengthening compared to @c std::is_weakly_incrementable , which requires a valid difference
/// type which is only a _signed integer like_ type. Implementing the integer-like requirement is extremely difficult in
/// C++14, so the stricter @c std::is_signed and @c std::is_integral traits are used instead.
template <typename Itr>
class is_weakly_incrementable : public weakly_incrementable_detail::weakly_incrementable_impl<Itr> {};

/// @brief Predicate trait for checking if an iterator models the weakly incrementable concept.
/// @tparam Itr The iterator type to be checked.
/// @return bool @c is_weakly_incrementable<Itr>::value
/// @see ::arene::base::is_weakly_incrementable<Itr>
template <typename Itr>
extern constexpr bool is_weakly_incrementable_v = is_weakly_incrementable<Itr>::value;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_IS_WEAKLY_INCREMENTABLE_HPP_
