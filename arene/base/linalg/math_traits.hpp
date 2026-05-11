// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_MATH_TRAITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_MATH_TRAITS_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"

namespace arene {
namespace base {
namespace linalg {

namespace math_traits_impl {

/// @brief Default implementation of @c math_traits which uses member typedefs for @c mdspan and is empty for other @c T
/// @tparam T The type whose properties are being inspected
template <typename T, typename = arene::base::constraints<>>
class default_math_traits {};

/// @brief Default implementation of @c math_traits which uses member typedefs for @c mdspan and is empty for other @c T
/// @tparam T The type whose properties are being inspected
template <typename T>
class default_math_traits<T, arene::base::constraints<std::enable_if_t<is_in_object_v<T>>>> {
 public:
  /// @brief The mdspan element type of @c T
  using element_type = typename T::element_type;
  /// @brief The mdspan value type of @c T
  using value_type = typename T::value_type;
  /// @brief The mdspan reference type of @c T
  using reference = typename T::reference;
  /// @brief The mdspan index type of @c T
  using index_type = typename T::index_type;
  /// @brief The mdspan size type of @c T
  using size_type = typename T::size_type;
  /// @brief The mdspan layout type of @c T
  using layout_type = typename T::layout_type;
  /// @brief The mdspan rank type of @c T
  using rank_type = typename T::rank_type;
  /// @brief The mdspan extents type of @c T
  using extents_type = typename T::extents_type;
  /// @brief The mdspan accessor type of @c T
  using accessor_type = typename T::accessor_type;
};

}  // namespace math_traits_impl

/// @brief A traits struct allowing SFINAE-friendly access to the properties of types which may be linalg objects
/// @tparam T The type whose properties are being inspected
template <typename T>
class math_traits : public math_traits_impl::default_math_traits<T> {};

/// @brief The mdspan-equivalent element type of @c T if @c T is a linalg object, or a substitution error if not
/// @tparam T The type to inspect
template <class T>
using math_element_t = typename math_traits<T>::element_type;

/// @brief The mdspan-equivalent value type of @c T if @c T is a linalg object, or a substitution error if not
/// @tparam T The type to inspect
template <class T>
using math_value_t = typename math_traits<T>::value_type;

/// @brief The mdspan-equivalent reference type of @c T if @c T is a linalg object, or a substitution error if not
/// @tparam T The type to inspect
template <class T>
using math_reference_t = typename math_traits<T>::reference;

/// @brief The mdspan-equivalent index type of @c T if @c T is a linalg object, or a substitution error if not
/// @tparam T The type to inspect
template <class T>
using math_index_t = typename math_traits<T>::index_type;

/// @brief The mdspan-equivalent size type of @c T if @c T is a linalg object, or a substitution error if not
/// @tparam T The type to inspect
template <class T>
using math_size_t = typename math_traits<T>::size_type;

/// @brief The mdspan-equivalent layout type of @c T if @c T is a linalg object, or a substitution error if not
/// @tparam T The type to inspect
template <class T>
using math_layout_t = typename math_traits<T>::layout_type;

/// @brief The mdspan-equivalent rank type of @c T if @c T is a linalg object, or a substitution error if not
/// @tparam T The type to inspect
template <class T>
using math_rank_t = typename math_traits<T>::rank_type;

/// @brief The mdspan-equivalent extents type of @c T if @c T is a linalg object, or a substitution error if not
/// @tparam T The type to inspect
template <class T>
using math_extents_t = typename math_traits<T>::extents_type;

/// @brief The mdspan-equivalent accessor type of @c T if @c T is a linalg object, or a substitution error if not
/// @tparam T The type to inspect
template <class T>
using math_accessor_t = typename math_traits<T>::accessor_type;

}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_MATH_TRAITS_HPP_
