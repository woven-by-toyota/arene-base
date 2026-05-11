// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_MEMBER_POINTER_CLASS_TYPE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_MEMBER_POINTER_CLASS_TYPE_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

namespace arene {
namespace base {

namespace member_pointer_class_type_detail {
/// @brief A helper struct for obtaining the class type for which the supplied
/// parameter is a pointer-to-member of.
/// @tparam PointerToMember The type of the pointer-to-member for which to obtain the class
/// of which it is a pointer to member of
template <typename PointerToMember>
struct member_pointer_class_type_impl;

/// @brief A helper struct for obtaining the class type for which the supplied
/// parameter is a pointer-to-member of.
/// This specialization handles the case of a non-qualified pointer-to-member
/// function.
/// @tparam R The return type of the member function
/// @tparam Class The class type of which this is a pointer to member
/// @tparam MFArgs The arguments for the member function
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename R, typename Class, typename... MFArgs>
struct member_pointer_class_type_impl<R (Class::*)(MFArgs...)> {
  /// @brief An alias to the class type to which the pointer-to-member relates
  using type = Class;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A helper struct for obtaining the class type for which the supplied
/// parameter is a pointer-to-member of.
/// This specialization handles the case of a const-qualified pointer-to-member
/// function.
/// @tparam R The return type of the member function
/// @tparam Class The class type of which this is a pointer to member
/// @tparam MFArgs The arguments for the member function
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename R, typename Class, typename... MFArgs>
struct member_pointer_class_type_impl<R (Class::*)(MFArgs...) const> {
  /// @brief An alias to the class type to which the pointer-to-member relates
  using type = Class;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A helper struct for obtaining the class type for which the supplied
/// parameter is a pointer-to-member of.
/// This specialization handles the case of an lvalue-ref-qualified
/// pointer-to-member function.
/// @tparam R The return type of the member function
/// @tparam Class The class type of which this is a pointer to member
/// @tparam MFArgs The arguments for the member function
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename R, typename Class, typename... MFArgs>
struct member_pointer_class_type_impl<R (Class::*)(MFArgs...)&> {
  /// @brief An alias to the class type to which the pointer-to-member relates
  using type = Class;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A helper struct for obtaining the class type for which the supplied
/// parameter is a pointer-to-member of.
/// This specialization handles the case of a const-lvalue-ref-qualified
/// pointer-to-member function.
/// @tparam R The return type of the member function
/// @tparam Class The class type of which this is a pointer to member
/// @tparam MFArgs The arguments for the member function
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename R, typename Class, typename... MFArgs>
struct member_pointer_class_type_impl<R (Class::*)(MFArgs...) const&> {
  /// @brief An alias to the class type to which the pointer-to-member relates
  using type = Class;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A helper struct for obtaining the class type for which the supplied
/// parameter is a pointer-to-member of.
/// This specialization handles the case of a rvalue-ref-qualified
/// pointer-to-member function.
/// @tparam R The return type of the member function
/// @tparam Class The class type of which this is a pointer to member
/// @tparam MFArgs The arguments for the member function
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename R, typename Class, typename... MFArgs>
struct member_pointer_class_type_impl<R (Class::*)(MFArgs...) &&> {
  /// @brief An alias to the class type to which the pointer-to-member relates
  using type = Class;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A helper struct for obtaining the class type for which the supplied
/// parameter is a pointer-to-member of.
/// This specialization handles the case of a const-rvalue-ref-qualified
/// pointer-to-member function.
/// @tparam R The return type of the member function
/// @tparam Class The class type of which this is a pointer to member
/// @tparam MFArgs The arguments for the member function
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename R, typename Class, typename... MFArgs>
struct member_pointer_class_type_impl<R (Class::*)(MFArgs...) const&&> {
  /// @brief An alias to the class type to which the pointer-to-member relates
  using type = Class;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief A helper struct for obtaining the class type for which the supplied
/// parameter is a pointer-to-member of.
/// This specialization handles the case of a pointer-to-data-member
/// @tparam R The type of the pointed to data member
/// @tparam Class The class type of which this is a pointer to member
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename R, typename Class>
struct member_pointer_class_type_impl<R(Class::*)> {
  /// @brief An alias to the class type to which the pointer-to-member relates
  using type = Class;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace member_pointer_class_type_detail

/// @brief A type alias to the class type for which the supplied parameter is a
/// pointer-to-member of.
/// @tparam PointerToMember The type of the pointer-to-member for which to obtain the class
/// of which it is a pointer to member of
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename PointerToMember>
using member_pointer_class_type =
    typename member_pointer_class_type_detail::member_pointer_class_type_impl<PointerToMember>::type;
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_MEMBER_POINTER_CLASS_TYPE_HPP_
