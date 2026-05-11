// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_OPTIONAL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_OPTIONAL_HPP_

// IWYU pragma: private
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/functional/invoke.hpp"
#include "arene/base/memory/construct_at.hpp"
#include "arene/base/optional/detail/optional_base.hpp"
#include "arene/base/optional/detail/optional_value_interface.hpp"  // IWYU pragma: keep
#include "arene/base/optional/optional.hpp"
#include "arene/base/scope_guard/scope_guard.hpp"  // IWYU pragma: keep
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_manipulation/implicit_constructor_base.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/decays_to.hpp"
#include "arene/base/type_traits/has_overloaded_address_operator.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
#include "arene/base/utility/forward_like.hpp"
#include "arene/base/utility/in_place.hpp"
#include "arene/base/utility/swap.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {
namespace detail {
/// @brief Tag type used for @c arene::base::nullopt_t constructor
struct unspecified_t {};
}  // namespace detail

/// @brief Empty class type used to indicate that @c arene::base::optional does not contain a value.
/// @note @c arene::base::nullopt_t is a non-aggregate type, has no default constructor, and no
///   initializer-list constructor.
class nullopt_t {
 public:
  /// @brief constructor for @c nullopt_t
  /// @note This constructor exists to support both <c> op = {}; </c> and <c> op = nullopt; </c> as the syntax for
  ///   indicating that an @c arene::base::optional does not contain a value.
  /// @note see ISO/IEC JTC1 SC22 WG21 N3672 for more nuanced discussion of the <c> op = {}; </c> syntax.
  /// @note In arene-base version 3.47.1 and prior, this @c arene::base::nullopt_t was erroneously default
  /// constructible.
  /// @note The type of the parameter to this constructor is unspecified. Users should rely on @c arene::base::nullopt
  /// for an instance of a @c arene::base::nullopt_t rather than constructing an instance of @c arene::base::nullopt_t
  /// directly.
  constexpr explicit nullopt_t(detail::unspecified_t) noexcept {}
};

/// @brief alias to maintain backwards compatibility with older versions of Arene Base.
/// @see arene::base::nullopt_t
/// @deprecated
using null_opt_t = nullopt_t;

/// @brief Tag variable for empty-optional construction
ARENE_MAYBE_UNUSED constexpr nullopt_t nullopt{{}};

/// @brief alias to maintain backwards compatibility with older versions of Arene Base.
/// @see arene::base::nullopt
/// @deprecated
ARENE_MAYBE_UNUSED constexpr nullopt_t null_opt{nullopt};

