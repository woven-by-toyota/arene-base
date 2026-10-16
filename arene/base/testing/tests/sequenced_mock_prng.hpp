// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_TESTS_SEQUENCED_MOCK_PRNG_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_TESTS_SEQUENCED_MOCK_PRNG_HPP_

#include "arene/base/inline_container/vector.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"

namespace arene {
namespace base {
namespace testing {
namespace tests {

/// @brief A testing PRNG that emits a pre-primed sequence of values, cycling back to the start once exhausted.
/// @tparam T result type produced by @c operator(); also used as the seed type.
template <typename T>
class sequenced_mock_prng {
  arene::base::inline_vector<T, 256> values_{};
  std::size_t index_{};

 public:
  using seed_type = T;
  using result_type = T;

  /// @brief Construct from a list of values that will be returned in order.
  /// @param values sequence of values to return on successive calls to @c operator().
  constexpr sequenced_mock_prng(std::initializer_list<T> values) noexcept
      : values_{values} {}

  /// @brief Return the next value in the buffer and advance the internal index, wrapping at the buffer boundary.
  /// @return the next primed value.
  constexpr auto operator()() noexcept -> result_type {
    auto const value = values_[index_ % values_.size()];
    ++index_;
    return value;
  }

  /// @brief Return the next value in the buffer without advancing the state.
  /// @return the value that the next call to @c operator() would produce.
  constexpr auto peek() noexcept -> result_type {
    auto const value = values_[index_ % values_.size()];
    return value;
  }

  /// @brief smallest value that @c operator() can return
  /// @return zero
  static constexpr auto min() noexcept -> result_type { return {}; }

  /// @brief largest value that @c operator() can return
  /// @return ~min()
  static constexpr auto max() noexcept -> result_type { return static_cast<result_type>(~min()); }

  /// @brief number of primed values in the buffer
  /// @return the size of the underlying value buffer
  constexpr auto size() noexcept -> result_type { return values_.size(); }
};

}  // namespace tests
}  // namespace testing
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_TESTS_SEQUENCED_MOCK_PRNG_HPP_
