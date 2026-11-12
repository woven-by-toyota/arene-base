// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_DEFAULT_DISTRIBUTION_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_DEFAULT_DISTRIBUTION_HPP_

#include "arene/base/testing/bitwise_uniform_distribution.hpp"

namespace arene {
namespace base {
namespace testing {

/// @brief Customization point selecting the default distribution used to generate values of type @c T.
/// @tparam T The value type to generate
///
/// This variable template is used by the property test runtime whenever a property macro
/// auto-generates an argument of type @c T. The primary template yields a default-constructed
/// @c bitwise_uniform_distribution<T>. Specialize it for a user type to change that type's default generation
/// strategy without modifying the framework, for example:
///
/// @code{.cpp}
/// template <>
/// constexpr auto default_distribution<my_type> = my_type_distribution{};
/// @endcode
template <class T>
extern constexpr bitwise_uniform_distribution<T> default_distribution{};

}  // namespace testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_DEFAULT_DISTRIBUTION_HPP_
