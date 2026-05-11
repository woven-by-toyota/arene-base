// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_IS_ACCESSOR_POLICY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_IS_ACCESSOR_POLICY_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_abstract.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_object.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/type_traits/is_copyable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"

namespace arene {
namespace base {

namespace is_accessor_policy_detail {

/// @brief Check if a type has the required accessor policy typedefs
/// @tparam AccessorPolicy The policy to check
template <typename AccessorPolicy, typename = arene::base::constraints<>>
extern constexpr bool has_typedefs_v = false;

/// @brief Check if a type has the required accessor policy typedefs
/// @tparam AccessorPolicy The policy to check
template <typename AccessorPolicy>
extern constexpr bool has_typedefs_v<
    AccessorPolicy,
    arene::base::constraints<
        typename AccessorPolicy::element_type,
        typename AccessorPolicy::data_handle_type,
        typename AccessorPolicy::reference,
        typename AccessorPolicy::offset_policy,
        typename AccessorPolicy::offset_policy::element_type>> =
    std::is_object<typename AccessorPolicy::element_type>::value &&
    !std::is_abstract<typename AccessorPolicy::element_type>::value &&
    is_copyable_v<typename AccessorPolicy::data_handle_type> &&
    std::is_nothrow_move_constructible<typename AccessorPolicy::data_handle_type>::value &&
    std::is_nothrow_move_assignable<typename AccessorPolicy::data_handle_type>::value &&
    is_nothrow_swappable_v<typename AccessorPolicy::data_handle_type> &&
    std::is_constructible<typename AccessorPolicy::offset_policy, AccessorPolicy const&>::value &&
    std::is_same<typename AccessorPolicy::offset_policy::element_type, typename AccessorPolicy::element_type>::value;

/// @brief Try to call the @c access() member function with the required arguments
/// @tparam AccessorPolicy The policy to check
template <typename AccessorPolicy>
using access_t = decltype(std::declval<AccessorPolicy const&>()
                              .access(std::declval<typename AccessorPolicy::data_handle_type const&>(), std::size_t{}));

/// @brief Try to call the @c offest() member function with the required arguments
/// @tparam AccessorPolicy The policy to check
template <typename AccessorPolicy>
using offset_t = decltype(std::declval<AccessorPolicy const&>()
                              .offset(std::declval<typename AccessorPolicy::data_handle_type const&>(), std::size_t{}));

/// @brief Check if a type has the required accessor policy typedefs and member functions
/// @tparam AccessorPolicy The policy to check
template <typename AccessorPolicy, typename = arene::base::constraints<>>
extern constexpr bool has_typedefs_and_functions_v = false;

/// @brief Check if a type has the required accessor policy typedefs and member functions
/// @tparam AccessorPolicy The policy to check
template <typename AccessorPolicy>
extern constexpr bool has_typedefs_and_functions_v<
    AccessorPolicy,
    arene::base::constraints<
        std::enable_if_t<has_typedefs_v<AccessorPolicy>>,
        access_t<AccessorPolicy>,
        offset_t<AccessorPolicy>>> =
    std::is_same<access_t<AccessorPolicy>, typename AccessorPolicy::reference>::value &&
    std::is_same<offset_t<AccessorPolicy>, typename AccessorPolicy::offset_policy::data_handle_type>::value;

}  // namespace is_accessor_policy_detail

/// @brief Check if a type is a valid accessor policy
/// @tparam AccessorPolicy The policy to check
///
/// A type A meets the accessor policy requirements if
/// * A has a type alias named @c element_type
/// * A has a type alias named @c data_handle_type
/// * A has a type alias named @c reference
/// * A has a type alias named @c offset_policy
/// * A has a member function named @c access that takes a @c data_handle_type and a @c std::size_t
/// * A has a member function named @c offest that takes a @c data_handle_type and a @c std::size_t
/// * @c arene::base::is_copyable_v<A> is true
/// * @c is_nothrow_move_constructible_v<A> is true
/// * @c is_nothrow_move_assignable_v<A> is true
/// * @c is_nothrow_swappable_v<A> is true
template <class AccessorPolicy>
extern constexpr bool is_accessor_policy_v =
    is_accessor_policy_detail::has_typedefs_and_functions_v<AccessorPolicy> && is_copyable_v<AccessorPolicy> &&
    std::is_nothrow_move_constructible<AccessorPolicy>::value &&
    std::is_nothrow_move_assignable<AccessorPolicy>::value && is_nothrow_swappable_v<AccessorPolicy>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_IS_ACCESSOR_POLICY_HPP_
