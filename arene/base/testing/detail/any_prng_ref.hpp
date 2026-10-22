// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_DETAIL_ANY_PRNG_REF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_DETAIL_ANY_PRNG_REF_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/testing/.*"

#include "arene/base/functional/function_ref.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace arene {
namespace base {
namespace testing {
namespace detail {

/// @brief non-owning, type-erased reference to a 64-bit PRNG
/// @attention The referenced PRNG must outlive uses of @c any_prng_ref.
class any_prng_ref {
  function_ref<std::uint64_t()> call_;

 public:
  using result_type = std::uint64_t;

  explicit any_prng_ref(function_ref<std::uint64_t()> call) noexcept
      : call_(call) {}

  auto operator()() const -> result_type { return call_(); }

  static constexpr auto min() noexcept -> result_type { return {}; }
  static constexpr auto max() noexcept -> result_type { return ~min(); }
};

}  // namespace detail
}  // namespace testing
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_DETAIL_ANY_PRNG_REF_HPP_
