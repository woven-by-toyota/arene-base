// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::is_invocable"
// parasoft-begin-suppress AUTOSAR-A5_0_3-a "False positive: There is no pointer indirection in return types"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_INVOCABLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_INVOCABLE_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/is_void.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/is_reference_wrapper.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

namespace arene {
namespace base {

namespace is_invocable_detail {

/// @brief A sentinel for an invalid return type.
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
struct invalid_return {};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Determine if a callable type is invocable with the specified argument types.
/// @tparam Fn The callable type.
/// @tparam Args The callable's parameter types.
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Fn, typename... Args>
class is_invocable_impl {
 public:
  /// @brief A declaration representing a successful invocation.
  /// @tparam MemFuncPtr The member function pointer type.
  /// @tparam Instance The qualified instance or reference type.
  /// @tparam Params The parameter types.
  /// @param dummy parameter used for overload resolution
  /// @return Not implememted. The return type is the return type of the invocation
  template <typename MemFuncPtr, typename Instance, typename... Params>
  static auto try_invoke(std::nullptr_t dummy
  ) noexcept(noexcept((std::declval<Instance>().*std::declval<MemFuncPtr>())(std::declval<Params>()...)))
      -> decltype((std::declval<Instance>().*std::declval<MemFuncPtr>())(std::declval<Params>()...));

  /// @brief A declaration representing a successful invocation.
  /// @tparam MemPtr The member pointer type.
  /// @tparam Instance The qualified instance or reference type.
  /// @param dummy parameter used for overload resolution
  /// @return Not implememted. The return type is the return type of the invocation
  template <typename MemPtr, typename Instance>
  static auto try_invoke(std::nullptr_t dummy) noexcept -> decltype((std::declval<Instance>().*std::declval<MemPtr>()));

  /// @brief A declaration representing a successful invocation.
  /// @tparam MemFuncPtr The member function pointer type.
  /// @tparam Pointer The qualified pointer type.
  /// @tparam Params The parameter types.
  /// @param dummy parameter used for overload resolution
  /// @return Not implememted. The return type is the return type of the invocation
  template <typename MemFuncPtr, typename Pointer, typename... Params>
  static auto try_invoke(std::nullptr_t dummy
  ) noexcept(noexcept(((*std::declval<Pointer>()).*std::declval<MemFuncPtr>())(std::declval<Params>()...)))
      -> decltype(((*std::declval<Pointer>()).*std::declval<MemFuncPtr>())(std::declval<Params>()...));

  /// @brief A declaration representing a successful invocation.
  /// @tparam MemPtr The member pointer type.
  /// @tparam Pointer The qualified pointer type.
  /// @param dummy parameter used for overload resolution
  /// @return Not implememted. The return type is the return type of the invocation
  template <typename MemPtr, typename Pointer>
  static auto try_invoke(std::nullptr_t dummy) noexcept(noexcept((*std::declval<Pointer>()).*std::declval<MemPtr>()))
      -> decltype(((*std::declval<Pointer>()).*std::declval<MemPtr>()));

  /// @brief A declaration representing a successful invocation.
  /// @tparam MemFuncPtr The member function pointer type.
  /// @tparam RefWrap The qualified reference wrapper type.
  /// @tparam Params The parameter types.
  /// @param dummy parameter used for overload resolution
  /// @return Not implememted. The return type is the return type of the invocation
  template <
      typename MemFuncPtr,
      typename RefWrap,
      typename... Params,
      constraints<std::enable_if_t<is_reference_wrapper_v<remove_cvref_t<RefWrap>>>> = nullptr>
  static auto try_invoke(std::nullptr_t dummy
  ) noexcept(noexcept((std::declval<RefWrap>().get().*std::declval<MemFuncPtr>())(std::declval<Params>()...)))
      -> decltype((std::declval<RefWrap>().get().*std::declval<MemFuncPtr>())(std::declval<Params>()...));

  /// @brief A declaration representing a successful invocation.
  /// @tparam MemPtr The member pointer type.
  /// @tparam RefWrap The qualified reference wrapper type.
  /// @param dummy parameter used for overload resolution
  /// @return Not implememted. The return type is the return type of the invocation
  template <
      typename MemPtr,
      typename RefWrap,
      constraints<std::enable_if_t<is_reference_wrapper_v<remove_cvref_t<RefWrap>>>> = nullptr>
  static auto try_invoke(std::nullptr_t dummy) noexcept
      -> decltype((std::declval<RefWrap>().get().*std::declval<MemPtr>()));

  /// @brief A declaration representing a successful invocation.
  /// @tparam Callable The callable type.
  /// @tparam Params The parameter types.
  /// @param dummy parameter used for overload resolution
  /// @return Not implememted. The return type is the return type of the invocation
  template <typename Callable, typename... Params>
  static auto try_invoke(std::nullptr_t dummy) noexcept(noexcept(std::declval<Callable>()(std::declval<Params>()...)))
      -> decltype(std::declval<Callable>()(std::declval<Params>()...));

  /// @brief A declaration representing an invalid invocation.
  /// @param dummy parameter used for overload resolution
  /// @return Not implememted. The return type is @c invalid_return
  template <typename...>
  static auto try_invoke(void* dummy) -> invalid_return;

