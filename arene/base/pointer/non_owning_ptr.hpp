// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
///
/// @file non_owning_ptr.hpp
/// @brief Defines non_owning_ptr, a checked nullable pointer type.
///
// Copyright 2023, Toyota Motor Corporation
///
/// This provides the implementation of the facility specified by <a
/// href="https://stargate.jamacloud.com/perspective.req?docId=4277752&projectId=67">Arene_Eco_System-L5SW-1299</a>
///
///

// IWYU pragma: private, include "arene/base/pointer.hpp"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_NON_OWNING_PTR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_NON_OWNING_PTR_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/hash.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_pointer.hpp"
#include "arene/base/stdlib_choice/is_reference.hpp"
#include "arene/base/stdlib_choice/remove_const.hpp"
#include "arene/base/stdlib_choice/remove_pointer.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {

// forward declaration
template <typename T>
class non_owning_ptr;

///
/// @brief Trait that deduces if a type is an @c non_owning_ptr to any type
///
/// The test is after removing const and reference qualification
///
/// @tparam T The type to test if it is a @c non_owning_ptr
///
template <typename T>
constexpr bool is_non_owning_ptr_v = is_instantiation_of_v<std::remove_const_t<T>, non_owning_ptr>;

// parasoft-begin-suppress AUTOSAR-A14_5_1-a "False positive: non_owning_ptr has a copy constructor"
// parasoft-begin-suppress AUTOSAR-A12_1_5-a-2 "False positive: Delegating constructors are used"
///
/// @brief A non-owning, nullable pointer with nullptr checks on dereference
///
/// @tparam T The type pointed to by the pointer.
///
/// A @c non_owning_ptr<T> models @c T* with the following differences:
/// * Dereferencing @c non_owning_ptr<T> has defined behavior: an
///   @c ARENE_PRECONDITION violation.
/// * The arithmetic operations are deleted.
/// * @c delete 'ing the held object through the @c non_owning_ptr is
///   unsupported.
///
template <typename T>
class non_owning_ptr : full_ordering_operators_from_less_than_and_equals<non_owning_ptr<T>> {
  // TODO: Decide if this constraint should be retained.
  static_assert(
      !std::is_pointer<T>::value,
      "Cannot create an object pointer to a raw pointer. Perhaps you "
      "meant non_owning_ptr<T> for T* semantics, or "
      "non_owning_ptr<non_owning_ptr<T>> for T** semantics?"
  );
  static_assert(
      !std::is_reference<T>::value,
      "Cannot create an object pointer to a reference. Perhaps you "
      "meant non_owning_ptr<T> rather than non_owning_ptr<T&>?"
  );

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: Does not hide member of unrelated template"
  /// @brief Type alias for base class
  using ordering_base = full_ordering_operators_from_less_than_and_equals<non_owning_ptr<T>>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'pointer' does not hide anything"
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: Does not hide member of unrelated template"
  /// @brief The type of a pointer to @c T.
  using pointer = T*;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e
  // parasoft-end-suppress AUTOSAR-A2_10_1-d
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: Does not hide member of unrelated template"
  /// @brief The type of the element pointed to.
  using element_type = T;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'reference' does not hide anything"
  /// @brief The type of a reference to @c T.
  using reference = T&;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  ///
  /// @brief Default, trivial constructor.
  /// @post Behavior is equivalent to constructing a raw pointer: If explicit,
  ///       @c get() will return nullptr, otherwise the state is undefined.
  ///
  constexpr non_owning_ptr() noexcept = default;

  ///
  /// @brief Constructor from nullptr.
  /// @post @c get() returns nullptr.
  ///
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr non_owning_ptr(std::nullptr_t) noexcept
      : ordering_base{},
        ptr_(nullptr) {}

