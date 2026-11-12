// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines submdspan_canonicalize_slices"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SUBMDSPAN_CANONICALIZE_SLICES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SUBMDSPAN_CANONICALIZE_SLICES_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/functional/bind_front.hpp"
#include "arene/base/mdspan/detail/canonical_slice.hpp"
#include "arene/base/mdspan/detail/is_valid_submdspan_slice_for.hpp"
#include "arene/base/mdspan/detail/mandate_each_argtype_is_submdspan_slice.hpp"
#include "arene/base/mdspan/detail/mandate_each_argtype_is_valid_submdspan_slice_type_for.hpp"
#include "arene/base/mdspan/detail/to_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/logical_and.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/tuple/apply.hpp"
#include "arene/base/tuple/detail/tuple_transform_reduce.hpp"
#include "arene/base/tuple/detail/tuple_zip.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/all_of.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"
// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {

namespace submdspan_canonicalize_slices_detail {

/// @brief convert slices to canonical form: a @c std::tuple of slices where
/// each is @c full_extent_t, a single index, or @c extent_slice
class submdspan_canonicalize_slices_fn {
 public:
  // parasoft-begin-suppress AUTOSAR-A2_7_2-a "False positive: no commented-out code, this is documentation"
  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: parameter 'src' is used"
  /// @brief convert slices to canonical form
  /// @tparam IndexType index type
  /// @tparam Extents static extent values
  /// @tparam SliceSpecifiers slice types
  /// @param src @c extents to slice
  /// @param slices @c submdspan slice specifiers
  ///
  /// @return @c std::tuple of canonical slices where the canonical form of a
  /// value @c s with type @c S will be:
  /// * @c static_cast<full_extent_t>(std::move(s)) if the slice is convertible
  ///   to @c full_extent_t; otherwise
  /// * @c std::integral_constant<IndexType, S::value> if the slice is
  ///   integral-constant-like; otherwise
  /// * @c IndexType(std::move(s)) if the slice is convertible to @c IndexType;
  ///   otherwise
  /// * a specialization of @c extent_slice if @c S is a specialization of
  ///   @c extent_slice or @c range_slice; otherwise
  /// * a specialization of @c extent_slice if @c S is index-pair-like
  ///
  /// @note Constraints <br>
  ///   * <c> sizeof...(Extents) == sizeof...(SliceSpecifiers) </c>
  /// @note Mandates <br>
  /// * each type @c S in @c SliceSpecifiers... is a valid submdspan slice for
  ///   @c IndexType
  /// * the canonical form of each @c S in @c SliceSpecifiers... is a valid
  ///   submdspan slice type for associated static extent of @c src
  // parasoft-end-suppress AUTOSAR-A2_7_2-a
  template <
      class IndexType,
      std::size_t... Extents,
      class... SliceSpecifiers,
      constraints<std::enable_if_t<sizeof...(Extents) == sizeof...(SliceSpecifiers)>> = nullptr,
      std::nullptr_t = mdspan_detail::mandate_each_argtype_is_submdspan_slice<IndexType, SliceSpecifiers...>()>
  constexpr auto operator()(extents<IndexType, Extents...> const& src, SliceSpecifiers... slices) const noexcept( //
      all_of_v<is_nothrow_invocable_v<mdspan_detail::canonical_slice_fn<IndexType>, SliceSpecifiers&&>...>
  ) ->
  std::tuple<mdspan_detail::canonical_slice_t<IndexType, SliceSpecifiers>...>
  {
    // GCC8 doesn't like having this in the template parameter list so we'll make it an ignored variable in the body.
    std::ignore = mdspan_detail::mandate_each_argtype_is_valid_submdspan_slice_type_for<IndexType, Extents...>(
        type_list<SliceSpecifiers...>{}
    );

    auto const canonicalized = std::tuple<mdspan_detail::canonical_slice_t<IndexType, SliceSpecifiers>...>{
        mdspan_detail::canonical_slice<IndexType>(std::move(slices))...
    };

    // Reference elements prevent tuple_transform_reduce from copying slices through tuple_zip,
    // avoiding the Clang 20 x86-64 miscompilation in https://github.com/llvm/llvm-project/issues/205291.
    using slice_refs = std::tuple<mdspan_detail::canonical_slice_t<IndexType, SliceSpecifiers> const&...>;
    // parasoft-begin-suppress AUTOSAR-M0_1_3-a "False positive: 'refs' is passed to tuple_zip and 'validate_slice' is
    // moved into tuple_transform_reduce"
    slice_refs refs{canonicalized};
    auto zipped = tuple_detail::tuple_zip(refs, mdspan_detail::to_array(src));
    auto validate_slice = arene::base::bind_front(apply, mdspan_detail::is_valid_submdspan_slice_for);
    // parasoft-end-suppress AUTOSAR-M0_1_3-a
    ARENE_PRECONDITION(
        tuple_detail::tuple_transform_reduce(std::move(zipped), true, std::logical_and<>{}, std::move(validate_slice))
    );

    return canonicalized;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_4-a
  // parasoft-end-suppress AUTOSAR-A2_7_2-a
};
}  // namespace submdspan_canonicalize_slices_detail

// parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variables are initialized"

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
/// @brief transforms @c submdspan slice arguments to canonical form
///
/// @see submdspan_canonicalize_slices_detail::submdspan_canonicalize_slices_fn
ARENE_CPP14_INLINE_VARIABLE(
    submdspan_canonicalize_slices_detail::submdspan_canonicalize_slices_fn,
    submdspan_canonicalize_slices
);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a
// parasoft-end-suppress CERT_CPP-DCL56-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SUBMDSPAN_CANONICALIZE_SLICES_HPP_
