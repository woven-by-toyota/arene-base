// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTS_MAP_TEST_HELPERS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTS_MAP_TEST_HELPERS_HPP_

#include <gtest/gtest.h>

#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/inline_container/map.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/remove_const.hpp"
#include "arene/base/strings/inline_string.hpp"  // IWYU pragma: export

namespace map_test {

constexpr std::size_t test_string_size{50};
using test_string = ::arene::base::inline_string<test_string_size>;

/// if we allow explicit conversion to the key-type or not.
enum class explicit_conversion_to_key { allowed, not_allowed };

/// @brief A helper type which is comparable to a key, but not implicitly convertible to it.
/// @tparam KeyType the key type to use
template <typename KeyType>
class comparable_not_convertible_to_key_impl {
 public:
  // the value of the transparent key
  // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
  KeyType value;
  explicit_conversion_to_key conversion_allowed{explicit_conversion_to_key::not_allowed};
  // NOLINTEND(misc-non-private-member-variables-in-classes)

  // compares a transparent key and an actual key
  static constexpr auto three_way_compare(
      comparable_not_convertible_to_key_impl const& lhs,
      KeyType const& rhs
  ) noexcept -> arene::base::strong_ordering {
    return arene::base::compare_three_way{}(lhs.value, rhs);
  }

  // _explicitly_ converts a transparent key to its key.
  explicit operator KeyType() const {
    if (conversion_allowed == explicit_conversion_to_key::allowed) {
      return value;
    }
    ARENE_INVARIANT_UNREACHABLE("Invalid conversions are not part of this test");
  }
};

///  @brief Helper fixture base class that initializes a map with values and provides some stock keys to use.
template <typename MapType>
// NOLINTNEXTLINE(readability-identifier-naming) Using Google Test Fixture style naming
class InlineMapTransparentComparisonEquivalencyBase {
 public:
  // The container type to use for equivalence testing
  using imap = MapType;
  // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
  // NOLINTBEGIN(cppcoreguidelines-avoid-const-or-ref-data-members)
  // A key which is less than all keys in the container
  typename imap::key_type const key_less_than_container{0};
  // A key which is between keys in the container
  typename imap::key_type const key_gap_in_container{10};
  // A key which is in the container
  typename imap::key_type const key_in_container = key_gap_in_container + 1;
  // A key which is greater than all keys in the container
  typename imap::key_type const key_greater_than_container{std::numeric_limits<typename imap::key_type>::max()};
  // NOLINTEND(cppcoreguidelines-avoid-const-or-ref-data-members)

  // The container to use for equivalence testing
  imap values;
  // NOLINTEND(misc-non-private-member-variables-in-classes)

