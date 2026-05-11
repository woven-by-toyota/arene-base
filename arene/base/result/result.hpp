// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file result.hpp
/// @brief Provides @c arene::base::result, a value/error sum-type suitable for use as a return value, similar to
///        @c std::expected
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RESULT_RESULT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RESULT_RESULT_HPP_

// IWYU pragma: private, include "arene/base/result.hpp"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/functional/invoke.hpp"
#include "arene/base/memory/construct_at.hpp"
#include "arene/base/monostate/monostate.hpp"
#include "arene/base/result/detail/in_place_error.hpp"  // IWYU pragma: export
#include "arene/base/result/detail/in_place_value.hpp"  // IWYU pragma: export
#include "arene/base/result/detail/result_base.hpp"
#include "arene/base/stdlib_choice/addressof.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/hash.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_object.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/is_void.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_const.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_manipulation/non_constructible_dummy.hpp"
#include "arene/base/type_traits/all_of.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/conditional.hpp"
#include "arene/base/type_traits/decays_to.hpp"
#include "arene/base/type_traits/has_overloaded_address_operator.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
#include "arene/base/utility/forward_like.hpp"
#include "arene/base/utility/swap.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-A10_2_1-b-2 "False Positive: Base class function made visible with 'using'"
// parasoft-begin-suppress AUTOSAR-A1_1_1-b-2 "False Positive: Copy constructor is defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-M14_5_3-a-2 "False Positive: Copy assignment is defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A12_0_1-a-2 "False Positive: Copy assignment is defined or deleted as appropriate"
/// @brief A sum type which can hold either a _value_ or an _error_. Similar to
/// [std::expected](https://en.cppreference.com/w/cpp/utility/expected).
///
/// @tparam T the type of the value channel. May be @c void , otherwise must satisfy @c std::is_object and must not have
///         const or volatile qualification.
/// @tparam E the type of the error channel. May not be @c void , must satisfy @c std::is_object and must not have
///         const or volatile qualification.
///
/// @c result<T,E> can be used as a return type from an API to express the possibility that API being _failable_ without
/// the need for outparams or exceptions. Its design is non-allocating, and it its memory usage is similar to that of a
/// @c union : it requires the amount of memory needed to store the larger of @c T or @c E , plus the space for the
/// discriminator.
///
/// Semantically, @c result<T,E> contains two _channels_, the _value channel_ and the _error channel_, and at any point
/// during program execution exactly **one** of these channels is populated. A default constructed @c result<T,E> has
/// its @c value_channel default constructed.
///
/// The following major operations are supported:
/// * Querying whether the value or error channel is populated (optionally with a specific value/error).
/// * Accessing the value/error channel's content. This is a _checked_ operation, meaning that an @c ARENE_PRECONDITION
///   violation will occur if the specified channel is not populated.
/// * Equality Comparison if both @c T and @c E are equality comparable. @c results with different channels populated
///   always compare unequal.
/// * Assignment if both @c T and @c E are assignable.
/// * @c swap if both @c T and @c E swappable and move-assignable.
/// * @c std::hash if both @c T and @c E support @c std::hash .
/// * Conversion to @c bool , which is equivalent to @c result<T,E>::has_value() .
/// * Dereferencing the @c result , which is equivalent to accessing @c result::value() .
///
/// Some basic example usages:
/// @snippet docs/examples/result_examples.cpp basic_example
///
/// Please see the detailed [user manual](result) for usage examples and best practices.
template <class T, class E>
// NOLINTNEXTLINE(hicpp-special-member-functions) false-positive, it can't see through the non-constructible-dummy trick
class result : result_detail::result_base<T, E> {
  /// @brief The type of the base class holding the storage
  using base_type = result_detail::result_base<T, E>;

  static_assert(
      !has_overloaded_address_operator_v<T>,
      "The value type stored in result must not have an overloaded address-of operator"
  );

 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d-2 "False Positive: This identifier does not hide anything"
  /// @brief The type when the @c result contains a value.
  using value_type = T;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d-2

  /// @brief The type when the @c result contains an error.
  using error_type = E;

 private:
  static_assert(
      std::is_same<remove_cvref_t<value_type>, value_type>::value,
      "value_type must be an unqualified type, non-reference type."
  );
  static_assert(
      std::is_same<remove_cvref_t<error_type>, error_type>::value,
      "error_type must be an unqualified type, non-reference type."
  );

  static_assert(!std::is_same<error_type, void>::value, "void as an error_type is not supported");
  static_assert(
      !std::is_same<remove_cvref_t<error_type>, monostate>::value,
      "monostate as an error_type is not supported"
  );

  static_assert(std::is_object<value_type>::value, "value_type must be an object type (it must not be a reference).");
  static_assert(std::is_object<error_type>::value, "error_type must be an object type (it must not be a reference).");

 public:
  ///
  /// @brief Constructs a @c result with a default-constructed @c value_type . Does not participate in overload
  ///        resolution if @c value_type is not default constructible.
  /// @post @c has_value() returns @c true
  /// @post @c value() returns reference to an object equivalent to default-constructing @c value_type .
  constexpr result() = default;

  ///
  /// @brief Constructs a @c result containing the @c value_type by way of direct-non-list-initialization.
  ///
  /// @tparam P The type of the initializers. Must satisfy @c std::is_constructible<value_type,P&&...> .
  /// @param args The arguments to initialize the value channel with.
  /// @post @c has_value() returns @c true
  /// @post @c value() returns reference to an object equivalent to constructing @c value_type with @c args .
  ///
  /// Example Usage:
  /// @snippet docs/examples/result_examples.cpp in_place_value
  template <class... P, constraints<std::enable_if_t<std::is_constructible<value_type, P&&...>::value>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr result(in_place_value_t, P&&... args) noexcept(noexcept(base_type(in_place_value, std::forward<P>(args)...))
  )
      : base_type(in_place_value, std::forward<P>(args)...) {}

  ///
  /// @brief Constructs a @c result containing the @c error_type by way of direct-non-list-initialization.
  ///
  /// @tparam P The type of the initializers. Must satisfy @c std::is_constructible<error_type,P&&...> .
  /// @param args The arguments to initialize the value channel with.
  /// @post @c has_error() returns @c true
  /// @post @c error() returns reference to an object equivalent to constructing @c error_type with @c args .
  ///
  /// Example Usage:
  /// @snippet docs/examples/result_examples.cpp in_place_error
  template <class... P, constraints<std::enable_if_t<std::is_constructible<error_type, P&&...>::value>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr result(in_place_error_t, P&&... args) noexcept(noexcept(base_type(in_place_error, std::forward<P>(args)...))
  )
      : base_type(in_place_error, std::forward<P>(args)...) {}

  /// @brief Default copy constructor
  constexpr result(result const& copy) noexcept(noexcept(base_type(std::declval<result const&>()))) = default;
  /// @brief Default move constructor
  // NOLINTNEXTLINE(hicpp-noexcept-move) We want to exactly mimic the noexcept properties of ValueType and ErrorType.
  constexpr result(result&& move) noexcept(noexcept(base_type(std::declval<result&&>()))) = default;
  /// @brief Destroy the stored value or error
  ~result() = default;

 private:
  /// @brief trait which detects if the result should be copy-assignable
  static constexpr bool is_copy_assignable{
      std::is_nothrow_copy_constructible<value_type>::value && std::is_nothrow_copy_assignable<value_type>::value &&
      std::is_nothrow_copy_constructible<error_type>::value && std::is_nothrow_copy_assignable<error_type>::value
  };

  /// @brief The argument for the "copy assign" operator: @c result if the assignment is possible, @c
  /// non_constructible_dummy if not
  using copy_assign_arg = conditional_t<is_copy_assignable, result, non_constructible_dummy>;
  /// @brief The argument for the *deleted* "copy assign" operator: @c non_constructible_dummy if the assignment is
  /// possible, @c result if not (so the copy assignment is deleted)
  using deleted_copy_assign_arg = conditional_t<is_copy_assignable, non_constructible_dummy, result>;

 public:
  /// @brief Copy-assignment for the case that @c value_type and @c error_type satisfy
  /// @c std::is_nothrow_copy_constructible and @c std::is_nothrow_copy_assignable, otherwise a dummy assignment
  /// operator that can never be called
  /// @param other The value to copy from
  /// @return *this
  constexpr auto operator=(copy_assign_arg const& other) noexcept -> result& {
    if (&other != this) {
      std::ignore = assign(other);
    }
    return *this;
  }
  /// @brief Deleted Copy-assignment if @c value_type or @c error_type do not satisfy
  /// @c std::is_nothrow_copy_constructible and @c std::is_nothrow_copy_assignable, otherwise a deleted dummy assignment
  /// operator
  /// @param other The value to copy from
  /// @return *this
  constexpr auto operator=(deleted_copy_assign_arg const& other) noexcept -> result& = delete;

 private:
  /// @brief trait to detect if the result should be move-assignable
  static constexpr bool is_move_assignable{
      std::is_nothrow_move_constructible<value_type>::value && std::is_nothrow_move_assignable<value_type>::value &&
      std::is_nothrow_move_constructible<error_type>::value && std::is_nothrow_move_assignable<error_type>::value
  };

  /// @brief The argument for the "move assign" operator: @c result if the assignment is possible, @c
  /// non_constructible_dummy if not
  using move_assign_arg = conditional_t<is_move_assignable, result, non_constructible_dummy>;
  /// @brief The argument for the *deleted* "move assign" operator: @c non_constructible_dummy if the assignment is
  /// possible, @c result if not (so the move assignment is deleted)
  using deleted_move_assign_arg = conditional_t<is_move_assignable, non_constructible_dummy, result>;

 public:
  /// @brief Move-assignment for the case that @c value_type and @c error_type satisfy
  /// @c std::is_nothrow_move_constructible and @c std::is_nothrow_move_assignable, otherwise a dummy assignment
  /// operator that can never be called
  /// @param other The value to move from
  /// @return *this
  constexpr auto operator=(move_assign_arg&& other) noexcept -> result& {
    std::ignore = assign(std::move(other));
    return *this;
  }
  /// @brief Deleted Move-assignment if @c value_type or @c error_type do not satisfy
  /// @c std::is_nothrow_move_constructible and @c std::is_nothrow_move_assignable, otherwise a deleted dummy assignment
  /// operator
  /// @param other The value to move from
  /// @return *this
  constexpr auto operator=(deleted_move_assign_arg&& other) noexcept -> result& = delete;

