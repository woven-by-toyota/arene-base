// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_TESTS_WRAPPED_ITERATOR_HELPERS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_TESTS_WRAPPED_ITERATOR_HELPERS_HPP_

#include "arene/base/detail/wrapped_iterator.hpp"  // IWYU pragma: keep
#include "arene/base/iterator/distance.hpp"
#include "arene/base/stdlib_choice/begin.hpp"
#include "arene/base/stdlib_choice/end.hpp"

namespace wrapped_iterator_tests {

// NOLINTBEGIN(hicpp-avoid-c-arrays) Explicitly testing behavior with c-arrays

/// simple passkey type for tests
class default_passkey {};

/// simple alias to avoid a lot of boilerplate
template <typename T, typename Passkey = default_passkey>
using wrapped_iterator_with_default_passkey = ::arene::base::detail::wrapped_iterator<T, default_passkey>;

/// Storage for a container-like object used in tests to allow uniform initialization including c-arrays and strings.
/// @{
template <typename Container>
class test_container_storage {
 public:
  Container data_{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
};

template <>
class test_container_storage<int[10]> {
 public:
  int data_[10]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
};

template <>
class test_container_storage<int const[10]> {
 public:
  int const data_[10]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
};

template <>
class test_container_storage<char[10]> {
 public:
  char data_[10]{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
};

template <>
class test_container_storage<char const[10]> {
 public:
  char const data_[10]{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
};
/// @}

/// A container-like object that provides the normal iterator APIs backed by some actual container-like object.
template <typename Container>
class test_container : private test_container_storage<Container> {
 public:
  constexpr auto begin() { return std::begin(this->data_); }
  constexpr auto end() { return std::end(this->data_); }
  constexpr auto cbegin() const { return std::cbegin(this->data_); }
  constexpr auto cend() const { return std::cend(this->data_); }
  constexpr auto size() const { return ::arene::base::distance(cbegin(), cend()); }
};

// NOLINTEND(hicpp-avoid-c-arrays) Explicitly testing behavior with c-arrays

}  // namespace wrapped_iterator_tests
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_TESTS_WRAPPED_ITERATOR_HELPERS_HPP_
