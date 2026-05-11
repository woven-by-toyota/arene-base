// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_FUNCTION_REF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_FUNCTION_REF_HPP_

// IWYU pragma: private, include "arene/base/functional.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/array/array.hpp"
#include "arene/base/byte/byte.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/stdlib_choice/addressof.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/is_function.hpp"
#include "arene/base/stdlib_choice/is_fundamental.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/is_trivially_copyable.hpp"
#include "arene/base/stdlib_choice/is_void.hpp"
#include "arene/base/stdlib_choice/max_value_overload.hpp"
#include "arene/base/stdlib_choice/memcpy.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_traits/conditional.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
#include "arene/base/type_traits/type_identity.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_C-EXP37-b-3 "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {

/// @brief An implementation of the proposed <c>std::function_ref</c> from [P0792](https://wg21.link/p0792). Provides a
/// type-erased callable like <c>std::function</c>, but just stores a reference to the wrapped callable rather than a
/// copy of it.
/// @tparam Signature The function signature being wrapped of the form <c>Return-type (Args...) optional-const</c>
/// @tparam IsNoexcept If the generated call operator should be noexcept
template <typename Signature, bool IsNoexcept = false>
class function_ref;

/// @brief A convience alias for @c function_ref with @c IsNoexcept set @c true
/// @tparam Signature The function signature being wrapped of the form <c>Return-type (Args...) optional-const</c>
template <typename Signature>
using noexcept_function_ref = function_ref<Signature, true>;

namespace function_ref_detail {
/// @brief Crash with a precondition violation if the supplied pointer is null, or dereference it if not
/// @tparam Func The function type being pointed to
/// @param ptr A function pointer
/// @return A reference to the object pointed to by @c ptr
/// @pre @c ptr is not null, otherwise @c ARENE_PRECONDITION violation
template <typename Func>
auto checked_deref(Func* ptr) noexcept -> Func& {
  ARENE_PRECONDITION(ptr != nullptr);
  return *ptr;
}

/// @brief The type to use for passing an argument to the @c function_ref trampoline
/// function: the type itself for fundamental types and small
/// trivial types, otherwise a reference to the argument
/// @tparam The type of the argument
template <typename T>
using arg_passing_type = conditional_t<
    std::is_fundamental<T>::value || (std::is_trivially_copyable<T>::value && sizeof(T) <= sizeof(void*)),
    T,
    T&>;

// parasoft-begin-suppress AUTOSAR-A12_1_5-a-2 "False positive: delegating constructors are used"
/// @brief The base class for @c function_ref that provides the implementation.
/// @tparam IsConst Is the @c Signature of the @c function_ref @c const qualified?
/// @tparam IsNoexcept Should the generated call operator be marked @c noexcept ?
/// @tparam R The return type
/// @tparam Args The types of the function call arguments
template <bool IsConst, bool IsNoexcept, typename R, typename... Args>
class function_ref_base {
  /// @brief The type of the buffer for storing the pointer. Assumes all function
  /// pointers have the same size.
  using storage_buffer = arene::base::array<byte, std::max(sizeof(void*), sizeof(void (*)()))>;

  /// @brief The type of the stored function to invoke the wrapped callable
  using call_type = R (*)(storage_buffer, arg_passing_type<Args>...);

  static_assert(sizeof(call_type) == sizeof(void (*)()), "Function pointers must have the same size");

  /// @brief Type alias to const-qualify the supplied type @c F if @c IsConst
  /// @tparam F the type of the callable object
  template <typename F>
  using func_type = conditional_t<IsConst, F const, F>;

  /// @brief Invoke a wrapped callable object or function of type @c F with the
  /// supplied arguments, where @c R is not @c void.
  /// @tparam F The type of the wrapped object
  /// @tparam ReturnType An alias for @c R to allow the constraint
  /// @param buffer A reference to the storage buffer
  /// @param args The supplied arguments for the call
  /// @return The result of invoking the wrapped callable object with the
  /// supplied arguments.
  template <
      typename F,
      typename ReturnType = R,
      constraints<std::enable_if_t<!std::is_void<ReturnType>::value>> = nullptr>
  static auto invoke_wrapped_callable(storage_buffer buffer, arg_passing_type<Args>... args) -> R {
    // parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "False positive: not declared with an auto specifier"
    F* local_f{nullptr};
    // parasoft-end-suppress AUTOSAR-A7_1_5-a-2
    std::ignore = std::memcpy(&local_f, buffer.data(), sizeof(local_f));
    // parasoft-begin-suppress AUTOSAR-A5_2_2-a-2 "False positive: C-style cast not used"
    return static_cast<R>((*local_f)(static_cast<Args&&>(args)...));
    // parasoft-end-suppress AUTOSAR-A5_2_2-a-2
  }