  // @brief Expose @c has_value from base type
  using base_type::has_value;

  /// @brief Checks if the value channel contains a specific value
  ///
  /// @param val The value to check for.
  /// @return true If @c has_value() and @c value()==val .
  /// @return false Otherwise.
  template <typename U = value_type, constraints<std::enable_if_t<is_equality_comparable_v<U>>> = nullptr>
  ARENE_NODISCARD constexpr auto has_value(value_type const& val) const
      noexcept(noexcept(val == std::declval<value_type>())) -> bool {
    if (this->has_value()) {
      return val == this->storage().unsafe_value();
    }
    return false;
  }

  // @brief Expose @c has_error from base type
  using base_type::has_error;

  // @brief Expose boolean conversion from base type
  using base_type::operator bool;

  /// @brief Accesses a pointer to the content of the value channel.
  ///
  /// @pre has_value(), else ARENE_PRECONDITION violation.
  /// @return A pointer to the contained value
  ARENE_NODISCARD constexpr auto operator->() noexcept -> value_type* { return &value(); }

  /// @brief Accesses a pointer to the content of the value channel.
  ///
  /// @pre has_value(), else ARENE_PRECONDITION violation.
  /// @return A pointer to the contained value
  ARENE_NODISCARD constexpr auto operator->() const noexcept -> value_type const* { return &value(); }

  /// @brief Accesses a reference to the content of the value channel.
  ///
  /// @pre has_value(), else ARENE_PRECONDITION violation.
  /// @return A reference to the contained value
  ARENE_NODISCARD constexpr auto operator*() & noexcept -> value_type& { return value(); }

  /// @brief Accesses a reference to the content of the value channel.
  ///
  /// @pre has_value(), else ARENE_PRECONDITION violation.
  /// @return A const reference to the contained value
  ARENE_NODISCARD constexpr auto operator*() const& noexcept -> value_type const& { return value(); }

  /// @brief Accesses a reference to the content of the value channel.
  ///
  /// @pre has_value(), else ARENE_PRECONDITION violation.
  /// @return An rvalue reference to the contained value
  ARENE_NODISCARD constexpr auto operator*() && noexcept -> value_type&& { return std::move(value()); }

  // parasoft-begin-suppress AUTOSAR-A18_9_3-a-2 "We want to ensure the qualifier is preserved"
  /// @brief Accesses a reference to the content of the value channel.
  ///
  /// @pre has_value(), else ARENE_PRECONDITION violation.
  /// @return A const rvalue reference to the contained value
  ARENE_NODISCARD constexpr auto operator*() const&& noexcept -> value_type const&& { return std::move(value()); }
  // parasoft-end-suppress AUTOSAR-A18_9_3-a-2

  // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the value"
  /// @brief Accesses a reference to the content of the value channel.
  ///
  /// @pre has_value(), else ARENE_PRECONDITION violation.
  /// @return A reference to the contained value
  ARENE_NODISCARD constexpr auto value() & noexcept -> value_type& {
    ARENE_PRECONDITION(this->has_value());
    return this->storage().unsafe_value();
  }
  // parasoft-end-suppress AUTOSAR-A9_3_1-a-2

  /// @brief Accesses a reference to the content of the value channel.
  ///
  /// @pre has_value(), else ARENE_PRECONDITION violation.
  /// @return A const reference to the contained value
  ARENE_NODISCARD constexpr auto value() const& noexcept -> value_type const& {
    ARENE_PRECONDITION(this->has_value());
    return this->storage().unsafe_value();
  }

  // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the value"
  /// @brief Accesses a reference to the content of the value channel.
  ///
  /// @pre has_value(), else ARENE_PRECONDITION violation.
  /// @return An rvalue reference to the contained value
  ARENE_NODISCARD constexpr auto value() && noexcept -> value_type&& {
    ARENE_PRECONDITION(this->has_value());
    return std::move(this->storage().unsafe_value());
  }
  // parasoft-end-suppress AUTOSAR-A9_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A18_9_3-a-2 "We want to ensure the qualifier is preserved"
  /// @brief Accesses a reference to the content of the value channel.
  ///
  /// @pre has_value(), else ARENE_PRECONDITION violation.
  /// @return A const rvalue reference to the contained value
  ARENE_NODISCARD constexpr auto value() const&& noexcept -> value_type const&& {
    ARENE_PRECONDITION(this->has_value());
    return std::move(this->storage().unsafe_value());
  }
  // parasoft-end-suppress AUTOSAR-A18_9_3-a-2

  /// @brief Returns the value if the value channel is populated, else another value. Not declared if @c T is @c void .
  /// @tparam U The type of the default value. Must satisfy @c std::is_convertible<U,value_type> .
  /// @param default_value The default value to use if the value channel is not populated.
  /// @return value_type If @c has_value() is true, equivalent to @c value() . Otherwise, @c default_value converted to
  ///         @c value_type .
  template <typename U = T, constraints<std::enable_if_t<std::is_convertible<U, value_type>::value>> = nullptr>
  ARENE_NODISCARD constexpr auto value_or(U&& default_value) const& noexcept -> value_type {
    return this->has_value() ? this->storage().unsafe_value() : static_cast<value_type>(std::forward<U>(default_value));
  }

  /// @brief Returns the value if the value channel is populated, else another value. Not declared if @c T is @c void .
  /// @tparam U The type of the default value. Must satisfy @c std::is_convertible<U,value_type> .
  /// @param default_value The default value to use if the value channel is not populated.
  /// @return value_type If @c has_value() is true, equivalent to @c value() . Otherwise, @c default_value converted to
  ///         @c value_type .
  template <typename U = T, constraints<std::enable_if_t<std::is_convertible<U, value_type>::value>> = nullptr>
  ARENE_NODISCARD constexpr auto value_or(U&& default_value) && noexcept -> value_type {
    return this->has_value() ? std::move(this->storage().unsafe_value())
                             : static_cast<value_type>(std::forward<U>(default_value));
  }

  // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the value"
  /// @brief Accesses a reference to the content of the error channel.
  ///
  /// @pre has_error(), else ARENE_PRECONDITION violation
  /// @return A reference to the contained error
  ARENE_NODISCARD constexpr auto error() & noexcept -> error_type& {
    ARENE_PRECONDITION(this->has_error());
    return this->storage().unsafe_error();
  }
  // parasoft-end-suppress AUTOSAR-A9_3_1-a-2

  /// @brief Accesses a reference to the content of the error channel.
  ///
  /// @pre has_error(), else ARENE_PRECONDITION violation
  /// @return A const reference to the contained error
  ARENE_NODISCARD constexpr auto error() const& noexcept -> error_type const& {
    ARENE_PRECONDITION(this->has_error());
    return this->storage().unsafe_error();
  }
  // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the value"
  /// @brief Accesses a reference to the content of the error channel.
  ///
  /// @pre has_error(), else ARENE_PRECONDITION violation
  /// @return An rvalue reference to the contained error
  ARENE_NODISCARD constexpr auto error() && noexcept -> error_type&& {
    ARENE_PRECONDITION(this->has_error());
    return std::move(this->storage().unsafe_error());
  }
  // parasoft-end-suppress AUTOSAR-A9_3_1-a-2
  // parasoft-begin-suppress AUTOSAR-A18_9_3-a-2 "We want to ensure the qualifier is preserved"
  /// @brief Accesses a reference to the content of the error channel.
  ///
  /// @pre has_error(), else ARENE_PRECONDITION violation
  /// @return A const rvalue reference to the contained error
  ARENE_NODISCARD constexpr auto error() const&& noexcept -> error_type const&& {
    ARENE_PRECONDITION(this->has_error());
    return std::move(this->storage().unsafe_error());
  }
  // parasoft-end-suppress AUTOSAR-A18_9_3-a-2

