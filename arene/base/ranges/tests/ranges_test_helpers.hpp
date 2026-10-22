// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_TESTS_RANGES_TEST_HELPERS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_TESTS_RANGES_TEST_HELPERS_HPP_

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/attributes/maybe_unused.hpp"
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/ranges/enable_borrowed_range.hpp"  // IWYU pragma: keep
#include "arene/base/stdlib_choice/add_const.hpp"
#include "arene/base/stdlib_choice/add_lvalue_reference.hpp"
#include "arene/base/stdlib_choice/add_rvalue_reference.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/type_list/apply_each.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"

namespace ranges_test {

namespace type_lists = ::arene::base::type_lists;

/// @brief A user-defined range with member @c begin and @c end functions.
class member_range {
  int* data_{};

 public:
  /// @brief Returns an iterator to the beginning of the range.
  constexpr auto begin() const noexcept -> int* { return data_; }
  /// @brief Returns an iterator to the end of the range.
  constexpr auto end() const noexcept -> int* { return data_; }
};  // namespace =::arene::base::type_lists

/// @brief A user-defined range with argument-dependent @c begin and @c end functions.
struct adl_range {};

/// @brief Returns an iterator to the beginning of an ADL range.
ARENE_MAYBE_UNUSED constexpr auto begin(adl_range&) noexcept -> int* { return nullptr; }

/// @brief Returns an iterator to the end of an ADL range.
ARENE_MAYBE_UNUSED constexpr auto end(adl_range&) noexcept -> int* { return nullptr; }

/// @brief Returns an iterator to the beginning of a const ADL range.
ARENE_MAYBE_UNUSED constexpr auto begin(adl_range const&) noexcept -> int const* { return nullptr; }

/// @brief Returns an iterator to the end of a const ADL range.
ARENE_MAYBE_UNUSED constexpr auto end(adl_range const&) noexcept -> int const* { return nullptr; }

/// @brief A type with a valid @c end function but no valid @c begin function.
class range_without_begin {
  int* data_{};

 public:
  /// @brief Returns an iterator to the end of the range.
  constexpr auto end() const noexcept -> int* { return data_; }
};

/// @brief A type with a valid @c begin function but no valid @c end function.
class range_without_end {
  int* data_{};

 public:
  /// @brief Returns an iterator to the beginning of the range.
  constexpr auto begin() const noexcept -> int* { return data_; }
};

/// @brief A type whose accessors are valid but whose results do not denote a range.
class range_with_invalid_iterators {
  int value_{};

 public:
  /// @brief Returns a non-iterator value.
  constexpr auto begin() const noexcept -> int { return value_; }
  /// @brief Returns a non-iterator value.
  constexpr auto end() const noexcept -> int { return value_; }
};

using non_range_raw_types = ::testing::Types<range_without_begin, range_without_end, range_with_invalid_iterators>;
using const_non_range_raw_types = type_lists::apply_each_t<non_range_raw_types, std::add_const_t>;
using lvalue_non_range_types = type_lists::apply_each_t<non_range_raw_types, std::add_lvalue_reference_t>;
using const_lvalue_non_range_types = type_lists::apply_each_t<const_non_range_raw_types, std::add_lvalue_reference_t>;
using rvalue_non_range_types = type_lists::apply_each_t<non_range_raw_types, std::add_rvalue_reference_t>;
using const_rvalue_non_range_types = type_lists::apply_each_t<const_non_range_raw_types, std::add_rvalue_reference_t>;

using all_qualified_non_range_types = type_lists::remove_duplicates_t<type_lists::concat_t<
    non_range_raw_types,
    const_non_range_raw_types,
    lvalue_non_range_types,
    const_lvalue_non_range_types,
    rvalue_non_range_types,
    const_rvalue_non_range_types>>;

/// @brief A user-defined range explicitly enabled as a borrowed range.
class explicitly_enabled_borrowed_range {
  int* data_{};

