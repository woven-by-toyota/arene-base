// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_IS_INDIRECTLY_READABLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_IS_INDIRECTLY_READABLE_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/iterator/iter_move.hpp"
#include "arene/base/iterator/iter_reference_t.hpp"
#include "arene/base/iterator/iter_rvalue_reference_t.hpp"
#include "arene/base/iterator/iter_value_t.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/type_traits/common_reference.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

namespace arene {
namespace base {
namespace is_indirectly_readable_detail {

/// @brief Computes the type resulting from dereferencing a const lvalue of the given iterator type.
/// @tparam Itr The iterator type for which to determine the dereference result type.
template <typename Itr>
using const_reference_t = decltype(*std::declval<Itr const&>());

/// @brief Computes the type resulting from moving from a const lvalue of the given iterator type.
/// @tparam Itr The iterator type for which to determine the iter_move result type.
template <typename Itr>
using const_iter_move_t = decltype(ranges::iter_move(std::declval<Itr const&>()));

/// @brief helper variable template for implementing @c is_indirectly_readable_v for when one of the required types is
/// not defined
/// @tparam Itr The iterator type
/// @see is_indirectly_readable_v
/// The default implementation is false
template <typename Itr, typename = constraints<>>
extern constexpr bool is_indirectly_readable{false};

/// @brief helper variable template for implementing @c is_indirectly_readable_v for when all of the required types are
/// defined
/// @tparam Itr The iterator type
/// @see is_indirectly_readable_v
template <typename Itr>
// parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variable 'is_indirectly_readable' is initialized"
extern constexpr bool is_indirectly_readable<
    Itr,
    constraints<
        std::enable_if_t<substitution_succeeds<iter_value_t, Itr>>,
        std::enable_if_t<substitution_succeeds<iter_reference_t, Itr>>,
        std::enable_if_t<substitution_succeeds<iter_rvalue_reference_t, Itr>>,
        std::enable_if_t<substitution_succeeds<const_reference_t, Itr>>,
        std::enable_if_t<substitution_succeeds<const_iter_move_t, Itr>>>>{
    //
    std::is_same<const_reference_t<Itr>, iter_reference_t<Itr>>::value &&
    std::is_same<const_iter_move_t<Itr>, iter_rvalue_reference_t<Itr>>::value &&
    has_common_reference_with_v<iter_reference_t<Itr>&&, iter_value_t<Itr>&> &&
    has_common_reference_with_v<iter_reference_t<Itr>&&, iter_rvalue_reference_t<Itr>&&> &&
    has_common_reference_with_v<iter_rvalue_reference_t<Itr>&&, iter_value_t<Itr> const&>
};
// parasoft-end-suppress CERT_CPP-DCL56-a

}  // namespace is_indirectly_readable_detail

/// @brief Query trait to determine if an iterator is indirectly readable. A backport of the
/// @c [std::indirectly_readable](https://en.cppreference.com/w/cpp/iterator/indirectly_readable) concept.
/// @tparam Itr The iterator type
/// @return bool @c true if all of the following are satisfied, else @c false :
/// - @c iter_value_t<Itr> is defined
/// - @c iter_reference_t<Itr> is defined
/// - @c iter_rvalue_reference_t<Itr> is defined
/// - <c>decltype(*std::declval<Itr const&>())</c> is the same type as @c iter_reference_t<Itr>
/// - <c>decltype(ranges::iter_move(std::declval<Itr const&>()))</c> is the same type as @c iter_rvalue_reference_t<Itr>
/// - @c iter_reference_t<Itr>&& and @c iter_value_t<Itr>& satisfy @c has_common_reference_with_v
/// - @c iter_reference_t<Itr>&& and @c iter_rvalue_reference_t<Itr>&& satisfy @c has_common_reference_with_v
/// - @c iter_rvalue_reference_t<Itr>&& and <c>iter_value_t<Itr> const&</c> satisfy @c has_common_reference_with_v
template <typename Itr>
extern constexpr bool
    is_indirectly_readable_v{is_indirectly_readable_detail::is_indirectly_readable<remove_cvref_t<Itr>>};

/// @brief Trait to determine if an iterator is indirectly readable.
/// @tparam Itr The iterator type
/// The member @c value is equivalent to @c is_indirectly_readable_v<Itr>.
template <typename Itr>
class is_indirectly_readable : public std::integral_constant<bool, is_indirectly_readable_v<Itr>> {};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_IS_INDIRECTLY_READABLE_HPP_