  /// @brief Swaps the state of two results.
  ///
  /// @tparam SourceValueType Must satisfy @c is_swappable_v<SourceValueType> and  @c
  /// std::is_nothrow_move_constructible<SourceValueType>
  /// @tparam SourceErrorType Must satisfy @c is_swappable_v<SourceErrorType> and  @c
  /// std::is_nothrow_move_constructible<SourceErrorType>
  /// @param other The result to swap with
  /// @post If both @c this and @c other have matching channels populated, the content of those channels are swapped as
  ///       if via @c arene::base::swap . Otherwise, it is equivalent to emplacing @c this with the active channel
  ///       from @c other , and vice-versa.
  ///
  template <
      typename SourceValueType = T,
      typename SourceErrorType = E,
      constraints<
          std::enable_if_t<is_swappable_v<SourceValueType>>,
          std::enable_if_t<is_swappable_v<SourceErrorType>>,
          std::enable_if_t<std::is_nothrow_move_constructible<SourceValueType>::value>,
          std::enable_if_t<std::is_nothrow_move_constructible<SourceErrorType>::value>> = nullptr>
  constexpr void swap(result& other
  ) noexcept(is_nothrow_swappable_v<SourceValueType> && is_nothrow_swappable_v<SourceErrorType>) {
    if (this->has_value()) {
      if (other.has_value()) {
        ::arene::base::swap(this->storage().unsafe_value(), other.storage().unsafe_value());
      } else {
        error_type tmp{std::move(other.storage().unsafe_error())};
        static_cast<void>(other.emplace(in_place_value, std::move(this->storage().unsafe_value())));
        static_cast<void>(this->emplace(in_place_error, std::move(tmp)));
      }
    } else if (this->has_error()) {
      if (other.has_error()) {
        ::arene::base::swap(this->storage().unsafe_error(), other.storage().unsafe_error());
      } else {
        value_type tmp{std::move(other.storage().unsafe_value())};
        static_cast<void>(other.emplace(in_place_error, std::move(this->storage().unsafe_error())));
        static_cast<void>(this->emplace(in_place_value, std::move(tmp)));
      }
    } else {
      // do nothing
    }
  }

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief Swaps the state of two results.
  ///
  /// @tparam SourceValueType Must satisfy @c is_swappable_v<SourceValueType> ,  @c
  /// std::is_nothrow_move_constructible<SourceValueType>
  /// @tparam SourceErrorType Must satisfy @c is_swappable_v<SourceErrorType> ,  @c
  /// std::is_nothrow_move_constructible<SourceErrorType>
  /// @param lhs The left hand result to swap
  /// @param rhs The right hand result to swap
  /// @post If both @c lhs and @c rhs have matching channels populated, the content of those channels are swapped as
  ///       if via @c arene::base::swap . Otherwise, it is equivalent to emplacing @c lhs with the active channel
  ///       from @c rhs , and vise-versa.
  ///
  template <
      typename SourceValueType = T,
      typename SourceErrorType = E,
      constraints<
          std::enable_if_t<is_swappable_v<SourceValueType>>,
          std::enable_if_t<is_swappable_v<SourceErrorType>>,
          std::enable_if_t<std::is_nothrow_move_constructible<SourceValueType>::value>,
          std::enable_if_t<std::is_nothrow_move_constructible<SourceErrorType>::value>> = nullptr>
  friend constexpr void swap(result& lhs, result& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // imports emplace from result_base.
  using base_type::emplace;

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the value"
  /// @brief Emplaces a value into the result
  ///
  /// @tparam ArgTypes the types of the arguments to construct the value with
  /// @param args The arguments to construct the value with
  /// @return A reference to the newly emplaced value
  /// @post @c has_value() returns @c true
  /// @post The content of @c value() will be equivalent to a @c value_type direct-initialized from @c args .
  ///
  template <
      typename... ArgTypes,
      constraints<std::enable_if_t<std::is_nothrow_constructible<value_type, ArgTypes&&...>::value>> = nullptr>
  auto emplace(in_place_value_t, ArgTypes&&... args) noexcept -> value_type& {
    this->reset();
    this->discriminator() = result_detail::discriminator_type::holds_value;
    return *::arene::base::construct_at(
        std::addressof(this->storage().unsafe_value()),
        std::forward<ArgTypes>(args)...
    );
  }
  // parasoft-end-suppress AUTOSAR-A9_3_1-a-2
  // parasoft-end-suppress CERT_C-EXP37-a-3

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  /// @brief Monadic API which invokes a functor with the contents of @c value() if the value channel is populated.
  ///
  /// @tparam F Type of the visiting functor. Must be invocable with @c value_type matching the const/ref qualification
  ///         of @c SelfType, and return @c result<U,error_type> , where @c U may be any valid @c value_type .
  /// @param handle_value the functor to invoke if @c self.has_value() is @c true .
  /// @return result<U, error_type> which will be either the result of invoking @c handle_value with the contents of
  ///         @c self.value() , or it will contain a copy of @c self.error() .
  ///
  /// Basic usage example for opening a file:
  /// @snippet docs/examples/result_examples.cpp and_then
  ///
  /// After this statement, @c maybe_file will contain either a valid instance of @c arene::base::file_handle in the
  /// value channel, or it will contain an instance of @c arene::base::error_code representing either the failure to
  /// open the file or the failure to open the directory.
  ///
  /// @{
  template <typename F>
  constexpr auto and_then(F&& handle_value) & {
    return result::and_then(*this, std::forward<F>(handle_value));
  }
  template <typename F>
  constexpr auto and_then(F&& handle_value) const& {
    return result::and_then(*this, std::forward<F>(handle_value));
  }
  template <typename F>
  constexpr auto and_then(F&& handle_value) && {
    return result::and_then(std::move(*this), std::forward<F>(handle_value));
  }
  // parasoft-begin-suppress AUTOSAR-A18_9_3-a-2 "We want to ensure the qualifier is preserved"
  template <typename F>
  constexpr auto and_then(F&& handle_value) const&& {
    return result::and_then(std::move(*this), std::forward<F>(handle_value));
  }
  // parasoft-end-suppress AUTOSAR-A18_9_3-a-2
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  /// @brief Monadic API which invokes a functor with the contents of @c error() if the error channel is populated.
  ///
  /// @tparam F Type of the visiting functor. Must be invocable with @c error_type matching the const/ref qualification
  ///         of @c SelfType and return @c result<value_type,E> , where @c E may be any valid @c error_type .
  /// @param handle_error the functor to invoke if @c self.has_error() is @c true .
  /// @return result<value_type, E> which will be either the result of invoking @c handle_error with the contents of
  ///         @c self.error() , or it will contain a copy of @c self.value() .
  ///
  /// Basic usage example for opening a directory:
  /// @snippet docs/examples/result_examples.cpp or_else
  ///
  /// @{
  template <typename F>
  constexpr auto or_else(F&& handle_error) & {
    return result::or_else(*this, std::forward<F>(handle_error));
  }
  template <typename F>
  constexpr auto or_else(F&& handle_error) const& {
    return result::or_else(*this, std::forward<F>(handle_error));
  }
  template <typename F>
  constexpr auto or_else(F&& handle_error) && {
    return result::or_else(std::move(*this), std::forward<F>(handle_error));
  }
  // parasoft-begin-suppress AUTOSAR-A18_9_3-a-2 "We want to ensure the qualifier is preserved"
  template <typename F>
  constexpr auto or_else(F&& handle_error) const&& {
    return result::or_else(std::move(*this), std::forward<F>(handle_error));
  }
  // parasoft-end-suppress AUTOSAR-A18_9_3-a-2
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: it is not hiding anything"
  /// @brief Calls a provided functor with the value channel if it is populated, and produces a new result with the
  ///        return from it.
  /// @tparam F Type of the visiting functor. Must be invocable with @c value_type matching the const/ref qualification
  ///         of @c this . May return any valid @c value_type
  /// @param handle_value the functor to invoke if @c has_value() is @c true .
  /// @return result<U, error_type> which will be one of:
  ///         * A @c result<U,error_type> constructed from the result of invoking @c handle_value with the contents of
  ///           @c value() if @c value_type is not @c void and the return from @c F is not @c void
  ///         * A @c result<void,error_type> constructed from the result of invoking @c handle_value with the contents
  ///           of @c value() if @c value_type is not @c void and the return from @c F is @c void
  ///         * A @c result<U,error_type> where @c U is the return type of @c F constructed from a copy of @c error() .
  /// @{
  template <typename F>
  constexpr auto transform(F&& handle_value) & {
    return result::transform(*this, std::forward<F>(handle_value));
  }
  template <typename F>
  constexpr auto transform(F&& handle_value) const& {
    return result::transform(*this, std::forward<F>(handle_value));
  }
  template <typename F>
  constexpr auto transform(F&& handle_value) && {
    return result::transform(std::move(*this), std::forward<F>(handle_value));
  }
  // parasoft-begin-suppress AUTOSAR-A18_9_3-a-2 "We want to ensure the qualifier is preserved"
  template <typename F>
  constexpr auto transform(F&& handle_value) const&& {
    return result::transform(std::move(*this), std::forward<F>(handle_value));
  }
  // parasoft-end-suppress AUTOSAR-A18_9_3-a-2
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_10_1-d
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  /// @brief Calls a provided functor with the error channel if it is populated, and produces a new result with the
  ///        return from it.
  /// @tparam F Type of the visiting functor. Must be invocable with @c error_type matching the const/ref qualification
  ///         of @c this. May return any valid @c error_type .
  /// @param handle_error the functor to invoke if @c self.has_error() is @c true .
  /// @return result<value_type, E> which will be either:
  ///         * A result constructed from the result of invoking @c handle_error with the contents of @c self.error() ,
  ///         * A result constructed from a copy of @c self.value() .
  /// @{
  template <typename F>
  constexpr auto transform_error(F&& handle_error) & {
    return result::transform_error(*this, std::forward<F>(handle_error));
  }
  template <typename F>
  constexpr auto transform_error(F&& handle_error) const& {
    return result::transform_error(*this, std::forward<F>(handle_error));
  }
  template <typename F>
  constexpr auto transform_error(F&& handle_error) && {
    return result::transform_error(std::move(*this), std::forward<F>(handle_error));
  }
  // parasoft-begin-suppress AUTOSAR-A18_9_3-a-2 "We want to ensure the qualifier is preserved"
  template <typename F>
  constexpr auto transform_error(F&& handle_error) const&& {
    return result::transform_error(std::move(*this), std::forward<F>(handle_error));
  }
  // parasoft-end-suppress AUTOSAR-A18_9_3-a-2
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function used in other TUs"
  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Conditionally noexcept based on underlying comparison"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: comparison operators are permitted"
  /// @brief Compare two results for equality.
  /// @param lhs The first result
  /// @param rhs The second result
  /// @return bool @c true if the both @c lhs and @c rhs hold values that are equal, or both hold errors that are
  ///          equal, @c false otherwise
  ARENE_NODISCARD friend constexpr auto operator==(result const& lhs, result const& rhs) noexcept(
      noexcept(lhs.has_value(rhs.value())) && noexcept(lhs.has_error(rhs.error()))
  ) -> bool {
    static_assert(is_equality_comparable_v<value_type>, "Value type must be equality comparable");
    static_assert(is_equality_comparable_v<error_type>, "Error type must be equality comparable");
    if (lhs.has_value()) {
      return rhs.has_value(lhs.storage().unsafe_value());
    }
    return rhs.has_error(lhs.storage().unsafe_error());
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function used in other TUs"
  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Conditionally noexcept based on underlying comparison"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: comparison operators are permitted"
  /// @brief Compare two results for inequality.
  /// @param lhs The first result
  /// @param rhs The second result
  /// @return bool @c true if the both @c lhs and @c rhs hold values that are not equal, or both hold errors that are
  ///          not equal, or one holds a value and the other holds an error, @c false otherwise
  ARENE_NODISCARD friend constexpr auto operator!=(result const& lhs, result const& rhs) noexcept(noexcept(lhs == rhs))
      -> bool {
    return !(lhs == rhs);
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

 private:
  /// @brief Helper for implementing assignment which handles copy/move.
  /// @tparam OtherType the result type to assign from. @c OtherType::value_type and @c OtherType::error_type must be
  /// nothrow constructible and nothrow assignable to @c value_type and @c error_type  respectably.
  /// @param other the result to assign from.
  /// @post The state of @c this will be as follows:
  ///       * If both @c this and @c other 's value channels are populated, then @c this will have its value channel's
  ///         state set as if via assignment from @c other 's value channel.
  ///       * If both @c this and @c other 's error channels are populated, then @c this will have its error channel's
  ///         state set as if via assignment from @c other 's error channel.
  ///       * Otherwise, @c this will have its state set as if @c emplace 'd with the active channel of @c other .
  /// @return result& A reference to @c this post assignment.
  template <typename OtherType, constraints<std::enable_if_t<decays_to_v<OtherType, result>>> = nullptr>
  constexpr auto assign(OtherType&& other) noexcept -> result& {
    if (this->has_value()) {
      if (other.has_value()) {
        this->storage().unsafe_value() =
            forward_like<OtherType>(std::forward<OtherType>(other).storage().unsafe_value());
        return *this;
      }
      std::ignore = this->emplace(
          in_place_error,
          forward_like<OtherType>(std::forward<OtherType>(other).storage().unsafe_error())
      );
      return *this;
    }
    if (other.has_error()) {
      this->storage().unsafe_error() = forward_like<OtherType>(std::forward<OtherType>(other).storage().unsafe_error());
      return *this;
    }
    std::ignore =
        this->emplace(in_place_value, forward_like<OtherType>(std::forward<OtherType>(other).storage().unsafe_value()));
    return *this;
  }

  ///
  /// @brief deduced-this helper for @c result::and_then<F>(F&&) .
  /// @tparam SelfType The qualified result type to invoke with. Must satisfy @c decays_to<SelfType,result>
  /// @tparam F Type of the visiting functor. Must be invocable with @c value_type matching the const/ref qualification
  ///         of @c SelfType , and return @c result<U,error_type> , where @c U may be any valid @c value_type .
  /// @param self the instance of the result to use.
  /// @param handle_value the functor to invoke if @c self.has_value() is @c true .
  /// @return result<U, error_type> which will be either the result of invoking @c handle_value with the contents of
  ///         @c self.value() , or it will contain a copy of @c self.error() .
  template <typename SelfType, typename F, constraints<std::enable_if<decays_to_v<SelfType, result>>> = nullptr>
  static constexpr auto and_then(SelfType&& self, F&& handle_value) {
    using f_result_t =
        invoke_result_t<F, decltype(forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_value()))>;
    static_assert(is_instantiation_of_v<f_result_t, ::arene::base::result>, "handle_value must return a result!");
    static_assert(
        std::is_same<typename f_result_t::error_type, typename remove_cvref_t<SelfType>::error_type>::value,
        "The result returned from handle_value's error channel must be the same as this result's error channel"
    );
    if (self.has_value()) {
      return ::arene::base::invoke(
          std::forward<F>(handle_value),
          forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_value())
      );
    }
    return f_result_t{in_place_error, forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_error())};
  }

