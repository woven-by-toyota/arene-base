// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_CONTAINER_BASE_TYPE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_CONTAINER_BASE_TYPE_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/inline_container/.*"

namespace arene {
namespace base {
namespace inline_container {
namespace detail {

/// @brief obtains a capacity-erased base type for a container
/// @tparam T container type
///
/// This primary template is empty. Container types must define a
/// specialization.
///
template <class T>
struct container_base_type {};

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: the typedef *is* preceded by a comment with @brief"
/// @brief obtains a capacity-erased base type for a container
/// @tparam T container type
///
template <class T>
using container_base_type_t = typename container_base_type<T>::type;
// parasoft-end-suppress AUTOSAR-A2_7_3-a

}  // namespace detail
}  // namespace inline_container
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_CONTAINER_BASE_TYPE_HPP_
