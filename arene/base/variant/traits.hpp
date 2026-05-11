// parasoft-begin-suppress AUTOSAR-A2_8_1-a "File contains content related to variant traits."

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_TRAITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_TRAITS_HPP_

// IWYU pragma: private, include "arene/base/variant.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/stdlib_choice/add_const.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/type_list/at.hpp"
#include "arene/base/type_list/type_list.hpp"

// parasoft-end-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

namespace arene {
namespace base {

/// @brief Forward declaration of variant
/// @tparam Ts the types in the variant
template <typename... Ts>
class variant;

/// @brief Special value used as the return value of @c variant::index() when @c variant::valueless_by_exception() is
/// true.
constexpr std::size_t variant_npos{std::numeric_limits<std::size_t>::max()};

/// @brief Declaration of @c variant_size_v. Default to @c variant_npos
template <typename Variant>
constexpr std::size_t variant_size_v = variant_npos;

/// @brief Specialization of @c variant_size_v for @c variant.
/// @tparam Ts the types in the variant
template <typename... Ts>
constexpr std::size_t variant_size_v<variant<Ts...>> = sizeof...(Ts);

/// @brief Specialization of @c variant_size_v for const variant.
/// @tparam Ts the types in the variant
template <typename... Ts>
constexpr std::size_t variant_size_v<variant<Ts...> const> = sizeof...(Ts);

/// @brief Helper struct to get the size of a variant.
/// @tparam Variant the variant type
template <typename Variant>
class variant_size : public std::integral_constant<std::size_t, variant_size_v<Variant>> {};

/// @brief Traits to get the I-th type of a variant.
/// @tparam I the index of the type
/// @tparam T the variant type
template <std::size_t I, class T>
struct variant_alternative;

/// @brief Implementation of @c variant_alternative for variant with actual types.
/// @tparam I the index of the type
/// @tparam Ts the types that the variant can hold
template <std::size_t I, class... Ts>
struct variant_alternative<I, variant<Ts...>> {
  /// @brief alternative type
  using type = type_lists::at_t<type_list<Ts...>, I>;
};

/// @brief Implementation of variant_alternative for const variant with actual types.
/// @tparam I the index of the type
/// @tparam T the variant type
template <std::size_t I, class T>
struct variant_alternative<I, T const> {
 public:
  /// @brief const-qualified alternative type
  using type = typename std::add_const<typename variant_alternative<I, T>::type>::type;
};

/// @brief Helper type of variant_size variant_alternative.
/// @tparam I the index of the type
/// @tparam T the variant type
template <std::size_t I, class T>
using variant_alternative_t = typename variant_alternative<I, T>::type;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_TRAITS_HPP_
