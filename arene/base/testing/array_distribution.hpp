// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_ARRAY_DISTRIBUTION_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_ARRAY_DISTRIBUTION_HPP_

#include "arene/base/algorithm/generate.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/reference_wrapper.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace arene {
namespace base {
namespace testing {

/// @brief A distribution adaptor producing an @c array by drawing each element from the wrapped distribution
/// @tparam Distribution A distribution supplying the element values
/// @tparam N The number of elements in the produced array
///
/// Elements are drawn in ascending index order, and a given PRNG state always yields the same array.
template <class Distribution, std::size_t N>
class array_distribution {
 public:
  /// @brief The value type produced by this adaptor
  using result_type = arene::base::array<typename Distribution::result_type, N>;

  static_assert(
      std::is_default_constructible<typename Distribution::result_type>::value,
      "array_distribution builds its result in place, so the element type must be default constructible"
  );

  /// @brief Default construct adapted distribution
  template <class T = Distribution, constraints<std::enable_if_t<std::is_default_constructible<T>::value>> = nullptr>
  constexpr array_distribution() noexcept
      : dist_{} {}

  /// @brief Construct the adaptor wrapping the given distribution
  /// @param dist The distribution to draw each element from
  explicit constexpr array_distribution(Distribution dist) noexcept
      : dist_{dist} {}

  /// @brief Draw an array whose every element comes from an independent draw of the wrapped distribution
  /// @tparam PRNG A bit generator (PRNG) type satisfying UniformRandomBitGenerator
  /// @param prng A bit generator (PRNG) instance
  /// @return An array of @c N values drawn from the wrapped distribution
  template <class PRNG>
  auto operator()(PRNG& prng) const noexcept(
      arene::base::is_nothrow_invocable_v<Distribution const&, PRNG&> &&
      std::is_nothrow_move_assignable<typename Distribution::result_type>::value &&
      std::is_nothrow_default_constructible<typename Distribution::result_type>::value &&
      std::is_nothrow_move_constructible<result_type>::value
  ) -> result_type {
    result_type result{};
    std::reference_wrapper<PRNG> const generator{prng};
    arene::base::generate(
        result,
        [this, generator]() noexcept(arene::base::is_nothrow_invocable_v<Distribution const&, PRNG&>) {
          return dist_(generator.get());
        }
    );
    return result;
  }

 private:
  /// @brief The distribution each element is drawn from
  Distribution dist_;
};

}  // namespace testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_ARRAY_DISTRIBUTION_HPP_