 public:
  /// @brief Returns an iterator to the beginning of the range.
  constexpr auto begin() const noexcept -> int* { return data_; }
  /// @brief Returns an iterator to the end of the range.
  constexpr auto end() const noexcept -> int* { return data_; }
};

// NOLINTNEXTLINE(hicpp-avoid-c-arrays) C-array behavior is part of the range trait contract.
using c_array = int[3];

using range_types = ::testing::Types<
    ::arene::base::array<int, 3>,
    ::arene::base::array<int, 0>,
    ::arene::base::inline_vector<int, 3>,
    c_array,
    member_range,
    adl_range>;

using const_range_types = type_lists::apply_each_t<range_types, std::add_const_t>;
using lvalue_reference_range_types = type_lists::apply_each_t<range_types, std::add_lvalue_reference_t>;
using const_lvalue_reference_range_types = type_lists::apply_each_t<const_range_types, std::add_lvalue_reference_t>;
using rvalue_reference_range_types = type_lists::apply_each_t<range_types, std::add_rvalue_reference_t>;
using const_rvalue_reference_range_types = type_lists::apply_each_t<const_range_types, std::add_rvalue_reference_t>;

using explicit_borrowed_range_types = ::testing::Types<explicitly_enabled_borrowed_range>;
using const_explicit_borrowed_range_types = type_lists::apply_each_t<explicit_borrowed_range_types, std::add_const_t>;
using lvalue_explicit_borrowed_range_types =
    type_lists::apply_each_t<explicit_borrowed_range_types, std::add_lvalue_reference_t>;
using const_lvalue_explicit_borrowed_range_types =
    type_lists::apply_each_t<lvalue_explicit_borrowed_range_types, std::add_const_t>;
using rvalue_explicit_borrowed_range_types =
    type_lists::apply_each_t<explicit_borrowed_range_types, std::add_rvalue_reference_t>;
using const_rvalue_explicit_borrowed_range_types =
    type_lists::apply_each_t<const_explicit_borrowed_range_types, std::add_rvalue_reference_t>;

using all_explicit_borrowed_range_types = type_lists::remove_duplicates_t<type_lists::concat_t<
    lvalue_explicit_borrowed_range_types,
    const_lvalue_explicit_borrowed_range_types,
    rvalue_explicit_borrowed_range_types,
    const_rvalue_explicit_borrowed_range_types>>;

using valid_range_types = type_lists::
    concat_t<lvalue_reference_range_types, const_lvalue_reference_range_types, all_explicit_borrowed_range_types>;
using non_borrowed_range_types = type_lists::concat_t<rvalue_reference_range_types, const_rvalue_reference_range_types>;
using all_qualified_range_types = type_lists::remove_duplicates_t<type_lists::concat_t<
    range_types,
    lvalue_reference_range_types,
    const_lvalue_reference_range_types,
    rvalue_reference_range_types,
    const_rvalue_reference_range_types>>;

using all_qualified_borrowed_range_types = type_lists::remove_duplicates_t<
    type_lists::
        concat_t<lvalue_reference_range_types, const_lvalue_reference_range_types, all_explicit_borrowed_range_types>>;

using all_qualified_non_range_non_borrowed_range_types =
    type_lists::remove_duplicates_t<type_lists::concat_t<non_borrowed_range_types, all_qualified_non_range_types>>;

using all_range_and_non_range_types = type_lists::remove_duplicates_t<
    type_lists::concat_t<all_qualified_range_types, all_qualified_non_range_types, all_qualified_borrowed_range_types>>;

}  // namespace ranges_test

namespace arene {
namespace base {
namespace ranges {

/// @brief A user specialization of @c enable_borrowed_range for the type
/// @c ranges_test::explicitly_enabled_borrowed_range .
template <>
class enable_borrowed_range<::ranges_test::explicitly_enabled_borrowed_range> : public std::true_type {};

static_assert(
    enable_borrowed_range_v<::ranges_test::explicitly_enabled_borrowed_range>,
    "The borrowed range is opted in"
);

}  // namespace ranges
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_TESTS_RANGES_TEST_HELPERS_HPP_