  ///
  /// @brief deduced-this helper for @c result::or_else<F>(F&&) .
  /// @tparam SelfType The qualified result type to invoke with. Must satisfy @c decays_to<SelfType,result>
  /// @tparam F Type of the visiting functor. Must be invocable with @c error_type matching the const/ref qualification
  ///         of @c SelfType and return @c result<value_type,E> , where @c E may be any valid @c error_type .
  /// @param self the instance of the result to use.
  /// @param handle_error the functor to invoke if @c self.has_error() is @c true .
  /// @return result<value_type, E> which will be either the result of invoking @c handle_error with the contents of
  ///         @c self.error() , or it will contain a copy of @c self.value() .
  template <typename SelfType, typename F, constraints<std::enable_if<decays_to_v<SelfType, result>>> = nullptr>
  static constexpr auto or_else(SelfType&& self, F&& handle_error) {
    using f_result_t =
        invoke_result_t<F, decltype(forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_error()))>;
    static_assert(is_instantiation_of_v<f_result_t, ::arene::base::result>, "handle_error must return a result!");
    static_assert(
        std::is_same<typename f_result_t::value_type, value_type>::value,
        "The result returned from handle_error's value channel must be the same as this result's value channel"
    );
    if (self.has_error()) {
      return ::arene::base::invoke(
          std::forward<F>(handle_error),
          forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_error())
      );
    }
    return f_result_t{in_place_value, forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_value())};
  }

  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: it isn't hiding anything."
  /// @brief deduced-this helper for @c result::transform<F>(F&&) .
  /// @tparam SelfType The qualified result type to invoke with. Must satisfy @c decays_to<SelfType,result>
  /// @tparam F Type of the visiting functor. Must be invocable with @c value_type matching the const/ref qualification
  ///         of @c SelfType. May return any valid @c value_type
  /// @param self the instance of the result to use.
  /// @param handle_value the functor to invoke if @c self.has_value() is @c true .
  /// @return result<U, error_type> which will be one of:
  ///         * A @c result<U,error_type> constructed from the result of invoking @c handle_value with the contents of
  ///           @c self.value()
  ///         * A @c result<U,error_type> where @c U is the return type of @c F constructed from a copy of @c error() .
  template <
      typename SelfType,
      typename F,
      constraints<
          std::enable_if<decays_to_v<SelfType, result>>,
          std::enable_if_t<!std::is_void<
              invoke_result_t<F, decltype(forward_like<SelfType>(std::declval<SelfType>().storage().unsafe_value()))>>::
                               value>> = nullptr>
  static constexpr auto transform(SelfType&& self, F&& handle_value) {
    using f_value_t =
        invoke_result_t<F, decltype(forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_value()))>;
    using result_t = result<f_value_t, error_type>;
    if (self.has_value()) {
      return result_t{
          in_place_value,
          ::arene::base::invoke(
              std::forward<F>(handle_value),
              forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_value())
          )
      };
    }
    return result_t{in_place_error, forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_error())};
  }
  /// @brief deduced-this helper for @c result::transform<F>(F&&) .
  /// @tparam SelfType The qualified result type to invoke with. Must satisfy @c decays_to<SelfType,result>
  /// @tparam F Type of the visiting functor. Must be invocable with @c value_type matching the const/ref qualification
  ///         of @c SelfType. May return any valid @c value_type
  /// @param self the instance of the result to use.
  /// @param handle_value the functor to invoke if @c self.has_value() is @c true .
  /// @return result<void, error_type> which will be one of:
  ///         * A @c result<void,error_type> constructed from the result of invoking @c handle_value with the contents
  ///           of @c self.value()
  ///         * A @c result<void,error_type> constructed from a copy of @c error() .
  template <
      typename SelfType,
      typename F,
      constraints<
          std::enable_if<decays_to_v<SelfType, result>>,
          std::enable_if_t<std::is_void<
              invoke_result_t<F, decltype(forward_like<SelfType>(std::declval<SelfType>().storage().unsafe_value()))>>::
                               value>> = nullptr>
  static constexpr auto transform(SelfType&& self, F&& handle_value) {
    using result_t = result<void, error_type>;
    if (self.has_value()) {
      ::arene::base::invoke(
          std::forward<F>(handle_value),
          forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_value())
      );
      return result_t{};
    }
    return result_t{in_place_error, forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_error())};
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  ///
  /// @brief deduced-this helper for @c result::transform_error<F>(F&&) .
  /// @tparam SelfType The qualified result type to invoke with. Must satisfy @c decays_to<SelfType,result>
  /// @tparam F Type of the visiting functor. Must be invocable with @c error_type matching the const/ref qualification
  ///         of @c SelfType. May return any valid @c error_type .
  /// @param self the instance of the result to use.
  /// @param handle_error the functor to invoke if @c self.has_error() is @c true .
  /// @return result<value_type, E> which will be either:
  ///         * A result constructed from the result of invoking @c handle_error with the contents of @c self.error() ,
  ///         * A result constructed from a copy of @c self.value() .
  template <typename SelfType, typename F, constraints<std::enable_if<decays_to_v<SelfType, result>>> = nullptr>
  static constexpr auto transform_error(SelfType&& self, F&& handle_error) {
    using f_error_t =
        invoke_result_t<F, decltype(forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_error()))>;
    using result_t = result<value_type, f_error_t>;
    if (self.has_error()) {
      return result_t{
          in_place_error,
          ::arene::base::invoke(
              std::forward<F>(handle_error),
              forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_error())
          )
      };
    }
    return result_t{in_place_value, forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_value())};
  }
};  // result<T,E>
// parasoft-end-suppress AUTOSAR-A12_0_1-a-2
// parasoft-end-suppress AUTOSAR-M14_5_3-a-2
// parasoft-end-suppress AUTOSAR-A1_1_1-b-2
// parasoft-end-suppress AUTOSAR-A10_2_1-b-2

// parasoft-begin-suppress AUTOSAR-A10_2_1-b-2 "False Positive: Base class function made visible with 'using'"
// parasoft-begin-suppress AUTOSAR-A1_1_1-b-2 "False Positive: Copy constructor is defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-M14_5_3-a-2 "False Positive: Copy assignment is defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A12_0_1-a-2 "False Positive: Copy assignment is defined or deleted as appropriate"
/// @brief Specialization of result<T, E> for void value types.
/// @tparam E The error type
template <class E>
// NOLINTNEXTLINE(hicpp-special-member-functions) false-positive, it can't see through the non-constructible-dummy trick
class result<void, E> : result_detail::result_base<monostate, E> {
  /// @brief The type of the base class holding the storage
  using base_type = result_detail::result_base<monostate, E>;

 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d-2 "False Positive: This identifier does not hide anything"
  /// @brief The type when the @c result contains a value.
  using value_type = void;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d-2

  /// @brief The type when the @c result contains an error.
  using error_type = E;

 private:
  static_assert(
      std::is_same<remove_cvref_t<value_type>, value_type>::value,
      "value_type must be an unqualified type, non-reference type."
  );
  static_assert(
      std::is_same<remove_cvref_t<error_type>, error_type>::value,
      "error_type must be an unqualified type, non-reference type."
  );

  static_assert(!std::is_same<error_type, void>::value, "void as an error_type is not supported");
  static_assert(
      !std::is_same<remove_cvref_t<error_type>, monostate>::value,
      "monostate as an error_type is not supported"
  );

  static_assert(std::is_object<error_type>::value, "error_type must be an object type (it must not be a reference).");

