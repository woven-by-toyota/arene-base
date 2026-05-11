// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_GIVE_CV_TO_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_GIVE_CV_TO_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/is_const.hpp"
#include "arene/base/stdlib_choice/is_volatile.hpp"
#include "arene/base/type_traits/conditional.hpp"

namespace arene {
namespace base {
namespace give_cv_to_detail {

/// @brief add const qualification to @p T
/// @tparam T the type to const-qualify
///
/// Equivalent to @c T @c const. Provided as a transparent alias to avoid
/// instantiating the class template behind @c std::add_const_t.
template <class T>
using add_const_t = T const;

// parasoft-begin-suppress AUTOSAR-A2_11_1-a-2 "This type trait is required to add volatile"
/// @brief add volatile qualification to @p T
/// @tparam T the type to volatile-qualify
///
/// Equivalent to @c T @c volatile. Provided as a transparent alias to avoid
/// instantiating the class template behind @c std::add_volatile_t.
template <class T>
using add_volatile_t = T volatile;
// parasoft-end-suppress AUTOSAR-A2_11_1-a-2

/// @brief add the const qualification from @c From onto @c To
/// @tparam From the type whose const qualification is copied
/// @tparam To the type that receives the const qualification
///
/// If @c From is const-qualified, the result is <c> To const </c>.
/// Otherwise the result is @c To unchanged. Existing qualifications
/// on @c To are preserved.
template <class From, class To>
using give_const_to_t = conditional_apply_t<std::is_const<From>::value, add_const_t, conditional_identity_t, To>;

/// @brief add the volatile qualification from @c From onto @c To
/// @tparam From the type whose volaitle qualification is copied
/// @tparam To the type that receives the volatile qualification
///
/// If @c From is volatile-qualified, the result is <c> To volatile </c>.
/// Otherwise the result is @c To unchanged. Existing qualifications
/// on @c To are preserved.
template <class From, class To>
using give_volatile_to_t =
    conditional_apply_t<std::is_volatile<From>::value, add_volatile_t, conditional_identity_t, To>;
// parasoft-end-suppress AUTOSAR-A2_11_1-a-2

}  // namespace give_cv_to_detail

/// @brief Add the cv qualification from one type to another
/// @tparam From type to copy cv-ref qualification from
/// @tparam To type to copy cv-ref qualification to
/// @note Does not decay @c To before adding cv qualification. If
///   replacement semantics are desired, apply @c std::remove_cvref_t to @c To.
template <class From, class To>
using give_cv_to_t =                         //
    give_cv_to_detail::give_volatile_to_t<   //
        From,                                //
        give_cv_to_detail::give_const_to_t<  //
            From,
            To>>;

/// @brief Add the cv qualification from one type to another
/// @tparam From type to copy cv-ref qualification from
/// @tparam To type to copy cv-ref qualification to
/// @note Does not decay @c To before adding cv qualification. If
///   replacement semantics are desired, apply @c std::remove_cvref_t to @c To.
template <class From, class To>
class give_cv_to {
 public:
  /// @brief result type
  using type = give_cv_to_t<From, To>;
};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_GIVE_CV_TO_HPP_
