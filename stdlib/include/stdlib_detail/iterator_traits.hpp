// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ITERATOR_TRAITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ITERATOR_TRAITS_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <iterator>

#include "arene/base/constraints.hpp"
#include "stdlib/include/stdlib_detail/cstddef.hpp"
#include "stdlib/include/stdlib_detail/iterator_tags.hpp"

namespace std {
namespace iterator_traits_detail {

/// @brief base type for non-iterator types
/// @tparam Iter non-iterator type
///
/// Provides an empty type to be used as a base class if @c iterator_traits is
/// used with a non-iterator. Use of the base class allows @c iterator_traits to
/// be SFINAE-friendly.
///
template <class Iter, typename = arene::base::constraints<>>
class iterator_traits_base {};

/// @brief base type for iterator types
/// @tparam Iter iterator type
///
/// This base type for @c iterator_traits is selected when the @c Iter type
/// defines all of the following member type aliases:
/// * difference_type
/// * value_type
/// * pointer
/// * reference
/// * iterator_category
///
template <class Iter>
class iterator_traits_base<
    Iter,
    arene::base::constraints<
        typename Iter::difference_type,
        typename Iter::value_type,
        typename Iter::reference,
        typename Iter::pointer,
        typename Iter::iterator_category>> {
 public:
  /// @brief difference_type
  using difference_type = typename Iter::difference_type;
  /// @brief value_type
  using value_type = typename Iter::value_type;
  /// @brief pointer
  using pointer = typename Iter::pointer;
  /// @brief reference
  using reference = typename Iter::reference;
  /// @brief iterator_category
  using iterator_category = typename Iter::iterator_category;
};

}  // namespace iterator_traits_detail

/// @brief provides a uniform interface to the properties of an iterator
/// @tparam Iter iterator type to retrieve properties for
///
/// @c std::iterator_traits is a type trait class that provides a uniform
/// interface to the properties of Iterator types, allowing implementations of
/// algorithms only in terms of iterators.
///
/// This template can be specialized for user-defined Iterator types so that the
/// information about the iterator can be retrieved even if type does not
/// provide the usual typedefs.
///
/// The member types provided by this type trait class are:
///
/// | ** nested type **    | ** definition **           |
/// |----------------------|----------------------------|
/// | @c difference_type   | @c Iter::difference_type   |
/// | @c value_type        | @c Iter::value_type        |
/// | @c pointer           | @c Iter::pointer           |
/// | @c reference         | @c Iter::reference         |
/// | @c iterator_category | @c Iter::iterator_category |
///
/// @note This class is SFINAE-friendly: if a type does not define *all* the
/// member types listed above, then @c iterator_traits is empty for that type.
///
template <class Iter>
class iterator_traits : public iterator_traits_detail::iterator_traits_base<Iter> {};

/// @brief specialization for pointer to non-const type
/// @tparam T type
///
template <class T>
class iterator_traits<T*> {
 public:
  /// @brief difference_type
  using difference_type = std::ptrdiff_t;
  /// @brief value_type
  using value_type = T;
  /// @brief pointer
  using pointer = T*;
  /// @brief reference
  using reference = T&;
  /// @brief iterator_category
  using iterator_category = std::random_access_iterator_tag;
};

/// @brief specialization for pointer to const type
/// @tparam T type
///
template <class T>
class iterator_traits<T const*> {
 public:
  /// @brief difference_type
  using difference_type = std::ptrdiff_t;
  /// @brief value_type
  using value_type = T;
  /// @brief pointer
  using pointer = T const*;
  /// @brief reference
  using reference = T const&;
  /// @brief iterator_category
  using iterator_category = std::random_access_iterator_tag;
};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ITERATOR_TRAITS_HPP_
