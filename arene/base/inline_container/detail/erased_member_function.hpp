// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: this file defines 'erased_member_function'""

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_ERASED_MEMBER_FUNCTION_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_ERASED_MEMBER_FUNCTION_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/inline_container/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/functional/function_traits.hpp"
#include "arene/base/functional/invoke.hpp"
#include "arene/base/inline_container/detail/container_base_type.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace inline_container {
namespace detail {

/// @brief helper for defining a capacity-erased member function
/// @tparam MemFn member function pointer of the non-erased container type
/// @tparam Func pointer to the member function
/// @tparam Return return type for this wrapper
/// @tparam Arguments argument types for @c MemFn, deduced
///
/// This primary template is empty.
///
template <class MemFn, MemFn Func, class Return, class Arguments = member_function_arguments_t<MemFn>>
class erased_member_function_wrapper {};

/// @brief helper for defining a capacity-erased member function
/// @tparam MemFn member function pointer of the non-erased container type
/// @tparam Func pointer to the member function
/// @tparam Return return type for this wrapper
/// @tparam Args argument types for @c Func
///
/// Specialization of @c erased_member_function_wrapper with argument types of
/// @c MemFn as a parameter pack.
///
template <class MemFn, MemFn Func, class Return, class... Args>
class erased_member_function_wrapper<MemFn, Func, Return, ::arene::base::type_list<Args...>> {
  /// @brief indirection used to avoid GCC 'return-local-addr' false positive
  /// @tparam B deduced template parameter for SFINAE, enables this function if @c true
  /// @tparam D un-erased derived class
  /// @tparam Ts forwarded argument types for the member function call
  /// @param unerased reference to the un-erased derived class
  /// @param args forwarded arguments for the member function call
  /// @return result of the member function call
  ///
  /// Overload if @c std::is_same<Return, member_function_return_t<MemFn>>::value is @c true.
  ///
  template <bool B, class D, class... Ts, constraints<std::enable_if_t<B>> = nullptr>
  static constexpr auto workaround_gcc_warning(std::integral_constant<bool, B>, D& unerased, Ts&&... args) -> Return {
    // parasoft-begin-suppress AUTOSAR-A7_5_1-a "If 'result' is a reference to const, this function simply forwards it.
    // 'result' never binds to a temporary argument, it is the return value of 'Func'."
    return ::arene::base::invoke(Func, unerased, std::forward<Ts>(args)...);
    // parasoft-end-suppress AUTOSAR-A7_5_1-a
  }
  /// @brief indirection used to avoid GCC 'return-local-addr' false positive
  /// @tparam B deduced template parameter for SFINAE, enables this function if @c false
  /// @tparam D un-erased derived class
  /// @tparam Ts forwarded argument types for the member function call
  /// @param unerased reference to the un-erased derived class
  /// @param args forwarded arguments for the member function call
  /// @return result of the member function call explicitly converted to @c Return
  ///
  /// Overload if @c std::is_same<Return, member_function_return_t<MemFn>>::value is @c false.
  ///
  template <bool B, class D, class... Ts, constraints<std::enable_if_t<!B>> = nullptr>
  static constexpr auto workaround_gcc_warning(std::integral_constant<bool, B>, D& unerased, Ts&&... args) -> Return {
    // With GCC, always constructing 'Return' from the result can trigger a
    // '-Wreturn-local-addr' warning, even if both types are the same
    // (e.g. 'const int&' and 'const int&').
    return Return(::arene::base::invoke(Func, unerased, std::forward<Ts>(args)...));
  }

 public:
  /// @brief class type of @c MemFn
  ///
  using container_type = typename member_pointer_traits<MemFn>::class_type;

 private:
  // parasoft-begin-suppress AUTOSAR-A3_3_2-a "False positive: variable 'is_invocable_v' is initialized"
  // parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variable 'is_invocable_v' is initialized"
  /// @brief determine if the object is @c const invocable
  ///
  static constexpr auto is_const_invocable_v = arene::base::is_invocable_v<MemFn, container_type const&, Args...>;
  // parasoft-end-suppress AUTOSAR-A3_3_2-a
  // parasoft-end-suppress CERT_CPP-DCL56-a

  /// @brief alias to possibly add @c const to a type
  /// @tparam T type to possibly add const to
  ///
  template <class T>
  using add_const_if_t = std::conditional_t<is_const_invocable_v, T const, T>;

 public:
  /// @brief invoke the provided member function with a base class pointer
  /// @param erased pointer tr the capacity-erased base class
  /// @param args arguments of the member function, excluding the implicit @c this pointer
  /// @return return value of @c MemFn, converted to @c Return
  ///
  /// Casts @c erased to the type associated and then invokes the specified
  /// member function.
  ///
  static constexpr auto func(add_const_if_t<container_base_type_t<container_type>>* erased, Args... args) -> Return {
    auto& unerased = *static_cast<add_const_if_t<container_type>*>(erased);

    // forward reference args and move non-reference args
    return workaround_gcc_warning(
        std::is_same<Return, member_function_return_t<MemFn>>{},
        unerased,
        static_cast<Args&&>(args)...
    );
  }
};

/// @brief helper for defining a capacity-erased member function
/// @tparam MemFn member function pointer of the non-erased container type
/// @tparam Func pointer to the member function
/// @tparam Return return type of this helper, defaults to @c member_function_return_t<MemFn>
///
/// Helper for container reference types to transform a non-erased container
/// member function to a capacity-erased free function. For example, given a
/// non-erased container member function
/// ~~~{.cpp}
/// auto inline_map<Key, Value, Capacity, Compare>::find(Key const&) const
/// -> const_iterator
/// ~~~
///
/// calling this helper like so:
/// ~~~{.cpp}
/// using Map = inline_map<Key, Value, Capacity, Compare>; // for a particular instantiation of inline_map
/// erased_member_function<
///   auto (Map::*)(typename Map::key_type const&) const->typename Map::const_iterator,
///   &Map::find,
//    map_reference_iterator<inline_map_base<Key, Value, Compare>, true>
/// >;
/// ~~~
///
/// will define a free function with signature:
/// ~~~{.cpp}
/// auto f(inline_map_base<Key, Value, Compare> const* Key const&)
/// -> map_reference_iterator<inline_map_base<Key, Value, Compare>, true>
/// ~~~
///
/// @note This does not provide full support for ref-qualified member functions.
///
template <class MemFn, MemFn Func, class Return = member_function_return_t<MemFn>>
extern constexpr auto erased_member_function = &erased_member_function_wrapper<MemFn, Func, Return>::func;

}  // namespace detail
}  // namespace inline_container
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_ERASED_MEMBER_FUNCTION_HPP_
