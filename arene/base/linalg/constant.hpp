// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::linalg::constant"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_CONSTANT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_CONSTANT_HPP_

// IWYU pragma: private, include "arene/base/linalg.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/mdspan/constant_accessor.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {

namespace constant_detail {
/// @brief function object to return a constant tensorlet
class constant_fn {
 public:
  /// @brief returns a constant tensorlet
  /// @tparam T numeric type
  /// @tparam Extents constant tensorlet extents type
  /// @param value constant tensorlet value
  /// @param extents constant tensorlet extents
  ///
  /// Provides a tensorlet (i.e. a vector for rank-1 and a matrix for rank-2)
  /// where the value at all indexes is the same constant @c value. The tensorlet
  /// stores a single copy of the constant @c value
  /// and access always returns a copy of the stored @c value.
  ///
  /// @return @c mdspan with value of @c value for all entries
  ///
  /// @note The layout of the returned @c mdspan is unspecified.
  template <
      class T,
      class Extents,
      constraints<  //
          std::enable_if_t<is_scalar_v<T>>,
          std::enable_if_t<is_extents_v<Extents>>> = nullptr>
  constexpr auto operator()(T const& value, Extents const& extents) const
      noexcept(std::is_nothrow_copy_constructible<T>::value) -> mdspan<T, Extents, layout_right, constant_accessor<T>> {
    return {value, extents};
  }
};
}  // namespace constant_detail

/// @brief Construct a constant-valued expression for a linear algebra operation.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(constant_detail::constant_fn, constant);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace linalg
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_CONSTANT_HPP_
