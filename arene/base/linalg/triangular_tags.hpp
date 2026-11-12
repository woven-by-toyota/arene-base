// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TRIANGULAR_TAGS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TRIANGULAR_TAGS_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"

namespace arene {
namespace base {
namespace linalg {

/// @brief Tag indicating that a triangular matrix stores its upper triangle.
///
/// The upper triangle of a matrix @c A is the set of all elements of @c A accessed by @c A[i,j] with <c>i <= j</c>. It
/// includes the diagonal.
class upper_triangle_t {
 public:
  /// @brief Explicit default constructor.
  explicit upper_triangle_t() = default;
};

/// @brief Tag indicating that a triangular matrix stores its lower triangle.
///
/// The lower triangle of a matrix @c A is the set of all elements of @c A accessed by @c A[i,j] with <c>i >= j</c>. It
/// includes the diagonal.
class lower_triangle_t {
 public:
  /// @brief Explicit default constructor.
  explicit lower_triangle_t() = default;
};

// parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variables are initialized"
// parasoft-begin-suppress AUTOSAR-M3_4_1-b "This is a per-TU reference to a global used in multiple TUs"
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."

/// @brief Tag indicating that a triangular matrix stores its upper triangle.
ARENE_CPP14_INLINE_VARIABLE(upper_triangle_t, upper_triangle);

/// @brief Tag indicating that a triangular matrix stores its lower triangle.
ARENE_CPP14_INLINE_VARIABLE(lower_triangle_t, lower_triangle);

// parasoft-end-suppress CERT_CPP-DCL56-a
// parasoft-end-suppress AUTOSAR-M3_4_1-b
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

/// @brief Determines whether a type, ignoring cv-qualification, is a triangular matrix storage tag.
/// @tparam T The type to inspect.
template <typename T>
extern constexpr bool is_triangular_tag_v{
    std::is_same<std::remove_cv_t<T>, upper_triangle_t>::value ||
    std::is_same<std::remove_cv_t<T>, lower_triangle_t>::value
};

}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TRIANGULAR_TAGS_HPP_