 public:
  ///
  /// @brief Constructs a @c result with the value-channel engaged.
  /// @post @c has_value() returns @c true
  constexpr result() = default;

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Constructs a @c result with the value-channel engaged.
  ///
  /// @tparam P The type of the initializers. Must satisfy @c std::is_constructible<value_type,P&&...> .
  /// @post @c has_value() returns @c true
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr result(in_place_value_t) noexcept(noexcept(base_type(in_place_value, monostate{})))
      : base_type(in_place_value, monostate{}) {}
  /// @brief Constructs a @c result with the value-channel engaged. Provided for backwards compatibility.
  ///
  /// @tparam P The type of the initializers. Must satisfy @c std::is_constructible<value_type,P&&...> .
  /// @post @c has_value() returns @c true
  constexpr result(in_place_value_t, monostate) noexcept(noexcept(base_type(in_place_value, monostate{})))
      : base_type(in_place_value, monostate{}) {}
  // parasoft-end-suppress CERT_C-EXP37-a-3

  ///
  /// @brief Constructs a @c result containing the @c error_type by way of direct-non-list-initialization.
  ///
  /// @tparam P The type of the initializers. Must satisfy @c std::is_constructible<error_type,P&&...> .
  /// @param args The arguments to initialize the value channel with.
  /// @post @c has_error() returns @c true
  /// @post @c error() returns reference to an object equivalent to constructing @c error_type with @c args .
  ///
  /// Example Usage:
  /// @snippet docs/examples/result_examples.cpp in_place_error_void
  template <class... P, constraints<std::enable_if_t<std::is_constructible<error_type, P&&...>::value>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr result(in_place_error_t, P&&... args) noexcept(noexcept(base_type(in_place_error, std::forward<P>(args)...))
  )
      : base_type(in_place_error, std::forward<P>(args)...) {}

  /// @brief Default copy constructor
  constexpr result(result const& copy) noexcept(noexcept(base_type(std::declval<result const&>()))) = default;
  /// @brief Default move constructor
  // NOLINTNEXTLINE(hicpp-noexcept-move) We want to exactly mimic the noexcept properties of ValueType and ErrorType.
  constexpr result(result&& move) noexcept(noexcept(base_type(std::declval<result&&>()))) = default;
  /// @brief Destroy the stored value or error
  ~result() = default;

 private:
  /// @brief trait which detects if the result should be copy-assignable
  static constexpr bool is_copy_assignable{
      std::is_nothrow_copy_constructible<error_type>::value && std::is_nothrow_copy_assignable<error_type>::value
  };

  /// @brief The argument for the "copy assign" operator: @c result if the assignment is possible, @c
  /// non_constructible_dummy if not
  using copy_assign_arg = conditional_t<is_copy_assignable, result, non_constructible_dummy>;
  /// @brief The argument for the *deleted* "copy assign" operator: @c non_constructible_dummy if the assignment is
  /// possible, @c result if not (so the copy assignment is deleted)
  using deleted_copy_assign_arg = conditional_t<is_copy_assignable, non_constructible_dummy, result>;

 public:
  /// @brief Copy-assignment for the case that @c error_type satisfies @c std::is_nothrow_copy_constructible and @c
  /// std::is_nothrow_copy_assignable, otherwise a dummy assignment operator that can never be called
  /// @param other The value to copy from
  /// @return *this
  constexpr auto operator=(copy_assign_arg const& other) noexcept -> result& {
    if (&other != this) {
      std::ignore = assign(other);
    }
    return *this;
  }
  /// @brief Deleted Copy-assignment if @c error_type does not satisfy @c std::is_nothrow_copy_constructible and @c
  /// std::is_nothrow_copy_assignable, otherwise a deleted dummy assignment operator
  /// @param other The value to copy from
  /// @return *this
  constexpr auto operator=(deleted_copy_assign_arg const& other) noexcept -> result& = delete;

 private:
  /// @brief trait to detect if the result should be move-assignable
  static constexpr bool is_move_assignable{
      std::is_nothrow_move_constructible<error_type>::value && std::is_nothrow_move_assignable<error_type>::value
  };

  /// @brief The argument for the "move assign" operator: @c result if the assignment is possible, @c
  /// non_constructible_dummy if not
  using move_assign_arg = conditional_t<is_move_assignable, result, non_constructible_dummy>;
  /// @brief The argument for the *deleted* "move assign" operator: @c non_constructible_dummy if the assignment is
  /// possible, @c result if not (so the move assignment is deleted)
  using deleted_move_assign_arg = conditional_t<is_move_assignable, non_constructible_dummy, result>;

 public:
  /// @brief Move-assignment for the case that @c error_type satisfies @c std::is_nothrow_move_constructible and @c
  /// std::is_nothrow_move_assignable, otherwise a dummy assignment operator that can never be called
  /// @param other The value to move from
  /// @return *this
  constexpr auto operator=(move_assign_arg&& other) noexcept -> result& {
    std::ignore = assign(std::move(other));
    return *this;
  }
  /// @brief Deleted Move-assignment if @c error_type does not satisfy @c std::is_nothrow_move_constructible and @c
  /// std::is_nothrow_move_assignable, otherwise a deleted dummy assignment operator
  /// @param other The value to move from
  /// @return *this
  constexpr auto operator=(deleted_move_assign_arg&& other) noexcept -> result& = delete;

  // @brief Use @c has_value from base type
  using base_type::has_value;

  // @brief Use @c has_value from base type
  using base_type::has_error;

  // @brief Use boolean conversion from base type
  using base_type::operator bool;

  /// @brief Access the content of the value channel.
  ///
  /// @pre has_value(), else ARENE_PRECONDITION violation.
  constexpr void operator*() const& noexcept { return value(); }

  /// @brief Access the content of the value channel.
  ///
  /// @pre has_value(), else ARENE_PRECONDITION violation.
  constexpr void value() const& noexcept { ARENE_PRECONDITION(this->has_value()); }
  /// @brief Access the content of the value channel.
  ///
  /// @pre has_value(), else ARENE_PRECONDITION violation.
  constexpr void value() && noexcept { ARENE_PRECONDITION(this->has_value()); }

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  /// @brief Accesses a reference to the content of the error channel.
  ///
  /// @pre has_error(), else ARENE_PRECONDITION violation
  /// @return A reference to the contained error
  /// @{
  // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the value"
  ARENE_NODISCARD constexpr auto error() & noexcept -> error_type& {
    ARENE_PRECONDITION(this->has_error());
    return this->storage().unsafe_error();
  }
  // parasoft-end-suppress AUTOSAR-A9_3_1-a-2
  ARENE_NODISCARD constexpr auto error() const& noexcept -> error_type const& {
    ARENE_PRECONDITION(this->has_error());
    return this->storage().unsafe_error();
  }
  // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the value"
  ARENE_NODISCARD constexpr auto error() && noexcept -> error_type&& {
    ARENE_PRECONDITION(this->has_error());
    return std::move(this->storage().unsafe_error());
  }
  // parasoft-end-suppress AUTOSAR-A9_3_1-a-2
  // parasoft-begin-suppress AUTOSAR-A18_9_3-a-2 "We want to ensure the qualifier is preserved"
  ARENE_NODISCARD constexpr auto error() const&& noexcept -> error_type const&& {
    ARENE_PRECONDITION(this->has_error());
    return std::move(this->storage().unsafe_error());
  }
  // parasoft-end-suppress AUTOSAR-A18_9_3-a-2
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2

