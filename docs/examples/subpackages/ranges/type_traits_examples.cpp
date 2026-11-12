// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @example examples/subpackages/ranges/type_traits_examples.cpp
/// @brief Examples for the ranges/type_traits subpackage documentation
///

#include <array>
#include <cstddef>
#include <type_traits>

//! [include_header]
#include "arene/base/ranges.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
//! [include_header]

namespace ranges_examples {

//! [range_types]
/// @brief A range whose iterators refer to storage owned by the range.
struct owning_range {
  auto begin() const noexcept -> std::array<int, 3>::const_iterator { return values_.begin(); }
  auto end() const noexcept -> std::array<int, 3>::const_iterator { return values_.end(); }

 private:
  std::array<int, 3> values_{};
};

/// @brief A generic range whose iterators refer to storage owned elsewhere.
template <typename T>
class user_borrowed_range {
 public:
  explicit user_borrowed_range(T const* first, T const* last) noexcept
      : first_(first),
        last_(last) {}

  auto begin() const noexcept -> T const* { return first_; }
  auto end() const noexcept -> T const* { return last_; }

 private:
  T const* first_;
  T const* last_;
};
//! [range_types]

}  // namespace ranges_examples

//! [specialize_borrowed_range]
/// @brief A user specialization of @c enable_borrowed_range which is unconditionally a borrowed range.
/// @note The specialization must be contained in the @c arene::base::ranges namespace.
/// This means doing it at global scope if done using an FQN like this example, or by explicitly opening
/// the @c arene::base::ranges namespace and placing it inside it.
template <typename T>
class arene::base::ranges::enable_borrowed_range<ranges_examples::user_borrowed_range<T>> : public std::true_type {};
//! [specialize_borrowed_range]

namespace ranges_examples {

namespace ranges = arene::base::ranges;

//! [query_borrowed_range]
static_assert(!ranges::enable_borrowed_range_v<owning_range>, "Owning ranges are not borrowed");
static_assert(ranges::enable_borrowed_range_v<user_borrowed_range<int>>, "The borrowed range is opted in");
static_assert(ranges::enable_borrowed_range_v<user_borrowed_range<int>>, "The borrowed range is opted in");
//! [query_borrowed_range]

//! [is_range_usage]
/// @brief A type with no range access operations.
struct not_a_range {};

/// @brief A type whose accessors do not return an iterator pair as defined by @c denotes_range .
struct invalid_range {
  static auto begin() noexcept -> int { return 0; }
  static auto end() noexcept -> int { return 0; }
};

static_assert(ranges::is_range_v<std::array<int, 3>&>, "std::array is a range");
static_assert(ranges::is_range_v<owning_range&>, "A non-borrowed range lvalue is a range");
static_assert(!ranges::is_range_v<owning_range&&>, "A non-borrowed range rvalue is not a range");
static_assert(ranges::is_range_v<user_borrowed_range<int>&>, "A borrowed range lvalue is a range");
static_assert(ranges::is_range_v<user_borrowed_range<int>&&>, "A borrowed range rvalue is a range");
static_assert(!ranges::is_range_v<not_a_range&>, "A type without accessors is not a range");
static_assert(!ranges::is_range_v<invalid_range&>, "Invalid iterator types do not form a range");
//! [is_range_usage]

//! [is_borrowed_range_usage]
static_assert(
    ranges::is_borrowed_range_v<owning_range&>,
    "A lvalue reference to a range which does not specialize enable_borrowed_range is a borrowed range"
);
static_assert(
    !ranges::is_borrowed_range_v<owning_range&&>,
    "An rvalue reference to a range which does not specialize enable_borrowed_range is not a borrowed range"
);
static_assert(
    ranges::is_borrowed_range_v<user_borrowed_range<int>&>,
    "A lvalue reference to a range which specializes enable_borrowed_range is a borrowed range"
);
static_assert(
    ranges::is_borrowed_range_v<user_borrowed_range<int>&&>,
    "An rvalue reference to a range which specializes enable_borrowed_range is a borrowed range"
);
//! [is_borrowed_range_usage]

//! [borrowed_iterator_t_usage]
static_assert(
    std::is_same<ranges::borrowed_iterator_t<user_borrowed_range<int>>, ranges::iterator_t<user_borrowed_range<int>>>::
        value,
    "For a borrowed range, borrowed_iterator_t should be the same as iterator_t"
);
static_assert(
    std::is_same<ranges::borrowed_iterator_t<owning_range>, ranges::dangling>::value,
    "For a non-borrowed range, borrowed_iterator_t should be dangling"
);
//! [borrowed_iterator_t_usage]

}  // namespace ranges_examples
