// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DIAGONAL_TAGS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DIAGONAL_TAGS_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"

namespace arene {
namespace base {
namespace linalg {

/// @brief Tag indicating that a triangular matrix has an implicit unit diagonal.
///
/// An algorithm that is passed a triangular matrix @c A with an implicit unit diagonal will not access the diagonal of
/// @c A and the algorithm will interpret @c A as if it has a unit diagonal, that is, a diagonal where each of whose
/// elements behaves as a multiplicative identity.
class implicit_unit_diagonal_t {
 public:
  /// @brief Explicit default constructor.
  explicit implicit_unit_diagonal_t() = default;
};

/// @brief Tag indicating that a triangular matrix stores its diagonal explicitly.
///
/// A matrix with an explicit diagonal is a triangular matrix where the diagonal elements are actually stored in the
/// matrix. The diagonal is the set of all elements of matrix @c A accessed by @c A[i,i] for <c>0 <= i <
/// min(A.extent(0), A.extent(1))</c>.
class explicit_diagonal_t {
 public:
  /// @brief Explicit default constructor.
  explicit explicit_diagonal_t() = default;
};

// parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variables are initialized"
// parasoft-begin-suppress AUTOSAR-M3_4_1-b "This is a per-TU reference to a global used in multiple TUs"
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."

/// @brief Tag indicating that a triangular matrix has an implicit unit diagonal.
ARENE_CPP14_INLINE_VARIABLE(implicit_unit_diagonal_t, implicit_unit_diagonal);

/// @brief Tag indicating that a triangular matrix stores its diagonal explicitly.
ARENE_CPP14_INLINE_VARIABLE(explicit_diagonal_t, explicit_diagonal);

// parasoft-end-suppress CERT_CPP-DCL56-a
// parasoft-end-suppress AUTOSAR-M3_4_1-b
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

/// @brief Determines whether a type, ignoring cv-qualification, is a diagonal storage tag.
/// @tparam T The type to inspect.
template <typename T>
extern constexpr bool is_diagonal_storage_tag_v{
    std::is_same<std::remove_cv_t<T>, implicit_unit_diagonal_t>::value ||
    std::is_same<std::remove_cv_t<T>, explicit_diagonal_t>::value
};

}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DIAGONAL_TAGS_HPP_
