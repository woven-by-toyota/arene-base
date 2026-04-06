// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_TESTS_MIXIN_AT_TEST_SIMPLE_CONTAINER_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_TESTS_MIXIN_AT_TEST_SIMPLE_CONTAINER_HPP_

#include <gtest/gtest.h>

#include "arene/base/constraints.hpp"
#include "arene/base/detail/exceptions/mixin_at.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_reference.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_list.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace mixin_at_test {

/// @brief A simple helper for using a size parameter in a type parameterized test
/// @tparam N the size.
template <std::size_t N>
using size_wrapper = std::integral_constant<std::size_t, N>;

/// @brief An enumeration of the different value categories.
enum disabled_qualification { none, lvalue, const_lvalue, rvalue, const_rvalue };

/// @brief A simple container-like type which has mixin_at mixed into it.
template <typename IndexReturnType, std::size_t N, disabled_qualification Disabled = disabled_qualification::none>
class simple_container : public arene::base::detail::mixin_at<simple_container<IndexReturnType, N>, std::size_t> {
  using value_type = std::remove_reference_t<IndexReturnType>;
  using lvalue_index_return_type =
      std::conditional_t<std::is_reference<IndexReturnType>::value, value_type&, value_type>;
  using clvalue_index_return_type =
      std::conditional_t<std::is_reference<IndexReturnType>::value, value_type const&, value_type>;

  using rvalue_index_return_type =
      std::conditional_t<std::is_reference<IndexReturnType>::value, value_type&&, value_type>;
  using crvalue_index_return_type =
      std::conditional_t<std::is_reference<IndexReturnType>::value, value_type const&&, value_type>;

  // NOLINTNEXTLINE(hicpp-avoid-c-arrays): Needs to be defined without taking dependencies on std::array
  value_type data_[N];

 public:
  constexpr simple_container()
      : simple_container(std::make_index_sequence<N>{}) {}
  template <std::size_t... Idxs>
  constexpr explicit simple_container(std::index_sequence<Idxs...>)
      : data_{static_cast<value_type>(Idxs)...} {}
  using size_type = typename size_wrapper<N>::value_type;
  constexpr auto size() const noexcept -> size_type { return N; }
  template <
      disabled_qualification DQ = Disabled,
      arene::base::constraints<std::enable_if_t<DQ != disabled_qualification::lvalue>> = nullptr>
  constexpr auto operator[](size_type idx) & -> lvalue_index_return_type {
    return data_[idx];
  }
  template <
      disabled_qualification DQ = Disabled,
      arene::base::constraints<std::enable_if_t<DQ != disabled_qualification::const_lvalue>> = nullptr>
  constexpr auto operator[](size_type idx) const& -> clvalue_index_return_type {
    return data_[idx];
  }
  template <
      disabled_qualification DQ = Disabled,
      arene::base::constraints<std::enable_if_t<DQ != disabled_qualification::rvalue>> = nullptr>
  constexpr auto operator[](size_type idx) && -> rvalue_index_return_type {
    return std::move(data_[idx]);
  }
  template <
      disabled_qualification DQ = Disabled,
      arene::base::constraints<std::enable_if_t<DQ != disabled_qualification::const_rvalue>> = nullptr>
  constexpr auto operator[](size_type idx) const&& -> crvalue_index_return_type {
    return std::move(data_[idx]);
  }
};

/// @brief A typedef of @c simple_container that extracts its template parameters from a type list.
template <typename TypeAndSize, disabled_qualification Disabled = disabled_qualification::none>
using container_t = simple_container<
    arene::base::type_lists::at_t<TypeAndSize, 0U>,
    arene::base::type_lists::at_t<TypeAndSize, 1U>::value,
    Disabled>;

/// @brief A set of instances of @c simple_container with various types and sizes.
using types = ::testing::Types<
    arene::base::type_list<int&, size_wrapper<1U>>,
    arene::base::type_list<int&, size_wrapper<10U>>,
    arene::base::type_list<testing::configurable_value<int>&, size_wrapper<1U>>,
    arene::base::type_list<testing::configurable_value<int>&, size_wrapper<10U>>,
    arene::base::type_list<int, size_wrapper<1U>>,
    arene::base::type_list<int, size_wrapper<10U>>,
    arene::base::type_list<testing::configurable_value<int>, size_wrapper<1U>>,
    arene::base::type_list<testing::configurable_value<int>, size_wrapper<10U>>>;

}  // namespace mixin_at_test
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_TESTS_MIXIN_AT_TEST_SIMPLE_CONTAINER_HPP_
