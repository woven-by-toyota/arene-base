// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @example examples/subpackages/ranges/disable_sized_range_examples.cpp
/// @brief Examples for the ranges/type_traits subpackage documentation
///

#include <cstddef>
#include <forward_list>
#include <iterator>
#include <type_traits>

//! [include_header]
#include "arene/base/ranges.hpp"
//! [include_header]

namespace ranges_examples {

//! [slow_size]
/// @brief A range whose @c size() delegates to a singly-linked list and so is not constant-time.
///
/// @c std::forward_list has no @c size() member precisely because counting its elements is a linear-time
/// operation, so a range that exposes one advertises a @c size() that should not be treated as constant-time.
struct forward_list_range {
  auto size() const noexcept -> std::size_t {
    return static_cast<std::size_t>(std::distance(values_.begin(), values_.end()));
  }

 private:
  std::forward_list<int> values_{};
};
//! [slow_size]

}  // namespace ranges_examples

//! [specialize_disable_sized_range]
/// @brief A user opt-out of the constant-time assumption for a range with a linear-time @c size() .
/// @note The specialization derives from @c std::true_type to opt out. It may be declared in any namespace enclosing
/// @c arene::base::ranges ; here it uses a fully-qualified name at global scope.
template <>
struct arene::base::ranges::disable_sized_range<ranges_examples::forward_list_range> : std::true_type {};
//! [specialize_disable_sized_range]

namespace ranges_examples {

//! [query_disable_sized_range]
struct constant_time_size_range {};

static_assert(
    !arene::base::ranges::disable_sized_range_v<constant_time_size_range>,
    "A range with a constant-time size() is not opted out"
);
static_assert(
    arene::base::ranges::disable_sized_range_v<forward_list_range>,
    "The range with a linear-time size() is opted out"
);
//! [query_disable_sized_range]

}  // namespace ranges_examples
