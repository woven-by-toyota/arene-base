// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_NON_NULL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_NON_NULL_HPP_

// IWYU pragma: private, include "arene/base/pointer.hpp"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/pointer/detail/to_address.hpp"
#include "arene/base/pointer/detail/type_traits.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/decay.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/hash.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_pointer.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/pointer_traits.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
#include "arene/base/utility/swap.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {

// forward declaration
template <typename P>
class non_null;

///
/// @brief Trait that deduces if a type is an @c non_null to any type
///
/// The test is after removing const and reference qualification
///
/// @tparam T The type to test if it is a @c non_null
///
template <typename T>
constexpr bool is_non_null_v = is_instantiation_of_v<remove_cvref_t<T>, non_null>;

namespace operators_detail {
/// @brief Type trait to check if we can provide a reversed operator== for this pair of types (breaks an infinite loop)
///
/// The @c non_null template already provides the reversed operators
///
/// @tparam Self The pointee type of the @c non_null instance which is inheriting from this operator mixin
/// @tparam Other The pointer type of the @c non_null instance which we're trying to compare with (the LHS of a
/// *reversed* comparison)
template <typename Self, typename Other>
constexpr bool eligible_for_reversed_equals_with_v<non_null<Self>, non_null<Other>> = false;
}  // namespace operators_detail

// parasoft-begin-suppress AUTOSAR-A12_1_5-a-2 "False positive: No duplication between constructors"
///
/// @brief A pointer type which is never @c nullptr
///
/// @tparam P The type of the pointer to wrap.
///
/// A wrapper type around a "pointer type" which maintains the invariant that the pointer never be @c nullptr . It is
/// considered a "malformed program" to attempt to construct a @c non_null with a pointer which is @c nullptr , and thus
/// an @c ARENE_PRECONDITION violation.
///
/// @c non_null<P> models the wrapped pointer type. This means the following properties are true:
/// - It is copyable if the wrapped pointer is copyable.
/// - It is movable if the wrapped pointer is movable.
/// - It is implicitly convertible to (a reference to) the wrapped pointer.
/// - It is 6-way comparable with the same semantics as the wrapped pointer.
/// - The dereference/arrow operators are equivalent to dereferencing the
///   wrapped pointer.
/// - Calling @c get() is equivalent to calling @c get() on the wrapped pointer if the wrapped pointer is a smart
///   pointer. See non_null<P>::get() for details.
/// - Calling @c reset() is equivalent to calling @c reset() on the wrapped pointer, if the wrapped pointer is a smart
///   pointer. See non_null<P>::reset() for details.
///
/// To maintain the invariant that the pointer is never @c nullptr , it is not possible to obtain a non-const lvalue
/// reference to the wrapped pointer. If @c P is not a raw pointer, it is possible to obtain an rvalue reference to the
/// wrapped pointer via @c unwrap()&& or by casting from an rvalue-qualified @c non_null .
///
/// @warning In order to be useful in real programs, a moved-from @c non_null which contains anything other than a raw
/// pointer may become @c nullptr . Thus the only valid operations which may be performed on a moved-from @c non_null is
/// to either assign/reset it to a new value, or let it be destroyed.
///
template <typename P>
class non_null : generic_ordering_from_three_way_compare_and_equals<non_null<P>> {
  static_assert(pointer::detail::is_pointer_like_v<P>, "non_null is only defined for pointer-like types.");

  /// @brief Type alias for base class
  using ordering_base = generic_ordering_from_three_way_compare_and_equals<non_null<P>>;

