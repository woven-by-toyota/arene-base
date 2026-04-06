// parasoft-suppress ALL "False Positive: there is no executable code on line 1"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_PERFECT_FORWARD_CALL_WRAPPER_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_PERFECT_FORWARD_CALL_WRAPPER_HPP_

// IWYU pragma: private, include "arene/base/functional.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/forward_as_tuple.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/type_manipulation/ebo_holder.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace functional_detail {

/// @brief A tag type to allow empty base class optimization for empty function objects.
struct pfcw_fun_tag {};

/// @brief A tag type to allow empty base class optimization for empty bound arguments.
struct pfcw_bargs_tag {};

///
/// @brief A Perfect Forwarding Call Wrapper which holds a functor and bound arguments and dispatches to a policy.
///
/// The primary utility of this wrapper is the following:
///
//  #. Store the functor
/// #. Store the bound arguments
/// #. Handle "perfect forwarding" of the functor and bound arguments to the bound policy. This means not only ensuring
///    the cref qualification of the object is properly propagated to the callable and arguments, but critically it also
///    means ensuring that if @c BoundFuncT declares a given call operator overload as @c =delete; , then the equivalent
///    operator overload is also deleted in the wrapper. See [this
///    article](https://quuxplusone.github.io/blog/2021/07/30/perfect-forwarding-call-wrapper/) for additional details.
///    @c noexcept of the given overload is also propagated.
///
/// The @c Policy is an _invocable_ which the actual work of invoking the function with the bound arguments and call
/// arguments. It will be invoked as <c>Policy{}(func, bound_args, call_args...)</c> where @c func is the instance of @c
/// BoundFuncT and @c bound_args is @c std::tuple<BoundArgs...> , both with the appropriate cref qualification.
/// @c call_args... are the forwarded arguments to the invoked call operator.
///
/// @see arene::base::bind_front for an example usage.
///
/// @tparam Policy An invocable policy to use which actually invokes the function with arguments.
/// @tparam Func The type of the function object to bind.
/// @tparam BoundArgs The types of the bound arguments.
///
// parasoft-begin-suppress AUTOSAR-A12_1_5-a "False positive: There are no constructors to delegate to."
// parasoft-begin-suppress AUTOSAR-A10_1_1-a "Multiple base classes needed for empty-base-class optimizations."
template <typename Policy, typename Func, typename... BoundArgs>
class perfect_forward_call_wrapper
    : ebo_holder<pfcw_fun_tag, Func>
    , ebo_holder<pfcw_bargs_tag, std::tuple<BoundArgs...>> {
  /// @brief The type of a tuple containing any bound arguments
  using bound_arg_tuple = std::tuple<BoundArgs...>;

  /// @brief The type of the ebo holder for the function object
  using func_ebo_holder = ebo_holder<pfcw_fun_tag, Func>;

  /// @brief The type of the ebo holder for the bound arguments
  using bargs_ebo_holder = ebo_holder<pfcw_bargs_tag, bound_arg_tuple>;

 protected:
  /// @brief default copy ctor
  constexpr perfect_forward_call_wrapper(perfect_forward_call_wrapper const& copy) = default;
  /// @brief default move ctor
  constexpr perfect_forward_call_wrapper(perfect_forward_call_wrapper&& move) = default;
  /// @brief default copy-assignment
  constexpr auto operator=(perfect_forward_call_wrapper const& copy) -> perfect_forward_call_wrapper& = default;
  /// @brief default move-assignment
  constexpr auto operator=(perfect_forward_call_wrapper&& move) -> perfect_forward_call_wrapper& = default;

 public:
  /// @brief Constructs the bind_front wrapper with the given function and bound arguments
  ///
  /// @tparam BoundFunc The type of the function object to bind.
  /// @tparam ArgsToBind The types of the bound arguments.
  /// @param func_to_bind The function to bind
  /// @param args_to_bind The arguments to bind
  template <typename BoundFunc, typename... ArgsToBind>
  constexpr explicit perfect_forward_call_wrapper(BoundFunc&& func_to_bind, ArgsToBind&&... args_to_bind) noexcept(
      std::is_nothrow_constructible<Func, BoundFunc&&>::value &&
      std::is_nothrow_constructible<std::tuple<BoundArgs...>, ArgsToBind&&...>::value
  )
      : func_ebo_holder(std::forward<BoundFunc>(func_to_bind)),
        bargs_ebo_holder(std::forward_as_tuple(std::forward<ArgsToBind>(args_to_bind)...)) {
    static_assert(sizeof...(ArgsToBind) == sizeof...(BoundArgs), "Must store all bound arguments");
  }

  /// @brief default dtor
  ~perfect_forward_call_wrapper() = default;

  /// @brief Invokes the held callable with both the held bound arguments and the call arguments.
  ///
  /// @tparam CallArgsT The types of the call arguments to invoke the callable with.
  /// @param call_args The set of call arguments to invoke the callable with.
  /// @return The return from invoking the held callable with the bound and call arguments.
  template <
      typename... CallArgsT,
      constraints<std::enable_if_t<is_invocable_v<Policy, Func&, bound_arg_tuple&, CallArgsT&&...>>> = nullptr>
  constexpr auto operator()(CallArgsT&&... call_args
  ) & noexcept(is_nothrow_invocable_v<Policy, Func&, bound_arg_tuple&, CallArgsT&&...>)
      -> invoke_result_t<Policy, Func&, bound_arg_tuple&, CallArgsT&&...> {
    // parasoft-begin-suppress AUTOSAR-M14_6_1-a "False positive: The dependent name is qualified."
    return Policy{
    }(this->func_ebo_holder::get_value(pfcw_fun_tag{}),
      this->bargs_ebo_holder::get_value(pfcw_bargs_tag{}),
      std::forward<CallArgsT>(call_args)...);
    // parasoft-end-suppress AUTOSAR-M14_6_1-a "False positive: The dependent name is qualified."
  }

  /// @brief Deleted lvalue overload if the wrapped callable's operator() is explicitly deleted for lvalues.
  ///
  /// @tparam CallArgsT The types of the call arguments to invoke the callable with.
  /// @param call_args The set of call arguments to invoke the callable with.
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: this operator is deleted, it invokes nothing."
  template <
      typename... CallArgsT,
      constraints<std::enable_if_t<!is_invocable_v<Policy, Func&, bound_arg_tuple&, CallArgsT&&...>>> = nullptr>
  constexpr auto operator()(CallArgsT&&... call_args) & = delete;
  // parasoft-end-suppress CERT_C-EXP37-a

  /// @brief Invokes the held callable with both the held bound arguments and the call arguments.
  ///
  /// @tparam CallArgsT The types of the call arguments to invoke the callable with.
  /// @param call_args The set of call arguments to invoke the callable with.
  /// @return The return from invoking the held callable with the bound and call arguments.
  template <
      typename... CallArgsT,
      constraints<std::enable_if_t<is_invocable_v<Policy, Func const&, bound_arg_tuple const&, CallArgsT&&...>>> =
          nullptr>
  constexpr auto operator()(CallArgsT&&... call_args
  ) const& noexcept(is_nothrow_invocable_v<Policy, Func const&, bound_arg_tuple const&, CallArgsT&&...>)
      -> invoke_result_t<Policy, Func const&, bound_arg_tuple const&, CallArgsT&&...> {
    // parasoft-begin-suppress AUTOSAR-M14_6_1-a "False positive: The dependent name is qualified."
    return Policy{
    }(this->func_ebo_holder::get_value(pfcw_fun_tag{}),
      this->bargs_ebo_holder::get_value(pfcw_bargs_tag{}),
      std::forward<CallArgsT>(call_args)...);
    // parasoft-end-suppress AUTOSAR-M14_6_1-a "False positive: The dependent name is qualified."
  }

  /// @brief Deleted clvalue overload if the wrapped callable's operator() is explicitly deleted for clvalues.
  ///
  /// @tparam CallArgsT The types of the call arguments to invoke the callable with.
  /// @param call_args The set of call arguments to invoke the callable with.
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: this operator is deleted, it invokes nothing."
  template <
      typename... CallArgsT,
      constraints<std::enable_if_t<!is_invocable_v<Policy, Func const&, bound_arg_tuple const&, CallArgsT&&...>>> =
          nullptr>
  constexpr auto operator()(CallArgsT&&... call_args) const& = delete;
  // parasoft-end-suppress CERT_C-EXP37-a

  /// @brief Invokes the held callable with both the held bound arguments and the call arguments.
  ///
  /// @tparam CallArgsT The types of the call arguments to invoke the callable with.
  /// @param call_args The set of call arguments to invoke the callable with.
  /// @return The return from invoking the held callable with the bound and call arguments.
  template <
      typename... CallArgsT,
      constraints<std::enable_if_t<is_invocable_v<Policy, Func&&, bound_arg_tuple&&, CallArgsT&&...>>> = nullptr>
  constexpr auto operator()(CallArgsT&&... call_args
  ) && noexcept(is_nothrow_invocable_v<Policy, Func&&, bound_arg_tuple&&, CallArgsT&&...>)
      -> invoke_result_t<Policy, Func&&, bound_arg_tuple&&, CallArgsT&&...> {
    // parasoft-begin-suppress AUTOSAR-M14_6_1-a "False positive: The dependent name is qualified."
    return Policy{
    }(std::move(this->func_ebo_holder::get_value(pfcw_fun_tag{})),
      std::move(this->bargs_ebo_holder::get_value(pfcw_bargs_tag{})),
      std::forward<CallArgsT>(call_args)...);
    // parasoft-end-suppress AUTOSAR-M14_6_1-a "False positive: The dependent name is qualified."
  }

  /// @brief Deleted lvalue overload if the wrapped callable's operator() is explicitly deleted for rvalues.
  ///
  /// @tparam CallArgsT The types of the call arguments to invoke the callable with.
  /// @param call_args The set of call arguments to invoke the callable with.
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: this operator is deleted, it invokes nothing."
  template <
      typename... CallArgsT,
      constraints<std::enable_if_t<!is_invocable_v<Policy, Func&&, bound_arg_tuple&&, CallArgsT&&...>>> = nullptr>
  constexpr auto operator()(CallArgsT&&... call_args) && = delete;
  // parasoft-end-suppress CERT_C-EXP37-a

  /// @brief Invokes the held callable with both the held bound arguments and the call arguments.
  ///
  /// @tparam CallArgsT The types of the call arguments to invoke the callable with.
  /// @param call_args The set of call arguments to invoke the callable with.
  /// @return The return from invoking the held callable with the bound and call arguments.
  template <
      typename... CallArgsT,
      constraints<std::enable_if_t<is_invocable_v<Policy, Func const&&, bound_arg_tuple const&&, CallArgsT&&...>>> =
          nullptr>
  constexpr auto operator()(CallArgsT&&... call_args
  ) const&& noexcept(is_nothrow_invocable_v<Policy, Func const&&, bound_arg_tuple const&&, CallArgsT&&...>)
      -> invoke_result_t<Policy, Func const&&, bound_arg_tuple const&&, CallArgsT&&...> {
    // parasoft-begin-suppress AUTOSAR-M14_6_1-a "False positive: The dependent name is qualified."
    // parasoft-begin-suppress AUTOSAR-A18_9_3-a "const-rvalue overload, so arguments must still be moved."
    return Policy{
    }(std::move(this->func_ebo_holder::get_value(pfcw_fun_tag{})),
      std::move(this->bargs_ebo_holder::get_value(pfcw_bargs_tag{})),
      std::forward<CallArgsT>(call_args)...);
    // parasoft-end-suppress AUTOSAR-A18_9_3-a "const-rvalue overload, so arguments must still be moved."
    // parasoft-end-suppress AUTOSAR-M14_6_1-a "False positive: The dependent name is qualified."
  }

  /// @brief Deleted crvalue overload if the wrapped callable's operator() is explicitly deleted for crvalues.
  ///
  /// @tparam CallArgsT The types of the call arguments to invoke the callable with.
  /// @param call_args The set of call arguments to invoke the callable with.
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: this operator is deleted, it invokes nothing."
  template <
      typename... CallArgsT,
      constraints<std::enable_if_t<!is_invocable_v<Policy, Func const&&, bound_arg_tuple const&&, CallArgsT&&...>>> =
          nullptr>
  constexpr auto operator()(CallArgsT&&... call_args) const&& = delete;
  // parasoft-end-suppress CERT_C-EXP37-a
};
// parasoft-end-suppress AUTOSAR-A10_1_1-a "Multiple base classes needed for empty-base-class optimizations."
// parasoft-end-suppress AUTOSAR-A12_1_5-a

}  // namespace functional_detail

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_PERFECT_FORWARD_CALL_WRAPPER_HPP_

// parasoft-end-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
// parasoft-end-suppress AUTOSAR-A5_0_3-a "False positive: There is no pointer indirection in return types"