  ///
  /// @brief Constructor from a pointer to T
  /// @param new_ptr The pointer to hold.
  /// @post @c get() returns new_ptr
  ///
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr non_owning_ptr(pointer new_ptr) noexcept
      : ordering_base{},
        ptr_(new_ptr) {}

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: Base class initialized by delegating constructor
  ///
  /// @brief Constructor from non_owning_ptr convertible to the held type.
  ///
  /// @tparam U the element type of the @c non_owning_ptr to convert from. Must
  ///         be pointer-convertible to @c pointer.
  /// @param new_ptr The pointer to hold.
  /// @post @c get() returns @c new_ptr converted to @c pointer.
  ///
  template <
      typename U = element_type,
      constraints<std::enable_if_t<std::is_convertible<typename non_owning_ptr<U>::pointer, pointer>::value>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr non_owning_ptr(non_owning_ptr<U> new_ptr) noexcept
      : non_owning_ptr(new_ptr.get()) {}

  ///
  /// @brief Constructor from a raw pointer convertible to the held type.
  ///
  /// @tparam U the pointer type to convert from. Must be convertible to
  ///        @c pointer .
  /// @param new_ptr The pointer to hold.
  /// @post @c get() returns @c new_ptr converted to @c pointer .
  ///
  template <
      typename U = pointer,
      constraints<
          std::enable_if_t<std::is_pointer<U>::value>,
          std::enable_if_t<std::is_convertible<U, pointer>::value>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr non_owning_ptr(U new_ptr) noexcept
      : non_owning_ptr(static_cast<pointer>(new_ptr)) {}

  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  ///
  /// @brief Accesses the held pointer directly
  ///
  /// @return pointer The held pointer
  ///
  ARENE_NODISCARD constexpr auto get() const noexcept -> pointer { return ptr_; }

  ///
  /// @brief Resets the held pointer to a new value.
  ///
  /// @tparam U the pointer type to convert from. Must be convertible to
  ///         @c pointer .
  /// @param new_ptr The pointer to reset from. Defaults to <c>nullptr</c>.
  /// @post @c get() returns @c new_ptr converted to @c pointer .
  ///
  template <
      typename U = pointer,
      constraints<
          std::enable_if_t<std::is_pointer<U>::value>,
          std::enable_if_t<std::is_convertible<U, pointer>::value>> = nullptr>
  constexpr void reset(U new_ptr = nullptr) {
    ptr_ = static_cast<pointer>(new_ptr);
  }

  ///
  /// @brief Dereference operator
  ///
  /// @pre <c>get() != nullptr</c>, otherwise an @c ARENE_PRECONDITION
  ///      violation.
  /// @return reference_type A reference to the object pointed to
  ///
  ARENE_NODISCARD constexpr auto operator*() const noexcept -> reference { return *checked_get(); }

  ///
  /// @brief Arrow operator
  ///
  /// @pre <c>get() != nullptr</c>, otherwise an @c ARENE_PRECONDITION
  ///      violation.
  /// @return pointer The held pointer
  ///
  ARENE_NODISCARD constexpr auto operator->() const noexcept -> pointer { return checked_get(); }

  ///
  /// @brief Conversion operator to pointer-like type
  ///
  /// @tparam U the type to convert to. Must be convertible from
  ///         @c pointer .
  /// @return U The held pointer, converted to @c U
  ///
  template <
      typename U,
      constraints<std::enable_if_t<!is_non_owning_ptr_v<U>>, std::enable_if_t<std::is_convertible<pointer, U>::value>> =
          nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr explicit operator U() const noexcept {
    return {get()};
  }

  ///
  /// @brief Conversion to bool.
  ///
  /// @return true If <c>get() != nullptr</c>.
  /// @return false If <c>get() == nullptr</c>.
  ///
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  ARENE_NODISCARD constexpr explicit operator bool() const noexcept { return get() != nullptr; }

