// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file type_traits.hpp
/// @brief Public export header for content from arene/base/type_traits
///

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/type_traits/all_are_same.hpp"                      // IWYU pragma: export
#include "arene/base/type_traits/all_of.hpp"                            // IWYU pragma: export
#include "arene/base/type_traits/always_false.hpp"                      // IWYU pragma: export
#include "arene/base/type_traits/any_of.hpp"                            // IWYU pragma: export
#include "arene/base/type_traits/arithmetic_traits.hpp"                 // IWYU pragma: export
#include "arene/base/type_traits/comparison_traits.hpp"                 // IWYU pragma: export
#include "arene/base/type_traits/conjunction.hpp"                       // IWYU pragma: export
#include "arene/base/type_traits/decays_to.hpp"                         // IWYU pragma: export
#include "arene/base/type_traits/denotes_range.hpp"                     // IWYU pragma: export
#include "arene/base/type_traits/disjunction.hpp"                       // IWYU pragma: export
#include "arene/base/type_traits/has_overloaded_address_operator.hpp"   // IWYU pragma: export
#include "arene/base/type_traits/index_of.hpp"                          // IWYU pragma: export
#include "arene/base/type_traits/is_array_convertible.hpp"              // IWYU pragma: export
#include "arene/base/type_traits/is_copyable.hpp"                       // IWYU pragma: export
#include "arene/base/type_traits/is_implicitly_constructible.hpp"       // IWYU pragma: export
#include "arene/base/type_traits/is_instantiation_of.hpp"               // IWYU pragma: export
#include "arene/base/type_traits/is_integral_constant_like.hpp"         // IWYU pragma: export
#include "arene/base/type_traits/is_invocable.hpp"                      // IWYU pragma: export
#include "arene/base/type_traits/is_one_of.hpp"                         // IWYU pragma: export
#include "arene/base/type_traits/is_only_explicitly_constructible.hpp"  // IWYU pragma: export
#include "arene/base/type_traits/is_reference_wrapper.hpp"              // IWYU pragma: export
#include "arene/base/type_traits/is_swappable.hpp"                      // IWYU pragma: export
#include "arene/base/type_traits/is_transparent_comparator_for.hpp"     // IWYU pragma: export
#include "arene/base/type_traits/is_tuple_like.hpp"                     // IWYU pragma: export
#include "arene/base/type_traits/iterator_category_traits.hpp"          // IWYU pragma: export
#include "arene/base/type_traits/member_pointer_class_type.hpp"         // IWYU pragma: export
#include "arene/base/type_traits/negation.hpp"                          // IWYU pragma: export
#include "arene/base/type_traits/none_of.hpp"                           // IWYU pragma: export
#include "arene/base/type_traits/remove_cvref.hpp"                      // IWYU pragma: export
#include "arene/base/type_traits/type_identity.hpp"                     // IWYU pragma: export
#include "arene/base/type_traits/unwrap_reference.hpp"                  // IWYU pragma: export
#include "arene/base/type_traits/void_t.hpp"                            // IWYU pragma: export
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_HPP_
