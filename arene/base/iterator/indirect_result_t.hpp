// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_INDIRECT_RESULT_T_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_INDIRECT_RESULT_T_HPP_

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/iterator/is_indirectly_readable.hpp"
#include "arene/base/iterator/iter_reference_t.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace arene {
namespace base {
namespace indirect_result_detail {
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: cannot apply static here"
/// @brief A deduction helper which ensures that all iterator types are indirectly readable and that the callable F is
/// invocable with the references obtained from them.
/// @tparam F The callable type.
/// @tparam Is The indirectly readable iterator types.
/// @return The result type of invoking F with the references obtained from the indirectly readable iterators.
/// @note Doing constraints via a constrained function overload like this results in clear compiler errors for
/// violations of the constraints.
template <typename F, typename... Is>
constexpr auto require_indirect_result(
    constraints<std::enable_if_t<is_indirectly_readable_v<Is>>...> = nullptr,
    constraints<std::enable_if_t<is_invocable_v<F, iter_reference_t<Is>...>>> = nullptr
) -> invoke_result_t<F, iter_reference_t<Is>...>;
// parasoft-end-suppress AUTOSAR-M3_3_2-a
}  // namespace indirect_result_detail

/// @brief Determines the result type of invoking a callable F with the references obtained from indirectly readable
/// iterators Is...
/// @tparam F The callable type.
/// @tparam Is The indirectly readable iterator types.
/// If all types in @c Is... satisfy @c is_indirectly_readable and @c is_invocable_v<F,iter_reference_t<Is>...> is
/// satisfied, then will be equivalent to @c invoke_result_t<F,iter_reference_t<Is>...> , otherwise will be ill-formed
/// in a manner that will cause substitution failure if evaluated in a SFINAE context.
template <class F, class... Is>
using indirect_result_t = decltype(indirect_result_detail::require_indirect_result<F, Is...>());
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_INDIRECT_RESULT_T_HPP_
