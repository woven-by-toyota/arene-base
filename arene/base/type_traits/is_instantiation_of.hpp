// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_INSTANTIATION_OF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_INSTANTIATION_OF_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"
namespace arene {
namespace base {

/// @c true iff @c Type is an instantiation of @c Template
/// @note The default definition is always @c false. See specialization below.
template <class Type, template <class...> class Template>
constexpr bool is_instantiation_of_v = false;

/// @c true iff @c Type is an instantiation of @c Template
/// @note This specialization is always @c true.
template <class... P, template <class...> class Template>
constexpr bool is_instantiation_of_v<Template<P...>, Template> = true;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_INSTANTIATION_OF_HPP_