/// @brief Implementation of @c std::optional for pre-C++17 compilers, and which replaces defined undefined behavior
/// with @c ARENE_PRECONDITION guards.
/// @tparam T The type which should be held in the optional.
///
/// An optional is a sum-type which can be in one of two states:
/// 1. It is _disengaged_, which means it holds no value
/// 2. It is _engaged_, which means it holds a value of type @c T .
///
/// It is generally used as a return type from an API which is _fallible_, but for which failure is an expected
/// operating condition and for which no additional context other than a simple boolean pass/fail is useful. It can also
/// be used as a member variable in a class to allow for deferred initialization without needing dynamic allocation, or
/// as an input parameter to a function where it is important to distinguish between "not provided" and "default value."
template <typename T>
// parasoft-begin-suppress AUTOSAR-A10_1_1-a-2 "False positive: Only one non-interface base class"
// NOLINTNEXTLINE(hicpp-special-member-functions,cppcoreguidelines-special-member-functions)
class optional
    : optional_detail::optional_base<T>
    , implicit_constructor_base<T>
    , generic_ordering_from_three_way_compare_and_equals<optional<T>>
    , public optional_detail::optional_value_interface<optional<T>> {
  // parasoft-end-suppress AUTOSAR-A10_1_1-a-2
  /// @brief Internal type alias for the base class
  using base_type = optional_detail::optional_base<T>;

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'storage_type' does not hide an identifier in
  // 'optional_base'"
  /// @brief Internal type alias for the storage type
  using storage_type = typename base_type::storage_type;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief Internal type alias for the base class
  using compare_base = generic_ordering_from_three_way_compare_and_equals<optional<T>>;

  /// @brief Internal type alias for the base class
  using constructor_base = implicit_constructor_base<T>;

  /// @brief Internal type alias for the base class
  using interface_base = optional_detail::optional_value_interface<optional<T>>;

  static_assert(!has_overloaded_address_operator_v<T>, "Stored type must not have an overloaded address operator");

 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'value_type' does not hide an identifier in
  // 'ebo_holder'"
  /// @brief The type stored in the optional
  using value_type = T;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A12_7_1-a "Bases must be initialized explicitly to address a compiler bug"
  /// @brief Default construct to an empty state
  // Explicitly initializes bases rather than using @c =default to address a gcc 8 compiler bug
  constexpr optional() noexcept
      : base_type{},
        constructor_base{},
        compare_base{},
        interface_base{} {}
  // parasoft-end-suppress AUTOSAR-A12_7_1-a

  /// @brief Default destructor
  ~optional() = default;

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: Constrained via SFINAE"
  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: Bases initialized via delegation"
  /// @brief Explicitly construct to an empty state
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr optional(nullopt_t) noexcept
      : optional{} {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_8_6-a "Not guaranteed to be a base class"
  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: Constrained via SFINAE"
  /// @brief Copy constructor
  constexpr optional(optional const&) noexcept(std::is_nothrow_copy_constructible<T>::value) = default;
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_6-a

  /// @brief Move constructor
  constexpr optional(optional&&) noexcept(
      // NOLINTNEXTLINE(hicpp-noexcept-move)
      std::is_nothrow_move_constructible<T>::value
  ) = default;

  /// @brief Construct from a value
  /// @tparam U The type of the value to construct from
  /// @param other The source to move from
  template <
      typename U,
      constraints<
          std::enable_if_t<!std::is_same<optional, remove_cvref_t<U>>::value>,
          std::enable_if_t<!std::is_same<nullopt_t, remove_cvref_t<U>>::value>,
          std::enable_if_t<std::is_constructible<T, U&&>::value>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions,bugprone-forwarding-reference-overload)
  constexpr optional(U&& other) noexcept(std::is_nothrow_constructible<T, U&&>::value)
      : base_type(std::forward<U>(other)),
        constructor_base{},
        compare_base{} {}

  /// @brief Construct from another optional
  /// @tparam U The type of the value to construct from
  /// @param other The source to move from
  template <
      typename U,
      constraints<
          std::enable_if_t<std::is_constructible<T, U>::value>,
          std::enable_if_t<!std::is_constructible<T, optional<U>>::value>,
          std::enable_if_t<!std::is_constructible<T, optional<U>&>::value>,
          std::enable_if_t<!std::is_constructible<T, optional<U> const&>::value>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr optional(optional<U>&& other) noexcept(noexcept(T(std::declval<U&&>())))
      : base_type(std::move(other)),
        constructor_base{},
        compare_base{} {}

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: Constrained via SFINAE"
  /// @brief Construct from another optional
  /// @tparam U The type of the value to construct from
  /// @param other The source to copy from
  template <
      typename U,
      constraints<
          std::enable_if_t<std::is_constructible<T, U>::value>,
          std::enable_if_t<!std::is_constructible<T, optional<U>>::value>,
          std::enable_if_t<!std::is_constructible<T, optional<U>&>::value>,
          std::enable_if_t<!std::is_constructible<T, optional<U> const&>::value>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr optional(optional<U> const& other) noexcept(noexcept(T(std::declval<U const&>())))
      : base_type(other),
        constructor_base{},
        compare_base{} {}
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: Constrained via SFINAE"
  /// @brief Construct from a set of arguments
  /// @tparam Args The types of the arguments to construct the value from
  /// @param args The arguments to construct the value from
  template <
      typename... Args,
      constraints<std::enable_if_t<sizeof(decltype(T(std::declval<Args>()...))) != 0>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr optional(in_place_t, Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...)))
      : base_type(in_place, std::forward<Args>(args)...),
        constructor_base{},
        compare_base{} {}
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_8_6-a "Not guaranteed to be a base class"

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: Constrained via SFINAE"
  /// @brief Reset via assignment
  constexpr auto operator=(nullopt_t) noexcept -> optional& {
    this->reset();
    return *this;
  }
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: Constrained via SFINAE"
  /// @brief Copy assign
  /// @param other The value to assign from
  // NOLINTNEXTLINE(bugprone-exception-escape)
  constexpr auto operator=(optional const& other
  ) noexcept(std::is_nothrow_copy_constructible<T>::value && std::is_nothrow_copy_assignable<T>::value) -> optional& {
    if (this != &other) {
      internal_do_assign(other);
    }
    return *this;
  }
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A15_5_1-b-2 "False positive: Conditionally noxecept"
  /// @brief Move assign
  /// @param other The value to assign from
  constexpr auto operator=(optional&& other)
      // NOLINTNEXTLINE(bugprone-exception-escape, hicpp-noexcept-move) We want to exactly match T.
      noexcept(std::is_nothrow_move_constructible<T>::value && std::is_nothrow_move_assignable<T>::value) -> optional& {
    internal_do_assign(std::move(other));
    return *this;
  }
  // parasoft-end-suppress AUTOSAR-A15_5_1-b-2

  /// @brief Assign from a value
  /// @param other The value to assign from
  template <
      typename U = T,
      constraints<
          std::enable_if_t<!std::is_same<remove_cvref_t<U>, optional>::value>,
          std::enable_if_t<std::is_constructible<T, U>::value>,
          std::enable_if_t<std::is_assignable<T, U>::value>> = nullptr>
  auto operator=(U&& other) noexcept(std::is_nothrow_assignable<T, U>::value&& noexcept(
      std::declval<optional&>().construct_from(std::forward<U>(other))
  )) -> optional& {
    if (this->has_value()) {
      **this = std::forward<U>(other);
    } else {
      construct_from(std::forward<U>(other));
    }
    return *this;
  }

  // parasoft-end-suppress AUTOSAR-A12_8_6-a

  // Make has_value public from base class
  using base_type::has_value;

  /// @brief Query if this object has a value
  ///
  /// @return bool Equivalent to @c has_value() .
  constexpr explicit operator bool() const noexcept { return this->has_value(); }

  // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "This is a wrapper, so non-const access to the value is required"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  /// @brief Accesses the value in the optional.
  ///
  /// @return A @c T which is cref-qualified the same as @c *this .
  /// @pre @c has_value() , else @c ARENE_PRECONDITION violation.
  /// @{
  constexpr auto operator*() const& noexcept -> T const& {
    ARENE_PRECONDITION(this->has_value());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    return this->storage().value;
  }
  constexpr auto operator*() & noexcept -> T& {
    ARENE_PRECONDITION(this->has_value());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    return this->storage().value;
  }
  constexpr auto operator*() const&& noexcept -> T const&& {
    ARENE_PRECONDITION(this->has_value());
    // parasoft-begin-suppress AUTOSAR-A18_9_3-a-2 "We want to ensure the qualifier is preserved"
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    return std::move(this->storage().value);
    // parasoft-end-suppress AUTOSAR-A18_9_3-a-2
  }
  constexpr auto operator*() && noexcept -> T&& {
    ARENE_PRECONDITION(this->has_value());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    return std::move(this->storage().value);
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2
  // parasoft-end-suppress AUTOSAR-A9_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  /// @brief Access a pointer to the value in the optional
  ///
  /// @return A @c T* which is const-qualified the same as @c *this .
  /// @pre @c has_value() , else @c ARENE_PRECONDITION violation.
  /// @{
  constexpr auto operator->() const noexcept -> T const* {
    ARENE_PRECONDITION(this->has_value());
    return &**this;
  }
  constexpr auto operator->() noexcept -> T* {
    ARENE_PRECONDITION(this->has_value());
    return &**this;
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  /// @brief Gets the value held in the optional, or else the provided default.
  /// @tparam U The type of the default value. @c T must be constructible from @c U.
  /// @param default_value The value to return if the optional is null.
  /// @return T constructed from the held value if @c has_value() is @c true , else an instance constructed from
  ///         @c default_value .
  /// @note This must return a new instance rather than a reference, otherwise it risks dangling references if
  ///       @c default_value is a temporary.
  ///@{
  template <
      typename U,
      typename R = T,
      constraints<
          std::enable_if_t<std::is_copy_constructible<R>::value>,
          std::enable_if_t<std::is_convertible<U, R>::value>> = nullptr>
  constexpr auto value_or(U&& default_value
  ) const& noexcept(noexcept(optional::value_or(std::declval<optional const&>(), std::forward<U>(default_value))))
      -> T {
    return optional::value_or(*this, std::forward<U>(default_value));
  }
  template <
      typename U,
      typename R = T,
      constraints<
          std::enable_if_t<std::is_move_constructible<R>::value>,
          std::enable_if_t<std::is_convertible<U, R>::value>> = nullptr>
  constexpr auto value_or(U&& default_value
  ) && noexcept(noexcept(optional::value_or(std::declval<optional&&>(), std::forward<U>(default_value)))) -> T {
    return optional::value_or(std::move(*this), std::forward<U>(default_value));
  }
  ///@}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  /// @brief Gets the value held in the optional, or else invokes the provided callable to generate one.
  /// @tparam F The type of the callable to invoke. Must satisfy @c is_invocable_v<F> and
  ///         @c std::is_convertible<invoke_result_t<F>,T> .
  /// @param default_generator The function to invoke to produce a default if @c !has_value() .
  /// @return T constructed from the held value if @c has_value() is @c true , else an instance constructed from
  ///         invoking @c default_generator .
  ///@{
  template <
      typename F,
      typename R = T,
      constraints<
          std::enable_if_t<std::is_copy_constructible<R>::value>,
          std::enable_if_t<std::is_convertible<invoke_result_t<F>, R>::value>> = nullptr>
  constexpr auto value_or_else(F&& default_generator) const& noexcept(
      noexcept(optional::value_or_else(std::declval<optional const&>(), std::forward<F>(default_generator)))
  ) -> T {
    return optional::value_or_else(*this, std::forward<F>(default_generator));
  }
  template <
      typename F,
      typename R = T,
      constraints<
          std::enable_if_t<std::is_move_constructible<R>::value>,
          std::enable_if_t<std::is_convertible<invoke_result_t<F>, R>::value>> = nullptr>
  constexpr auto value_or_else(F&& default_generator
  ) && noexcept(noexcept(optional::value_or_else(std::declval<optional&&>(), std::forward<F>(default_generator))))
      -> T {
    return optional::value_or_else(std::move(*this), std::forward<F>(default_generator));
  }
  ///@}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2

  /// @brief Destroy the old value if there is one, and construct a new value
  /// @tparam Args The types of the arguments for constructing the new value
  /// @param args The arguments for constructing the new value
  template <typename... Args>
  constexpr void emplace(Args&&... args
  ) noexcept(noexcept(std::declval<optional<T>>().construct_from(std::forward<Args>(args)...))) {
    this->reset();
    construct_from(std::forward<Args>(args)...);
  }

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  /// @brief Monadic API which invokes a functor with the contents of @c value() if @c has_value() is @c true.
  /// @tparam F Type of the visiting functor. Must be invocable with @c value_type matching the const/ref qualification
  ///         of @c Self, and return any valid <c>optional&lt;U&gt;</c>.
  /// @param handle_value the functor to invoke if @c self.has_value() is @c true .
  /// @return U where @c U is the return type of @c F and is an instantiation of @c optional , which will be:
  ///         * The result of invoking @c handle_value with the contents of @c value() if @c has_value() is @c true.
  ///         * A @c U instantiated with @c nullopt otherwise.
  ///@{
  template <typename F>
  constexpr auto and_then(F&& handle_value) & -> invoke_result_t<F, T&> {
    static_assert(is_invocable_v<F, T&>, "handle_value must be invocable with value_type.");
    static_assert(
        is_instantiation_of_v<invoke_result_t<F, T&>, ::arene::base::optional>,
        "handle_value must return an optional"
    );
    return optional::and_then(*this, std::forward<F>(handle_value));
  }
  template <typename F>
  constexpr auto and_then(F&& handle_value) const& -> invoke_result_t<F, T const&> {
    static_assert(is_invocable_v<F, T const&>, "handle_value must be invocable with value_type.");
    static_assert(
        is_instantiation_of_v<invoke_result_t<F, T const&>, ::arene::base::optional>,
        "handle_value must return an optional"
    );
    return optional::and_then(*this, std::forward<F>(handle_value));
  }
  template <typename F>
  constexpr auto and_then(F&& handle_value) && -> invoke_result_t<F, T&&> {
    static_assert(is_invocable_v<F, T&&>, "handle_value must be invocable with value_type.");
    static_assert(
        is_instantiation_of_v<invoke_result_t<F, T&&>, ::arene::base::optional>,
        "handle_value must return an optional"
    );
    return optional::and_then(std::move(*this), std::forward<F>(handle_value));
  }
  template <typename F>
  constexpr auto and_then(F&& handle_value) const&& -> invoke_result_t<F, T const&&> {
    static_assert(is_invocable_v<F, T const&&>, "handle_value must be invocable with value_type.");
    static_assert(
        is_instantiation_of_v<invoke_result_t<F, T const&&>, ::arene::base::optional>,
        "handle_value must return an optional"
    );
    // parasoft-begin-suppress AUTOSAR-A18_9_3-a-2 "We want to ensure the qualifier is preserved"
    return optional::and_then(std::move(*this), std::forward<F>(handle_value));
    // parasoft-end-suppress AUTOSAR-A18_9_3-a-2
  }
  ///@}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  /// @brief Monadic API which invokes a functor if @c has_value() is @c false.
  /// @tparam F Type of the visiting functor. Must be invocable with no arguments and return @c optional<T> .
  /// @param handle_null the functor to invoke if @c self.has_value() is @c false .
  /// @return optional<T> which will be:
  ///         * The result of invoking @c handle_value with the contents of @c value() if @c has_value() is @c false.
  ///         * A instance of @c optional<T> constructed from forwarding @c self .
  ///@{
  template <
      typename F,
      typename R = T,
      constraints<std::enable_if_t<is_invocable_v<F>>, std::enable_if_t<std::is_copy_constructible<R>::value>> =
          nullptr>
  constexpr auto or_else(F&& handle_null) const& -> optional<T> {
    static_assert(
        decays_to_v<invoke_result_t<F>, optional<T>>,
        "handle_null must be void invocable and return an optional with the same type as this."
    );
    return optional::or_else(*this, std::forward<F>(handle_null));
  }
  template <
      typename F,
      typename R = T,
      constraints<std::enable_if_t<is_invocable_v<F>>, std::enable_if_t<std::is_move_constructible<R>::value>> =
          nullptr>
  constexpr auto or_else(F&& handle_null) && -> optional<T> {
    static_assert(
        decays_to_v<invoke_result_t<F>, optional<T>>,
        "handle_null must be void invocable and return an optional with the same type as this."
    );
    return optional::or_else(std::move(*this), std::forward<F>(handle_null));
  }
  ///@}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: it is not hiding anything"
  /// @brief Calls a provided functor with @c value if it is populated, and produces a new @c optional with the return
  ///        from it.
  /// @tparam F Type of the visiting functor. Must be invocable with @c value_type matching the const/ref qualification
  ///         of @c this. May return any valid @c value_type
  /// @param value_transformer the functor to invoke if @c self.has_value() is @c true .
  /// @return <c>optional&lt;U&gt;</c> where @c U is the return type of @c F ,  which will contain:
  ///         * The result of invoking @c handle_value with the contents of @c value() if @c has_value() is @c true.
  ///         * @c nullopt otherwise.
  ///@{
  template <typename F>
  constexpr auto transform(F&& value_transformer) & -> optional<invoke_result_t<F, T&>> {
    static_assert(is_invocable_v<F, T&>, "value_transformer must be invocable with value_type.");
    static_assert(
        substitution_succeeds<::arene::base::optional, invoke_result_t<F, T&>>,
        "value_transformer must return a type that can be validly placed in an optional"
    );
    return optional::transform(*this, std::forward<F>(value_transformer));
  }
  template <typename F>
  constexpr auto transform(F&& value_transformer) const& -> optional<invoke_result_t<F, T const&>> {
    static_assert(is_invocable_v<F, T const&>, "value_transformer must be invocable with value_type.");
    static_assert(
        substitution_succeeds<::arene::base::optional, invoke_result_t<F, T const&>>,
        "value_transformer must return a type that can be validly placed in an optional"
    );
    return optional::transform(*this, std::forward<F>(value_transformer));
  }
  template <typename F>
  constexpr auto transform(F&& value_transformer) && -> optional<invoke_result_t<F, T&&>> {
    static_assert(is_invocable_v<F, T&&>, "value_transformer must be invocable with value_type.");
    static_assert(
        substitution_succeeds<::arene::base::optional, invoke_result_t<F, T&&>>,
        "value_transformer must return a type that can be validly placed in an optional"
    );
    return optional::transform(std::move(*this), std::forward<F>(value_transformer));
  }
  template <typename F>
  constexpr auto transform(F&& value_transformer) const&& -> optional<invoke_result_t<F, T const&&>> {
    static_assert(is_invocable_v<F, T const&&>, "value_transformer must be invocable with value_type.");
    static_assert(
        substitution_succeeds<::arene::base::optional, invoke_result_t<F, T const&&>>,
        "value_transformer must return a type that can be validly placed in an optional"
    );
    // parasoft-begin-suppress AUTOSAR-A18_9_3-a-2 "We want to ensure the qualifier is preserved"
    return optional::transform(std::move(*this), std::forward<F>(value_transformer));
    // parasoft-end-suppress AUTOSAR-A18_9_3-a-2
  }
  ///@}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False Positive: 'swap' does not hide anything"
  ///
  /// @brief Exchanges the state of two optionals.
  ///
  /// @tparam U Must satisfy @c is_swappable_v and @c std::is_move_constructable.
  /// @param other The optional to swap with.
  /// @post If both optionals are engaged, they will have their values exchanged as if via @c arene::base::swap .
  ///       Otherwise if one of the optionals was @c nullopt , the states are exchanged as if by move-constructing the
  ///       value from the engaged optional into the unengaged optional, and resetting the previously engaged optional.
  ///
  template <
      typename U = T,
      constraints<std::enable_if_t<is_swappable_v<U>>, std::enable_if_t<std::is_move_constructible<U>::value>> =
          nullptr>
  constexpr void swap(optional& other) noexcept(  //
      is_nothrow_swappable_v<U> && std::is_nothrow_move_constructible<U>::value
  ) {
    if (this->has_value() && other.has_value()) {
      ::arene::base::swap(**this, *other);
    } else if (this->has_value()) {
      other.construct_from(std::move(**this));
      this->reset();
    } else {
      construct_from(std::move(*other));
      other.reset();
    }
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  // parasoft-begin-suppress AUTOSAR-A2_7_2-a "False positive: no commented-out code"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief swaps the elements between two optionals.
  ///
  /// @tparam U the type of the element in the optional. Must satisfy @c is_swappable_v .
  /// @param lhs the left hand optional to swap.
  /// @param rhs the right hand optional to swap.
  /// @post Equivalent to having called @c lhs.swap(rhs);
  /// @see optional::swap.
  ///
  template <
      typename U = T,
      constraints<std::enable_if_t<is_swappable_v<U>>, std::enable_if_t<std::is_move_constructible<U>::value>> =
          nullptr>
  friend constexpr void swap(optional& lhs, optional& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A2_7_2-a

  /// @brief Make @c reset public from base class
  using base_type::reset;

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'three_way_compare' does not hide an identifier in
  // 'optional'"
  /// @brief Three-way comparison between an @c optional and @c nullopt_t
  /// @param lhs The first object to compare
  /// @return strong_ordering @c strong_ordering::equal if the @c lhs is empty, otherwise @c strong_ordering::greater
  static constexpr auto three_way_compare(optional const& lhs, nullopt_t) noexcept -> strong_ordering {
    return lhs ? strong_ordering::greater : strong_ordering::equal;
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'fast_inequality_check' does not hide an identifier in
  // 'optional'"
  /// @brief Fast inequality check between two @c optional instances, where there is not a fast inequality check between
  /// the two stored types.
  /// @tparam U The value type of the second @c optional
  /// @param lhs The first object to compare
  /// @param rhs The first object to compare
  /// @return inequality_heuristic @c inequality_heuristic::definitely_not_equal if @c lhs has a value and @c rhs does
  /// not, or vice versa, @c inequality_heuristic::may_or_may_not_be_equal otherwise
  template <typename U, constraints<std::enable_if_t<!has_fast_inequality_check_v<T, U>>> = nullptr>
  static constexpr auto fast_inequality_check(optional const& lhs, optional<U> const& rhs) noexcept
      -> inequality_heuristic {
    return (lhs.has_value() != rhs.has_value()) ? inequality_heuristic::definitely_not_equal
                                                : inequality_heuristic::may_be_equal_or_not_equal;
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'fast_inequality_check' does not hide an identifier in
  // 'optional'"
  /// @brief Fast inequality check between two @c optional instances, where there is not a fast inequality check between
  /// the two stored types.
  /// @tparam U The value type of the second @c optional
  /// @param lhs The first object to compare
  /// @param rhs The first object to compare
  /// @return inequality_heuristic @c inequality_heuristic::definitely_not_equal if @c lhs has a value and @c rhs does
  /// not, or vice versa, @c inequality_heuristic::may_or_may_not_be_equal if neither has a value, and delegeates to @c
  /// fast_inequality_check on the stored values otherwise
  template <typename U, constraints<std::enable_if_t<has_fast_inequality_check_v<T, U>>> = nullptr>
  static constexpr auto fast_inequality_check(optional const& lhs, optional<U> const& rhs) noexcept
      -> inequality_heuristic {
    if (lhs.has_value() != rhs.has_value()) {
      return inequality_heuristic::definitely_not_equal;
    }
    if (!lhs) {
      return inequality_heuristic::may_be_equal_or_not_equal;
    }
    return T::fast_inequality_check(*lhs, *rhs);
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'three_way_compare' does not hide an identifier in
  // 'optional'"
  /// @brief Three-way comparison between two @c optional instances
  /// @tparam Self The type of the optional
  /// @tparam U The value type of the second @c optional
  /// @param lhs The first object to compare
  /// @param rhs The first object to compare
  /// @return strong_ordering @c strong_ordering::less if @c lhs is empty and @c rhs has a value, @c
  /// strong_ordering::equal if both @c lhs and @c rhs are empty, @c strong_ordering::greater if @c lhs has a value and
  /// @c rhs is empty, and the result of three-way comparison between the stored values if both @c lhs and @c rhs have
  /// values
  /// @pre @c T and @c U must be three-way-comparable
  template <
      typename Self,
      typename U,
      constraints<
          std::enable_if_t<std::is_same<Self, optional>::value>,
          std::enable_if_t<compare_three_way_supported_v<T, U>>> = nullptr>
  static constexpr auto three_way_compare(Self const& lhs, optional<U> const& rhs) noexcept -> strong_ordering {
    if (lhs) {
      if (rhs) {
        return compare_three_way{}(*lhs, *rhs);
      }
      return strong_ordering::greater;
    }
    if (rhs) {
      return strong_ordering::less;
    }
    return strong_ordering::equal;
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'fast_inequality_check' does not hide an identifier in
  // 'optional'"
  /// @brief Fast inequality check between an @c optional instance and a value, where there is not a fast inequality
  /// check between the two stored types.
  /// @tparam U The value type of the second @c optional
  /// @param lhs The first object to compare
  /// @return inequality_heuristic @c inequality_heuristic::definitely_not_equal if @c lhs is empty, @c
  /// inequality_heuristic::may_or_may_not_be_equal otherwise
  template <
      typename U,
      constraints<
          std::enable_if_t<!has_fast_inequality_check_v<T, U>>,
          std::enable_if_t<!std::is_same<U, nullopt_t>::value>> = nullptr>
  static constexpr auto fast_inequality_check(optional const& lhs, U const&) noexcept -> inequality_heuristic {
    return !lhs ? inequality_heuristic::definitely_not_equal : inequality_heuristic::may_be_equal_or_not_equal;
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'fast_inequality_check' does not hide an identifier in
  // 'optional'"
  /// @brief Fast inequality check between an @c optional instance and a value, where there is a fast inequality
  /// check between the stored type and the other value.
  /// @tparam U The value type of the second @c optional
  /// @param lhs The first object to compare
  /// @param rhs The first object to compare
  /// @return inequality_heuristic @c inequality_heuristic::definitely_not_equal if @c lhs is empty, delegeates to @c
  /// fast_inequality_check on the stored values otherwise
  template <
      typename U,
      constraints<
          std::enable_if_t<has_fast_inequality_check_v<T, U>>,
          std::enable_if_t<!std::is_same<U, nullopt_t>::value>> = nullptr>
  static constexpr auto fast_inequality_check(optional const& lhs, U const& rhs) noexcept -> inequality_heuristic {
    if (!lhs) {
      return inequality_heuristic::definitely_not_equal;
    }
    return T::fast_inequality_check(*lhs, rhs);
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'three_way_compare' does not hide an identifier in
  // 'optional'"
  /// @brief Three-way comparison between an @c optional instance and a value
  /// @tparam Self The type of the optional
  /// @tparam U the type of the value to compare against
  /// @param lhs The first object to compare
  /// @param rhs The first object to compare
  /// @return strong_ordering @c strong_ordering::less if @c lhs is empty, and the result of three-way comparison
  /// between the stored value of @c lhs and @c rhs otherwise
  /// @pre @c T and @c U must be three-way-comparable
  template <
      typename Self,
      typename U,
      constraints<
          std::enable_if_t<std::is_same<Self, optional>::value>,
          std::enable_if_t<!optional_detail::is_optional<U>::value>,
          std::enable_if_t<compare_three_way_supported_v<T, U>>> = nullptr>
  static constexpr auto three_way_compare(Self const& lhs, U const& rhs) noexcept -> strong_ordering {
    if (lhs) {
      return compare_three_way{}(*lhs, rhs);
    }
    return strong_ordering::less;
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Can only suppress this here, not in user code; std::vector does the same thing");
  ARENE_IGNORE_ALL("-Wsign-compare", "Can only suppress this here, not in user code; std::vector does the same thing");

  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: comparison operators are permitted"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  /// @brief equality comparison between an @c optional instance and a value
  /// @tparam Self The type of the optional
  /// @tparam U the type of the value to compare against the optional
  /// @param lhs The first object to compare
  /// @param rhs The first object to compare
  /// @return bool @c false if @c lhs is empty, and the result of an equality comparison
  /// between the stored value of @c lhs and @c rhs otherwise
  /// @pre @c T and @c U must be equality-comparable
  template <
      typename Self,
      typename U,
      constraints<
          std::enable_if_t<std::is_same<Self, optional>::value>,
          std::enable_if_t<!optional_detail::is_optional<U>::value>,
          std::enable_if_t<is_equality_comparable_v<T, U>>> = nullptr>
  friend constexpr auto operator==(Self const& lhs, U const& rhs) noexcept -> bool {
    if (lhs) {
      return *lhs == rhs;
    }
    return false;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2

  ARENE_IGNORE_END();

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: comparison operators are permitted"
  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  /// @brief equality comparison between two @c optional instances
  /// @tparam Self The type of the optional
  /// @tparam U the type stored in the other optional
  /// @param lhs The first object to compare
  /// @param rhs The first object to compare
  /// @return bool @c true if both @c lhs and @c rhs are empty, @c false if only one of @c lhs and @c rhs is empty, and
  /// the result of an equality comparison between the stored value of @c lhs and the stored value of @c rhs otherwise
  /// @pre @c T and @c U must be equality-comparable
  template <
      typename Self,
      typename U,
      constraints<
          std::enable_if_t<std::is_same<Self, optional>::value>,
          std::enable_if_t<!optional_detail::is_optional<U>::value>,
          std::enable_if_t<is_equality_comparable_v<T, U>>> = nullptr>
  friend constexpr auto operator==(Self const& lhs, optional<U> const& rhs) noexcept -> bool {
    if (lhs) {
      if (rhs) {
        return *lhs == *rhs;
      }
      return false;
    }
    if (rhs) {
      return false;
    }
    return true;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: these are public functions with external linkage"
  /// @brief equality comparison between an @c optional instance and an explicit @c nullopt
  /// @param lhs The @c optional instance to compare
  /// @return bool @c true if @c lhs is null (that is, !has_value()), @c false if it has a value
  friend constexpr auto operator==(optional const& lhs, nullopt_t) noexcept -> bool { return !lhs.has_value(); }

  /// @brief inequality comparison between an @c optional instance and an explicit @c nullopt
  /// @param lhs The @c optional instance to compare
  /// @return bool @c true if @c lhs has a value (that is, has_value()), @c false if it does not
  friend constexpr auto operator!=(optional const& lhs, nullopt_t) noexcept -> bool { return lhs.has_value(); }

  /// @brief inequality comparison between an explicit @c nullopt and an @c optional instance
  /// @param rhs The @c optional instance to compare
  /// @return bool @c true if @c rhs has a value (that is, has_value()), @c false if it does not
  friend constexpr auto operator!=(nullopt_t, optional const& rhs) noexcept -> bool { return rhs.has_value(); }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  /// @brief inequality comparison between two @c optional instances
  /// @tparam Self The type of the optional
  /// @tparam U the type stored in the other optional
  /// @param lhs The first object to compare
  /// @param rhs The first object to compare
  /// @return bool @c false if @c lhs is empty, and the result of an inequality comparison
  /// between the stored value of @c lhs and @c rhs otherwise
  /// @pre @c T and @c U must be equality-comparable
  template <
      typename Self,
      typename U,
      constraints<
          std::enable_if_t<std::is_same<Self, optional>::value>,
          std::enable_if_t<!optional_detail::is_optional<U>::value>,
          std::enable_if_t<is_equality_comparable_v<T, U>>> = nullptr>
  friend constexpr auto operator!=(Self const& lhs, optional<U> const& rhs) noexcept -> bool {
    return !(lhs == rhs);
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

 private:
  /// @brief Internal function to construct a new @c T in the storage.
  /// This overload handles the case where everything is @c noexcept
  /// @tparam Args The types of the arguments to construct from
  /// @param args The arguments to construct from
  /// Assumes @c has_value() is @c false
  template <
      typename... Args,
      constraints<std::enable_if_t<noexcept(storage_type(in_place, std::declval<Args>()...))>> = nullptr>
  void construct_from(Args&&... args) noexcept {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    this->storage().~storage_type();
    construct_at(&this->storage(), in_place, std::forward<Args>(args)...);
    this->set_has_value();
  }

  /// @brief Internal function to construct a new @c T in the storage.
  /// This overload handles the case where the constructor might throw
  /// @tparam Args The types of the arguments to construct from
  /// @param args The arguments to construct from
  /// Assumes @c has_value() is @c false
  template <
      typename... Args,
      constraints<std::enable_if_t<!noexcept(storage_type(in_place, std::declval<Args>()...))>> = nullptr>
  void construct_from(Args&&... args) {
    // parasoft-begin-suppress AUTOSAR-A7_1_7-a "False positive: no expression statement and identifier declaration on
    // same line"

    // If the constructor throws, we want to leave the optional in a valid state while letting the exception bubble
    // without using try/catch/throw in order to compile cleanly with -fno-exceptions. Therefore, we use a scope_guard
    // which resets the storage, and cancel it if construction is successful.
    auto make_empty_on_throw = on_scope_exit([this]() noexcept { construct_at(&this->storage()); });

    // parasoft-end-suppress AUTOSAR-A7_1_7-a

    construct_at(&this->storage(), in_place, std::forward<Args>(args)...);
    this->set_has_value();
    make_empty_on_throw.cancel();
  }

  /// @brief Internal function to implement assignment.
  /// @tparam OtherOptional The type of the optional being assigned from, including the value category
  /// @param other The optional being assigned from
  template <typename OtherOptional>
  constexpr void internal_do_assign(OtherOptional&& other) noexcept(
      std::is_nothrow_constructible<T, decltype(*std::forward<OtherOptional>(other))>::value &&
      std::is_nothrow_assignable<T&, decltype(*std::forward<OtherOptional>(other))>::value
  ) {
    if (this->has_value() && other.has_value()) {
      **this = *std::forward<OtherOptional>(other);
    } else if (this->has_value()) {
      this->reset();
    } else if (other.has_value()) {
      construct_from(*std::forward<OtherOptional>(other));
    } else {
      // do nothing
    }
  }

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Preserve type safety by allowing different instantiations of optional
  // to access private members without making them public"
  template <typename U>
  friend class optional;
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  /// @brief deduced-this helper for @c optional::value_or<F>(F&&) .
  /// @tparam Self The qualified result type to invoke with. Must satisfy @c decays_to<Self,optional>
  /// @tparam U The type of the default value. @c T must be constructible from @c U.
  /// @param self The @c optional to operate on
  /// @param default_value The value to return if the optional is null.
  /// @return T constructed from the held value if @c has_value() is @c true , else an instance constructed from
  ///         @c default_value .
  /// @note This must return a new instance rather than a reference, otherwise it risks dangling references if
  ///       @c default_value is a temporary.
  template <typename Self, typename U, constraints<std::enable_if_t<decays_to_v<Self, optional>>> = nullptr>
  static constexpr auto value_or(Self&& self, U&& default_value) noexcept(noexcept(T{default_value}
  ) && noexcept(T{*std::forward<Self>(self)})) -> T {
    return self.has_value() ? static_cast<T>(forward_like<Self>(std::forward<Self>(self).storage().value))
                            : static_cast<T>(std::forward<U>(default_value));
  }

  /// @brief deduced-this helper for @c optional::value_or_else<F>(F&&) .
  /// @tparam Self The qualified result type to invoke with. Must satisfy @c decays_to<Self,optional>
  /// @tparam F The type of the callable to invoke. Must have signature @c U(void) , where @c U is a type from which
  ///         @c T is constructible.
  /// @param self the instance of the result to use.
  /// @param default_generator The function to invoke to produce a default if optional is null.
  /// @return T constructed from the held value if @c has_value() is @c true , else an instance constructed from
  ///         invoking @c default_generator .
  template <typename Self, typename F, constraints<std::enable_if_t<decays_to_v<Self, optional>>> = nullptr>
  static constexpr auto value_or_else(Self&& self, F&& default_generator) noexcept(noexcept(T{
      ::arene::base::invoke(std::forward<F>(default_generator))
  }) && noexcept(T{*std::forward<Self>(self)})) -> T {
    return self.has_value() ? static_cast<T>(forward_like<Self>(std::forward<Self>(self).storage().value))
                            : static_cast<T>(::arene::base::invoke(std::forward<F>(default_generator)));
  }

  ///
  /// @brief deduced-this helper for @c optional::and_then<F>(F&&) .
  /// @tparam Self The qualified result type to invoke with. Must satisfy @c decays_to<Self,optional>
  /// @tparam F Type of the visiting functor. Must be invocable with @c value_type matching the const/ref qualification
  ///         of @c Self, and return any valid <c>optional&lt;U&gt;</c>.
  /// @param self the instance of the result to use.
  /// @param handle_value the functor to invoke if @c self.has_value() is @c true .
  /// @return U, where @c U is the return type of @c F and is an instantiation of @c optional , which will be:
  ///         * The result of invoking @c handle_value with the contents of @c value() if @c has_value() is @c true.
  ///         * A @c U instantiated with @c nullopt otherwise.
  template <typename Self, typename F, constraints<std::enable_if_t<decays_to_v<Self, optional>>> = nullptr>
  static constexpr auto and_then(Self&& self, F&& handle_value)
      -> invoke_result_t<F, decltype(forward_like<Self>(std::declval<Self&&>().storage().value))> {
    using f_optional_t = invoke_result_t<F, decltype(forward_like<Self>(std::forward<Self>(self).storage().value))>;
    if (self.has_value()) {
      return ::arene::base::invoke(
          std::forward<F>(handle_value),
          forward_like<Self>(std::forward<Self>(self).storage().value)
      );
    }
    return f_optional_t{};
  }

  ///
  /// @brief deduced-this helper for @c optional::or_else<F>(F&&) .
  /// @tparam Self The qualified result type to invoke with. Must satisfy @c decays_to<Self,optional>
  /// @tparam F Type of the visiting functor. Must be invocable with no arguments and return @c optional<T> .
  /// @param self the instance of the result to use.
  /// @param handle_null the functor to invoke if @c self.has_value() is @c false .
  /// @return optional<T> which will be:
  ///         * The result of invoking @c handle_value with the contents of @c value() if @c has_value() is @c false.
  ///         * A instance of @c optional<T> constructed from forwarding @c self .
  template <typename Self, typename F, constraints<std::enable_if_t<decays_to_v<Self, optional>>> = nullptr>
  static constexpr auto or_else(Self&& self, F&& handle_null) -> optional<T> {
    if (self.has_value()) {
      return {std::forward<Self>(self)};
    }
    return ::arene::base::invoke(std::forward<F>(handle_null));
  }

  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: it is not hiding anything"
  ///
  /// @brief deduced-this helper for @c optional::transform<F>(F&&) .
  /// @tparam Self The qualified result type to invoke with. Must satisfy @c decays_to<Self,optional>
  /// @tparam F Type of the visiting functor. Must be invocable with @c value_type matching the const/ref qualification
  ///         of @c Self. May return any valid @c value_type
  /// @param self the instance of the result to use.
  /// @param value_transformer the functor to invoke if @c self.has_value() is @c true .
  /// @return optional<U>, where @c U is the return type of @c F ,  which will contain:
  ///         * The result of invoking @c handle_value with the contents of @c value() if @c has_value() is @c true.
  ///         * @c nullopt otherwise.
  template <typename Self, typename F, constraints<std::enable_if_t<decays_to_v<Self, optional>>> = nullptr>
  static constexpr auto transform(Self&& self, F&& value_transformer)
      -> optional<invoke_result_t<F, decltype(forward_like<Self>(std::declval<Self&&>().storage().value))>> {
    using f_return_t = invoke_result_t<F, decltype(forward_like<Self>(std::forward<Self>(self).storage().value))>;
    using optional_t = optional<f_return_t>;
    if (self.has_value()) {
      return optional_t{::arene::base::invoke(
          std::forward<F>(value_transformer),
          forward_like<Self>(std::forward<Self>(self).storage().value)
      )};
    }
    return optional_t{};
  }
};
// parasoft-end-suppress AUTOSAR-A2_10_1-d "False positive: it is not hiding anything"

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a-2
// parasoft-end-suppress AUTOSAR-A7_1_5-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_OPTIONAL_HPP_
