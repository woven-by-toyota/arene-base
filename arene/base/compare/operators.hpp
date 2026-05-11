// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPARE_OPERATORS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPARE_OPERATORS_HPP_

// IWYU pragma: private, include "arene/base/compare.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: It is allowed to declare comparison operators as friend
// functions"
// parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1"

namespace arene {
namespace base {

namespace operators_detail {
/// @brief Helper class to define operator != in terms of the == operator with a left-hand operand of type @c LhsType
/// and a right-hand operand of type @c RhsType
/// @tparam LhsType The type of the left-hand operator parameter
/// @tparam RhsType The type of the right-hand operator parameter
template <typename LhsType, typename RhsType>
class inequality_operator_mixin {
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function"
  /// @brief Compare whether two objects are not equal.
  ///
  /// @param lhs Left-hand side of != operator.
  /// @param rhs Right-hand side of != operator.
  /// @return Return @c true if @c lhs is not equal to @c rhs, otherwise @c
  /// false.
  ARENE_NODISCARD friend constexpr auto operator!=(LhsType const& lhs, RhsType const& rhs) noexcept -> bool {
    static_assert(is_nothrow_equality_comparable_v<LhsType const&, RhsType const&>, "operator == should be noexcept");
    return !(lhs == rhs);
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

 protected:
  /// @brief Default constructor is protected
  constexpr inequality_operator_mixin() noexcept = default;
  /// @brief Default destructor is protected
  ~inequality_operator_mixin() noexcept = default;
  /// @brief Default copy constructor is protected
  /// @param other The value to copy from
  constexpr inequality_operator_mixin(inequality_operator_mixin const& other) noexcept = default;
  /// @brief Default move constructor is protected
  /// @param other The value to copy from
  constexpr inequality_operator_mixin(inequality_operator_mixin&& other) noexcept = default;
  /// @brief Default copy assignment is protected
  /// @param other The value to copy from
  constexpr auto operator=(inequality_operator_mixin const& other) & noexcept -> inequality_operator_mixin& = default;
  /// @brief Default move assignment is protected
  /// @param other The value to copy from
  constexpr auto operator=(inequality_operator_mixin&& other) & noexcept -> inequality_operator_mixin& = default;
};

}  // namespace operators_detail

/// @brief Helper class that can be inherited from by a class to define the != operator in terms of the == operator
/// defined by that class.
///
/// Requires that operator==(const Derived&, const Other&) is defined and does not throw exceptions
///
/// @tparam Derived The type of the derived class. The comparison operators provided take parameters of this type.
/// @tparam Other The type of the other object to compare. The comparison operators provided take parameters of this
/// type.
///
/// For example:
/// @code {cpp}
/// class MyClass : arene::base::full_equality_operators_from_basic_equality<MyClass>
/// {
///   ...
///   // No need to explicitly define 'operator!=' here.
///   friend bool operator==(const MyClass& a, const MyClass& b) noexcept;
/// };
/// @endcode
// parasoft-begin-suppress AUTOSAR-A10_1_1-a-2 "False positive: all base classes are empty"
template <typename Derived, typename Other = Derived>
class full_equality_operators_from_basic_equality
    : operators_detail::inequality_operator_mixin<Derived, Other>
    , operators_detail::inequality_operator_mixin<Other, Derived> {
  // parasoft-end-suppress
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function"
  /// @brief Equality comparison operator for instances of @c Derived and @c Other.
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return bool Result of @c lhs==rhs
  ARENE_NODISCARD friend constexpr auto operator==(Other const& lhs, Derived const& rhs) noexcept -> bool {
    return rhs == lhs;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

 protected:
  /// @brief Default constructor is protected
  constexpr full_equality_operators_from_basic_equality() noexcept = default;
  /// @brief Default destructor is protected
  ~full_equality_operators_from_basic_equality() noexcept = default;
  /// @brief Default copy constructor is protected
  /// @param other The value to copy from
  constexpr full_equality_operators_from_basic_equality(full_equality_operators_from_basic_equality const& other
  ) noexcept = default;
  /// @brief Default move constructor is protected
  /// @param other The value to copy from
  constexpr full_equality_operators_from_basic_equality(full_equality_operators_from_basic_equality&& other
  ) noexcept = default;
  /// @brief Default copy assignment is protected
  /// @param other The value to copy from
  constexpr auto operator=(full_equality_operators_from_basic_equality const& other) & noexcept
      -> full_equality_operators_from_basic_equality& = default;
  /// @brief Default move assignment is protected
  /// @param other The value to copy from
  constexpr auto operator=(full_equality_operators_from_basic_equality&& other) & noexcept
      -> full_equality_operators_from_basic_equality& = default;
};

// @cond INTERNAL
/// @brief Specialization for when @c Other is @c Derived.
template <typename Derived>
class full_equality_operators_from_basic_equality<Derived, Derived>
    : operators_detail::inequality_operator_mixin<Derived, Derived> {
 protected:
  /// @brief Default constructor is protected
  constexpr full_equality_operators_from_basic_equality() noexcept = default;
  /// @brief Default destructor is protected
  ~full_equality_operators_from_basic_equality() noexcept = default;
  /// @brief Default copy constructor is protected
  /// @param other The value to copy from
  constexpr full_equality_operators_from_basic_equality(full_equality_operators_from_basic_equality const& other
  ) noexcept = default;
  /// @brief Default move constructor is protected
  /// @param other The value to copy from
  constexpr full_equality_operators_from_basic_equality(full_equality_operators_from_basic_equality&& other
  ) noexcept = default;
  /// @brief Default copy assignment is protected
  /// @param other The value to copy from
  constexpr auto operator=(full_equality_operators_from_basic_equality const& other) & noexcept
      -> full_equality_operators_from_basic_equality& = default;
  /// @brief Default move assignment is protected
  /// @param other The value to copy from
  constexpr auto operator=(full_equality_operators_from_basic_equality&& other) & noexcept
      -> full_equality_operators_from_basic_equality& = default;
};
// @endcond

namespace operators_detail {
/// @brief Helper class to define <, >, <= and >= operators in terms of the @c LhsType::three_way_compare function with
/// a left-hand operand of type @c LhsType and a right-hand operand of type @c RhsType
/// @tparam LhsType The type of the left-hand operator parameter
/// @tparam RhsType The type of the right-hand operator parameter
template <typename LhsType, typename RhsType>
class ordering_operators_from_three_way_mixin {
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function"
  /// @brief Less-than comparison operator for instances of @c LhsType and @c RhsType.
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return @c True if @c lhs is less than @c rhs, @c false otherwise.
  ARENE_NODISCARD friend constexpr auto operator<(LhsType const& lhs, RhsType const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(
        std::is_same<decltype(LhsType::three_way_compare(lhs, rhs)), strong_ordering>::value,
        "LhsType class must provide three_way_compare function that "
        "returns strong_ordering"
    );
    static_assert(noexcept(LhsType::three_way_compare(lhs, rhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return LhsType::three_way_compare(lhs, rhs) == strong_ordering::less;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function"
  /// @brief Greater-than comparison operator for instances of @c LhsType and @c RhsType
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return @c True if @c lhs is greater than @c rhs, @c false otherwise.
  ARENE_NODISCARD friend constexpr auto operator>(LhsType const& lhs, RhsType const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(
        std::is_same<decltype(LhsType::three_way_compare(lhs, rhs)), strong_ordering>::value,
        "LhsType class must provide three_way_compare function that "
        "returns strong_ordering"
    );
    static_assert(noexcept(LhsType::three_way_compare(lhs, rhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return LhsType::three_way_compare(lhs, rhs) == strong_ordering::greater;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function"
  /// @brief Less-than-or-equal comparison operator for instances of @c LhsType and @c RhsType.
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return @c True if @c lhs is less than or equal to @c rhs, @c false
  /// otherwise.
  ARENE_NODISCARD friend constexpr auto operator<=(LhsType const& lhs, RhsType const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(
        std::is_same<decltype(LhsType::three_way_compare(lhs, rhs)), strong_ordering>::value,
        "LhsType class must provide three_way_compare function that "
        "returns strong_ordering"
    );
    static_assert(noexcept(LhsType::three_way_compare(lhs, rhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    auto const cmp_res = LhsType::three_way_compare(lhs, rhs);
    return cmp_res == strong_ordering::less || cmp_res == strong_ordering::equal;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function"
  /// @brief Greater-than-or-equal comparison operator for instances of @c LhsType and
  /// @c RhsType.
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return true if @c LhsType::three_way_compare(lhs,rhs) returns @c strong_ordering::greater or @c
  /// strong_ordering::equal .
  /// @return false if @c LhsType::three_way_compare(lhs,rhs) returns @c strong_ordering::less .
  /// otherwise.
  ARENE_NODISCARD friend constexpr auto operator>=(LhsType const& lhs, RhsType const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(
        std::is_same<decltype(LhsType::three_way_compare(lhs, rhs)), strong_ordering>::value,
        "LhsType class must provide three_way_compare function that "
        "returns strong_ordering"
    );
    static_assert(noexcept(LhsType::three_way_compare(lhs, rhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    auto const cmp_res = LhsType::three_way_compare(lhs, rhs);
    return cmp_res == strong_ordering::greater || cmp_res == strong_ordering::equal;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

 protected:
  /// @brief Default constructor is protected
  constexpr ordering_operators_from_three_way_mixin() noexcept = default;
  /// @brief Default destructor is protected
  ~ordering_operators_from_three_way_mixin() noexcept = default;
  /// @brief Default copy constructor is protected
  /// @param other The source object
  constexpr ordering_operators_from_three_way_mixin(ordering_operators_from_three_way_mixin const& other
  ) noexcept = default;
  /// @brief Default move constructor is protected
  /// @param other The source object
  constexpr ordering_operators_from_three_way_mixin(ordering_operators_from_three_way_mixin&& other) noexcept = default;
  /// @brief Default copy assignment is protected
  /// @param other The source object
  constexpr auto operator=(ordering_operators_from_three_way_mixin const& other) & noexcept
      -> ordering_operators_from_three_way_mixin& = default;
  /// @brief Default move assignment is protected
  /// @param other The source object
  constexpr auto operator=(ordering_operators_from_three_way_mixin&& other) & noexcept
      -> ordering_operators_from_three_way_mixin& = default;
};
}  // namespace operators_detail

/// @brief Helper class that can be inherited from by a class to define the @c <, @c >, @c <= and @c >= comparison
/// operators in terms of a @c three_way_compare function provided by the Derived class, and define the @c != operator
/// in terms of the @c == operator.
///
/// Requires that the derived class provides a @c static member function @c three_way_compare that accepts a <c>const
/// Derived&</c> and a <c>const Other&</c>, and returns a @c strong_ordering value indicating whether the first is less
/// than, equal to or greater than the second. This @c three_way_compare functions must not throw exceptions.
///
/// Requires that operator==(const Derived&, const Other&) is defined and does not throw exceptions
///
/// @tparam Derived The type of the derived class. The comparison operators provided take parameters of this type.
/// @tparam Other The type of the other object to compare. The comparison operators provided take parameters of this
/// type.
///
/// For example:
/// @code {cpp}
/// class MyClass :
/// arene::base::full_ordering_operators_from_three_way_compare_and_equals<MyClass> {
///   ...
///   friend arene::base::string_ordering three_way_compare(
///     const MyClass& a, const MyClass& b) noexcept;
///   friend bool operator==(const MyClass& a, const MyClass& b) noexcept;
/// };
/// @endcode
///
/// @deprecated Prefer arene::base::generic_ordering_from_three_way_compare
// parasoft-begin-suppress AUTOSAR-A10_1_1-a-2 "False positive: all base classes are empty"
template <typename Derived, typename Other = Derived>
class ARENE_DEPRECATED_WITH("Use generic_ordering_from_three_way_compare instead"
) full_ordering_operators_from_three_way_compare_and_equals
    : full_equality_operators_from_basic_equality<Derived, Other>
    , operators_detail::ordering_operators_from_three_way_mixin<Derived, Other> {
  // parasoft-end-suppress
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function"
  /// @brief Less-than comparison operator for instances of @c Derived and @c Other.
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return bool The result of @c rhs>lhs .
  ARENE_NODISCARD friend constexpr auto operator<(Other const& lhs, Derived const& rhs) noexcept -> bool {
    return rhs > lhs;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function"
  /// @brief Greater-than comparison operator for instances of @c Derived and @c Other.
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return bool The result of @c rhs<lhs .
  ARENE_NODISCARD friend constexpr auto operator>(Other const& lhs, Derived const& rhs) noexcept -> bool {
    return rhs < lhs;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function"
  /// @brief Less-than-or-equal comparison operator for instances of @c Derived and @c
  /// Other.
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return bool The result of @c rhs>=lhs .
  ARENE_NODISCARD friend constexpr auto operator<=(Other const& lhs, Derived const& rhs) noexcept -> bool {
    return rhs >= lhs;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function"
  /// @brief Greater-than-or-equal comparison operator for instances of @c Derived and
  /// @c Other.
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return bool The result of @c rhs<=lhs .
  ARENE_NODISCARD friend constexpr auto operator>=(Other const& lhs, Derived const& rhs) noexcept -> bool {
    return rhs <= lhs;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

 protected:
  /// @brief Default constructor is protected
  constexpr full_ordering_operators_from_three_way_compare_and_equals() noexcept = default;
  /// @brief Default destructor is protected
  ~full_ordering_operators_from_three_way_compare_and_equals() noexcept = default;
  /// @brief Default copy constructor is protected
  /// @param other The source object
  constexpr full_ordering_operators_from_three_way_compare_and_equals(
      full_ordering_operators_from_three_way_compare_and_equals const& other
  ) noexcept = default;
  /// @brief Default move constructor is protected
  /// @param other The source object
  constexpr full_ordering_operators_from_three_way_compare_and_equals(
      full_ordering_operators_from_three_way_compare_and_equals&& other
  ) noexcept = default;
  /// @brief Default copy assignment is protected
  /// @param other The source object
  constexpr auto operator=(full_ordering_operators_from_three_way_compare_and_equals const& other) & noexcept
      -> full_ordering_operators_from_three_way_compare_and_equals& = default;
  /// @brief Default move assignment is protected
  /// @param other The source object
  constexpr auto operator=(full_ordering_operators_from_three_way_compare_and_equals&& other) & noexcept
      -> full_ordering_operators_from_three_way_compare_and_equals& = default;
};

// @cond INTERNAL
ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wdeprecated-declarations", "We still want to specialize the deprecated classes");
/// @brief Specialization for the case that @c Other is @c Derived
// parasoft-begin-suppress AUTOSAR-A10_1_1-a-2 "False positive: all base classes are empty"
template <typename Derived>
class full_ordering_operators_from_three_way_compare_and_equals<Derived, Derived>
    : full_equality_operators_from_basic_equality<Derived>
    , operators_detail::ordering_operators_from_three_way_mixin<Derived, Derived> {
  // parasoft-end-suppress
 protected:
  /// @brief Default constructor is protected
  constexpr full_ordering_operators_from_three_way_compare_and_equals() noexcept = default;
  /// @brief Default destructor is protected
  ~full_ordering_operators_from_three_way_compare_and_equals() noexcept = default;
  /// @brief Default copy constructor is protected
  /// @param other The source object
  constexpr full_ordering_operators_from_three_way_compare_and_equals(
      full_ordering_operators_from_three_way_compare_and_equals const& other
  ) noexcept = default;
  /// @brief Default move constructor is protected
  /// @param other The source object
  constexpr full_ordering_operators_from_three_way_compare_and_equals(
      full_ordering_operators_from_three_way_compare_and_equals&& other
  ) noexcept = default;
  /// @brief Default copy assignment is protected
  /// @param other The source object
  constexpr auto operator=(full_ordering_operators_from_three_way_compare_and_equals const& other) & noexcept
      -> full_ordering_operators_from_three_way_compare_and_equals& = default;
  /// @brief Default move assignment is protected
  /// @param other The source object
  constexpr auto operator=(full_ordering_operators_from_three_way_compare_and_equals&& other) & noexcept
      -> full_ordering_operators_from_three_way_compare_and_equals& = default;
};
ARENE_IGNORE_END();
// @endcond

ARENE_IGNORE_START();
ARENE_IGNORE_ALL(
    "-Wdeprecated-declarations",
    "We still want to use the deprecated class in this other deprecated class"
);
/// @brief Helper class that can be inherited from by a class to define the @c
/// <, @c >, @c <=, @c >=, @c ==, and @c != comparison operators in terms of a
/// @c three_way_compare function provided by the Derived class.
///
/// Requires that the derived class provides a @c static member function @c three_way_compare that accepts two <c>const
/// Derived&</c> and returns a @c strong_ordering value indicating whether the first is less than, equal to or greater
/// than the second. This @c three_way_compare functions must not throw exceptions.
///
/// @tparam Derived The type of the derived class. The comparison operators
/// provided take parameters of this type.
///
/// For example:
/// @code {cpp}
/// class MyClass :
/// arene::base::full_ordering_operators_from_three_way_compare<MyClass> {
///   ...
///   friend arene::base::string_ordering three_way_compare(
///     const MyClass& a, const MyClass& b) noexcept;
/// };
/// @endcode
///
/// @deprecated Prefer arene::base::generic_ordering_from_three_way_compare
template <typename Derived, typename Other = Derived>
class ARENE_DEPRECATED_WITH("Use generic_ordering_from_three_way_compare instead"
) full_ordering_operators_from_three_way_compare
    : full_ordering_operators_from_three_way_compare_and_equals<Derived, Other> {
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function"
  /// @brief Equality comparison operator for instances of @c Derived and @c Other.
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return @c True if @c lhs is equal to @c rhs, @c false otherwise.
  ARENE_NODISCARD friend constexpr auto operator==(Derived const& lhs, Other const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(
        std::is_same<decltype(Derived::three_way_compare(lhs, rhs)), strong_ordering>::value,
        "Derived class must provide three_way_compare function that "
        "returns strong_ordering"
    );
    static_assert(noexcept(Derived::three_way_compare(lhs, rhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(lhs, rhs) == strong_ordering::equal;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

 protected:
  /// @brief Default constructor is protected
  constexpr full_ordering_operators_from_three_way_compare() noexcept = default;
  /// @brief Default destructor is protected
  ~full_ordering_operators_from_three_way_compare() noexcept = default;
  /// @brief Default copy constructor is protected
  /// @param other The source object
  constexpr full_ordering_operators_from_three_way_compare(full_ordering_operators_from_three_way_compare const& other
  ) noexcept = default;
  /// @brief Default move constructor is protected
  /// @param other The source object
  constexpr full_ordering_operators_from_three_way_compare(full_ordering_operators_from_three_way_compare&& other
  ) noexcept = default;
  /// @brief Default copy assignment is protected
  /// @param other The source object
  constexpr auto operator=(full_ordering_operators_from_three_way_compare const& other) & noexcept
      -> full_ordering_operators_from_three_way_compare& = default;
  /// @brief Default move assignment is protected
  /// @param other The source object
  constexpr auto operator=(full_ordering_operators_from_three_way_compare&& other) & noexcept
      -> full_ordering_operators_from_three_way_compare& = default;
};
ARENE_IGNORE_END();

namespace operators_detail {
/// @brief Helper class to define >, <= and >= operators in terms of the < operator with a left-hand operand of type @c
/// LhsType and a right-hand operand of type @c RhsType
/// @tparam LhsType The type of the left-hand operator parameter
/// @tparam RhsType The type of the right-hand operator parameter
template <typename LhsType, typename RhsType>
class ordering_operators_from_less_than_mixin {
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function"
  /// @brief Greater-than comparison operator for instances of @c LhsType and @c RhsType
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return @c True if @c lhs is greater than @c rhs, @c false otherwise.
  ARENE_NODISCARD friend constexpr auto operator>(LhsType const& lhs, RhsType const& rhs) noexcept -> bool {
    static_assert(is_nothrow_less_than_comparable_v<RhsType const&, LhsType const&>, "rhs < lhs should be noexcept");
    return static_cast<bool>(rhs < lhs);
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function"
  /// @brief Less-than-or-equal comparison operator for instances of @c LhsType and @c
  /// RhsType.
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return @c True if @c lhs is less than or equal to @c rhs, @c false
  /// otherwise.
  ARENE_NODISCARD friend constexpr auto operator<=(LhsType const& lhs, RhsType const& rhs) noexcept -> bool {
    static_assert(is_nothrow_less_than_comparable_v<RhsType const&, LhsType const&>, "rhs < lhs should be noexcept");
    return !static_cast<bool>(rhs < lhs);
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function"
  /// @brief Greater-than-or-equal comparison operator for instances of @c LhsType and
  /// @c RhsType.
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return @c True if @c lhs is greater than or equal to @c rhs, @c false
  /// otherwise.
  ARENE_NODISCARD friend constexpr auto operator>=(LhsType const& lhs, RhsType const& rhs) noexcept -> bool {
    static_assert(is_nothrow_less_than_comparable_v<RhsType const&, LhsType const&>, "rhs < lhs should be noexcept");
    return !static_cast<bool>(lhs < rhs);
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

 protected:
  /// @brief Default constructor is protected
  constexpr ordering_operators_from_less_than_mixin() noexcept = default;
  /// @brief Default destructor is protected
  ~ordering_operators_from_less_than_mixin() noexcept = default;
  /// @brief Default copy constructor is protected
  /// @param other The source object
  constexpr ordering_operators_from_less_than_mixin(ordering_operators_from_less_than_mixin const& other
  ) noexcept = default;
  /// @brief Default move constructor is protected
  /// @param other The source object
  constexpr ordering_operators_from_less_than_mixin(ordering_operators_from_less_than_mixin&& other) noexcept = default;
  /// @brief Default copy assignment is protected
  /// @param other The source object
  constexpr auto operator=(ordering_operators_from_less_than_mixin const& other) & noexcept
      -> ordering_operators_from_less_than_mixin& = default;
  /// @brief Default move assignment is protected
  /// @param other The source object
  constexpr auto operator=(ordering_operators_from_less_than_mixin&& other) & noexcept
      -> ordering_operators_from_less_than_mixin& = default;
};
}  // namespace operators_detail

/// @brief Helper class that can be inherited from by a class to define the >, <=, >=, != operators in terms of a <
/// operator and == operator that the derived class provides.
///
/// The derived class is required to provide:
/// - operator<(const Derived&, const Other&)
/// - operator<(const Other&, const Derived&)
/// - operator==(const Derived&, const Other&)
///
/// This class defines the following operators in terms of those:
/// - operator>(const Derived&, const Other&)
/// - operator<=(const Derived&, const Other&)
/// - operator>=(const Derived&, const Other&)
/// - operator!=(const Derived&, const Other&)
/// - operator>(const Other&, const Derived&)
/// - operator<=(const Other&, const Derived&)
/// - operator>=(const Other&, const Derived&)
/// - operator==(const Other&, const Derived&)
/// - operator!=(const Other&, const Derived&)
///
/// For example:
/// @code {cpp}
/// class MyString :
/// arene::base::full_ordering_operators_from_less_than_and_equals<MyString, const
/// char*> {
///   ...
///   // Only need to define less-than and equals. Others defined in terms of
///   this.
///   friend bool operator<(const MyString& a, const char* b) noexcept;
///   friend bool operator<(const char* a, const MyString& b) noexcept;
///   friend bool operator==(const MyString& a, const char* b) noexcept;
/// };
/// @endcode
// parasoft-begin-suppress AUTOSAR-A10_1_1-a-2 "False positive: all base classes are empty"
template <typename Derived, typename Other = Derived>
class full_ordering_operators_from_less_than_and_equals
    : operators_detail::ordering_operators_from_less_than_mixin<Derived, Other>
    , operators_detail::ordering_operators_from_less_than_mixin<Other, Derived>
    , full_equality_operators_from_basic_equality<Derived, Other> {
  // parasoft-end-suppress
 protected:
  /// @brief Default constructor is protected
  constexpr full_ordering_operators_from_less_than_and_equals() noexcept = default;
  /// @brief Default destructor is protected
  ~full_ordering_operators_from_less_than_and_equals() noexcept = default;
  /// @brief Default copy constructor is protected
  /// @param other The source object
  constexpr full_ordering_operators_from_less_than_and_equals(
      full_ordering_operators_from_less_than_and_equals const& other
  ) noexcept = default;
  /// @brief Default move constructor is protected
  /// @param other The source object
  constexpr full_ordering_operators_from_less_than_and_equals(full_ordering_operators_from_less_than_and_equals&& other
  ) noexcept = default;
  /// @brief Default copy assignment is protected
  /// @param other The source object
  constexpr auto operator=(full_ordering_operators_from_less_than_and_equals const& other) & noexcept
      -> full_ordering_operators_from_less_than_and_equals& = default;
  /// @brief Default move assignment is protected
  /// @param other The source object
  constexpr auto operator=(full_ordering_operators_from_less_than_and_equals&& other) & noexcept
      -> full_ordering_operators_from_less_than_and_equals& = default;
};

// @cond INTERNAL
/// @brief Specialization for the case that @c Other is @c Derived
template <typename Derived>
class full_ordering_operators_from_less_than_and_equals<Derived, Derived>
    : operators_detail::ordering_operators_from_less_than_mixin<Derived, Derived>
    , full_equality_operators_from_basic_equality<Derived> {
 protected:
  /// @brief Default constructor is protected
  constexpr full_ordering_operators_from_less_than_and_equals() noexcept = default;
  /// @brief Default destructor is protected
  ~full_ordering_operators_from_less_than_and_equals() noexcept = default;
  /// @brief Default copy constructor is protected
  /// @param other The source object
  constexpr full_ordering_operators_from_less_than_and_equals(
      full_ordering_operators_from_less_than_and_equals const& other
  ) noexcept = default;
  /// @brief Default move constructor is protected
  /// @param other The source object
  constexpr full_ordering_operators_from_less_than_and_equals(full_ordering_operators_from_less_than_and_equals&& other
  ) noexcept = default;
  /// @brief Default copy assignment is protected
  /// @param other The source object
  constexpr auto operator=(full_ordering_operators_from_less_than_and_equals const& other) & noexcept
      -> full_ordering_operators_from_less_than_and_equals& = default;
  /// @brief Default move assignment is protected
  /// @param other The source object
  constexpr auto operator=(full_ordering_operators_from_less_than_and_equals&& other) & noexcept
      -> full_ordering_operators_from_less_than_and_equals& = default;
};
// @endcond

/// @brief Helper class that can be inherited from by a class to define the >, <=, >=, == and != operators in terms of a
/// < operator that the derived class provides.
///
/// The derived class is required to provide:
/// - operator<(const Derived&, const Other&)
/// - operator<(const Other&, const Derived&)
///
/// This class defines the following operators in terms of those:
/// - operator>(const Derived&, const Other&)
/// - operator<=(const Derived&, const Other&)
/// - operator>=(const Derived&, const Other&)
/// - operator==(const Derived&, const Other&)
/// - operator!=(const Derived&, const Other&)
/// - operator>(const Other&, const Derived&)
/// - operator<=(const Other&, const Derived&)
/// - operator>=(const Other&, const Derived&)
/// - operator==(const Other&, const Derived&)
/// - operator!=(const Other&, const Derived&)
///
/// For example:
/// @code {cpp}
/// class MyString : arene::base::full_ordering_operators_from_less_than<MyString,
/// const char*> {
///   ...
///   // Only need to define less-than. Others defined in terms of this.
///   friend bool operator<(const MyString& a, const char* b) noexcept;
///   friend bool operator<(const char* a, const MyString& b) noexcept;
/// };
/// @endcode
template <typename Derived, typename Other = Derived>
class full_ordering_operators_from_less_than : full_ordering_operators_from_less_than_and_equals<Derived, Other> {
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function"
  /// @brief Greater-than-or-equal comparison operator for instances of @c LhsType and
  /// @c RhsType.
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return @c True if @c lhs is greater than or equal to @c rhs, @c false
  /// otherwise.
  ARENE_NODISCARD friend constexpr auto operator==(Derived const& lhs, Other const& rhs) noexcept -> bool {
    static_assert(is_nothrow_less_than_comparable_v<Derived const&, Other const&>, "lhs < rhs should be noexcept");
    static_assert(is_nothrow_less_than_comparable_v<Other const&, Derived const&>, "rhs < lhs should be noexcept");
    return !static_cast<bool>(lhs < rhs) && !static_cast<bool>(rhs < lhs);
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2
};

/// @brief An enumeration used for the return value of @c fast_inequality_check functions to indicate if two values are
/// definitely not equal, or may be equal or not. This is used in generic implementations of @c operator== and @c
/// operator!= to avoid comparing for ordering when only equality is required.
enum class inequality_heuristic : bool {
  ///@brief The two values checked are definitely not equal to each other
  definitely_not_equal,
  ///@brief The values checked may or may not be equal to each other; a more detailed check is required
  may_be_equal_or_not_equal
};

/// @brief Type trait to check if an instance of type @c T can be compared to an instance of type @c U with a static
/// @c fast_inequality_check member function that returns @c inequality_heuristic.
/// @tparam T The type of the lhs
/// @tparam U The type of the rhs. Defaults to @c T
/// @return true If @c T::fast_inequality_check(T,U) is defined and returns @c inequality_heuristic.
/// @return false Otherwise.
template <typename T, typename U = T, typename = constraints<>>
extern constexpr bool has_fast_inequality_check_v = false;

/// @brief Specialization for the case that @c T and @c U can be compared.
template <typename T, typename U>
extern constexpr bool has_fast_inequality_check_v<
    T,
    U,
    constraints<std::enable_if_t<std::is_same<
        decltype(T::fast_inequality_check(std::declval<T const&>(), std::declval<U const&>())),
        inequality_heuristic>::value>>> = true;

/// @brief Helper class that can be inherited from by a class to define the relational comparison operators in terms of
/// a @c three_way_compare function provided by the @c Derived class, and an optional fast-inequality check heuristic.
///
/// Generates the @c <, @c >, @c <=, @c >=, @c ==, and @c != comparison operators from a possibly overloaded @c static
/// member function @c three_way_compare in the @c Derived class that accepts a <c>const Derived&</c> as the first
/// argument and any other type as the second argument, and returns a @c strong_ordering value indicating whether the
/// first is less than, equal to or greater than the second. These @c three_way_compare functions must not throw
/// exceptions.
///
/// The derived class can optionally provide a @c static member function @c fast_inequality_check that accepts a
/// <c>const Derived&</c> as the first argument and any other type as the second argument, and returns a @c
/// inequality_heuristic value indicating whether the first is definitely not equal to the second, or may or may not be
/// equal to the second. These @c fast_inequality_check functions must not throw exceptions. The generated @c operator==
/// and @c operator!= overloads will invoke @c fast_inequality_check prior to @c three_way_compare if there is an
/// overload provided that accepts the given types.
///
/// @tparam Derived The type of the derived class. The comparison operators provided take parameters of this type.
///
/// For example:
/// @snippet docs/examples/ordering_examples.cpp string_like_generic_example
///
/// Here, @c MyClass has comparison operators for comparing instances of @c MyClass with each other, or with @c
/// OtherClass, or with @c YetAnotherClass. Equality comparisons with @c OtherClass will use @c fast_inequality_check to
/// avoid calling a potentially expensive @c three_way_compare for instances that are definitely not equal. e.g. this
/// may be used to check the size of a container, and avoid doing elementwise comparisons when ordering is not required.
template <typename Derived>
class generic_ordering_from_three_way_compare {
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Less-than comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Derived to compare
  /// @param rhs The instance of @c Other to compare
  /// @return bool @c true if @c lhs is less than @c rhs, @c false otherwise
  /// @pre @c Derived::three_way_compare(lhs,rhs) is well-formed
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator<(Self const& lhs, Other const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(lhs, rhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(lhs, rhs) == strong_ordering::less;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Less-than comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other The type of the object to compare to
  /// @param lhs The instance of @c Other to compare
  /// @param rhs The instance of @c Derived to compare
  /// @return bool @c true if @c lhs is less than @c rhs, @c false otherwise
  /// @pre @c Derived::three_way_compare(rhs,lhs) is well-formed
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>,
          std::enable_if_t<!is_three_way_comparable_v<Other, Derived>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator<(Other const& lhs, Self const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(rhs, lhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(rhs, lhs) == strong_ordering::greater;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Greater-than comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Derived to compare
  /// @param rhs The instance of @c Other to compare
  /// @return bool @c true if @c lhs is greater than @c rhs, @c false otherwise
  /// @pre @c Derived::three_way_compare(lhs,rhs) is well-formed
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator>(Self const& lhs, Other const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(lhs, rhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(lhs, rhs) == strong_ordering::greater;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Greater-than comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Other to compare
  /// @param rhs The instance of @c Derived to compare
  /// @return bool @c true if @c lhs is greater than @c rhs, @c false otherwise
  /// @pre @c Derived::three_way_compare(rhs,lhs) is well-formed
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>,
          std::enable_if_t<!is_three_way_comparable_v<Other, Derived>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator>(Other const& lhs, Self const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(rhs, lhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(rhs, lhs) == strong_ordering::less;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Less-than-or-equal comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Derived to compare
  /// @param rhs The instance of @c Other to compare
  /// @return bool @c true if @c lhs is less than or equal to @c rhs, @c false otherwise
  /// @pre @c Derived::three_way_compare(lhs,rhs) is well-formed
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator<=(Self const& lhs, Other const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(lhs, rhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(lhs, rhs) != strong_ordering::greater;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Less-than-or-equal comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Other to compare
  /// @param rhs The instance of @c Derived to compare
  /// @return bool @c true if @c lhs is less than or equal to @c rhs, @c false otherwise
  /// @pre @c Derived::three_way_compare(rhs,lhs) is well-formed
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>,
          std::enable_if_t<!is_three_way_comparable_v<Other, Derived>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator<=(Other const& lhs, Self const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(rhs, lhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(rhs, lhs) != strong_ordering::less;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Greater-than-or-equal comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Derived to compare
  /// @param rhs The instance of @c Other to compare
  /// @return bool @c true if @c lhs is greater than or equal to @c rhs, @c false otherwise
  /// @pre @c Derived::three_way_compare(lhs,rhs) is well-formed
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator>=(Self const& lhs, Other const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(lhs, rhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(lhs, rhs) != strong_ordering::less;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Greater-than-or-equal comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Other to compare
  /// @param rhs The instance of @c Derived to compare
  /// @return bool @c true if @c lhs is greater than or equal to @c rhs, @c false otherwise
  /// @pre @c Derived::three_way_compare(rhs,lhs) is well-formed
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>,
          std::enable_if_t<!is_three_way_comparable_v<Other, Derived>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator>=(Other const& lhs, Self const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(rhs, lhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(rhs, lhs) != strong_ordering::greater;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Equality comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Derived to compare
  /// @param rhs The instance of @c Other to compare
  /// @return bool @c true if @c lhs is equal to @c rhs, @c false otherwise
  /// @pre @c Derived::three_way_compare(lhs,rhs) is well-formed
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>,
          std::enable_if_t<!has_fast_inequality_check_v<Derived, Other>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator==(Self const& lhs, Other const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(lhs, rhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(lhs, rhs) == strong_ordering::equal;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Equality comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Derived to compare
  /// @param rhs The instance of @c Other to compare
  /// @return bool @c true if @c lhs is equal to @c rhs, @c false otherwise
  /// @pre @c Derived::three_way_compare(lhs,rhs) is well-formed
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>,
          std::enable_if_t<has_fast_inequality_check_v<Derived, Other>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator==(Self const& lhs, Other const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(lhs, rhs)), "three_way_compare must be noexcept");
    static_assert(noexcept(Derived::fast_inequality_check(lhs, rhs)), "fast_inequality_check must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return (Derived::fast_inequality_check(lhs, rhs) == inequality_heuristic::may_be_equal_or_not_equal) &&
           (Derived::three_way_compare(lhs, rhs) == strong_ordering::equal);
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Equality comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Derived to compare
  /// @param rhs The instance of @c Other to compare
  /// @return bool @c true if @c lhs is equal to @c rhs, @c false otherwise
  /// @pre @c Derived::three_way_compare(rhs,lhs) is well-formed
  /// @pre @c Other::three_way_compare(lhs,rhs) is *not* well-formed
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>,
          std::enable_if_t<!is_three_way_comparable_v<Other, Derived>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator==(Other const& lhs, Self const& rhs) noexcept -> bool {
    return rhs == lhs;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Inequality comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Derived to compare
  /// @param rhs The instance of @c Other to compare
  /// @return bool @c true if @c lhs is not equal to @c rhs, @c false otherwise
  /// @pre @c Derived::three_way_compare(lhs,rhs) is well-formed
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator!=(Self const& lhs, Other const& rhs) noexcept -> bool {
    return !(lhs == rhs);
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Inequality comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Derived to compare
  /// @param rhs The instance of @c Other to compare
  /// @return bool @c true if @c lhs is not equal to @c rhs, @c false otherwise
  /// @pre @c Derived::three_way_compare(rhs,lhs) is well-formed
  /// @pre @c Other::three_way_compare(lhs,rhs) is *not* well-formed
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>,
          std::enable_if_t<!is_three_way_comparable_v<Other, Derived>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator!=(Other const& lhs, Self const& rhs) noexcept -> bool {
    return !(lhs == rhs);
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

 protected:
  /// @brief Default constructor is protected
  constexpr generic_ordering_from_three_way_compare() noexcept = default;
  /// @brief Default destructor is protected
  ~generic_ordering_from_three_way_compare() noexcept = default;
  /// @brief Default copy constructor is protected
  /// @param other The source object
  constexpr generic_ordering_from_three_way_compare(generic_ordering_from_three_way_compare const& other
  ) noexcept = default;
  /// @brief Default move constructor is protected
  /// @param other The source object
  constexpr generic_ordering_from_three_way_compare(generic_ordering_from_three_way_compare&& other) noexcept = default;
  /// @brief Default copy assignment is protected
  /// @param other The source object
  constexpr auto operator=(generic_ordering_from_three_way_compare const& other) & noexcept
      -> generic_ordering_from_three_way_compare& = default;
  /// @brief Default move assignment is protected
  /// @param other The source object
  constexpr auto operator=(generic_ordering_from_three_way_compare&& other) & noexcept
      -> generic_ordering_from_three_way_compare& = default;
};

namespace operators_detail {
/// @brief Type trait to check if we can provide a reversed operator== for this pair of types (breaks an infinite loop)
/// @tparam Self The type which is inheriting from this operator mixin
/// @tparam Other The type which we're trying to compare with (the LHS of a *reversed* comparison)
template <typename Self, typename Other>
constexpr bool eligible_for_reversed_equals_with_v =
    !std::is_same<Self, Other>::value && is_equality_comparable_v<Self, Other>;
}  // namespace operators_detail

/// @brief Helper class that can be inherited from by a class to define the relational comparison operators in terms of
/// a @c three_way_compare function provided by the @c Derived class, but preferentially implementing @c == and @c !=
/// to use <c>operator==(Derived, Other)</c> rather than <c>three_way_compare</c>.
///
/// Generates the @c <, @c >, @c <=, and @c >= comparison operators from a possibly overloaded @c static member function
/// @c three_way_compare in the @c Derived class that accepts a <c>const Derived&</c> as the first argument and any
/// other type as the second argument, and returns a @c strong_ordering value indicating whether the first is less than,
/// equal to or greater than the second. These @c three_way_compare functions must not throw exceptions.
///
/// Even if the derived class provides a @c fast_inequality_check function, it is not used, because the exact @c
/// operator== is preferred over <c>three_way_compare</c>.
///
/// @tparam Derived The type of the derived class. The comparison operators provided take parameters of this type.
///
/// For example:
/// @snippet docs/examples/ordering_examples.cpp three_way_and_equals_mixin_example
///
/// Here, @c MyClass has comparison operators for comparing instances of @c MyClass with each other, or with @c
/// OtherClass, or with @c YetAnotherClass. Equality comparisons with @c OtherClass will use <c>operator==</c>, while
/// comparisons with @c YetAnotherClass will use <c>three_way_compare</c>.
template <typename Derived>
class generic_ordering_from_three_way_compare_and_equals {
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Less-than comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Derived to compare
  /// @param rhs The instance of @c Other to compare
  /// @return bool @c true if @c lhs is less than @c rhs, @c false otherwise
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator<(Self const& lhs, Other const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(lhs, rhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(lhs, rhs) == strong_ordering::less;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Less-than comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other The type of the object to compare to
  /// @param lhs The instance of @c Other to compare
  /// @param rhs The instance of @c Derived to compare
  /// @return bool @c true if @c lhs is less than @c rhs, @c false otherwise
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>,
          std::enable_if_t<!is_three_way_comparable_v<Other, Derived>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator<(Other const& lhs, Self const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(rhs, lhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(rhs, lhs) == strong_ordering::greater;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Greater-than comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Derived to compare
  /// @param rhs The instance of @c Other to compare
  /// @return bool @c true if @c lhs is greater than @c rhs, @c false otherwise
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator>(Self const& lhs, Other const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(lhs, rhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(lhs, rhs) == strong_ordering::greater;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Greater-than comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Other to compare
  /// @param rhs The instance of @c Derived to compare
  /// @return bool @c true if @c lhs is greater than @c rhs, @c false otherwise
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>,
          std::enable_if_t<!is_three_way_comparable_v<Other, Derived>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator>(Other const& lhs, Self const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(rhs, lhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(rhs, lhs) == strong_ordering::less;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Less-than-or-equal comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Derived to compare
  /// @param rhs The instance of @c Other to compare
  /// @return bool @c true if @c lhs is less than or equal to @c rhs, @c false otherwise
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator<=(Self const& lhs, Other const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(lhs, rhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(lhs, rhs) != strong_ordering::greater;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Less-than-or-equal comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Other to compare
  /// @param rhs The instance of @c Derived to compare
  /// @return bool @c true if @c lhs is less than or equal to @c rhs, @c false otherwise
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>,
          std::enable_if_t<!is_three_way_comparable_v<Other, Derived>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator<=(Other const& lhs, Self const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(rhs, lhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(rhs, lhs) != strong_ordering::less;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Greater-than-or-equal comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Derived to compare
  /// @param rhs The instance of @c Other to compare
  /// @return bool @c true if @c lhs is greater than or equal to @c rhs, @c false otherwise
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator>=(Self const& lhs, Other const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(lhs, rhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(lhs, rhs) != strong_ordering::less;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Greater-than-or-equal comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Other to compare
  /// @param rhs The instance of @c Derived to compare
  /// @return bool @c true if @c lhs is greater than or equal to @c rhs, @c false otherwise
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_three_way_comparable_v<Derived, Other>>,
          std::enable_if_t<!is_three_way_comparable_v<Other, Derived>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator>=(Other const& lhs, Self const& rhs) noexcept -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(noexcept(Derived::three_way_compare(rhs, lhs)), "three_way_compare must be noexcept");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return Derived::three_way_compare(rhs, lhs) != strong_ordering::greater;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Equality comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Derived to compare
  /// @param rhs The instance of @c Other to compare
  /// @return bool @c true if @c lhs is equal to @c rhs, @c false otherwise
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<operators_detail::eligible_for_reversed_equals_with_v<Self, Other>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator==(Other const& lhs, Self const& rhs) noexcept -> bool {
    return rhs == lhs;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Inequality comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Derived to compare
  /// @param rhs The instance of @c Other to compare
  /// @return bool @c true if @c lhs is not equal to @c rhs, @c false otherwise
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<is_equality_comparable_v<Self, Other>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator!=(Self const& lhs, Other const& rhs) noexcept -> bool {
    return !(lhs == rhs);
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Inequality comparison operator for instances of @c Derived with @c Other.
  /// @tparam Self the type of the @c Derived object to compare, used for constraints
  /// @tparam Other the type of the object to compare to
  /// @param lhs The instance of @c Derived to compare
  /// @param rhs The instance of @c Other to compare
  /// @return bool @c true if @c lhs is not equal to @c rhs, @c false otherwise
  template <
      typename Self,
      typename Other,
      constraints<
          std::enable_if_t<std::is_same<Self, Derived>::value>,
          std::enable_if_t<operators_detail::eligible_for_reversed_equals_with_v<Self, Other>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator!=(Other const& lhs, Self const& rhs) noexcept -> bool {
    return !(lhs == rhs);
  }

 protected:
  /// @brief Default constructor is protected
  constexpr generic_ordering_from_three_way_compare_and_equals() noexcept = default;
  /// @brief Default destructor is protected
  ~generic_ordering_from_three_way_compare_and_equals() noexcept = default;
  /// @brief Default copy constructor is protected
  /// @param other The source object
  constexpr generic_ordering_from_three_way_compare_and_equals(
      generic_ordering_from_three_way_compare_and_equals const& other
  ) noexcept = default;
  /// @brief Default move constructor is protected
  /// @param other The source object
  constexpr generic_ordering_from_three_way_compare_and_equals(
      generic_ordering_from_three_way_compare_and_equals&& other
  ) noexcept = default;
  /// @brief Default copy assignment is protected
  /// @param other The source object
  constexpr auto operator=(generic_ordering_from_three_way_compare_and_equals const& other) & noexcept
      -> generic_ordering_from_three_way_compare_and_equals& = default;
  /// @brief Default move assignment is protected
  /// @param other The source object
  constexpr auto operator=(generic_ordering_from_three_way_compare_and_equals&& other) & noexcept
      -> generic_ordering_from_three_way_compare_and_equals& = default;
};

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A11_3_1-a-2
// parasoft-end-suppress AUTOSAR-A13_5_5-b-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPARE_OPERATORS_HPP_
