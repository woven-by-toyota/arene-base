// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_REPEAT_TYPE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_REPEAT_TYPE_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/stdlib_choice/cstddef.hpp"

namespace arene {
namespace base {

namespace repeat_type_detail {
// forward declares
template <std::size_t N, template <typename...> class T, typename Arg, typename... Args>
struct repeat_type_impl;

template <std::size_t N, template <typename...> class T, typename U, typename Arg, typename... Args>
struct repeat_type_impl_ex;
}  // namespace repeat_type_detail

/// @brief Repeat a template argument a specified number of times.
/// @tparam N The number of times to repeat.
/// @tparam T The templated type.
/// @tparam Arg The repeated argument.
template <std::size_t N, template <typename...> class T, typename Arg>
struct repeat_type {
  /// @brief The resulting type
  using type = typename repeat_type_detail::repeat_type_impl<N - 1UL, T, Arg>::type;
};

/// @brief Repeat a template argument a specified number of times.
/// @details The partial specialization that terminates repetition.
/// @tparam T The templated type.
/// @tparam Arg The repeated argument.
template <template <typename...> class T, typename Arg>
struct repeat_type<0UL, T, Arg> {
  /// @brief The resulting type
  using type = T<>;
};

/// @brief Repeat a template argument a specified number of times.
/// @tparam N The number of times to repeat.
/// @tparam T The templated type.
/// @tparam Arg The repeated argument.
template <std::size_t N, template <typename...> class T, typename Arg>
using repeat_type_t = typename repeat_type<N, T, Arg>::type;

/// @brief Repeat a template argument a specified number of times with an extra leading template argument.
/// @tparam N The number of times to repeat.
/// @tparam T The templated type.
/// @tparam U The extra leading template argument.
/// @tparam Arg The repeated argument.
template <std::size_t N, template <typename...> class T, class U, typename Arg>
struct repeat_type_ex {
  /// @brief The resulting type
  using type = typename repeat_type_detail::repeat_type_impl_ex<N - 1UL, T, U, Arg>::type;
};

/// @brief Repeat a template argument a specified number of times with an extra leading template argument.
/// @details The partial specialization that terminates repetition.
/// @tparam T The templated type.
/// @tparam U The extra leading template argument.
/// @tparam Arg The repeated argument.
template <template <typename...> class T, class U, typename Arg>
struct repeat_type_ex<0UL, T, U, Arg> {
  /// @brief The resulting type
  using type = T<U>;
};

namespace repeat_type_detail {
/// @brief A helper for repeating a template argument a specified number of times.
/// @tparam N The number of times to repeat.
/// @tparam T The templated type.
/// @tparam Arg The repeated argument.
/// @tparam Args The remaining template arguments.
template <std::size_t N, template <typename...> class T, typename Arg, typename... Args>
struct repeat_type_impl {
  /// @brief The resulting type
  using type = typename repeat_type_impl<N - 1UL, T, Arg, Arg, Args...>::type;
};

/// @brief A helper for repeating a template argument a specified number of
/// times.
/// @details The partial specialization that terminates repetition.
/// @tparam T The templated type.
/// @tparam Arg The repeated argument.
/// @tparam Args The remaining template arguments.
template <template <typename...> class T, typename Arg, typename... Args>
struct repeat_type_impl<0UL, T, Arg, Args...> {
  /// @brief The resulting type
  using type = T<Arg, Args...>;
};

/// @brief A helper for repeating a template argument a specified number of
/// times with an extra leading template argument.
/// @tparam N The number of times to repeat.
/// @tparam T The templated type.
/// @tparam U The extra leading template argument.
/// @tparam Arg The repeated argument.
/// @tparam Args The remaining template arguments.
template <std::size_t N, template <typename...> class T, typename U, typename Arg, typename... Args>
struct repeat_type_impl_ex {
  /// @brief The resulting type
  using type = typename repeat_type_impl_ex<N - 1UL, T, U, Arg, Arg, Args...>::type;
};

/// @brief A helper for repeating a template argument a specified number of
/// times with an extra leading template argument.
/// @details The partial specialization that terminates repetition.
/// @tparam T The templated type.
/// @tparam U The extra leading template argument.
/// @tparam Arg The repeated argument.
/// @tparam Args The remaining template arguments.
template <template <typename...> class T, class U, typename Arg, typename... Args>
struct repeat_type_impl_ex<0UL, T, U, Arg, Args...> {
  /// @brief The resulting type
  using type = T<U, Arg, Args...>;
};
}  // namespace repeat_type_detail

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_REPEAT_TYPE_HPP_
