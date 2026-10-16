// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::iter_value_t"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_VALUE_T_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_VALUE_T_HPP_

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/iterator/indirectly_readable_traits.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_cv.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

namespace arene {
namespace base {

/// @brief Determins the value type of the given iterator.
/// @tparam Itr The iterator type.
/// The result type will be equivalent to @c indirectly_readable_traits<remove_cvref_t<Itr>>::value_type if it is
/// defined. Otherwise @c iter_value_t<Itr> will be ill-formed.
///
/// @note unlike @c std::iter_value_t<Itr> , this does not have the fallback to explicit user specializations of
/// @c std::iterator_traits<Itr>::value_type . Users should specialize @c indirectly_readable_traits for their custom
/// iterators if the default behavior is not suitable.
template <typename Itr>
using iter_value_t = typename indirectly_readable_traits<remove_cvref_t<Itr>>::value_type;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_VALUE_T_HPP_
