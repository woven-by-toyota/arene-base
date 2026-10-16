// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TRANSPOSED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TRANSPOSED_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/linalg/layout_transpose.hpp"
#include "arene/base/mdspan/detail/reverse_extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {

namespace transposed_detail {

/// @brief The transposed extents type for an input extents type @c InputExtents
/// @tparam InputExtents The input mdspan's extents type
///
/// Reverses the extents and, as a single point of enforcement for the @c transposed mandate, statically rejects any
/// input whose rank is not 2. Every @c transposed_fn overload derives its result extents through this alias, so the
/// mandate is checked exactly once regardless of which layout overload is selected.
/// @{
template <class InputExtents>
class transpose_extents {
  // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
  // side-effects"
  static_assert(InputExtents::rank() == 2U, "transposed requires a rank-2 mdspan.");
  // parasoft-end-suppress CERT_C-PRE31-c-3

 public:
  /// @brief The reversed extents type
  using type = mdspan_detail::reverse_extents_t<InputExtents>;
};

/// @brief The transposed extents type for an input extents type @c InputExtents
/// @tparam InputExtents The input mdspan's extents type
template <class InputExtents>
using transpose_extents_t = typename transpose_extents<InputExtents>::type;
/// @}

/// @brief Class to implement @c transposed
class transposed_fn {
 public:
  /// @brief Return a transposed view of a @c layout_left mdspan
  /// @tparam ElementType The input mdspan's element type
  /// @tparam InputExtents The input mdspan's extents type. @c InputExtents::rank()==2 must be @c true else malformed.
  /// @tparam AccessorPolicy The input mdspan's accessor policy
  /// @param matrix The rank-2 mdspan to transpose
  /// @return An mdspan over the same data with swapped extents and a @c layout_right mapping
  /// @note The input matrix's data are neither copied nor modified.
  template <class ElementType, class InputExtents, class AccessorPolicy>
  constexpr auto operator()(mdspan<ElementType, InputExtents, layout_left, AccessorPolicy> matrix) const noexcept
      -> mdspan<ElementType, transpose_extents_t<InputExtents>, layout_right, AccessorPolicy> {
    using transposed_extents_type = transpose_extents_t<InputExtents>;
    using return_type = mdspan<ElementType, transposed_extents_type, layout_right, AccessorPolicy>;
    return return_type{
        matrix.data_handle(),
        typename return_type::mapping_type{transposed_extents_type{matrix.extent(1U), matrix.extent(0U)}},
        matrix.accessor()
    };
  }

  /// @brief Return a transposed view of a @c layout_right mdspan
  /// @tparam ElementType The input mdspan's element type
  /// @tparam InputExtents The input mdspan's extents type. @c InputExtents::rank()==2 must be @c true else malformed.
  /// @tparam AccessorPolicy The input mdspan's accessor policy
  /// @param matrix The rank-2 mdspan to transpose
  /// @return An mdspan over the same data with swapped extents and a @c layout_left mapping
  /// @note The input matrix's data are neither copied nor modified.
  template <class ElementType, class InputExtents, class AccessorPolicy>
  constexpr auto operator()(mdspan<ElementType, InputExtents, layout_right, AccessorPolicy> matrix) const noexcept
      -> mdspan<ElementType, transpose_extents_t<InputExtents>, layout_left, AccessorPolicy> {
    using transposed_extents_type = transpose_extents_t<InputExtents>;
    using return_type = mdspan<ElementType, transposed_extents_type, layout_left, AccessorPolicy>;
    return return_type{
        matrix.data_handle(),
        typename return_type::mapping_type{transposed_extents_type{matrix.extent(1U), matrix.extent(0U)}},
        matrix.accessor()
    };
  }

