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
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/type_manipulation/ebo_holder.hpp"
#include "arene/base/type_traits/decays_to.hpp"
#include "arene/base/type_traits/give_cvref_to.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"

namespace arene {
namespace base {

namespace functional_detail {

/// @brief A tag type to allow empty base class optimization for empty function objects.
template <class Self>
struct invoke_tag {};

/// @brief A tag type to allow empty base class optimization for empty bound arguments.
template <class Self>
struct bound_args_tag {};

// parasoft-begin-suppress AUTOSAR-A10_2_1-b "'perfect_forward_call_wrapper::invocable' and
// 'perfect_forward_call_Wrapper::bound_args' are not part of the public interface and are not hidden."

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
    : ebo_holder<invoke_tag<perfect_forward_call_wrapper<Policy, Func, BoundArgs...>>, Func>
    , ebo_holder<bound_args_tag<perfect_forward_call_wrapper<Policy, Func, BoundArgs...>>, std::tuple<BoundArgs...>> {
  /// @brief The type of a tuple containing any bound arguments
  using bound_arg_tuple = std::tuple<BoundArgs...>;

  /// @brief The type of the ebo holder for the function object
  using invocable_wrapper = ebo_holder<invoke_tag<perfect_forward_call_wrapper>, Func>;

  /// @brief The type of the ebo holder for the bound arguments
  using bound_args_wrapper = ebo_holder<bound_args_tag<perfect_forward_call_wrapper>, bound_arg_tuple>;

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a  "False positive: 'self' is forwarded correctly by use of 'static_cast'"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a  "False positive: 'self' is forwarded correctly by use of 'static_cast'"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a "False positive: 'self' is forwarded correctly by use of 'static_cast'"

  /// @brief access the stored invocable with the cvref-qualification of the wrapper
  /// @tparam Self type of the wrapper
  /// @param self reference to a wrapper
  /// @return reference to the stored invocable, cvref-qualified to match @c Self
  template <class Self>
  static constexpr auto invocable(Self&& self) noexcept -> give_cvref_to_t<Self&&, Func> {
    return static_cast<give_cvref_to_t<Self&&, invocable_wrapper>>(self).get_value(
        invoke_tag<perfect_forward_call_wrapper>{}
    );
  }

  /// @brief access the stored args with the cvref-qualification of the wrapper
  /// @tparam Self type of the wrapper
  /// @param self reference to a wrapper
  /// @return reference to the stored argument tuple, cvref-qualified to match @c Self
  template <class Self>
  static constexpr auto bound_args(Self&& self) noexcept -> give_cvref_to_t<Self&&, std::tuple<BoundArgs...>> {
    return static_cast<give_cvref_to_t<Self&&, bound_args_wrapper>>(self).get_value(
        bound_args_tag<perfect_forward_call_wrapper>{}
    );
  }

  // parasoft-end-suppress AUTOSAR-A8_4_6-a
  // parasoft-end-suppress AUTOSAR-A8_4_5-a
  // parasoft-end-suppress AUTOSAR-A12_8_4-a

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
  /// @brief Constructs the argument binding wrapper with the given function and bound arguments
  ///
  /// @tparam BoundFunc The type of the function object to bind.
  /// @tparam ArgsToBind The types of the bound arguments.
  /// @param func_to_bind The function to bind
  /// @param args_to_bind The arguments to bind
  template <
      typename BoundFunc,
      typename... ArgsToBind,
      constraints<
          std::enable_if_t<!decays_to_v<BoundFunc, perfect_forward_call_wrapper>>,
          std::enable_if_t<std::is_constructible<Func, BoundFunc&&>::value>,
          std::enable_if_t<std::is_constructible<std::tuple<BoundArgs...>, ArgsToBind&&...>::value>> = nullptr>
  constexpr explicit perfect_forward_call_wrapper(BoundFunc&& func_to_bind, ArgsToBind&&... args_to_bind) noexcept(
      std::is_nothrow_constructible<Func, BoundFunc&&>::value &&
      std::is_nothrow_constructible<std::tuple<BoundArgs...>, ArgsToBind&&...>::value
  )
      : invocable_wrapper{std::forward<BoundFunc>(func_to_bind)},
        bound_args_wrapper{std::forward<ArgsToBind>(args_to_bind)...} {
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
  constexpr auto operator()(CallArgsT&&... call_args) & noexcept(              //
      is_nothrow_invocable_v<Policy, Func&, bound_arg_tuple&, CallArgsT&&...>  //
  ) -> invoke_result_t<Policy, Func&, bound_arg_tuple&, CallArgsT&&...> {
    // parasoft-begin-suppress AUTOSAR-M14_6_1-a "False positive: The dependent name is qualified."
    return Policy{}(  //
        invocable(*this),
        bound_args(*this),
        std::forward<CallArgsT>(call_args)...
    );
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
  constexpr auto operator()(CallArgsT&&... call_args) const& noexcept(                     //
      is_nothrow_invocable_v<Policy, Func const&, bound_arg_tuple const&, CallArgsT&&...>  //
  ) -> invoke_result_t<Policy, Func const&, bound_arg_tuple const&, CallArgsT&&...> {
    // parasoft-begin-suppress AUTOSAR-M14_6_1-a "False positive: The dependent name is qualified."
    return Policy{}(  //
        invocable(*this),
        bound_args(*this),
        std::forward<CallArgsT>(call_args)...
    );
    // parasoft-end-suppress AUTOSAR-M14_6_1-a "False positive: The dependent name is qualified."
  }

  /// @brief Deleted const lvalue overload if the wrapped callable's operator() is explicitly deleted for const lvalues.
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
  constexpr auto operator()(CallArgsT&&... call_args) && noexcept(               //
      is_nothrow_invocable_v<Policy, Func&&, bound_arg_tuple&&, CallArgsT&&...>  //
  ) -> invoke_result_t<Policy, Func&&, bound_arg_tuple&&, CallArgsT&&...> {
    // parasoft-begin-suppress AUTOSAR-M14_6_1-a "False positive: The dependent name is qualified."
    return Policy{}(  //
        invocable(std::move(*this)),
        bound_args(std::move(*this)),
        std::forward<CallArgsT>(call_args)...
    );
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
  constexpr auto operator()(CallArgsT&&... call_args) const&& noexcept(                      //
      is_nothrow_invocable_v<Policy, Func const&&, bound_arg_tuple const&&, CallArgsT&&...>  //
  ) -> invoke_result_t<Policy, Func const&&, bound_arg_tuple const&&, CallArgsT&&...> {
    // parasoft-begin-suppress AUTOSAR-M14_6_1-a "False positive: The dependent name is qualified."
    // parasoft-begin-suppress AUTOSAR-A18_9_3-a "const-rvalue overload, so arguments must still be moved."
    return Policy{}(  //
        invocable(std::move(*this)),
        bound_args(std::move(*this)),
        std::forward<CallArgsT>(call_args)...
    );
    // parasoft-end-suppress AUTOSAR-A18_9_3-a "const-rvalue overload, so arguments must still be moved."
    // parasoft-end-suppress AUTOSAR-M14_6_1-a "False positive: The dependent name is qualified."
  }

  /// @brief Deleted const rvalue overload if the wrapped callable's operator() is explicitly deleted for const rvalues.
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

// parasoft-end-suppress AUTOSAR-A10_2_1-b

}  // namespace functional_detail

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_PERFECT_FORWARD_CALL_WRAPPER_HPP_

// parasoft-end-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
// parasoft-end-suppress AUTOSAR-A5_0_3-a "False positive: There is no pointer indirection in return types"