  InlineMapTransparentComparisonEquivalencyBase()
      : values([this]() {
          std::remove_const_t<imap> result;
          typename imap::mapped_type value{};
          for (typename imap::key_type key = key_less_than_container + 1; key <= key_in_container + 1; ++key) {
            if (key != key_gap_in_container) {
              result[key] = ++value;
            }
          }
          return result;
        }()) {}
};

/// @brief specialization for string keys from a non-const map.
template <typename MappedType, std::size_t Capacity, typename Compare>
class InlineMapTransparentComparisonEquivalencyBase<
    arene::base::inline_map<test_string, MappedType, Capacity, Compare>> {
 public:
  // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
  // NOLINTBEGIN(cppcoreguidelines-avoid-const-or-ref-data-members)
  // The container type to use for equivalence testing
  using imap = arene::base::inline_map<test_string, MappedType, Capacity, Compare>;
  // A key which is less than all keys in the container
  typename imap::key_type const key_less_than_container;
  // A key which is between keys in the container
  typename imap::key_type const key_gap_in_container{"d"};
  // A key which is in the container
  typename imap::key_type const key_in_container{"e"};
  // A key which is greater than all keys in the container
  typename imap::key_type const key_greater_than_container{"z"};

  // The container to use for equivalence testing
  // NOLINTNEXTLINE(readability-magic-numbers)
  imap values{{"a", 1}, {"b", 2}, {"c", 3}, {"e", 5}, {"f", 6}};
  // NOLINTEND(cppcoreguidelines-avoid-const-or-ref-data-members)
  // NOLINTEND(misc-non-private-member-variables-in-classes)
};

/// @brief specialization for string keys from a const map.
template <typename MappedType, std::size_t Capacity, typename Compare>
class InlineMapTransparentComparisonEquivalencyBase<
    arene::base::inline_map<test_string, MappedType, Capacity, Compare> const> {
 public:
  // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
  // NOLINTBEGIN(cppcoreguidelines-avoid-const-or-ref-data-members)
  // The container type to use for equivalence testing
  using imap = arene::base::inline_map<test_string, MappedType, Capacity, Compare> const;
  // A key which is less than all keys in the container
  typename imap::key_type const key_less_than_container;
  // A key which is between keys in the container
  typename imap::key_type const key_gap_in_container{"d"};
  // A key which is in the container
  typename imap::key_type const key_in_container{"e"};
  // A key which is greater than all keys in the container
  typename imap::key_type const key_greater_than_container{"z"};

  // The container to use for equivalence testing
  // NOLINTNEXTLINE(readability-magic-numbers)
  imap values{{"a", 1}, {"b", 2}, {"c", 3}, {"e", 5}, {"f", 6}};
  // NOLINTEND(cppcoreguidelines-avoid-const-or-ref-data-members)
  // NOLINTEND(misc-non-private-member-variables-in-classes)
};

template <typename T>
// safe, and required as you ultimately _must_ inherit from testing::Test. Using Google Test Fixture style naming
// NOLINTNEXTLINE(fuchsia-multiple-inheritance, readability-identifier-naming)
class InlineMapTransparentComparisonEquivalencyTest
    : public InlineMapTransparentComparisonEquivalencyBase<T>
    , public ::testing::Test {
 public:
  using imap = typename InlineMapTransparentComparisonEquivalencyBase<T>::imap;
  using comparable_not_convertible_to_key = comparable_not_convertible_to_key_impl<typename imap::key_type>;
};

// NOLINTNEXTLINE(readability-identifier-naming) Using Google Test Fixture style naming
class InlineMapTransparentComparisonMutatingEquivalencyTest
    : public InlineMapTransparentComparisonEquivalencyTest<
          // NOLINTNEXTLINE(readability-magic-numbers)
          arene::base::inline_map<test_string, int, 10, arene::base::compare_three_way>> {
 public:
  // NOLINTNEXTLINE(readability-magic-numbers)
  imap::mapped_type inserted_value = 100;
};

// NOLINTBEGIN(readability-magic-numbers)
using const_and_non_const_transparent_imaps = ::testing::Types<
    arene::base::inline_map<std::uint32_t, int, 20>,
    arene::base::inline_map<std::uint32_t, int, 20> const,
    arene::base::inline_map<test_string, int, 20>,
    arene::base::inline_map<test_string, int, 20> const>;
// NOLINTEND(readability-magic-numbers)

struct reverse_less_than {
  template <typename T, typename U>
  auto operator()(T const& lhs, U const& rhs) const -> bool {
    return rhs < lhs;
  }
};

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables) explicitly tracking global counts
template <typename T>
struct lt_comparable_counted {
  T val;

  static std::size_t lt_count;

  static void reset() { lt_count = 0; }

  friend auto operator<(lt_comparable_counted const& lhs, lt_comparable_counted const& rhs) noexcept -> bool {
    ++lt_count;
    return lhs.val < rhs.val;
  }
};
template <typename T>
std::size_t lt_comparable_counted<T>::lt_count = 0;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)
}  // namespace map_test
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTS_MAP_TEST_HELPERS_HPP_
