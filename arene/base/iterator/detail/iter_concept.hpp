// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_ITER_CONCEPT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_ITER_CONCEPT_HPP_

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/type_traits/priority_tag.hpp"  // IWYU pragma: export

namespace arene {
namespace base {
namespace iterator_detail {
namespace iter_concept_detail {
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: cannot apply static here"

/// @brief helper to determine the iterator concept
/// @tparam I iterator type
/// @return <c>std::iterator_traits<I>::iterator_concept</c>
template <class I>
auto iter_concept(priority_tag<2>) -> typename std::iterator_traits<I>::iterator_concept;

/// @brief helper to determine the iterator concept
/// @tparam I iterator type
/// @return <c>I::iterator_concept</c>
template <class I>
auto iter_concept(priority_tag<1>) -> typename I::iterator_concept;

/// @brief helper to determine the iterator concept
/// @tparam I iterator type
/// @return <c>std::iterator_traits<I>::iterator_category</c>
template <class I>
auto iter_concept(priority_tag<0>) -> typename std::iterator_traits<I>::iterator_category;

// parasoft-end-suppress AUTOSAR-M3_3_2-a
}  // namespace iter_concept_detail

/// @brief determines the iterator concept tag for an iterator
/// @tparam I iterator type
///
/// This is an approximation of the exposition-only @c ITER_CONCEPT(I) from the
/// [iterator.concepts.general]. The tag is selected using the following
/// precedence:
/// 1. @c std::iterator_traits<I>::iterator_concept, if valid and names a type
/// 2. @c I::iterator_concept, if valid and names a type
/// 3. @c std::iterator_traits<I>::iterator_category, otherwise
///
/// See the notes below for cases where this trait diverges from the standard.
///
/// @note This trait diverges from the standard's @c ITER_CONCEPT(I). The
/// standard first computes @c ITER_TRAITS(I), which is either:
/// * @c I, when @c std::iterator_traits<I> is generated from the primary template
/// * @c std::iterator_traits<I> otherwise.
/// The @c iterator_concept and @c iterator_category lookups are then performed
/// against that single source, as opposed to clauses 1 and 2 in this
/// implementation.
///
/// @note Detecting whether @c std::iterator_traits<I> is a primary-template
/// specialization cannot be done portably. As a result, when a user-provided @c
/// std::iterator_traits<I> specialization omits @c iterator_concept while @c I
/// itself declares one, this trait yields @c I::iterator_concept whereas the
/// standard would fall through to the specialization's @c iterator_category.
///
/// @note The standard's @c std::random_access_iterator_tag fallback
/// ([iterator.concepts.general]/1.3) is not reproduced. It requires detecting a
/// primary-template specialization, which is not portable. It only applies to
/// iterators declaring neither @c iterator_concept nor @c iterator_category,
/// which are rare; such types resolve via the @c iterator_category lookup above
/// (or are ill-formed if they lack @c iterator_category entirely).
template <class I>
using iter_concept_t = decltype(iter_concept_detail::iter_concept<I>(priority_tag<2>{}));

}  // namespace iterator_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_DETAIL_ITER_CONCEPT_HPP_