  /// @brief The result type of an invocation.
  using result = decltype(is_invocable_impl::try_invoke<Fn, Args...>(nullptr));

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property"
  /// @brief Whether an invocation is noexcept.
  static constexpr bool is_noexcept{noexcept(is_invocable_impl::try_invoke<Fn, Args...>(nullptr))};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Determine if a callable type is invocable with the specified return and argument types.
/// @tparam RequireNoThrow Whether the invocable is expected to be no-throw.
/// @tparam Ret The callable's return type.
/// @tparam Fn The callable type.
/// @tparam Args The callable's parameter types.
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <bool RequireNoThrow, typename Ret, typename Fn, typename... Args>
struct is_invocable_r final {
  /// @brief Provides details on the invocation
  using invoke_detail = is_invocable_impl<Fn, Args...>;

  /// @brief If the public trait requires @c noexcept
  static constexpr bool noexcept_ok{!RequireNoThrow || invoke_detail::is_noexcept};

  /// @brief If the expression is valid
  static constexpr bool return_valid{!std::is_same<typename invoke_detail::result, invalid_return>::value};

  /// @brief If the expression result is convertible to the specified return type
  static constexpr bool return_ok{
      std::is_void<Ret>::value || std::is_convertible<typename invoke_detail::result, Ret>::value
  };

  /// @brief The value of the type trait
  static constexpr bool value{noexcept_ok && return_valid && return_ok};
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Determine the return type of a functor invoked with the specified arguments
/// @tparam Fn The functor type.
/// @tparam Args The functor parameter types.
///
/// Primary template with no @c type member.
///
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Fn, typename Args, typename = constraints<>>
class invoke_result {};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Determine the return type of a functor invoked with the specified arguments
/// @tparam Fn The functor type.
/// @tparam Args The functor parameter types.
///
/// Specialization if @c is_invocable_v<Fn, Args...> is @c true
///
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Fn, typename... Args>
class invoke_result<
    Fn,
    type_list<Args...>,
    constraints<
        std::enable_if_t<!std::is_same<invalid_return, typename is_invocable_impl<Fn, Args...>::result>::value>>> {
 public:
  /// @brief The type of the result of invoking @c Fn with @c Args...
  using type = typename is_invocable_impl<Fn, Args...>::result;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace is_invocable_detail

/// @brief Determine if a functor type is invocable with the specified argument types.
/// @tparam Fn The functor type.
/// @tparam Args The functor parameter types.
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Fn, typename... Args>
class is_invocable
    : public std::integral_constant<bool, is_invocable_detail::is_invocable_r<false, void, Fn, Args...>::value> {};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Determine if a functor type is invocable with the specified argument types.
/// @tparam Fn The functor type.
/// @tparam Args The functor parameter types.
template <typename Fn, typename... Args>
extern constexpr bool is_invocable_v = is_invocable<Fn, Args...>::value;

/// @brief Determine if a functor type is invocable with the specified return and argument types.
/// @tparam Fn The functor type.
/// @tparam Args The functor parameter types.
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Ret, typename Fn, typename... Args>
class is_invocable_r
    : public std::integral_constant<bool, is_invocable_detail::is_invocable_r<false, Ret, Fn, Args...>::value> {};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Determine if a functor type is invocable with the specified return and argument types.
/// @tparam Ret The functor return type.
/// @tparam Fn The functor type.
/// @tparam Args The functor parameter types.
template <typename Ret, typename Fn, typename... Args>
extern constexpr bool is_invocable_r_v = is_invocable_r<Ret, Fn, Args...>::value;

/// @brief Determine if a functor type is no-throw invocable with the specified argument types.
/// @tparam Fn The functor type.
/// @tparam Args The functor parameter types.
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Fn, typename... Args>
class is_nothrow_invocable
    : public std::integral_constant<bool, is_invocable_detail::is_invocable_r<true, void, Fn, Args...>::value> {};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Determine if a functor type is no-throw invocable with the specified argument types.
/// @tparam Fn The functor type.
/// @tparam Args The functor parameter types.
template <typename Fn, typename... Args>
extern constexpr bool is_nothrow_invocable_v = is_nothrow_invocable<Fn, Args...>::value;

/// @brief Determine if a functor type is no-throw invocable with the specified return and argument types.
/// @tparam Ret The functor return type.
/// @tparam Fn The functor type.
/// @tparam Args The functor parameter types.
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Ret, typename Fn, typename... Args>
class is_nothrow_invocable_r
    : public std::integral_constant<bool, is_invocable_detail::is_invocable_r<true, Ret, Fn, Args...>::value> {};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Determine if a functor type is no-throw invocable with the specified return and argument types.
/// @tparam Ret The functor return type.
/// @tparam Fn The functor type.
/// @tparam Args The functor parameter types.
template <typename Ret, typename Fn, typename... Args>
extern constexpr bool is_nothrow_invocable_r_v = is_nothrow_invocable_r<Ret, Fn, Args...>::value;

/// @brief Determines the return type of a functor invoked with the specified arguments
/// @tparam Fn The functor type.
/// @tparam Args The functor parameter types.
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Fn, typename... Args>
class invoke_result : public is_invocable_detail::invoke_result<Fn, type_list<Args...>> {};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief The return type of a functor invoked with the specified arguments
/// @tparam Fn The functor type.
/// @tparam Args The functor parameter types.
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename Fn, typename... Args>
using invoke_result_t = typename invoke_result<Fn, Args...>::type;
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_INVOCABLE_HPP_
        // parasoft-end-suppress AUTOSAR-A5_0_3-a "False positive: There is no pointer indirection in return types"
