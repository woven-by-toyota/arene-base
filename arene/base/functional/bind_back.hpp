// parasoft-suppress ALL "False Positive: there is no executable code on line 1"
// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::bind_front"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_BIND_BACK_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_BIND_BACK_HPP_

// IWYU pragma: private, include "arene/base/functional.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/functional/invoke.hpp"
#include "arene/base/functional/perfect_forward_call_wrapper.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/decay.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/type_traits/all_of.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace bind_back_detail {

/// @brief Forward declaration to allow specialization
/// @tparam NBound The number of bound arguments.
/// @tparam Indexes A @c std::index_sequence of size @c NBound .
template <std::size_t NBound, typename Indexes = std::make_index_sequence<NBound>>
class bind_back_policy;

/// @brief Implementation helper for @c arene::base::bind_back intended to be used as a policy for
///        @c arene::base::functional_detail::perfect_forwarding_call_wrapper .
///
/// @tparam NBound The number of bound arguments.
/// @tparam Is The set of indexes of the bound arguments in the BoundArgs tuple .
template <std::size_t NBound, std::size_t... Is>
class bind_back_policy<NBound, std::index_sequence<Is...>> {
 public:
  /// @brief Invokes the provided function as expression-equivalent to @c func(call_args...,bound_args...) .
  ///
  /// @tparam FuncT The type of the callable to invoke.
  /// @tparam BoundArgsTupleT The type of a tuple holding the bound arguments.
  /// @tparam CallArgsT The types of the call arguments to invoke the callable with.
  /// @param func The invocable to invoke.
  /// @param bound_args A tuple of bound arguments to pass to the invocable.
  /// @param call_args The set of call arguments to pass to the invocable.
  /// @return The return from invoking the held callable with the bound and call arguments.
  template <typename FuncT, typename BoundArgsTupleT, typename... CallArgsT>
  constexpr auto operator()(FuncT&& func, BoundArgsTupleT&& bound_args, CallArgsT&&... call_args) const
      noexcept((noexcept(::arene::base::invoke(
          std::forward<FuncT>(func),
          std::forward<CallArgsT>(call_args)...,
          std::get<Is>(std::forward<BoundArgsTupleT>(bound_args))...
      ))))
          -> decltype(::arene::base::invoke(
              std::forward<FuncT>(func),
              std::forward<CallArgsT>(call_args)...,
              std::get<Is>(std::forward<BoundArgsTupleT>(bound_args))...
          )) {
    return ::arene::base::invoke(
        std::forward<FuncT>(func),
        std::forward<CallArgsT>(call_args)...,
        std::get<Is>(std::forward<BoundArgsTupleT>(bound_args))...
    );
  }
};

/// @brief Simplifies correctly setting up the call wrapper and policy
///
/// @tparam BoundFunc The type of the function to bind, post decaying.
/// @tparam BoundArgs The type of the arguments to bind, post decaying.
/// @note This needs to be a struct rather than just a type alias or else gcc crashes on all versions between 8 and 12.
template <typename BoundFunc, typename... BoundArgs>
class bind_back_t
    : public functional_detail::
          perfect_forward_call_wrapper<bind_back_policy<sizeof...(BoundArgs)>, BoundFunc, BoundArgs...> {
 public:
  using functional_detail::perfect_forward_call_wrapper<
      bind_back_policy<sizeof...(BoundArgs)>,
      BoundFunc,
      BoundArgs...>::perfect_forward_call_wrapper;
};

}  // namespace bind_back_detail

/// @brief Produces a call wrapper which allows invoking an input callable with its last N arguments bound to @c args .
///
/// @tparam BoundFunc The type of the callable to create a wrapper for. Must satisfy
//          @c std::is_constructible<std::decay_t<BoundFunc>,BoundFunc> and
//          @c std::is_move_constructible<std::decay_t<BoundFunc>> or else the program is ill-formed.
/// @tparam BoundArgs The types of the arguments to bind to the callable. Must satisfy
//          @c std::is_constructible<std::tuple<std::decay_t<BoundArgs>...>, BoundArgs&&...> and
//          @c std::is_move_constructible<std::tuple<std::decay_t<BoundArgs>...>> or else the program is ill-formed.
/// @param bound_func The callable to create a wrapper for. It will be perfect-forwarded into the call wrapper.
/// @param bound_args The set of arguments to bind to the callable. They will be perfect-forwarded into the call
///        wrapper.
/// @return An implementation defined callable object, whose call operator can consume a set of arguments @c call_args .
///         <c> bind_back(bound_func, bound_args...)(call_args...) </c> is expression-equivalent to
///         <c> invoke(bound_func, call_args..., bound_args...) </c> . The @c noexcept -ness of @c bound_func is
///         preserved by the call operator of the returned object.
///
/// The wrapper perfect forwards the input callable and binding arguments _into the wrapper_, otherwise temporaries
/// would always dangle, and references would be at high risk of dangling. When the returned callable is invoked, its
/// reference qualification is applied to the access of the bound objects. If @c g is the callable returned from @c
/// bind_front(f,args...) , then invoking @c std::move(g)(call_args...) will allow the bound arguments to be moved into
/// the bound callable. Similarly, if @c f were to have non-const lvalue parameters at the position of bound
/// arguments, then invoking @c g(call_args..) when @c g is a non-const lvalue will result in the instances of the bound
/// arguments held in the wrapper being passed to @c f , similar to a @c mutable lambda with a capture block.
///
/// If a caller wishes to maintain "reference semantics" of the bound arguments when it can be assured the lifetime of
/// the bound arguments will exceed the lifetime of the returned callable, this can be achieved by wrapping the argument
/// in @c std::reference_wrapper .
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: there is no function pointer decay/conversion."
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: static cannot be applied, this is not a member function."
template <typename BoundFunc, typename... BoundArgs>
constexpr auto bind_back(BoundFunc&& bound_func, BoundArgs&&... bound_args) noexcept(
    std::is_nothrow_constructible<std::decay_t<BoundFunc>, BoundFunc>::value &&
    std::is_nothrow_constructible<std::tuple<std::decay_t<BoundArgs>...>, BoundArgs&&...>::value
) -> bind_back_detail::bind_back_t<std::decay_t<BoundFunc>, std::decay_t<BoundArgs>...> {
  static_assert(
      std::is_constructible<std::decay_t<BoundFunc>, BoundFunc&&>::value,
      "Bound function must be constructible from input."
  );
  static_assert(
      std::is_move_constructible<std::decay_t<BoundFunc>>::value,
      "Bound function must be move-constructible."
  );
  static_assert(
      ::arene::base::all_of_v<std::is_constructible<std::decay_t<BoundArgs>, BoundArgs&&>::value...>,
      "Bound arguments must be capturable from input."
  );
  static_assert(
      ::arene::base::all_of_v<std::is_move_constructible<std::decay_t<BoundArgs>>::value...>,
      "Bound arguments must be move-constructible."
  );
  return bind_back_detail::bind_back_t<std::decay_t<BoundFunc>, std::decay_t<BoundArgs>...>{
      std::forward<BoundFunc>(bound_func),
      std::forward<BoundArgs>(bound_args)...,
  };
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a "False positive: static cannot be applied, this is not a member function."
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: there is no function pointer decay/conversion."

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_BIND_BACK_HPP_
        // parasoft-end-suppress AUTOSAR-A2_8_1-a-2
