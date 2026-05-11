// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_CONCAT_UNIQUE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_CONCAT_UNIQUE_HPP_

// IWYU pragma: private, include "arene/base/type_list.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"

namespace arene {
namespace base {
namespace type_lists {

/// @brief Concatenate multiple lists together with duplicate types removed.
/// @tparam Ls The type lists to concatenate
template <class... Ls>
using concat_unique_t = arene::base::type_lists::remove_duplicates_t<arene::base::type_lists::concat_t<Ls...>>;

}  // namespace type_lists
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_CONCAT_UNIQUE_HPP_