  /// @brief Return a transposed view of a @c layout_stride mdspan, swapping its two strides
  /// @tparam ElementType The input mdspan's element type
  /// @tparam InputExtents The input mdspan's extents type. @c InputExtents::rank()==2 must be @c true else malformed.
  /// @tparam AccessorPolicy The input mdspan's accessor policy
  /// @param matrix The rank-2 mdspan to transpose
  /// @return An mdspan over the same data with swapped extents and swapped strides
  /// @note The input matrix's data are neither copied nor modified.
  template <class ElementType, class InputExtents, class AccessorPolicy>
  constexpr auto operator()(mdspan<ElementType, InputExtents, layout_stride, AccessorPolicy> matrix) const noexcept
      -> mdspan<ElementType, transpose_extents_t<InputExtents>, layout_stride, AccessorPolicy> {
    using transposed_extents_type = transpose_extents_t<InputExtents>;
    using return_type = mdspan<ElementType, transposed_extents_type, layout_stride, AccessorPolicy>;
    return return_type{
        matrix.data_handle(),
        typename return_type::mapping_type{
            transposed_extents_type{matrix.extent(1U), matrix.extent(0U)},
            array<typename InputExtents::index_type, 2U>{matrix.mapping().stride(1U), matrix.mapping().stride(0U)}
        },
        matrix.accessor()
    };
  }

  /// @brief Return a transposed view of a @c layout_transpose mdspan by unwrapping it to its nested layout
  /// @tparam ElementType The input mdspan's element type
  /// @tparam InputExtents The input mdspan's extents type. @c InputExtents::rank()==2 must be @c true else malformed.
  /// @tparam NestedLayout The nested (un-transposed) layout policy
  /// @tparam AccessorPolicy The input mdspan's accessor policy
  /// @param matrix The rank-2 mdspan to transpose
  /// @return An mdspan over the same data using the nested (un-transposed) mapping
  /// @note The input matrix's data are neither copied nor modified.
  template <class ElementType, class InputExtents, class NestedLayout, class AccessorPolicy>
  constexpr auto operator()(mdspan<ElementType, InputExtents, layout_transpose<NestedLayout>, AccessorPolicy> matrix
  ) const noexcept -> mdspan<ElementType, transpose_extents_t<InputExtents>, NestedLayout, AccessorPolicy> {
    using transposed_extents_type = transpose_extents_t<InputExtents>;
    using return_type = mdspan<ElementType, transposed_extents_type, NestedLayout, AccessorPolicy>;
    return return_type{matrix.data_handle(), matrix.mapping().nested_mapping(), matrix.accessor()};
  }

  /// @brief Return a transposed view of any other layout by wrapping it in a @c layout_transpose mapping
  /// @tparam ElementType The input mdspan's element type
  /// @tparam InputExtents The input mdspan's extents type. @c InputExtents::rank()==2 must be @c true else malformed.
  /// @tparam InputLayout The input mdspan's layout policy
  /// @tparam AccessorPolicy The input mdspan's accessor policy
  /// @param matrix The rank-2 mdspan to transpose
  /// @return An mdspan over the same data with a @c layout_transpose mapping around the input mapping
  /// @note The input matrix's data are neither copied nor modified.
  template <class ElementType, class InputExtents, class InputLayout, class AccessorPolicy>
  constexpr auto operator()(mdspan<ElementType, InputExtents, InputLayout, AccessorPolicy> matrix) const noexcept
      -> mdspan<ElementType, transpose_extents_t<InputExtents>, layout_transpose<InputLayout>, AccessorPolicy> {
    using transposed_extents_type = transpose_extents_t<InputExtents>;
    using return_type = mdspan<ElementType, transposed_extents_type, layout_transpose<InputLayout>, AccessorPolicy>;
    return return_type{matrix.data_handle(), typename return_type::mapping_type{matrix.mapping()}, matrix.accessor()};
  }
};

}  // namespace transposed_detail

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "Unnamed namespace creates a per-TU reference to a single global object"
// parasoft-begin-suppress CERT_CPP-DCL59-a "Unnamed namespace creates a per-TU reference to a single global object"
/// @brief Return a transposed view of a rank-2 mdspan that aliases the input data in place
///
/// Selects an overload of @c transposed_detail::transposed_fn::operator() by the input layout: a @c layout_left input
/// yields a @c layout_right view (and vice versa), a @c layout_stride input yields a @c layout_stride view with its two
/// strides swapped, a @c layout_transpose input is unwrapped to its nested layout, and any other layout is wrapped in
/// @c layout_transpose.
/// @note Mandates: the input mdspan has rank 2; the input's data are neither copied nor modified.
ARENE_CPP14_INLINE_VARIABLE(transposed_detail::transposed_fn, transposed);
// parasoft-end-suppress CERT_CPP-DCL59-a
// parasoft-end-suppress AUTOSAR-M7_3_3-a

}  // namespace linalg
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A7_1_5-a
// parasoft-end-suppress AUTOSAR-M2_10_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TRANSPOSED_HPP_
