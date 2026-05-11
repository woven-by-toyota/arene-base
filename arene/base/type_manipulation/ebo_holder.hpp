// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_EBO_HOLDER_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_EBO_HOLDER_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_class.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_final.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/move.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_cv.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-A14_5_1-a "False positive: The single
// argument template constructor of 'ebo_holder' does not hide copy/move
// constructors via use of SFINAE."
// parasoft-begin-suppress AUTOSAR-A10_2_1 "False positive: There is no inheritance, and therefore functions cannot be
// hidden this way."

/// @brief A class to hold an instance of a type that is likely to be empty, such as a comparator or allocator, making
///        use of the empty base class optimization if possible
/// @tparam Tag A tag type to indicate this instance, for classes that have multiple values to hold
/// @tparam Value The type of the value to hold
// parasoft-begin-suppress AUTOSAR-A12_1_5-a "False positive: nowhere to use a delegating constructor here"
template <typename Tag, typename Value, bool = std::is_class<Value>::value && !std::is_final<Value>::value>
class ebo_holder {
  /// @brief The instance of the value
  Value value_;

 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'value_type' does not hide anything"
  /// @brief The type held by the EBO holder.
  using value_type = Value;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  /// @brief default constructor
  /// @tparam T template parameter used to check constraints
  ///
  /// Value initializes the contained value.
  template <class T = Value, constraints<std::enable_if_t<std::is_default_constructible<T>::value>> = nullptr>
  constexpr ebo_holder() noexcept(std::is_nothrow_default_constructible<T>::value)
      : value_{} {}

  ARENE_IGNORE_START();
  ARENE_IGNORE_ARMCLANG("-Wimplicit-int-float-conversion", "This type supports the same conversions as the Value");
  /// @brief in-place constructor
  /// @tparam Args types to construct from
  /// @param args arguments to construct from
  ///
  /// Constructs value from @c args.
  template <
      class... Args,
      constraints<
          std::enable_if_t<std::is_constructible<Value, Args&&...>::value>,
          std::enable_if_t<   //
              !std::is_same<  //
                  type_list<ebo_holder>,
                  type_list<remove_cvref_t<Args>...>  //
                  >::value                            //
              >                                       //
          > = nullptr>
  // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
  constexpr explicit ebo_holder(Args&&... args) noexcept(std::is_nothrow_constructible<Value, Args&&...>::value)
      : value_{std::forward<Args>(args)...} {}
  ARENE_IGNORE_END();

  // parasoft-begin-suppress AUTOSAR-M9_3_1-a-2 "False positive: returns const handle"
  // parasoft-begin-suppress AUTOSAR-A9_3_1-b-2 "False positive: returns const handle"
  /// @brief Get the value
  /// @return const value_type& A reference to the held value
  constexpr auto get_value(Tag) const& noexcept -> value_type const& { return value_; }
  // parasoft-end-suppress AUTOSAR-A9_3_1-b-2
  // parasoft-end-suppress AUTOSAR-M9_3_1-a-2

  /// @brief Get the value
  /// @return value_type& A reference to the held value
  // parasoft-begin-suppress AUTOSAR-A9_3_1-a "This class doesn't contain/own the data, it wraps it with type info"
  // parasoft-begin-suppress AUTOSAR-A9_3_1-b "This class doesn't contain/own the data, it wraps it with type info"
  constexpr auto get_value(Tag) & noexcept -> value_type& { return value_; }
  // parasoft-end-suppress AUTOSAR-A9_3_1-b
  // parasoft-end-suppress AUTOSAR-A9_3_1-a

  /// @brief Get the value
  /// @return const value_type&& A reference to the held value
  constexpr auto get_value(Tag) const&& noexcept -> value_type const&& { return std::move(value_); }

  /// @brief Get the value
  /// @return value_type&& A reference to the held value
  constexpr auto get_value(Tag) && noexcept -> value_type&& { return std::move(value_); }
};
// parasoft-end-suppress AUTOSAR-A12_1_5-a

/// @brief A class to hold a value, making use of the empty base class optimization if possible. This specialization is
///        for types that can be used as a base class
///
/// @tparam Tag A tag type to indicate this instance, for classes that have multiple values to hold
/// @tparam Value The type of the value to hold
template <typename Tag, typename Value>
class ebo_holder<Tag, Value, true> : Value {
 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'value_type' does not hide anything"
  /// @brief The type held by the EBO holder.
  using value_type = Value;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  // parasoft-begin-suppress AUTOSAR-A12_7_1-a "False positive: =default will have wrong semantics"

  /// @brief default constructor
  /// @tparam T template parameter used to check constraints
  ///
  /// Default constructs the contained value.
  template <class T = Value, constraints<std::enable_if_t<std::is_default_constructible<T>::value>> = nullptr>
  constexpr ebo_holder() noexcept(std::is_nothrow_default_constructible<T>::value)
      : Value{} {}

  // parasoft-end-suppress AUTOSAR-A12_7_1-a

  /// @brief in-place constructor
  /// @tparam Args types to construct from
  /// @param args arguments to construct from
  ///
  /// Constructs value from @c args.
  template <
      class... Args,
      constraints<
          std::enable_if_t<std::is_constructible<Value, Args&&...>::value>,
          std::enable_if_t<   //
              !std::is_same<  //
                  type_list<ebo_holder>,
                  type_list<remove_cvref_t<Args>...>  //
                  >::value                            //
              >                                       //
          > = nullptr>
  // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
  constexpr explicit ebo_holder(Args&&... args) noexcept(std::is_nothrow_constructible<Value, Args&&...>::value)
      : Value{std::forward<Args>(args)...} {}

  /// @brief Get the value
  /// @return const value_type& A reference to the held value
  constexpr auto get_value(Tag) const& noexcept -> value_type const& { return static_cast<value_type const&>(*this); }
  /// @brief Get the value
  /// @return value_type& A reference to the held value
  constexpr auto get_value(Tag) & noexcept -> value_type& { return static_cast<value_type&>(*this); }
  /// @brief Get the value
  /// @return const value_type&& A reference to the held value
  constexpr auto get_value(Tag) const&& noexcept -> value_type const&& {
    return static_cast<value_type const&&>(*this);
  }
  /// @brief Get the value
  /// @return value_type&& A reference to the held value
  constexpr auto get_value(Tag) && noexcept -> value_type&& { return static_cast<value_type&&>(*this); }
};

// parasoft-end-suppress AUTOSAR-A14_5_1-a

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_EBO_HOLDER_HPP_
