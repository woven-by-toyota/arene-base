// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_OBJECT_TRAITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_OBJECT_TRAITS_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/type_traits/is_copyable.hpp"

namespace arene {
namespace base {
namespace linalg {

/// @brief Determine if the given type is a scalar for the purposes of linalg algorithms
/// @tparam T The type to check
/// @note To be a scalar for this purpose, the type must be semiregular (having all special member functions) and not
/// be an mdspan. The <c>value_type</c>s of vectors and matrices used in linalg must be scalars in this sense.
/// @note C++26 also requires @c !std::is_execution_policy_v<T> but there are no execution policies in C++14.
template <typename T>
extern constexpr bool is_scalar_v{
    is_copyable_v<std::remove_cv_t<T>> &&       //
    std::is_default_constructible<T>::value &&  //
    !is_mdspan_v<T>                             //
};

namespace linalg_traits_detail {
/// @brief Determine if the given type is an in-vector, usable as an input vector for linalg algorithms
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_in_vector_v{false};

/// @brief Determine if the given type is an in-vector, usable as an input vector for linalg algorithms
/// @tparam ElementType The element type of an @c mdspan being checked
/// @tparam Extents The extents type of an @c mdspan being checked
/// @tparam LayoutPolicy The layout policy type (note: not the layout mapping) of an @c mdspan being checked
/// @tparam AccessorPolicy The accessor policy type of an @c mdspan being checked
template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
extern constexpr bool is_in_vector_v<mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>>{
    mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>::rank() == 1U &&                   //
    is_scalar_v<typename mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>::value_type>  //
};

/// @brief Determine if the given type is an out-vector, usable as an output vector for linalg algorithms
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_out_vector_v{false};

/// @brief Determine if the given type is an out-vector, usable as an output vector for linalg algorithms
/// @tparam ElementType The element type of an @c mdspan being checked
/// @tparam Extents The extents type of an @c mdspan being checked
/// @tparam LayoutPolicy The layout policy type (note: not the layout mapping) of an @c mdspan being checked
/// @tparam AccessorPolicy The accessor policy type of an @c mdspan being checked
template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
extern constexpr bool is_out_vector_v<mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>>{
    is_in_vector_v<mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>> &&
    std::is_assignable<
        typename mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>::reference,
        typename mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>::element_type>::value &&
    mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>::is_always_unique()
};

/// @brief Determine if the given type is an in-matrix, usable as an input matrix for linalg algorithms
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_in_matrix_v{false};

/// @brief Determine if the given type is an in-matrix, usable as an input matrix for linalg algorithms
/// @tparam ElementType The element type of an @c mdspan being checked
/// @tparam Extents The extents type of an @c mdspan being checked
/// @tparam LayoutPolicy The layout policy type (note: not the layout mapping) of an @c mdspan being checked
/// @tparam AccessorPolicy The accessor policy type of an @c mdspan being checked
template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
extern constexpr bool is_in_matrix_v<mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>>{
    mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>::rank() == 2U &&                   //
    is_scalar_v<typename mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>::value_type>  //
};

/// @brief Determine if the given type is an out-matrix, usable as an output matrix for linalg algorithms
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_out_matrix_v{false};

/// @brief Determine if the given type is an out-matrix, usable as an output matrix for linalg algorithms
/// @tparam ElementType The element type of an @c mdspan being checked
/// @tparam Extents The extents type of an @c mdspan being checked
/// @tparam LayoutPolicy The layout policy type (note: not the layout mapping) of an @c mdspan being checked
/// @tparam AccessorPolicy The accessor policy type of an @c mdspan being checked
template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
extern constexpr bool is_out_matrix_v<mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>>{
    is_in_matrix_v<mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>> &&
    std::is_assignable<
        typename mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>::reference,
        typename mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>::element_type>::value &&
    mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>::is_always_unique()
};
}  // namespace linalg_traits_detail

/// @brief Determine if the given type is an in-vector, usable as an input vector for linalg algorithms
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_in_vector_v{linalg_traits_detail::is_in_vector_v<std::remove_cv_t<T>>};

/// @brief Determine if the given type is an out-vector, usable as an output vector for linalg algorithms
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_out_vector_v{linalg_traits_detail::is_out_vector_v<std::remove_cv_t<T>>};

/// @brief Determine if the given type is an inout-vector, usable as both input and output vector for linalg algorithms
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_inout_vector_v{is_out_vector_v<T>};

/// @brief Determine if the given type is an in-matrix, usable as an input matrix for linalg algorithms
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_in_matrix_v{linalg_traits_detail::is_in_matrix_v<std::remove_cv_t<T>>};

/// @brief Determine if the given type is an out-matrix, usable as an output matrix for linalg algorithms
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_out_matrix_v{linalg_traits_detail::is_out_matrix_v<std::remove_cv_t<T>>};

/// @brief Determine if the given type is an inout-matrix, usable as both input and output matrix for linalg algorithms
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_inout_matrix_v{is_out_matrix_v<T>};

/// @brief Determine if the given type is an in-object, i.e. an in-vector or in-matrix
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_in_object_v{is_in_vector_v<T> || is_in_matrix_v<T>};

/// @brief Determine if the given type is an out-object, i.e. an out-vector or out-matrix
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_out_object_v{is_out_vector_v<T> || is_out_matrix_v<T>};

/// @brief Determine if the given type is an inout-object, i.e. an inout-vector or inout-matrix
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_inout_object_v{is_inout_vector_v<T> || is_inout_matrix_v<T>};

}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_OBJECT_TRAITS_HPP_