  ///
  /// @brief Swaps the state of two results.
  ///
  /// @tparam SourceErrorType Must satisfy @c is_swappable_v<error_type> and @c
  /// std::is_nothrow_move_constructable<error_type>
  /// @param other The result to swap with
  /// @post If both @c this and @c other have matching channels populated, the content of those channels are swapped as
  ///       if via @c arene::base::swap . Otherwise, it is equivalent to emplacing @c this with the active channel
  ///       from @c other , and vice-versa.
  ///
  template <
      typename SourceErrorType = E,
      constraints<
          std::enable_if_t<is_swappable_v<SourceErrorType>>,
          std::enable_if_t<std::is_nothrow_move_constructible<SourceErrorType>::value>> = nullptr>
  constexpr void swap(result& other) noexcept(is_nothrow_swappable_v<SourceErrorType>) {
    if (this->has_value()) {
      if (other.has_value()) {
        // swapping voids isn't meaningful
        return;
      }
      static_cast<void>(this->emplace(in_place_error, std::move(other.storage().unsafe_error())));
      other.emplace(in_place_value);
    } else if (this->has_error()) {
      if (other.has_error()) {
        ::arene::base::swap(this->storage().unsafe_error(), other.storage().unsafe_error());
      } else {
        static_cast<void>(other.emplace(in_place_error, std::move(this->storage().unsafe_error())));
        this->emplace(in_place_value);
      }
    } else {
      // nothing to do
    }
  }

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief Swaps the state of two results.
  ///
  /// @tparam SourceErrorType Must satisfy @c is_swappable_v<error_type> and @c
  /// std::is_nothrow_move_constructable<error_type>
  /// @param lhs The left hand result to swap
  /// @param rhs The right hand result to swap
  /// @post If both @c lhs and @c rhs have matching channels populated, the content of those channels are swapped as
  ///       if via @c arene::base::swap . Otherwise, it is equivalent to move-assigning @c lhs with the active channel
  ///       from @c rhs , and vise-versa.
  ///
  template <
      typename SourceErrorType = E,
      constraints<
          std::enable_if_t<is_swappable_v<SourceErrorType>>,
          std::enable_if_t<std::is_move_constructible<SourceErrorType>::value>> = nullptr>
  friend constexpr void swap(result& lhs, result& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // imports emplace from result_base.
  using base_type::emplace;

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  ///
  /// @brief Emplaces the void value into the result
  /// @post @c has_value() returns @c true .
  ///
  void emplace(in_place_value_t) noexcept {
    this->reset();
    this->discriminator() = result_detail::discriminator_type::holds_value;
    ::arene::base::construct_at(std::addressof(this->storage().unsafe_value()), monostate{});
  }
  // parasoft-end-suppress CERT_C-EXP37-a-3

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  /// @brief Monadic API which invokes a functor with the contents of @c value() if the value channel is populated.
  ///
  /// @tparam F Type of the visiting functor. Must be invocable with @c value_type matching the const/ref qualification
  ///         of @c SelfType , or with no arguments if @c value_type is @c void , and return @c result<U,error_type> ,
  ///         where
  ///         @c U may be any valid @c value_type .
  /// @param handle_value the functor to invoke if @c self.has_value() is @c true .
  /// @return result<U, error_type> which will be either the result of invoking @c handle_value with the contents of
  ///         @c self.value() , or it will contain a copy of @c self.error() .
  ///
  /// Basic usage example for opening a file:
  /// @snippet docs/examples/result_examples.cpp and_then
  ///
  /// After this statement, @c maybe_file will contain either a valid instance of @c arene::base::file_handle in the
  /// value channel, or it will contain an instance of @c arene::base::error_code representing either the failure to
  /// open the file or the failure to open the directory.
  ///
  /// @{
  template <typename F>
  constexpr auto and_then(F&& handle_value) & {
    return result::and_then(*this, std::forward<F>(handle_value));
  }
  template <typename F>
  constexpr auto and_then(F&& handle_value) const& {
    return result::and_then(*this, std::forward<F>(handle_value));
  }
  template <typename F>
  constexpr auto and_then(F&& handle_value) && {
    return result::and_then(std::move(*this), std::forward<F>(handle_value));
  }
  template <typename F>
  constexpr auto and_then(F&& handle_value) const&& {
    return result::and_then(std::move(*this), std::forward<F>(handle_value));
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  /// @brief Monadic API which invokes a functor with the contents of @c error() if the error channel is populated.
  ///
  /// @tparam F Type of the visiting functor. Must be invocable with @c error_type matching the const/ref qualification
  ///         of @c SelfType and return @c result<value_type,E> , where @c E may be any valid @c error_type .
  /// @param handle_error the functor to invoke if @c self.has_error() is @c true .
  /// @return result<value_type, E> which will be either the result of invoking @c handle_error with the contents of
  ///         @c self.error() , or it will contain a copy of @c self.value() .
  ///
  /// Basic usage example for opening a directory:
  /// @snippet docs/examples/result_examples.cpp or_else
  ///
  /// After this statement, @c maybe_dir will contain either a valid instance of @c arene::base::directory_handle in
  /// the value channel, or it will have logged the error and contain the passed through @c error_code .
  ///
  /// @{
  template <typename F>
  constexpr auto or_else(F&& handle_error) & {
    return result::or_else(*this, std::forward<F>(handle_error));
  }
  template <typename F>
  constexpr auto or_else(F&& handle_error) const& {
    return result::or_else(*this, std::forward<F>(handle_error));
  }
  template <typename F>
  constexpr auto or_else(F&& handle_error) && {
    return result::or_else(std::move(*this), std::forward<F>(handle_error));
  }
  template <typename F>
  constexpr auto or_else(F&& handle_error) const&& {
    return result::or_else(std::move(*this), std::forward<F>(handle_error));
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: it is not hiding anything"
  /// @brief Calls a provided functor with the value channel if it is populated, and produces a new result with the
  ///        return from it.
  /// @tparam F Type of the visiting functor. Must be invocable with @c value_type matching the const/ref qualification
  ///         of @c this , or with no arguments if @c value_type is @c void . May return any valid @c value_type
  /// @param handle_value the functor to invoke if @c has_value() is @c true .
  /// @return result<U, error_type> which will be one of:
  ///         * A @c result<U,error_type> constructed from the result of invoking @c handle_value if the return from
  ///           @c F is not @c void
  ///         * A @c result<void,error_type> constructed from the result of invoking @c handle_value if the return from
  ///           @c F is @c void
  ///         * A @c result<U,error_type> where @c U is the return type of @c F constructed from a copy of
  ///           @c self.error() .
  /// @{
  template <typename F>
  constexpr auto transform(F&& handle_value) & {
    return result::transform(*this, std::forward<F>(handle_value));
  }
  template <typename F>
  constexpr auto transform(F&& handle_value) const& {
    return result::transform(*this, std::forward<F>(handle_value));
  }
  template <typename F>
  constexpr auto transform(F&& handle_value) && {
    return result::transform(std::move(*this), std::forward<F>(handle_value));
  }
  // parasoft-begin-suppress AUTOSAR-A18_9_3-a-2 "We want to ensure the qualifier is preserved"
  template <typename F>
  constexpr auto transform(F&& handle_value) const&& {
    return result::transform(std::move(*this), std::forward<F>(handle_value));
  }
  // parasoft-end-suppress AUTOSAR-A18_9_3-a-2
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  /// @brief Calls a provided functor with the error channel if it is populated, and produces a new result with the
  ///        return from it.
  /// @tparam F Type of the visiting functor. Must be invocable with @c error_type matching the const/ref qualification
  ///         of @c this. May return any valid @c error_type .
  /// @param handle_error the functor to invoke if @c self.has_error() is @c true .
  /// @return result<value_type, E> which will be either:
  ///         * A result constructed from the result of invoking @c handle_error with the contents of @c self.error() ,
  ///         * A result constructed from a copy of @c self.value() .
  /// Basic usage example for opening a directory:
  /// @snippet docs/examples/result_examples.cpp transform_error
  ///
  /// After this statement, @c maybe_dir will contain either a valid instance of @c arene::base::directory_handle in
  /// the value channel, or it will have logged the error and contain the passed through @c error_code .
  template <typename F>
  constexpr auto transform_error(F&& handle_error) & {
    return result::transform_error(*this, std::forward<F>(handle_error));
  }
  /// @brief Calls a provided functor with the error channel if it is populated, and produces a new result with the
  ///        return from it.
  /// @tparam F Type of the visiting functor. Must be invocable with @c error_type matching the const/ref qualification
  ///         of @c this. May return any valid @c error_type .
  /// @param handle_error the functor to invoke if @c self.has_error() is @c true .
  /// @return result<value_type, E> which will be either:
  ///         * A result constructed from the result of invoking @c handle_error with the contents of @c self.error() ,
  ///         * A result constructed from a copy of @c self.value() .
  /// Basic usage example for opening a directory:
  /// @snippet docs/examples/result_examples.cpp transform_error
  ///
  /// After this statement, @c maybe_dir will contain either a valid instance of @c arene::base::directory_handle in
  /// the value channel, or it will have logged the error and contain the passed through @c error_code .
  template <typename F>
  constexpr auto transform_error(F&& handle_error) const& {
    return result::transform_error(*this, std::forward<F>(handle_error));
  }
  /// @brief Calls a provided functor with the error channel if it is populated, and produces a new result with the
  ///        return from it.
  /// @tparam F Type of the visiting functor. Must be invocable with @c error_type matching the const/ref qualification
  ///         of @c this. May return any valid @c error_type .
  /// @param handle_error the functor to invoke if @c self.has_error() is @c true .
  /// @return result<value_type, E> which will be either:
  ///         * A result constructed from the result of invoking @c handle_error with the contents of @c self.error() ,
  ///         * A result constructed from a copy of @c self.value() .
  /// Basic usage example for opening a directory:
  /// @snippet docs/examples/result_examples.cpp transform_error
  ///
  /// After this statement, @c maybe_dir will contain either a valid instance of @c arene::base::directory_handle in
  /// the value channel, or it will have logged the error and contain the passed through @c error_code .
  template <typename F>
  constexpr auto transform_error(F&& handle_error) && {
    return result::transform_error(std::move(*this), std::forward<F>(handle_error));
  }
  /// @brief Calls a provided functor with the error channel if it is populated, and produces a new result with the
  ///        return from it.
  /// @tparam F Type of the visiting functor. Must be invocable with @c error_type matching the const/ref qualification
  ///         of @c this. May return any valid @c error_type .
  /// @param handle_error the functor to invoke if @c self.has_error() is @c true .
  /// @return result<value_type, E> which will be either:
  ///         * A result constructed from the result of invoking @c handle_error with the contents of @c self.error() ,
  ///         * A result constructed from a copy of @c self.value() .
  /// Basic usage example for opening a directory:
  /// @snippet docs/examples/result_examples.cpp transform_error
  ///
  /// After this statement, @c maybe_dir will contain either a valid instance of @c arene::base::directory_handle in
  /// the value channel, or it will have logged the error and contain the passed through @c error_code .
  template <typename F>
  constexpr auto transform_error(F&& handle_error) const&& {
    return result::transform_error(std::move(*this), std::forward<F>(handle_error));
  }

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function used in other TUs"
  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Conditionally noexcept based on underlying comparison"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: comparison operators are permitted"
  /// @brief Compare two results for equality.
  /// @param lhs The first result
  /// @param rhs The second result
  /// @return bool @c true if the both @c lhs and @c rhs hold values that are equal, or both hold errors that are
  ///          equal, @c false otherwise
  ARENE_NODISCARD friend constexpr auto operator==(result const& lhs, result const& rhs) noexcept(
      noexcept(lhs.has_error(rhs.error()))
  ) -> bool {
    static_assert(is_equality_comparable_v<error_type>, "Error type must be equality comparable");
    if (lhs.has_value()) {
      return rhs.has_value();
    }
    return rhs.has_error(lhs.storage().unsafe_error());
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function used in other TUs"
  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Conditionally noexcept based on underlying comparison"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: comparison operators are permitted"
  /// @brief Compare two results for inequality.
  /// @param lhs The first result
  /// @param rhs The second result
  /// @return bool @c true if the both @c lhs and @c rhs hold values that are not equal, or both hold errors that are
  ///          not equal, or one holds a value and the other holds an error, @c false otherwise
  ARENE_NODISCARD friend constexpr auto operator!=(result const& lhs, result const& rhs) noexcept(noexcept(lhs == rhs))
      -> bool {
    return !(lhs == rhs);
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

 private:
  /// @brief Helper for implementing assignment which handles copy/move.
  /// @tparam OtherType the result type to assign from. @c OtherType::error_type must be nothrow constructible and
  /// nothrow
  ///         assignable to @c error_type  respectably.
  /// @param other the result to assign from.
  /// @post The state of @c this will be as follows:
  ///       * If both @c this and @c other 's value channels are populated, it is a noop.
  ///       * If both @c this and @c other 's error channels are populated, then @c this will have its error channel's
  ///         state set as if via assignment from @c other 's error channel.
  ///       * OtherTypewise, @c this will have its state set as if @c emplace 'd with the active channel of @c other .
  /// @return result& A reference to @c this post assignment.
  template <typename OtherType, constraints<std::enable_if_t<decays_to_v<OtherType, result>>> = nullptr>
  constexpr auto assign(OtherType&& other) noexcept -> result& {
    if (this->has_value()) {
      if (other.has_value()) {
        // assignment from two void values isn't meaningful, so nothing to do.
        return *this;
      }
      std::ignore = this->emplace(
          in_place_error,
          forward_like<OtherType>(std::forward<OtherType>(other).storage().unsafe_error())
      );
      return *this;
    }
    if (other.has_error()) {
      this->storage().unsafe_error() = forward_like<OtherType>(std::forward<OtherType>(other).storage().unsafe_error());
      return *this;
    }
    // we switch ourselves to void.
    this->emplace(in_place_value);
    return *this;
  }

  ///
  /// @brief deduced-this helper for @c result::and_then<F>(F&&) .
  /// @tparam SelfType The qualified result type to invoke with. Must satisfy @c decays_to<SelfType,result>
  /// @tparam F Type of the visiting functor. Must be invocable no arguments, and return @c result<U,error_type> , where
  ///         @c U may be any valid @c value_type .
  /// @param self the instance of the result to use.
  /// @param handle_value the functor to invoke if @c self.has_value() is @c true .
  /// @return result<U, error_type> which will be either the result of invoking @c handle_value , or it will contain a
  ///         copy of @c self.error() .
  template <typename SelfType, typename F, constraints<std::enable_if<decays_to_v<SelfType, result>>> = nullptr>
  static constexpr auto and_then(SelfType&& self, F&& handle_value) {
    using f_result_t = invoke_result_t<F>;
    static_assert(is_instantiation_of_v<f_result_t, ::arene::base::result>, "handle_value must return a result!");
    static_assert(
        std::is_same<typename f_result_t::error_type, typename remove_cvref_t<SelfType>::error_type>::value,
        "The result returned from handle_value's error channel must be the same as this result's error channel"
    );
    if (self.has_value()) {
      return ::arene::base::invoke(std::forward<F>(handle_value));
    }
    return f_result_t{in_place_error, forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_error())};
  }

  ///
  /// @brief deduced-this helper for @c result::or_else<F>(F&&) .
  /// @tparam SelfType The qualified result type to invoke with. Must satisfy @c decays_to<SelfType,result>
  /// @tparam F Type of the visiting functor. Must be invocable with @c error_type matching the const/ref qualification
  ///         of @c SelfType and return @c result<void,E> , where @c E may be any valid @c error_type .
  /// @param self the instance of the result to use.
  /// @param handle_error the functor to invoke if @c self.has_error() is @c true .
  /// @return result<void, E> which will be either the result of invoking @c handle_error with the contents of
  ///         @c self.error() , or it will have its value channel populated .
  template <typename SelfType, typename F, constraints<std::enable_if<decays_to_v<SelfType, result>>> = nullptr>
  static constexpr auto or_else(SelfType&& self, F&& handle_error) {
    using f_result_t =
        invoke_result_t<F, decltype(forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_error()))>;
    static_assert(is_instantiation_of_v<f_result_t, ::arene::base::result>, "handle_error must return a result!");
    static_assert(
        std::is_same<typename f_result_t::value_type, typename remove_cvref_t<SelfType>::value_type>::value,
        "The result returned from handle_error's value channel must be the same as this result's value channel"
    );
    if (self.has_error()) {
      return ::arene::base::invoke(
          std::forward<F>(handle_error),
          forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_error())
      );
    }
    return f_result_t{};
  }

  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: it is not hiding anything"
  /// @brief deduced-this helper for @c result::transform<F>(F&&) .
  /// @tparam SelfType The qualified result type to invoke with. Must satisfy @c decays_to<SelfType,result>
  /// @tparam F Type of the visiting functor. Must be invocable with @c value_type matching the const/ref qualification
  ///         of @c SelfType , or with no arguments if @c value_type is @c void . May return any valid @c value_type
  /// @param self the instance of the result to use.
  /// @param handle_value the functor to invoke if @c self.has_value() is @c true .
  /// @return result<U, error_type> where @c U is the return type of @c F , which will be one of:
  ///         * A @c result<U,error_type> constructed from the result of invoking @c handle_value if @c self.has_value()
  ///         is @c true
  ///         * A @c result<U,error_type> constructed from a copy of @c self.error() otherwise.
  template <
      typename SelfType,
      typename F,
      constraints<
          std::enable_if<decays_to_v<SelfType, result>>,
          std::enable_if_t<!std::is_void<invoke_result_t<F>>::value>> = nullptr>
  static constexpr auto transform(SelfType&& self, F&& handle_value) {
    using f_value_t = invoke_result_t<F>;
    using result_t = result<f_value_t, error_type>;
    if (self.has_value()) {
      return result_t{in_place_value, ::arene::base::invoke(std::forward<F>(handle_value))};
    }
    return result_t{in_place_error, forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_error())};
  }

  /// @brief deduced-this helper for @c result::transform<F>(F&&) .
  /// @tparam SelfType The qualified result type to invoke with. Must satisfy @c decays_to<SelfType,result>
  /// @tparam F Type of the visiting functor. Must be invocable with @c value_type matching the const/ref qualification
  ///         of @c SelfType , or with no arguments if @c value_type is @c void . May return any valid @c value_type
  /// @param self the instance of the result to use.
  /// @param handle_value the functor to invoke if @c self.has_value() is @c true .
  /// @return result<void, error_type> which will be one of:
  ///         * A default-constructed @c result<void,error_type> if @c self.has_value() is @c true
  ///         * A @c result<void,error_type> constructed from a copy of @c self.error() otherwise.
  template <
      typename SelfType,
      typename F,
      constraints<
          std::enable_if<decays_to_v<SelfType, result>>,
          std::enable_if_t<std::is_void<invoke_result_t<F>>::value>> = nullptr>
  static constexpr auto transform(SelfType&& self, F&& handle_value) {
    using result_t = result<void, error_type>;
    if (self.has_value()) {
      ::arene::base::invoke(std::forward<F>(handle_value));
      return result_t{};
    }
    return result_t{in_place_error, forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_error())};
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  ///
  /// @brief deduced-this helper for @c result::transform_error<F>(F&&) .
  /// @tparam SelfType The qualified result type to invoke with. Must satisfy @c decays_to<SelfType,result>
  /// @tparam F Type of the visiting functor. Must be invocable with @c error_type matching the const/ref qualification
  ///         of @c SelfType. May return any valid @c error_type .
  /// @param self the instance of the result to use.
  /// @param handle_error the functor to invoke if @c self.has_error() is @c true .
  /// @return result<value_type, E> which will be either:
  ///         * A result constructed from the result of invoking @c handle_error with the contents of @c self.error() ,
  ///         * A result constructed from a copy of @c self.value() .
  template <typename SelfType, typename F, constraints<std::enable_if<decays_to_v<SelfType, result>>> = nullptr>
  static constexpr auto transform_error(SelfType&& self, F&& handle_error) {
    using f_error_t =
        invoke_result_t<F, decltype(forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_error()))>;
    using result_t = result<void, f_error_t>;
    if (self.has_error()) {
      return result_t{
          in_place_error,
          ::arene::base::invoke(
              std::forward<F>(handle_error),
              forward_like<SelfType>(std::forward<SelfType>(self).storage().unsafe_error())
          )
      };
    }
    return result_t{};
  }
};  // result<void,E>
// parasoft-end-suppress AUTOSAR-A12_0_1-a-2
// parasoft-end-suppress AUTOSAR-M14_5_3-a-2
// parasoft-end-suppress AUTOSAR-A1_1_1-b-2
// parasoft-end-suppress AUTOSAR-A10_2_1-b-2

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Visit the contained value or error with two independent functors.
/// @tparam ResultType The type of the @c result object to visit.
/// @tparam OnValue The type of the invocable to invoke if the @c result holds a value . Must satisfy @c is_invocable
///         with the result of @c self.value() , and must have the same return type as @c OnValue .
/// @tparam OnError The type of the invocable to invoke if the @c result holds an error. Must satisfy @c is_invocable
///         with the result of @c self.error() , and must have the same return type as @c OnValue .
/// @param self The @c result object
/// @param on_expected The invocable to invoke if @c self holds a value
/// @param on_unexpected The invocable to invoke if @c self holds an error
/// @return auto The result of invoking the appropriate invocable
///
/// Example Usage:
/// @snippet docs/examples/result_examples.cpp expand_result
template <
    class ResultType,
    class OnValue,
    class OnError,
    constraints<
        std::enable_if_t<is_instantiation_of_v<std::remove_const_t<std::remove_reference_t<ResultType>>, result>>,
        std::enable_if_t<std::is_same<
            invoke_result_t<OnValue, decltype(std::declval<ResultType>().value())>,
            invoke_result_t<OnError, decltype(std::declval<ResultType>().error())>>::value>> = nullptr>