 public:
  /// @brief The pointer type being wrapped by the @c non_null .
  using held_pointer = P;
  /// @brief The type of the element pointed to.
  using element_type = typename std::pointer_traits<held_pointer>::element_type;
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'pointer' does not hide anything"
  /// @brief The type of a raw pointer to the element pointed to by the @c non_null .
  using pointer = decltype(::arene::base::pointer::detail::to_address(std::declval<held_pointer>()));
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  /// @brief @c non_null cannot be default constructed
  constexpr non_null() = delete;
  /// @brief @c non_null cannot be constructed from a literal @c nullptr .
  constexpr explicit non_null(std::nullptr_t) = delete;

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: No forwarding reference"
  /// @brief Default copy constructor
  constexpr non_null(non_null const&) = default;
  /// @brief Default move constructor
  constexpr non_null(non_null&&) = default;
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2
  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: No forwarding reference"
  /// @brief Default copy assignment
  constexpr auto operator=(non_null const&) -> non_null& = default;
  /// @brief Default move assignment
  constexpr auto operator=(non_null&&) -> non_null& = default;
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  /// @brief Default destructor
  ~non_null() = default;

  ///
  /// @brief Construction from a pointer type
  ///
  /// @tparam U The type of the pointer to construct from. Must be @c convertible to @c held_pointer
  /// @param ptr_value The pointer to construct from.
  /// @pre @c ptr_value must not be @c nullptr , or else @c ARENE_PRECONDITION violation.
  /// @post @c get() will return a pointer to the element pointed to by @c ptr_value .
  ///
  template <
      typename U,
      constraints<
          std::enable_if_t<!is_non_null_v<U>, std::enable_if_t<std::is_constructible<held_pointer, U>::value>>> =
          nullptr>
  // NOLINTNEXTLINE(bugprone-forwarding-reference-overload) The check cannot see through constraints.
  constexpr explicit non_null(U&& ptr_value) noexcept(std::is_nothrow_constructible<held_pointer, U>::value)
      : ordering_base{},
        ptr_(std::forward<U>(ptr_value)) {
    ARENE_PRECONDITION(ptr_ != nullptr);
  }

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: Constrained via SFINAE"
  ///
  /// @brief Copy constructor from @c non_null
  ///
  /// @tparam U Type of the held pointer of the @c non_null to copy
  /// @param copy_from The @c non_null to copy
  ///
  template <
      typename U = held_pointer,
      constraints<std::enable_if_t<std::is_constructible<held_pointer, U const&>::value>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions) The check can't tell this is a copy ctor
  constexpr non_null(non_null<U> const& copy_from) noexcept(std::is_nothrow_constructible<held_pointer, U const&>::value
  )
      : ordering_base{},
        ptr_(copy_from.unwrap()) {}
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  ///
  /// @brief Move constructor from @c non_null
  ///
  /// @tparam U Type of the held pointer of the @c non_null to move
  /// @param move_from The @c non_null to move
  ///
  template <
      typename U = held_pointer,
      constraints<std::enable_if_t<std::is_constructible<held_pointer, U&&>::value>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions) The check can't tell this a move ctor
  constexpr non_null(non_null<U>&& move_from) noexcept(std::is_nothrow_constructible<held_pointer, U&&>::value)
      : ordering_base{},
        ptr_(std::move(move_from).unwrap()) {}

  ///
  /// @brief Assignment from a pointer type
  ///
  /// @tparam U The type of the pointer to assign from. Must be @c assignable to @c held_pointer
  /// @param new_ptr The pointer to assign from.
  /// @pre @c new_ptr must not be @c nullptr , or else @c ARENE_PRECONDITION violation.
  ///
  template <
      typename U = held_pointer,
      constraints<std::enable_if_t<!is_non_null_v<U>>, std::enable_if_t<std::is_assignable<held_pointer&, U>::value>> =
          nullptr>
  constexpr auto operator=(U&& new_ptr) noexcept(std::is_nothrow_assignable<held_pointer&, U>::value) -> non_null& {
    ARENE_PRECONDITION(new_ptr != nullptr);
    ptr_ = std::forward<U>(new_ptr);
    return *this;
  }

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "Constrained via SFINAE, permitted by A13-3-1 Permit #1"
  ///
  /// @brief Copy assignment from @c non_null
  ///
  /// @tparam U Type of the held pointer of the @c non_null to copy. Must be @c assignable to @c held_pointer .
  /// @param copy_from The @c non_null to copy
  ///
  template <
      typename U = held_pointer,
      constraints<std::enable_if_t<std::is_assignable<held_pointer&, U const&>::value>> = nullptr>
  constexpr auto operator=(non_null<U> const& copy_from
  ) noexcept(std::is_nothrow_assignable<held_pointer&, U const&>::value) -> non_null& {
    ptr_ = copy_from.unwrap();
    return *this;
  }
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  ///
  /// @brief Move assignment from @c non_null
  ///
  /// @tparam U Type of the held pointer of the @c non_null to move. Must be @c assignable to @c held_pointer .
  /// @param move_from The @c non_null to move
  ///
  template <
      typename U = held_pointer,
      constraints<std::enable_if_t<std::is_assignable<held_pointer&, U&&>::value>> = nullptr>
  constexpr auto operator=(non_null<U>&& move_from) noexcept(std::is_nothrow_assignable<held_pointer&, U&&>::value)
      -> non_null& {
    ptr_ = std::move(move_from.unwrap());
    return *this;
  }

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  ///
  /// @brief Gets a pointer to the pointed-to element
  /// @return pointer A pointer to the pointed-to element. The type is
  ///         @c pointer .
  /// @invariant The returned pointer will never be equal to @c nullptr .
  ///
  /// Behavior is as if @c non_null<P> models @c P . This means that:
  /// - If @c P is a raw pointer, then returns the pointer.
  /// - If @c P is a "smart" pointer, then forwards the return from calling
  ///   @c get() on the smart pointer.
  /// Examples:
  /// @code{cpp}
  /// decltype(non_null<int*>.get()) == int*;
  /// decltype(non_null<std::unique_ptr<int*>>.get()) == int*;
  /// @endcode
  ///
  /// @{
  template <typename U = held_pointer, constraints<std::enable_if_t<std::is_pointer<U>::value>> = nullptr>
  ARENE_NODISCARD constexpr auto get() const noexcept -> held_pointer {
    // A raw pointer can never become null as we can't move from it, so we don't need to check the invariant
    return ptr_;
  }
  template <typename U = held_pointer, constraints<std::enable_if_t<!std::is_pointer<U>::value>> = nullptr>
  ARENE_NODISCARD constexpr auto get() const noexcept -> decltype(auto) {
    // A fancy pointer can become null if used-after-move, so we have to check
    ARENE_INVARIANT(ptr_ != nullptr);
    return ::arene::base::pointer::detail::to_address(ptr_);
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  ///
  /// @brief Gets the held pointer backing the @c non_null
  ///
  /// @return A reference to the underlying pointer.
  /// @{
  // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: returns const reference"
  // parasoft-begin-suppress AUTOSAR-M9_3_1-a-2 "False positive: returns const reference"
  ARENE_NODISCARD constexpr auto unwrap() const& noexcept -> held_pointer const& { return ptr_; }
  // parasoft-end-suppress AUTOSAR-A9_3_1-a-2
  // parasoft-end-suppress AUTOSAR-M9_3_1-a-2

  template <typename U = held_pointer, constraints<std::enable_if_t<!std::is_pointer<U>::value>> = nullptr>
  ARENE_NODISCARD constexpr auto unwrap() && noexcept -> held_pointer&& {
    return std::move(ptr_);
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2

  ///
  /// @brief Re-assigns the @c non_null to a new held pointer.
  ///
  /// The reset is performed using @c held_pointer::reset() .
  ///
  /// @tparam U The type of pointer to rest with. Must be valid to call @c held_pointer::rest(U) .
  /// @param reset_to The pointer to reset the @c non_null to.
  /// @pre @c ptr must not be @c nullptr , or else @c ARENE_PRECONDITION violation.
  /// @post The state of the object is as if it had been constructed by @c ptr .
  /// @note does not participate in overload resolution if @c held_pointer::reset(U) is not well formed.
  ///
  template <
      typename U = held_pointer,
      constraints<std::enable_if_t<arene::base::pointer::detail::has_reset<held_pointer&, U>>> = nullptr>
  constexpr void reset(U&& reset_to) noexcept(noexcept(std::declval<held_pointer>().reset(std::declval<U>()))) {
    ARENE_PRECONDITION(reset_to != nullptr);
    ptr_.reset(std::forward<U>(reset_to));
  }

  ///
  /// @brief Re-assigns the @c non_null to a new held pointer.
  ///
  /// The reset is performed by assignment to the held pointer.
  ///
  /// @tparam U The type of pointer to rest with. Must be @c assignable to @c held_pointer
  /// @param reset_to The pointer to reset the @c non_null to.
  /// @pre @c ptr must not be @c nullptr , or else @c ARENE_PRECONDITION violation.
  /// @post The state of the object is as if it had been constructed by @c ptr .
  /// @note does not participate in overload resolution if @c held_pointer::reset(U) is well formed.
  ///
  template <
      typename U = held_pointer,
      constraints<
          std::enable_if_t<!arene::base::pointer::detail::has_reset<held_pointer&, U>>,
          std::enable_if_t<std::is_assignable<held_pointer&, U>::value>> = nullptr>
  constexpr void reset(U&& reset_to) noexcept(std::is_nothrow_assignable<held_pointer&, U>::value) {
    ARENE_PRECONDITION(reset_to != nullptr);
    ptr_ = std::forward<U>(reset_to);
  }

  /// @brief Resetting to a @c nullptr literal is not supported.
  constexpr void reset(std::nullptr_t) noexcept = delete;

  ///
  /// @brief Swaps this with another @c non_null .
  ///
  /// @tparam U Must satisfy @c arene::base::is_swappable_v
  /// @param swap_with The @c non_null to swap with.
  /// @post @c this is equal to the original @c swap_with, and @c swap_with is equal to the original @c this .
  ///
  template <typename U = P, constraints<std::enable_if_t<is_swappable_v<P>>> = nullptr>
  constexpr void swap(non_null& swap_with) noexcept(is_nothrow_swappable_v<P>) {
    ::arene::base::swap(ptr_, swap_with.ptr_);
  }

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: static cannot be applied, not a member function"
  /// @brief Swaps two @c non_null .
  ///
  /// @tparam U Must satisfy @c arene::base::is_swappable_v
  /// @param lhs The left-hand @c non_null to swap.
  /// @param rhs The right-hand @c non_null to swap.
  /// @post Equivalent to calling @c lhs.swap(rhs)
  /// @see arene::base::non_null::swap
  ///
  template <typename U = P, constraints<std::enable_if_t<is_swappable_v<P>>> = nullptr>
  friend constexpr void swap(non_null& lhs, non_null& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  ///
  /// @brief Arrow operator
  ///
  /// @return pointer A pointer to the pointed-to element
  /// @invariant The returned pointer will never be equal to @c nullptr .
  /// @note Only exists if @c held_pointer is not @c void* .
  ///
  template <typename U = element_type, typename = std::enable_if_t<!std::is_same<std::remove_cv_t<U>, void>::value>>
  ARENE_NODISCARD constexpr auto operator->() const noexcept -> pointer {
    return get();
  }

  ///
  /// @brief Dereference operator
  ///
  /// @return element_type& A reference to the pointed-to element.
  /// @invariant The returned pointer will never be equal to @c nullptr .
  /// @note Only exists if @c held_pointer is not @c void* .
  ///
  template <typename U = element_type, typename = std::enable_if_t<!std::is_same<std::remove_cv_t<U>, void>::value>>
  ARENE_NODISCARD constexpr auto operator*() const noexcept -> decltype(auto) {
    return *get();
  }

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  ///
  /// @brief Conversion to @c bool , equivalent to a @c != comparison to @c nullptr.
  ///
  /// @return true @c non_null can never contain @c nullptr , so the return is always @c true .
  ///
  /// @{
  template <typename U = held_pointer, constraints<std::enable_if_t<!std::is_pointer<U>::value>> = nullptr>
  ARENE_NODISCARD explicit constexpr operator bool() const noexcept {
    return ptr_ != nullptr;
  }
  template <typename U = held_pointer, constraints<std::enable_if_t<std::is_pointer<U>::value>> = nullptr>
  ARENE_NODISCARD explicit constexpr operator bool() const noexcept {
    return true;
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2

  ///
  /// @brief Boolean negation operator, equivalent to a @c == comparison to @c nullptr.
  ///
  /// @return false @c non_null can never contain @c nullptr , so the return is always @c false .
  ///
  ARENE_NODISCARD constexpr auto operator!() const noexcept -> bool { return !(static_cast<bool>(*this)); }

  // parasoft-begin-suppress AUTOSAR-A13_5_2-a-2 "Implicit conversion to the underlying pointer type is part of the API"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A13_2_3-a-2 "False positive: This is not a relational operator"
  // parasoft-begin-suppress AUTOSAR-M9_3_1-a "False positive: This class is a container of the value"
  // parasoft-begin-suppress AUTOSAR-A9_3_1-a "False positive: This class is a container of the value"
  ///
  /// @brief Implicit conversion operator to the underlying pointer
  ///
  /// @return The underlying pointer. If @c held_pointer is a raw pointer or sufficiently trivial type, this will be a
  /// by-value return. Otherwise, it will be a reference.
  /// @{
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  ARENE_NODISCARD constexpr operator held_pointer const&() const& noexcept { return ptr_; }
  // parasoft-end-suppress AUTOSAR-A13_2_3-a-2

  // parasoft-begin-suppress AUTOSAR-A13_2_3-a "False positive: Not a relational operator"
  template <typename U = held_pointer, constraints<std::enable_if_t<!std::is_pointer<U>::value>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  ARENE_NODISCARD constexpr operator held_pointer&&() && noexcept {
    return std::move(ptr_);
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-A9_3_1-a
  // parasoft-end-suppress AUTOSAR-M9_3_1-a
  // parasoft-end-suppress AUTOSAR-A13_2_3-a
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2
  // parasoft-end-suppress AUTOSAR-A13_5_2-a-2

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  ///
  /// @brief Explicit conversion operator to @c U
  ///
  /// @tparam U The type to convert the @c non_null to. @c held_pointer must be @c convertible to @c U .
  /// @return U An instance of the converted-to type produced by applying @c static_cast<U>() to the held pointer.
  /// @{
  template <
      typename U,
      constraints<
          std::enable_if_t<!is_non_null_v<U>>,
          std::enable_if_t<std::is_constructible<U, held_pointer const&>::value>> = nullptr>
  ARENE_NODISCARD explicit constexpr operator U(
  ) const& noexcept(std::is_nothrow_constructible<U, held_pointer const&>::value) {
    return static_cast<U>(ptr_);
  }

  // parasoft-begin-suppress AUTOSAR-A13_2_3-a "False positive: Not a relational operator"
  template <
      typename U,
      constraints<
          std::enable_if_t<!is_non_null_v<U>>,
          std::enable_if_t<!std::is_pointer<U>::value>,
          std::enable_if_t<std::is_constructible<U, held_pointer&&>::value>> = nullptr>
  ARENE_NODISCARD explicit constexpr operator U() && noexcept(std::is_nothrow_constructible<U, held_pointer&&>::value) {
    return static_cast<U>(std::move(ptr_));
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-A13_2_3-a
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: static cannot be applied, not a member function"
  // parasoft-begin-suppress AUTOSAR-A13_5_5-b "Mixed comparisons permitted by A13-5-5 Permit #1"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "False positive: A11-3-1 allows friend for comparisons"
  /// @brief Equality comparison between two @c non_null instances. The result is equivalent to comparing the held
  /// pointers
  /// @tparam OtherType The type held in the other @c non_null instance being compared to. @c held_pointer must be @c
  /// comparable to @c OtherType .
  /// @param lhs The first value being compared
  /// @param rhs The second value being compared
  /// @return The result of the comparison of the held pointer of @c lhs to the held pointer of @c rhs
  template <
      typename OtherType,
      constraints<std::enable_if_t<is_equality_comparable_v<held_pointer const&, OtherType const&>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator==(non_null const& lhs, non_null<OtherType> const& rhs) noexcept
      -> bool {
    return lhs.unwrap() == rhs.unwrap();
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a
  // parasoft-end-suppress AUTOSAR-A13_5_5-b
  // parasoft-end-suppress AUTOSAR-M3_3_2-a

  // parasoft-begin-suppress AUTOSAR-A13_5_5-b "Mixed comparisons permitted by A13-5-5 Permit #1"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "False positive: A11-3-1 allows friend for comparisons"
  /// @brief Equality comparison of @c non_null against another value. The result is equivalent to comparing the held
  /// pointer against that other value
  /// @tparam NonNullType Must be @c non_null This is a constrained template parameter to avoid implicit conversions and
  /// ambiguous overload resolution.
  /// @tparam OtherType The type to compare the @c non_null to. It must not be a instantiation of @c non_null. @c
  /// held_pointer must be @c comparable to @c OtherType .
  /// @param lhs The first value being compared
  /// @param rhs The second value being compared
  /// @return The result of the comparison of the held pointer of @c lhs to the other value
  template <
      typename NonNullType,
      typename OtherType,
      constraints<
          std::enable_if_t<std::is_same<NonNullType, non_null>::value>,
          std::enable_if_t<!is_non_null_v<OtherType>>,
          std::enable_if_t<is_equality_comparable_v<held_pointer const&, OtherType const&>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator==(NonNullType const& lhs, OtherType const& rhs) noexcept -> bool {
    return lhs.unwrap() == rhs;
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a
  // parasoft-end-suppress AUTOSAR-A13_5_5-b

  // parasoft-begin-suppress AUTOSAR-A13_5_5-b "Mixed comparisons permitted by A13-5-5 Permit #1"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "False positive: A11-3-1 allows friend for comparisons"
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a "This is an inline function used in multiple translation units"
  /// @brief Equality comparison of @c non_null against a null pointer.
  /// @param lhs The first value being compared
  /// @param rhs The second value being compared
  /// @return The result of this check is always @c false unless the @c non_null instance has been moved from, and wraps
  /// a movable smart pointer like @c std::unique_ptr
  ARENE_NODISCARD friend constexpr auto operator==(non_null const& lhs, std::nullptr_t const rhs) noexcept -> bool {
    std::ignore = rhs;
    return !lhs;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a
  // parasoft-end-suppress AUTOSAR-A11_3_1-a
  // parasoft-end-suppress AUTOSAR-A13_5_5-b

  /// @brief Three-way comparison between two @c non_null instances. The result is equivalent to comparing the held
  /// pointers
  /// @tparam OtherType The type held in the other @c non_null instance being compared to. @c held_pointer must be @c
  /// three-way-comparable to @c OtherType .
  /// @param lhs The first value being compared
  /// @param rhs The second value being compared
  /// @return The result of the three-way comparison of the held pointer of @c lhs to the held pointer of @c rhs
  template <
      typename OtherType,
      constraints<std::enable_if_t<compare_three_way_supported_v<held_pointer const&, OtherType const&>>> = nullptr>
  ARENE_NODISCARD static constexpr auto three_way_compare(non_null const& lhs, non_null<OtherType> const& rhs) noexcept
      -> strong_ordering {
    return compare_three_way{}(lhs.unwrap(), rhs.unwrap());
  }

  /// @brief Three-way comparison of @c non_null against another value. The result is equivalent to comparing the held
  /// pointer against that other value
  /// @tparam OtherType The type to compare the @c non_null to. It must not be a instantiation of @c non_null. @c
  /// held_pointer must be @c comparable to @c OtherType .
  /// @param lhs The first value being compared
  /// @param rhs The second value being compared
  /// @return The result of the three-way comparison of the held pointer of @c lhs to @c rhs
  template <
      typename OtherType,
      constraints<
          std::enable_if_t<compare_three_way_supported_v<held_pointer const&, OtherType const&>>,
          std::enable_if_t<!is_non_null_v<OtherType>>> = nullptr>
  ARENE_NODISCARD static constexpr auto three_way_compare(non_null const& lhs, OtherType const& rhs) noexcept
      -> strong_ordering {
    return compare_three_way{}(lhs.unwrap(), rhs);
  }

  /// @brief Three-way comparison of @c non_null against a null pointer.
  /// @tparam Dummy Dummy template parameter that is the same as @c held_pointer to enable SFINAE
  /// @param lhs The @c non_null value being compared
  /// @return The result of this check is always @c strong_ordering::greater unless the @c non_null instance has been
  /// moved from, and wraps a movable smart pointer like @c std::unique_ptr
  template <typename Dummy = held_pointer, constraints<std::enable_if_t<!std::is_pointer<Dummy>::value>> = nullptr>
  ARENE_NODISCARD static constexpr auto three_way_compare(non_null const& lhs, std::nullptr_t const) noexcept
      -> strong_ordering {
    if (!lhs) {
      return strong_ordering::equal;
    }
    return strong_ordering::greater;
  }

  /// @brief Three-way comparison of @c non_null against a null pointer.
  /// @tparam Dummy Dummy template parameter that is the same as @c held_pointer to enable SFINAE
  /// @return The result of this check is always @c strong_ordering::greater
  template <typename Dummy = held_pointer, constraints<std::enable_if_t<std::is_pointer<Dummy>::value>> = nullptr>
  ARENE_NODISCARD static constexpr auto three_way_compare(non_null const&, std::nullptr_t const) noexcept
      -> strong_ordering {
    return strong_ordering::greater;
  }

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  /// @brief Pointer arithmetic is explicitly deleted
  /// @{
  template <typename I>
  constexpr auto operator[](I) = delete;
  template <typename I>
  constexpr auto operator[](I) const = delete;
  template <typename I>
  constexpr auto operator+(I const&) const -> non_null = delete;
  template <typename I>
  constexpr auto operator-(I const&) const -> non_null = delete;
  constexpr auto operator++() -> non_null& = delete;
  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "False positive: postincrement requires an int parameter"
  constexpr auto operator++(int) -> non_null& = delete;
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2
  constexpr auto operator--() -> non_null& = delete;
  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "False positive: postdecrement requires an int parameter"
  constexpr auto operator--(int) -> non_null& = delete;
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2

 private:
  /// @brief The held pointer which must not be null
  held_pointer ptr_;
};
// parasoft-end-suppress AUTOSAR-A12_1_2-a-2

///
/// @brief Factory function to deduce the type of a @c non_null
///
/// @tparam P The type of the pointer
/// @param ptr The pointer to construct the @c non_null from
/// @return non_null<P> The resulting non-null produced from the input pointer
template <typename P>
ARENE_NODISCARD constexpr auto make_non_null(P&& ptr) noexcept -> non_null<std::decay_t<P>> {
  return non_null<std::decay_t<P>>(std::forward<P>(ptr));
}
/// @brief Deleted overload for not constructing a non-null pointer from a null pointer
/// @param ptr The null pointer
constexpr void make_non_null(std::nullptr_t ptr) = delete;

///
/// @brief Alias for a @c non_null which holds a raw pointer
///
/// @tparam T The type of the pointed to element
///
template <typename T>
using non_null_ptr = non_null<T*>;

}  // namespace base
}  // namespace arene

// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "False positive: specialization of standard templates is permitted"
// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "False positive: specialization of standard templates is permitted"
namespace std {
// parasoft-begin-suppress AUTOSAR-A11_0_2-a-2 "False positive: must follow the primary template"
///
/// @brief @c std::hash specialization for @c non_null.
///
/// The hash is equivalent to the hash of the held pointer.
/// @tparam P the type of the underlying pointer
///
template <typename P>
struct hash<::arene::base::non_null<P>> {
  /// @brief Hash the pointer
  /// @param ptr The pointer to hash
  /// @return The hashed value
  constexpr auto operator()(::arene::base::non_null<P> const& ptr) const noexcept -> size_t {
    return hash<typename ::arene::base::non_null<P>::held_pointer>{}(ptr);
  }
};
// parasoft-end-suppress AUTOSAR-A11_0_2-a-2

}  // namespace std
// parasoft-end-suppress CERT_CPP-DCL58-a-2
// parasoft-end-suppress AUTOSAR-A17_6_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_NON_NULL_HPP_
