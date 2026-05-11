// parasoft-begin-suppress AUTOSAR-A5_0_3-a "False positive: There is no pointer indirection in return types"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_INVOKE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_INVOKE_HPP_

// IWYU pragma: private, include "arene/base/functional.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_base_of.hpp"
#include "arene/base/stdlib_choice/is_member_function_pointer.hpp"
#include "arene/base/stdlib_choice/is_member_object_pointer.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_reference_wrapper.hpp"
#include "arene/base/type_traits/member_pointer_class_type.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A15_4_3-a-2 "False positive: This file is the only declaration of these functions"
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {

namespace invoke_detail {
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress CERT_CPP-ERR55-a-2 "False positive: Exception specification is conditional"
// parasoft-begin-suppress CERT_CPP-ERR50-h-3 "False positive: Exception specification is conditional"
// parasoft-begin-suppress AUTOSAR-A15_5_3-h-2 "False positive: Exception specification is conditional"
/// @brief Invoke a function or object with a function call operator, passing a specified list of arguments
/// @tparam F The type of the function or callable object
/// @tparam Args The types of the arguments to pass to the function or callable object
/// @param func The function or callable object
/// @param args The arguments to pass to the invoked function or callable object
/// @return The return value of the invocation of the specified function or callable object with the specified
///          arguments
template <
    typename F,
    typename... Args,
    constraints<
        std::enable_if_t<!std::is_member_function_pointer<std::remove_reference_t<F>>::value>,
        std::enable_if_t<!std::is_member_object_pointer<std::remove_reference_t<F>>::value>> = nullptr>
constexpr auto invoke_impl(F&& func, Args&&... args) noexcept(noexcept(std::forward<F>(func)(std::forward<Args>(args)...
))) -> decltype(std::forward<F>(func)(std::forward<Args>(args)...)) {
  // parasoft-begin-suppress CERT_CPP-ERR51-b-3 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress AUTOSAR-A15_3_2-a-2 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress AUTOSAR-A15_5_3-g-2 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress CERT_CPP-ERR50-g-3 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress AUTOSAR-M15_3_4-b-2 "False positive: Exception is caught"
  // parasoft-begin-suppress AUTOSAR-A15_2_1-b-2 "False positive: Throwing constructor not invoked"
  return std::forward<F>(func)(std::forward<Args>(args)...);
  // parasoft-end-suppress AUTOSAR-A15_2_1-b-2
  // parasoft-end-suppress AUTOSAR-M15_3_4-b-2
  // parasoft-end-suppress CERT_CPP-ERR50-g-3
  // parasoft-end-suppress AUTOSAR-A15_5_3-g-2
  // parasoft-end-suppress AUTOSAR-A15_3_2-a-2
  // parasoft-end-suppress CERT_CPP-ERR51-b-3
}
// parasoft-end-suppress AUTOSAR-A15_5_3-h-2
// parasoft-end-suppress CERT_CPP-ERR55-a-2
// parasoft-end-suppress CERT_CPP-ERR50-h-3
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Invoke a pointer to a member function with a @c std::reference_wrapper to an object, and a specified list of
/// arguments
/// @tparam F The type of the pointer to member function
/// @tparam Object The type of the refeence object to apply the pointer to member function to
/// @tparam Args The types of the arguments to pass to the member function
/// @param func The pointer to member function to invoke
/// @param object The instance from which to obtain the object to apply the pointer to member function to
/// @param args The arguments to pass to the invoked member function
/// @return The return value of the invocation of the pointed-to member function for the specified object with the
///          specified arguments
template <
    typename F,
    typename Object,
    typename... Args,
    constraints<std::enable_if_t<is_reference_wrapper_v<remove_cvref_t<Object>>>> = nullptr>
auto invoke_member_function_impl(F func, Object object, Args&&... args) noexcept(
    noexcept((object.get().*func)(std::forward<Args>(args)...))
) -> decltype((object.get().*func)(std::forward<Args>(args)...)) {
  return (object.get().*func)(std::forward<Args>(args)...);
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Invoke a pointer to a member function against an object, with a specified list of arguments
/// @tparam F The type of the pointer to member function
/// @tparam Object The type of the object to apply the pointer to member function to.
/// @tparam Args The types of the arguments to pass to the member function
/// @param func The pointer to member function to invoke
/// @param object The object to apply the pointer to member function to. If it is a pointer-like object, it will be
///            dereferenced before invoking the member pointer.
/// @param args The arguments to pass to the invoked member function
/// @return The return value of the invocation of the pointed-to member function for the specified object with the
///          specified arguments
/// @{
template <
    typename F,
    typename Object,
    typename... Args,
    constraints<std::enable_if_t<std::is_base_of<member_pointer_class_type<F>, remove_cvref_t<Object>>::value>> =
        nullptr>
constexpr auto invoke_member_function_impl(F func, Object&& object, Args&&... args) noexcept(
    noexcept((std::forward<Object>(object).*func)(std::forward<Args>(args)...))
) -> decltype((std::forward<Object>(object).*func)(std::forward<Args>(args)...)) {
  return (std::forward<Object>(object).*func)(std::forward<Args>(args)...);
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: Documented by group"
// parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: Documented by group"
template <
    typename F,
    typename Object,
    typename... Args,
    constraints<
        std::enable_if_t<!std::is_base_of<member_pointer_class_type<F>, remove_cvref_t<Object>>::value>,
        std::enable_if_t<!is_reference_wrapper_v<remove_cvref_t<Object>>>> = nullptr>
constexpr auto invoke_member_function_impl(F func, Object&& object, Args&&... args) noexcept(
    noexcept(((*std::forward<Object>(object)).*func)(std::forward<Args>(args)...))
) -> decltype(((*std::forward<Object>(object)).*func)(std::forward<Args>(args)...)) {
  return ((*std::forward<Object>(object)).*func)(std::forward<Args>(args)...);
}
// parasoft-end-suppress AUTOSAR-A2_7_3-a-2
// parasoft-end-suppress AUTOSAR-A2_7_3-b-2
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

/// @}

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Invoke a pointer to a member function with an object, a reference to an object, a @c std::reference_wrapper
/// to an object, or a pointer-like entity that refers to an object, and a specified list of arguments
/// @tparam F The type of the pointer to member function
/// @tparam Object The type from which to obtain the object to apply the pointer to member function to
/// @tparam Args The types of the arguments to pass to the member function
/// @param func The pointer to member function to invoke
/// @param object The instance from which to obtain the object to apply the pointer to member function to
/// @param args The arguments to pass to the invoked member function
/// @return The return value of the invocation of the pointed-to member function for the specified object with the
///          specified arguments
template <
    typename F,
    typename Object,
    typename... Args,
    constraints<std::enable_if_t<std::is_member_function_pointer<std::remove_reference_t<F>>::value>> = nullptr>
constexpr auto invoke_impl(F&& func, Object&& object, Args&&... args) noexcept(noexcept(
    invoke_member_function_impl(std::forward<F>(func), std::forward<Object>(object), std::forward<Args>(args)...)
))
    -> decltype(invoke_member_function_impl(
        std::forward<F>(func),
        std::forward<Object>(object),
        std::forward<Args>(args)...
    )) {
  return invoke_member_function_impl(std::forward<F>(func), std::forward<Object>(object), std::forward<Args>(args)...);
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Invoke a pointer to a data member against an object object
/// @tparam F The type of the pointer to data member
/// @tparam Object The type of the object. May be the object itself, or a pointer-like type pointing to the object.
/// @param func The pointer to data member to invoke
/// @param object The object to apply the pointer to data member to. If it is a pointer-like object, it will be
///            dereferenced before invoking the member pointer.
/// @return The result of applying the the pointed-to data member to the specified object.
/// @{
template <
    typename F,
    typename Object,
    constraints<std::enable_if_t<std::is_base_of<member_pointer_class_type<F>, remove_cvref_t<Object>>::value>> =
        nullptr>
constexpr auto invoke_data_member_impl(F func, Object&& object) noexcept
    -> decltype(std::forward<Object>(object).*func) {
  return std::forward<Object>(object).*func;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress CERT_CPP-ERR55-a-2 "False positive: Exception specification is conditional"
// parasoft-begin-suppress CERT_CPP-ERR50-h-3 "False positive: Exception specification is conditional"
// parasoft-begin-suppress AUTOSAR-A15_5_3-h-2 "False positive: Exception specification is conditional"
// parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: Documented by group"
// parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: Documented by group"
template <
    typename F,
    typename Object,
    constraints<
        std::enable_if_t<!std::is_base_of<member_pointer_class_type<F>, remove_cvref_t<Object>>::value>,
        std::enable_if_t<!is_reference_wrapper_v<remove_cvref_t<Object>>>> = nullptr>
constexpr auto invoke_data_member_impl(F func, Object&& object) noexcept(noexcept((*std::forward<Object>(object)).*func)
) -> decltype((*std::forward<Object>(object)).*func) {
  // parasoft-begin-suppress CERT_CPP-ERR51-b-3 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress AUTOSAR-A15_3_2-a-2 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress AUTOSAR-A15_5_3-g-2 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress CERT_CPP-ERR50-g-3 "False positive: Exception specification is conditional"
  // parasoft-begin-suppress AUTOSAR-M15_3_4-b-2 "False positive: Exception is caught"
  // parasoft-begin-suppress AUTOSAR-A15_2_1-b-2 "False positive: Throwing constructor not invoked"
  return (*std::forward<Object>(object)).*func;
  // parasoft-end-suppress AUTOSAR-A15_2_1-b-2
  // parasoft-end-suppress AUTOSAR-M15_3_4-b-2
  // parasoft-end-suppress CERT_CPP-ERR50-g-3
  // parasoft-end-suppress AUTOSAR-A15_5_3-g-2
  // parasoft-end-suppress AUTOSAR-A15_3_2-a-2
  // parasoft-end-suppress CERT_CPP-ERR51-b-3
}
// parasoft-end-suppress AUTOSAR-A2_7_3-a-2
// parasoft-end-suppress AUTOSAR-A2_7_3-b-2
// parasoft-end-suppress AUTOSAR-A15_5_3-h-2
// parasoft-end-suppress CERT_CPP-ERR55-a-2
// parasoft-end-suppress CERT_CPP-ERR50-h-3
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2
/// @}

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Invoke a pointer to a data member with a @c std::reference_wrapper to an object
/// @tparam F The type of the pointer to data member
/// @tparam Object The type of the referenced object
/// @param func The pointer to data member to invoke
/// @param object The instance from which to obtain the object to apply the pointer to data member to
/// @return The value of the pointed-to data member for the specified object
template <
    typename F,
    typename Object,
    constraints<std::enable_if_t<is_reference_wrapper_v<remove_cvref_t<Object>>>> = nullptr>
auto invoke_data_member_impl(F func, Object object) noexcept -> decltype(object.get().*func) {
  return object.get().*func;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress CERT_CPP-ERR55-a-2 "False positive: Exception specification is conditional"
// parasoft-begin-suppress CERT_CPP-ERR50-h-3 "False positive: Exception specification is conditional"
// parasoft-begin-suppress AUTOSAR-A15_5_3-h-2 "False positive: Exception specification is conditional"
/// @brief Invoke a pointer to a data member with an object, a reference to an object, a @c std::reference_wrapper to an
/// object, or a pointer-like entity that refers to an object.
/// @tparam F The type of the pointer to data member
/// @tparam Object The type from which to obtain the object to apply the pointer to data member to
/// @param func The pointer to data member to invoke
/// @param object The instance from which to obtain the object to apply the pointer to data member to
/// @return The value of the pointed-to data member for the specified object
template <
    typename F,
    typename Object,
    constraints<std::enable_if_t<std::is_member_object_pointer<std::remove_reference_t<F>>::value>> = nullptr>
constexpr auto invoke_impl(F&& func, Object&& object) noexcept(
    noexcept(invoke_data_member_impl(std::forward<F>(func), std::forward<Object>(object)))
) -> decltype(invoke_data_member_impl(std::forward<F>(func), std::forward<Object>(object))) {
  return invoke_data_member_impl(std::forward<F>(func), std::forward<Object>(object));
}
// parasoft-end-suppress AUTOSAR-A15_5_3-h-2
// parasoft-end-suppress CERT_CPP-ERR55-a-2
// parasoft-end-suppress CERT_CPP-ERR50-h-3
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2
}  // namespace invoke_detail

// parasoft-begin-suppress CERT_CPP-ERR55-a-2 "False positive: Exception specification is conditional"
// parasoft-begin-suppress CERT_CPP-ERR50-h-3 "False positive: Exception specification is conditional"
// parasoft-begin-suppress AUTOSAR-A15_5_3-h-2 "False positive: Exception specification is conditional"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Implementation of std::invoke from C++23: invokes the specified callable with the specified arguments.
/// @tparam F The type of the callable to invoke
/// @tparam Args The types of the arguments
/// @param func The callable to invoke
/// @param args The arguments with which to invoke the callable
/// @return The result of invoking the callable with the specified arguments
/// @note Does not participate in overload resolution if the specified @c F cannot be invoked with the specified @c Args
template <typename F, typename... Args, constraints<std::enable_if_t<is_invocable_v<F, Args...>>> = nullptr>
constexpr auto invoke(F&& func, Args&&... args) noexcept(is_nothrow_invocable_v<F, Args...>)
    -> decltype(invoke_detail::invoke_impl(std::forward<F>(func), std::forward<Args>(args)...)) {
  return invoke_detail::invoke_impl(std::forward<F>(func), std::forward<Args>(args)...);
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2
// parasoft-end-suppress AUTOSAR-A15_5_3-h-2
// parasoft-end-suppress CERT_CPP-ERR55-a-2
// parasoft-end-suppress CERT_CPP-ERR50-h-3

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_INVOKE_HPP_
// parasoft-end-suppress AUTOSAR-A5_0_3-a "False positive: There is no pointer indirection in return types"