constexpr auto expand_result(
    ResultType&& self,
    OnValue&& on_expected,
    OnError&& on_unexpected
) noexcept(is_nothrow_invocable_v<OnValue, decltype(std::declval<ResultType>().value())>&&
               is_nothrow_invocable_v<OnError, decltype(std::declval<ResultType>().error())>) -> decltype(auto) {
  if (self.has_value()) {
    return ::arene::base::invoke(std::forward<OnValue>(on_expected), std::forward<ResultType>(self).value());
  }
  return ::arene::base::invoke(std::forward<OnError>(on_unexpected), std::forward<ResultType>(self).error());
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

///
/// @brief Intermediary type for constructing a @c result with a given @c value_type .
/// @note Generally this should not be constructed manually; it intended as a transient type that allow to construct a
/// @c result Object while using type deduction for @c value_type and @c error_type .
/// @tparam ValueType the @c value_type of a @c result constructed from this type.
/// @see @c arene::base::value_result for the factory function that should be used in user code.
/// @{
template <typename ValueType>
class value_result_t {
 public:
  // parasoft-begin-suppress AUTOSAR-A13_5_2-a-2 "Implicit conversion to result is part of the API"
  // parasoft-begin-suppress AUTOSAR-A13_2_3-a-2 "False positive: Not a relational operator"
  /// @brief Instantiates a @c result<ValueType,Error> from the contained value instance. Consumes the container.
  /// @tparam Error the @c error_type of the produced @c result .
  template <typename Error>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr operator result<ValueType, Error>(
  ) const& noexcept(noexcept(result<ValueType, Error>(in_place_value, std::declval<ValueType const&>()))) {
    return {in_place_value, value_};
  }
  /// @brief Instantiates a @c result<ValueType,Error> from the contained value instance. Consumes the container.
  /// @tparam Error the @c error_type of the produced @c result .
  template <typename Error>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr operator result<ValueType, Error>(
  ) && noexcept(noexcept(result<ValueType, Error>(in_place_value, std::declval<ValueType&&>()))) {
    return {in_place_value, std::move(value_)};
  }
  // parasoft-end-suppress AUTOSAR-A13_2_3-a-2
  // parasoft-end-suppress AUTOSAR-A13_5_2-a-2

  /// @brief Construct the stored value from the specified arguments
  /// @tparam ArgTypes the types of the arguments
  /// @param args The arguments
  template <
      typename... ArgTypes,
      constraints<std::enable_if_t<all_of_v<!std::is_same<remove_cvref_t<ArgTypes>, value_result_t>::value...>>> =
          nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr value_result_t(ArgTypes&&... args) noexcept(std::is_nothrow_constructible<ValueType, ArgTypes...>::value)
      : value_(std::forward<ArgTypes>(args)...) {}

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: Constructor constrained via SFINAE is permitted"
  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Copy constructor
  constexpr value_result_t(value_result_t const&) = default;
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  /// @brief Move constructor
  constexpr value_result_t(value_result_t&&) = default;

  /// @brief Copy assignment
  constexpr auto operator=(value_result_t const&) -> value_result_t& = default;
  /// @brief Move constructor
  constexpr auto operator=(value_result_t&&) -> value_result_t& = default;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  /// @brief destructor
  ~value_result_t() = default;

 private:
  /// @brief The contained value
  ValueType value_;
};

/// @brief Intermediary type for constructing a @c result with a @c void value
/// @note Generally this should not be constructed manually; it intended as a transient type that allow to construct a
/// @c result Object while using type deduction for @c value_type and @c error_type .
/// @see @c arene::base::value_result for the factory function that should be used in user code.
template <>
class value_result_t<void> {
 public:
  // parasoft-begin-suppress AUTOSAR-A13_5_2-a-2 "Implicit conversion to result is part of the API"
  // parasoft-begin-suppress AUTOSAR-A13_2_3-a-2 "False positive: Not a relational operator"
  /// @brief Instantiates a @c result<Value,Error> from the contained value instance. Consumes the container.
  /// @tparam Error the @c error_type of the produced @c result .
  template <typename Error>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr operator result<void, Error>() noexcept {
    return {};
  }
  // parasoft-end-suppress AUTOSAR-A13_2_3-a-2
  // parasoft-end-suppress AUTOSAR-A13_5_2-a-2
};
/// @}

///
/// @brief Intermediary type for constructing a @c result with a given @c error_type .
/// @note Generally this should not be constructed manually; it intended as a transient type that allow to construct a
/// @c result Object while using type deduction for @c value_type and @c error_type .
/// @tparam ErrorType the @c error_type of a @c result constructed from this type.
/// @see @c arene::base::error_result for the factory function that should be used in user code.
template <typename ErrorType>
class error_result_t {
 public:
  // parasoft-begin-suppress AUTOSAR-A13_5_2-a-2 "Implicit conversion to result is part of the API"
  // parasoft-begin-suppress AUTOSAR-A13_2_3-a-2 "False positive: Not a relational operator"
  /// @brief Instantiates a @c result<Value,ErrorType> from the contained error instance.
  /// @tparam Value the @c value_type of the produced @c result .
  template <typename Value>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr operator result<Value, ErrorType>(
  ) const& noexcept(noexcept(result<Value, ErrorType>(in_place_error, std::declval<ErrorType const&>()))) {
    return {in_place_error, error_};
  }
  /// @brief Instantiates a @c result<Value,ErrorType> from the contained error instance, moving the error value into
  /// the @c result
  /// @tparam Value the @c value_type of the produced @c result .
  template <typename Value>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr operator result<Value, ErrorType>(
  ) && noexcept(noexcept(result<Value, ErrorType>(in_place_error, std::declval<ErrorType&&>()))) {
    return {in_place_error, std::move(error_)};
  }
  // parasoft-end-suppress AUTOSAR-A13_2_3-a-2
  // parasoft-end-suppress AUTOSAR-A13_5_2-a-2

  /// @brief Construct the stored error from the specified arguments
  /// @tparam ArgTypes the types of the arguments
  /// @param args The arguments
  template <
      typename... ArgTypes,
      constraints<std::enable_if_t<all_of_v<!std::is_same<remove_cvref_t<ArgTypes>, error_result_t>::value...>>> =
          nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr error_result_t(ArgTypes&&... args) noexcept(std::is_nothrow_constructible<ErrorType, ArgTypes...>::value)
      : error_(std::forward<ArgTypes>(args)...) {}

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: Constructor constrained via SFINAE is permitted"
  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Copy constructor
  constexpr error_result_t(error_result_t const&) = default;
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  /// @brief Move constructor
  constexpr error_result_t(error_result_t&&) = default;

  /// @brief Copy assignment
  constexpr auto operator=(error_result_t const&) -> error_result_t& = default;
  /// @brief Move constructor
  constexpr auto operator=(error_result_t&&) -> error_result_t& = default;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  /// @brief destructor
  ~error_result_t() = default;

 private:
  /// @brief The contained error
  ErrorType error_;
};

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress AUTOSAR-A2_10_1-a-2 "False positive: No global or namespace entity named 'value'"
///
/// @brief Convenience function to construct a @c result<ValueType,Error> containing the @c ValueType from any
/// reference.
///
/// Example Usage:
/// @snippet docs/examples/result_examples.cpp value_result
///
/// @tparam ValueType the @c value_type of the produced result.
/// @param value value to instantiate the @c result with.
/// @return value_result_t An object which is move-convertible to any @c result<ValueType,Error> .
template <typename ValueType>
constexpr auto value_result(ValueType&& value
) noexcept(std::is_nothrow_constructible<typename std::remove_reference<ValueType>::type, ValueType&&>::value)
    -> value_result_t<std::remove_reference_t<ValueType>> {
  return {std::forward<ValueType>(value)};
}
// parasoft-end-suppress AUTOSAR-A2_10_1-a-2
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

///
/// @brief Convenience function to construct a @c result<void,Error> .
///
/// Example Usage:
/// @snippet docs/examples/result_examples.cpp value_result_void
///
/// @return value_result_t An object which is move-convertible to any @c result<void,Error> .
inline constexpr auto value_result() noexcept -> value_result_t<void> { return {}; }

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress AUTOSAR-A2_10_1-a-2 "False positive: No global or namespace entity named 'error'"
/// @brief Convenience function to construct a @c result<Value,ErrorType> containing the @c ErrorType from any
/// reference.
///
/// Example Usage:
/// @snippet docs/examples/result_examples.cpp error_result
///
/// @tparam ErrorType the @c error_type of the produced result.
/// @param error error to instantiate the @c result with.
/// @return error_result_t An object which is move-convertible to any @c result<Value,ErrorType> .
template <typename ErrorType>
constexpr auto error_result(ErrorType&& error
) noexcept(std::is_nothrow_constructible<typename std::remove_reference<ErrorType>::type, ErrorType&&>::value)
    -> error_result_t<std::remove_reference_t<ErrorType>> {
  return {std::forward<ErrorType>(error)};
}
// parasoft-end-suppress AUTOSAR-A2_10_1-a-2
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

}  // namespace base
}  // namespace arene

// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "False positive: specialization of standard templates is permitted"
// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "False positive: specialization of standard templates is permitted"
namespace std {

// parasoft-begin-suppress AUTOSAR-A11_0_2-a-2 "False positive: must follow the primary template"
/// @brief Specialization of @c std::hash for @c arene::base::result
///
/// @tparam V the type of the value channel. @c std::hash<V> must be well-formed for this specialization to be
///         well-formed.
/// @tparam E the type of the error channel. @c std::hash<E> must be well-formed for this specialization to be
///         well-formed.
///
template <typename V, typename E>
struct hash<::arene::base::result<V, E>> {
  // parasoft-begin-suppress AUTOSAR-A18_1_6-a "False positive: The declaration does have a noexcept specifier"
  // parasoft-begin-suppress AUTOSAR-A2_10_1-a-2 "False positive: No global or namespace entity named 'value'"
  /// @brief Calculate the hash of a @c result
  /// @param value The instance of @c result to hash
  /// @return The result of hashing the stored value if @c value has a value, the result of hashing the stored error
  /// otherwise
  constexpr auto operator()(::arene::base::result<V, E> const& value) const
      noexcept(noexcept(hash<E>{}(std::declval<E const&>())) && noexcept(hash<V>{}(std::declval<V const&>())))
          -> std::size_t {
    return value.has_value() ? hash<V>{}(value.value()) : hash<E>{}(value.error());
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-a-2
  // parasoft-end-suppress AUTOSAR-A18_1_6-a-2
};
// parasoft-end-suppress AUTOSAR-A11_0_2-a-2

// parasoft-begin-suppress AUTOSAR-A11_0_2-a-2 "False positive: must follow the primary template"
/// @brief Specialization of @c std::hash for @c arene::base::result<void, E>
///
/// @tparam E the type of the error channel. @c std::hash<E> must be well-formed for this specialization to be
///         well-formed.
///
template <typename E>
struct hash<::arene::base::result<void, E>> {
  // parasoft-begin-suppress AUTOSAR-A18_1_6-a "False positive: The declaration does have a noexcept specifier"
  // parasoft-begin-suppress AUTOSAR-A2_10_1-a-2 "False positive: No global or namespace entity named 'value'"
  /// @brief Calculate the hash of a @c result
  /// @param value The instance of @c result to hash
  /// @return The result of hashing @c monostate if @c value has a value, the result of hashing the stored error
  /// otherwise
  constexpr auto operator()(::arene::base::result<void, E> const& value) const
      noexcept(noexcept(hash<E>{}(std::declval<E const&>()))) -> std::size_t {
    return value.has_value() ? hash<::arene::base::monostate>{}({}) : hash<E>{}(value.error());
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-a-2
  // parasoft-end-suppress AUTOSAR-A18_1_6-a-2
};
// parasoft-end-suppress AUTOSAR-A11_0_2-a-2

}  // namespace std
// parasoft-end-suppress CERT_CPP-DCL58-a-2
// parasoft-end-suppress AUTOSAR-A17_6_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RESULT_RESULT_HPP_
