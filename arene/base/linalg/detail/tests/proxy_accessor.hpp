// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_TESTS_PROXY_ACCESSOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_TESTS_PROXY_ACCESSOR_HPP_

#include "arene/base/mdspan/default_accessor.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"

namespace arene {
namespace base {
namespace linalg_testing {

/// @brief Mutable reference proxy returned by value from an accessor
class integer_reference_proxy {
 public:
  /// @brief Construct a proxy referring to @c value
  /// @param value The element to which this proxy refers
  constexpr explicit integer_reference_proxy(int& value) noexcept
      : value_{value} {}

  /// @brief Assign to the referred-to element
  /// @param value The new element value
  /// @return This proxy
  constexpr auto operator=(int const value) noexcept -> integer_reference_proxy& {
    value_ = value;
    return *this;
  }

  /// @brief Exchange the elements referred to by two proxies
  /// @param left The first proxy
  /// @param right The second proxy
  friend constexpr auto swap(integer_reference_proxy left, integer_reference_proxy right) noexcept -> void {
    auto const temporary = left.value_;
    left.value_ = right.value_;
    right.value_ = temporary;
  }

 private:
  int& value_;
};

/// @brief Accessor that returns mutable reference proxies by value
class proxy_accessor : public arene::base::default_accessor<int> {
 public:
  /// @brief Accessor used after offsetting a data handle
  using offset_policy = proxy_accessor;
  /// @brief Reference proxy returned by @c access
  using reference = integer_reference_proxy;

  /// @brief Access an element through a mutable proxy
  /// @param data The first element in the range
  /// @param index The element offset
  /// @return A proxy referring to the selected element
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  constexpr auto access(data_handle_type data, std::size_t const index) const noexcept -> reference {
    return reference{data[index]};
  }
};

}  // namespace linalg_testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_DETAIL_TESTS_PROXY_ACCESSOR_HPP_
