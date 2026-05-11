// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file value_category_probe.hpp
/// @brief Single-element tuple-like whose ADL @c get overloads report the caller's value category.
///

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_TESTS_VALUE_CATEGORY_PROBE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_TESTS_VALUE_CATEGORY_PROBE_HPP_

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/tuple_element.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"

namespace arene {
namespace base {
namespace tuple_testing {

/// @brief Tag identifying which value category of a probe reached the @c get overload.
enum class input_category { lvalue_ref, const_lvalue_ref, rvalue_ref, const_rvalue_ref };

/// @brief Single-element tuple-like used to observe which ADL @c get overload a caller dispatches to.
///
/// Specializations of @c std::tuple_size and @c std::tuple_element make the type tuple-like with one
/// element of type @c input_category. Four ADL @c get<0> overloads, one per value category, return a
/// distinct @c input_category value; a consumer that forwards a probe into @c get<0> can assert on the
/// returned value to prove which overload was selected.
struct value_category_probe {};

/// @brief Return value for an lvalue probe.
/// @param probe the probe (ignored)
/// @return @c input_category::lvalue_ref
template <std::size_t Idx>
inline auto get(value_category_probe& /*probe*/) noexcept -> input_category {
  static_assert(Idx == 0, "value_category_probe has exactly one element");
  return input_category::lvalue_ref;
}

/// @brief Return value for a const-lvalue probe.
/// @param probe the probe (ignored)
/// @return @c input_category::const_lvalue_ref
template <std::size_t Idx>
inline auto get(value_category_probe const& /*probe*/) noexcept -> input_category {
  static_assert(Idx == 0, "value_category_probe has exactly one element");
  return input_category::const_lvalue_ref;
}

/// @brief Return value for an rvalue probe.
/// @param probe the probe (ignored)
/// @return @c input_category::rvalue_ref
template <std::size_t Idx>
inline auto get(value_category_probe&& /*probe*/) noexcept -> input_category {
  static_assert(Idx == 0, "value_category_probe has exactly one element");
  return input_category::rvalue_ref;
}

/// @brief Return value for a const-rvalue probe.
/// @param probe the probe (ignored)
/// @return @c input_category::const_rvalue_ref
template <std::size_t Idx>
inline auto get(value_category_probe const&& /*probe*/) noexcept -> input_category {
  static_assert(Idx == 0, "value_category_probe has exactly one element");
  return input_category::const_rvalue_ref;
}

}  // namespace tuple_testing
}  // namespace base
}  // namespace arene

template <>
class std::tuple_size<arene::base::tuple_testing::value_category_probe>
    : public std::integral_constant<std::size_t, 1> {};

template <>
class std::tuple_element<0, arene::base::tuple_testing::value_category_probe> {
 public:
  using type = arene::base::tuple_testing::input_category;
};

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_TESTS_VALUE_CATEGORY_PROBE_HPP_