  /// @brief Invoke a wrapped callable object or function of type @c F with the
  /// supplied arguments, where @c R is @c void.
  /// @tparam F The type of the wrapped object
  /// @tparam ReturnType An alias for @c R to allow the constraint
  /// @param buffer A reference to the storage buffer
  /// @param args The supplied arguments for the call
  template <
      typename F,
      typename ReturnType = R,
      constraints<std::enable_if_t<std::is_void<ReturnType>::value>> = nullptr>
  static void invoke_wrapped_callable(storage_buffer buffer, arg_passing_type<Args>... args) {
    // parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "False positive: not declared with an auto specifier"
    F* local_f{nullptr};
    // parasoft-end-suppress AUTOSAR-A7_1_5-a-2
    std::ignore = std::memcpy(&local_f, buffer.data(), sizeof(local_f));
    static_cast<void>((*local_f)(static_cast<Args&&>(args)...));
  }

  /// @brief Internal constructor to reduce duplication. Copies a non-owning pointer to the argument.
  /// @tparam PointeeType the type of the pointed-to entity to be stored
  /// @tparam CallType the type to cast the pointee to when invoking
  /// @param f_ref A reference to the function to store
  template <typename PointeeType, typename CallType>
  function_ref_base(PointeeType const& f_ref, type_identity<CallType>) noexcept
      : call_(&invoke_wrapped_callable<CallType>) {
    // parasoft-begin-suppress AUTOSAR-A7_1_5-a "False positive: 'f_pointer' is not declared with 'auto'
    PointeeType const* f_pointer{std::addressof(f_ref)};
    // parasoft-end-suppress AUTOSAR-A7_1_5-a
    std::ignore = std::memcpy(buffer_.data(), &f_pointer, sizeof(f_pointer));
  }

  /// @brief Trait to determine if a callable is invocable with matching constness, noexcept, return and arguments.
  /// @tparam F the function type to test.
  /// @return If @c IsNoexcept , then equivalent to @c is_nothrow_invocable_r<R,F,Args...> . Otherwise,
  ///         equivalent to @c is_invocable_r<R,F,Args...>
  template <typename F>
  static constexpr bool
      is_invocable_n_r_v{IsNoexcept ? ::arene::base::is_nothrow_invocable_r_v<R, F, Args...> : ::arene::base::is_invocable_r_v<R, F, Args...>};