  ///
  /// @brief Boolean negation operator.
  ///
  /// @return true If <c>get() == nullptr</c>
  /// @return false If <c>get() != nullptr</c>
  ///
  ARENE_NODISCARD constexpr auto operator!() const noexcept -> bool { return !static_cast<bool>(*this); }

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: Function is namespace scope and used in other
  // translation units"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: comparison operators are permitted"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  /// @brief 6-way comparison is equivalent to comparing raw pointers.
  /// @param lhs The first value to compare
  /// @param rhs The second value to compare
  /// @return The result of the comparison of the underlying pointers
  /// @{
  ARENE_NODISCARD friend constexpr auto operator==(non_owning_ptr const& lhs, non_owning_ptr const& rhs) noexcept
      -> bool {
    return lhs.get() == rhs.get();
  }

  ARENE_NODISCARD friend constexpr auto operator<(non_owning_ptr const& lhs, non_owning_ptr const& rhs) noexcept
      -> bool {
    return lhs.get() < rhs.get();
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  /// @brief Pointer arithmetic is explicitly deleted
  /// @{
  template <typename I>
  constexpr auto operator[](I) = delete;
  template <typename I>
  constexpr auto operator[](I) const = delete;
  template <typename I>
  constexpr auto operator+(I const&) const -> non_owning_ptr = delete;
  template <typename I>
  constexpr auto operator-(I const&) const -> non_owning_ptr = delete;
  constexpr auto operator++() -> non_owning_ptr& = delete;
  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "False positive: postincrement requires an int parameter"
  constexpr auto operator++(int) -> non_owning_ptr& = delete;
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2
  constexpr auto operator--() -> non_owning_ptr& = delete;
  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "False positive: postdecrement requires an int parameter"
  constexpr auto operator--(int) -> non_owning_ptr& = delete;
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2

 private:
  ///
  /// @brief Equivalent to @c get() but with a precondition check
  /// @pre <c>get() != nullptr</c>, otherwise an @c ARENE_PRECONDITION
  /// violation.
  /// @return pointer The held pointer
  ///
  ARENE_NODISCARD constexpr auto checked_get() const -> pointer {
    ARENE_PRECONDITION(ptr_);
    return ptr_;
  }

  /// @brief The held pointer
  pointer ptr_;
};
// parasoft-end-suppress AUTOSAR-A12_1_5-a-2
// parasoft-end-suppress AUTOSAR-A14_5_1-a

///
/// @brief @c non_owning_ptr factory to allow template deduction.
///
/// @tparam T The raw pointer type to construct the @c non_owning_ptr from
/// @param ptr The pointer to construct the @c non_owning_ptr from.
/// @return Equivalent to @c non_owning_ptr<std::remove_pointer_t<T>>{ptr}.
///
template <typename T, constraints<std::enable_if_t<std::is_pointer<T>::value>> = nullptr>
ARENE_NODISCARD constexpr auto make_non_owning_ptr(T ptr) noexcept {
  return non_owning_ptr<std::remove_pointer_t<T>>(ptr);
}

}  // namespace base
}  // namespace arene

// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "False positive: specialization of standard templates is permitted"
// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "False positive: specialization of standard templates is permitted"
namespace std {
// parasoft-begin-suppress AUTOSAR-A11_0_2-a-2 "False positive: must follow the primary template"
/// @brief @c std::hash specialization for @c non_owning_ptr.
/// The hash is equivalent to the hash of the underlying pointer.
///
template <typename T>
struct hash<::arene::base::non_owning_ptr<T>> {
  /// @brief Calculate the hash of the pointer
  /// @param ptr The pointer to calculate the hash of
  /// @return The hash
  constexpr auto operator()(::arene::base::non_owning_ptr<T> const& ptr) const noexcept -> size_t {
    return hash<typename arene::base::non_owning_ptr<T>::pointer>()(ptr.get());
  }
};
// parasoft-end-suppress AUTOSAR-A11_0_2-a-2

}  // namespace std
// parasoft-end-suppress CERT_CPP-DCL58-a-2
// parasoft-end-suppress AUTOSAR-A17_6_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_NON_OWNING_PTR_HPP_
