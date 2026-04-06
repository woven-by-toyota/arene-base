// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTRUSIVE_DETAIL_DEFAULT_TAG_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTRUSIVE_DETAIL_DEFAULT_TAG_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/intrusive/.*"

namespace arene {
namespace base {
namespace intrusive {
namespace detail {
/// @brief Default tag type given to the intrusive container base hook
/// if no custom tag is specified. Custom tag can be any incomplete type.
/// A tag is used to differentiate between different intrusive containers
/// operating on the same user type. See the intrusive priority queue
/// test for an example.
struct default_tag {};
}  // namespace detail
}  // namespace intrusive
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTRUSIVE_DETAIL_DEFAULT_TAG_HPP_