 public:
  // parasoft-begin-suppress CERT_C-EXP37-a "False-positive: All arguments are named"
  /// @brief Invoke the wrapped callable with the specified arguments
  /// @param args The arguments to pass to the callable
  /// @return The result of the invocation of the wrapped callable
  auto operator()(Args... args) const noexcept(IsNoexcept) -> R {
    // NOLINTNEXTLINE(bugprone-exception-escape) noexcept isn't captured in the type signature. Invariants ensure it is.
    return call_(buffer_, args...);
  }
  // parasoft-end-suppress CERT_C-EXP37-a

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "This class stores a reference, so does not copy or move data"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "This class stores a reference, so does not copy or move data"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "This class stores a reference, so does not copy or move data"
  /// @brief Construct a @c function_ref from a callable object. If @c IsNoexcept , must satisfy
  ///        @c is_nothrow_invocable_r<R,F,Args...> , otherwise @c is_invocable_r<R,F,Args...>
  /// @tparam F The deduced type of the supplied callable object or a reference to it
  /// @param func A reference to the supplied callable object
  /// @note This takes a forwarding @c F&& reference in order to account for cases where @c F is overloaded, so the
  /// current @c function_ref specialization can select the overload which matches its CV qualification. @c func is
  /// always copied, not moved from, even when it's deduced as an rvalue reference.
  template <
      typename F,
      constraints<
          std::enable_if_t<!std::is_same<function_ref_base, remove_cvref_t<F>>::value>,
          std::enable_if_t<!std::is_function<std::remove_reference_t<F>>::value>,
          std::enable_if_t<is_invocable_n_r_v<F>>> = nullptr>
  // AUTOSAR A12-1-4 exception. We *want* @c function_ref to be implicitly
  // convertible from callable objects that can be invoked with the appropriate
  // signature, in order to facilitate the use of @c function_ref as a function
  // argument type.
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions,bugprone-forwarding-reference-overload)
  function_ref_base(F&& func) noexcept
      : function_ref_base(std::forward<F>(func), type_identity<func_type<std::remove_reference_t<F>>>{}) {}
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: Constrained via SFINAE"
  /// @brief Construct a @c function_ref from a function pointer.
  /// @tparam F The deduced type of the supplied function
  /// @param func A pointer to the supplied function
  /// @pre Requires <c>f(args...)</c> is a valid expression (where each element
  /// of @c args has the type of the corresponding element of @c Args), and
  /// convertible to @c R
  /// @pre @c func must not be <c>nullptr</c>, otherwise @c ARENE_PRECONDITION violation
  template <
      typename F,
      constraints<std::enable_if_t<std::is_function<F>::value>, std::enable_if_t<is_invocable_n_r_v<F>>> = nullptr>
  // AUTOSAR A12-1-4 exception. We *want* @c function_ref to be implicitly
  // convertible from functions that can be invoked with the appropriate
  // signature, in order to facilitate the use of @c function_ref as a function
  // argument type.
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  function_ref_base(F* func) noexcept
      : function_ref_base(function_ref_detail::checked_deref(func), type_identity<F>{}) {}

 protected:
  /// @brief default copy constructor
  /// @param other The source
  function_ref_base(function_ref_base const& other) = default;
  /// @brief default move constructor
  /// @param other The source
  function_ref_base(function_ref_base&& other) = default;
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  /// @brief Default destructor
  ~function_ref_base() = default;

  /// @brief default copy assignment
  /// @param other The source
  auto operator=(function_ref_base const& other) -> function_ref_base& = default;
  /// @brief default move assignment
  /// @param other The source
  auto operator=(function_ref_base&& other) -> function_ref_base& = default;

 private:
  /// @brief The buffer for storing the pointer.
  alignas(void*) alignas(call_type) storage_buffer buffer_{};
  /// @brief Pointer to the trampoline function which will invoke the wrapped function
  /// or callable object
  call_type call_;
};
// parasoft-end-suppress AUTOSAR-A12_1_5-a-2

}  // namespace function_ref_detail

/// @brief This specialization handles non-const-qualified function signatures.
/// @tparam R The return type for the function call operator
/// @tparam Args The types of the arguments for the function call operator
/// @tparam IsNoexcept If @c true , the function will have a @c noexcept qualified call operator. Otherwise, it will
///         not.
template <typename R, typename... Args, bool IsNoexcept>
class function_ref<R(Args...), IsNoexcept> : function_ref_detail::function_ref_base<false, IsNoexcept, R, Args...> {
  /// @brief Convenience typedef for the base class
  using base_class = function_ref_detail::function_ref_base<false, IsNoexcept, R, Args...>;

 public:
  /// Expose the base class constructors
  using base_class::base_class;
  /// Expose the base class function call operator
  using base_class::operator();
};

/// @cond INTERNAL
/// @brief This specialization handles const-qualified function signatures.
/// @tparam R The return type for the function call operator
/// @tparam Args The types of the arguments for the function call operator
/// @tparam IsNoexcept If @c true , the function will have a @c noexcept qualified call operator. Otherwise, it will
///         not.
template <typename R, typename... Args, bool IsNoexcept>
class function_ref<R(Args...) const, IsNoexcept>
    : function_ref_detail::function_ref_base<true, IsNoexcept, R, Args...> {
  /// @brief Convenience typedef for the base class
  using base_class = function_ref_detail::function_ref_base<true, IsNoexcept, R, Args...>;

 public:
  /// @brief Expose the base class constructors
  using base_class::base_class;
  /// @brief Expose the base class function call operator
  using base_class::operator();
};
/// @endcond

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_FUNCTION_REF_HPP_
