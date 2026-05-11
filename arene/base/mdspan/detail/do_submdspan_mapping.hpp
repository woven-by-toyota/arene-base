// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines
// arene::base::mdspan_detail::do_submdspan_mapping"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_DO_SUBMDSPAN_MAPPING_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_DO_SUBMDSPAN_MAPPING_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/move.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief Poison pill definition of @c submdspan_mapping
///
/// This is to ensure that @c do_submdspan_mapping_fn only looks for a function declaration and does not pick up some
/// other global object named @c submdspan_mapping
template <class T>
auto submdspan_mapping() -> void = delete;

/// @brief function object that dispatches to the correct @c submdspan_mapping overload via ADL
///
/// Invokes the @c submdspan_mapping customization point for the given layout mapping and slice specifiers.
/// The appropriate overload is found via argument-dependent lookup.
class do_submdspan_mapping_fn {
 public:
  /// @brief invoke @c submdspan_mapping for the given mapping and slices
  /// @tparam Mapping a layout mapping type
  /// @tparam Slices types of the slice specifiers
  /// @param map the layout mapping of the original mdspan
  /// @param slices the slice specifiers defining the submdspan
  /// @return the result of the @c submdspan_mapping customization point
  template <class Mapping, class... Slices>
  constexpr auto operator()(Mapping const& map, Slices... slices) const noexcept(  //
      noexcept(submdspan_mapping(std::declval<Mapping const&>(), std::declval<Slices>()...))
  ) -> decltype(submdspan_mapping(std::declval<Mapping const&>(), std::declval<Slices>()...)) {
    return submdspan_mapping(map, std::move(slices)...);
  }
};

/// @def arene::base::mdspan_detail::do_submdspan_mapping
/// @copydoc arene::base::mdspan_detail::do_submdspan_mapping_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(mdspan_detail::do_submdspan_mapping_fn, do_submdspan_mapping);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_DO_SUBMDSPAN_MAPPING_HPP_
