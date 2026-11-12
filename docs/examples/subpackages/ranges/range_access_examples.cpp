// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @example examples/subpackages/ranges/range_access_examples.cpp
/// @brief Examples for the ranges/range_access subpackage documentation
///

#include <cstddef>
#include <vector>

#include "arene/base/array.hpp"

//! [include_header]
#include "arene/base/ranges.hpp"
//! [include_header]

namespace ranges_range_access_examples {

//! [generic_range_access]
/// @brief Sums the elements in a range using the range access objects.
template <typename Range>
auto sum_range(Range& range) -> int {
  auto first = arene::base::ranges::begin(range);
  auto last = arene::base::ranges::end(range);
  auto total = 0;
  while (first != last) {
    total += *first;
    ++first;
  }
  return total;
}
//! [generic_range_access]

//! [container_usage]
/// @brief Sums values from standard containers with the same generic function.
auto use_containers() -> int {
  arene::base::array<int, 3> fixed_values{{1, 2, 3}};
  std::vector<int> dynamic_values{1, 2, 3};

  return sum_range(fixed_values) + sum_range(dynamic_values);
}
//! [container_usage]

//! [member_range]
/// @brief A range customized with member begin() and end() functions.
class member_range {
 public:
  using iterator = arene::base::array<int, 3>::const_iterator;

  explicit member_range(arene::base::array<int, 3> const& values) noexcept
      : values_(&values) {}

  auto begin() const noexcept -> iterator { return values_->begin(); }
  auto end() const noexcept -> iterator { return values_->end(); }

 private:
  arene::base::array<int, 3> const* values_;
};
//! [member_range]

//! [member_range_usage]
/// @brief Uses a range with member begin() and end() functions.
auto use_member_range() -> int {
  arene::base::array<int, 3> const values{{1, 2, 3}};
  member_range range{values};

  return sum_range(range);
}
//! [member_range_usage]

namespace adl_range {

//! [adl_range]
/// @brief A range customized with free begin() and end() functions.
class range {
 public:
  using iterator = arene::base::array<int, 3>::const_iterator;

  explicit range(arene::base::array<int, 3> const& values) noexcept
      : values_(&values) {}

  auto values() const noexcept -> arene::base::array<int, 3> const& { return *values_; }

 private:
  arene::base::array<int, 3> const* values_;
};

/// @brief Returns the beginning iterator for range through ADL.
auto begin(range const& input) noexcept -> range::iterator { return input.values().begin(); }

/// @brief Returns the ending iterator for range through ADL.
auto end(range const& input) noexcept -> range::iterator { return input.values().end(); }
//! [adl_range]

}  // namespace adl_range

//! [adl_range_usage]
/// @brief Uses a range customized with ADL begin() and end() functions.
auto use_adl_range() -> int {
  arene::base::array<int, 3> const values{{1, 2, 3}};
  adl_range::range range{values};

  return sum_range(range);
}
//! [adl_range_usage]

//! [member_size]
/// @brief A range whose size is reported by a constant-time member size().
class member_sized_range {
 public:
  explicit member_sized_range(arene::base::array<int, 3> const& values) noexcept
      : values_(&values) {}

  auto size() const noexcept -> std::size_t { return values_->size(); }

 private:
  arene::base::array<int, 3> const* values_;
};
//! [member_size]

//! [begin_end_size]
/// @brief A range that exposes only begin()/end(); its size is computed as end() - begin().
class begin_end_sized_range {
 public:
  using iterator = arene::base::array<int, 3>::const_iterator;

  explicit begin_end_sized_range(arene::base::array<int, 3> const& values) noexcept
      : values_(&values) {}

  auto begin() const noexcept -> iterator { return values_->begin(); }
  auto end() const noexcept -> iterator { return values_->end(); }

 private:
  arene::base::array<int, 3> const* values_;
};
//! [begin_end_size]

//! [query_size]
/// @brief The member-size path and the begin()/end() path agree when they describe the same elements.
auto sizes_agree() -> bool {
  arene::base::array<int, 3> const values{{1, 2, 3}};
  member_sized_range const member_range{values};
  begin_end_sized_range const begin_end_range{values};

  // Uses the constant-time member size().
  std::size_t const from_member = arene::base::ranges::size(member_range);
  // Computed from end() - begin() because no size() member is present.
  std::size_t const from_begin_end = arene::base::ranges::size(begin_end_range);
  return from_member == from_begin_end;
}
//! [query_size]

}  // namespace ranges_range_access_examples
