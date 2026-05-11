// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_GIVE_CVREF_TO_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_GIVE_CVREF_TO_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/is_lvalue_reference.hpp"
#include "arene/base/stdlib_choice/is_rvalue_reference.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_traits/conditional.hpp"
#include "arene/base/type_traits/give_cv_to.hpp"

namespace arene {
namespace base {
namespace give_cvref_to_detail {

/// @brief add the reference qualification from @c From onto @c To
/// @tparam From the type whose reference qualification is copied
/// @tparam To the type that receives the reference qualification
///
/// If @c From is an lvalue reference, the result is @c To&. If @c From
/// is an rvalue reference, the result is @c To&&. Otherwise the result
/// is @c To unchanged. Existing qualifications on @c To are preserved.
template <class From, class To>
using give_reference_to = conditional_t<  //
    std::is_lvalue_reference<From>::value,
    To&,
    conditional_t<  //
        std::is_rvalue_reference<From>::value,
        To&&,
        To> >;

}  // namespace give_cvref_to_detail

/// @brief Add the cv-ref qualification from one type to another
/// @tparam From type to copy cv-ref qualification from
/// @tparam To type to copy cv-ref qualification to
/// @note Does not decay @c To before adding cv or ref qualification. If
///   replacement semantics are desired, apply @c std::remove_cvref_t to @c To.
template <class From, class To>
using give_cvref_to_t =                       //
    give_cvref_to_detail::give_reference_to<  //
        From,
        give_cv_to_t<std::remove_reference_t<From>, To> >;

/// @brief Add the cv-ref qualification from one type to another
/// @tparam From type to copy cv-ref qualification from
/// @tparam To type to copy cv-ref qualification to
/// @note Does not decay @c To before adding cv or ref qualification. If
///   replacement semantics are desired, apply @c std::remove_cvref_t to @c To.
template <class From, class To>
class give_cvref_to {
 public:
  /// @brief result type
  using type = give_cvref_to_t<From, To>;
};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_GIVE_CVREF_TO_HPP_
