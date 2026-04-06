// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::is_implicitly_constructible"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_IMPLICITLY_CONSTRUCTIBLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_IMPLICITLY_CONSTRUCTIBLE_HPP_

#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/type_traits/priority_tag.hpp"

// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: Everything is preceeded by a comment with @brief"
// parasoft-begin-suppress AUTOSAR-A2_7_3-b "False positive: Every function is preceeded by a comment with @return"

namespace arene {
namespace base {

namespace is_implicitly_constructible_detail {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

/// @brief A function which takes a @c T to implicitly construct it.
/// @tparam T The type to check if it can be implicitly constructed.
/// @return Always return @c std::true_type
template <class T>
constexpr auto implicitly_construct(T) noexcept -> std::true_type;

/// @brief A function to try to implicitly construct @c T from @c Args via brace initialization.
/// @tparam T The type to check if it can be implicitly constructed.
/// @tparam Args The arguments to try to implicitly construct with.
/// @return @c std::true_type if @c T is able to be implicitly constructed from @c Args
template <class T, class... Args>
constexpr auto try_to_implicitly_construct(priority_tag<1>) noexcept(
    noexcept(implicitly_construct<T>({std::declval<Args>()...}))
) -> decltype(implicitly_construct<T>({std::declval<Args>()...}));

/// @brief A fallback function to indicate that @c T is not implicitly constructible
/// @tparam T The type to check if it can be implicitly constructed.
/// @tparam Args The arguments to try to implicitly construct with.
/// @return @c Always returns @c std::false_type
template <class T, class... Args>
constexpr auto try_to_implicitly_construct(priority_tag<0>) noexcept -> std::false_type;

/// @brief Helper to determine if a type is nothrow implicitly constructible
/// @tparam IsConstructible True if @c T is implicitly constructible from @c Args
/// @tparam T The type to check if it can be implicitly constructed.
/// @tparam Args The arguments to try to implicitly construct with.
template <bool IsConstructible, class T, class... Args>
class is_nothrow_implicitly_constructible_impl : public std::false_type {};

/// @brief Helper to determine if a type is nothrow implicitly constructible
/// @tparam T The type to check if it can be implicitly constructed.
/// @tparam Args The arguments to try to implicitly construct with.
template <class T, class... Args>
class is_nothrow_implicitly_constructible_impl<true, T, Args...> {
 public:
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "False positive: this is not 'member data', it is a public property"
  /// @brief True if @c T if nothrow implicitly constructible from @c Args
  static constexpr bool value{
      noexcept(is_implicitly_constructible_detail::try_to_implicitly_construct<T, Args...>(priority_tag<1>{}))
  };
  // parasoft-end-suppress AUTOSAR-M11_0_1-a
};

// parasoft-end-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

}  // namespace is_implicitly_constructible_detail

/// @brief Type trait to detect if a type is implicitly constructible from arguments of the specified types
/// @tparam T The type to check if it can be implicitly constructed.
/// @tparam Args The arguments to try to implicitly construct with.
template <class T, class... Args>
using is_implicitly_constructible =
    decltype(is_implicitly_constructible_detail::try_to_implicitly_construct<T, Args...>(priority_tag<1>{}));

/// @brief Type trait to detect if a type is implicitly constructible from arguments of the specified types
/// @tparam T The type to check if it can be implicitly constructed.
/// @tparam Args The arguments to try to implicitly construct with.
template <class T, class... Args>
extern constexpr bool is_implicitly_constructible_v = is_implicitly_constructible<T, Args...>::value;

/// @brief Type trait to detect if a type is nothrow implicitly constructible from arguments of the specified types
/// @tparam T The type to check if it can be nothrow implicitly constructed.
/// @tparam Args The arguments to try to nothrow implicitly construct with.
template <class T, class... Args>
using is_nothrow_implicitly_constructible = is_implicitly_constructible_detail::
    is_nothrow_implicitly_constructible_impl<is_implicitly_constructible_v<T, Args...>, T, Args...>;

/// @brief Type trait to detect if a type is nothrow implicitly constructible from arguments of the specified types
/// @tparam T The type to check if it can be nothrow implicitly constructed.
/// @tparam Args The arguments to try to nothrow implicitly construct with.
template <class T, class... Args>
extern constexpr bool is_nothrow_implicitly_constructible_v = is_nothrow_implicitly_constructible<T, Args...>::value;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_IMPLICITLY_CONSTRUCTIBLE_HPP_
