// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file optional_base.hpp
/// @brief details implementation of arene::base::optional to handle constexpr compatibility
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_DETAIL_OPTIONAL_BASE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_DETAIL_OPTIONAL_BASE_HPP_

// IWYU pragma: private, include "arene/base/optional/optional.hpp"
// IWYU pragma: friend "arene/base/optional/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_destructible.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
#include "arene/base/utility/in_place.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {

/// @brief forward declaration of the primary template
template <typename T>
class optional;

namespace optional_detail {

/// @brief Check if @c T is an instantiation of @c optional
/// @tparam T the type to check
template <typename T>
struct is_optional {
  // parasoft-begin-suppress CERT_CPP-DCL56-a-3 "False positive: variable is initialized"
  // parasoft-begin-suppress AUTOSAR-A3_3_2-a-2 "False positive: initializer is constant"
  /// @brief The result of the check: @c true if @c T is an instantiation of @c optional, @c false otherwise
  static constexpr bool value{is_instantiation_of_v<remove_cvref_t<T>, optional>};
  // parasoft-end-suppress AUTOSAR-A3_3_2-a-2
  // parasoft-end-suppress CERT_CPP-DCL56-a-3
};

/// @brief Copy the value from the other storage if required, otherwise
/// default-construct
/// @param other The source storage to transfer from
/// @param copy True if the value should be transferred; false if the target should be empty
/// @return A new storage type holding the result
template <typename StorageType>
constexpr auto copy_if_value(StorageType const& other, bool copy) -> StorageType {
  if (copy) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    return {in_place, other.value};
  }
  return StorageType();
}
/// @brief Copy the value from the other storage if required, otherwise
/// default-construct
/// @param other The source storage to transfer from
/// @param copy True if the value should be transferred; false if the target should be empty
/// @return A new storage type holding the result
template <typename StorageType>
constexpr auto copy_if_value(StorageType& other, bool copy) -> StorageType {
  if (copy) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    return {in_place, other.value};
  }
  return StorageType();
}
/// @brief Move the value from the other storage if required, otherwise
/// default-construct
/// @param other The source storage to transfer from
/// @param move True if the value should be transferred; false if the target should be empty
/// @return A new storage type holding the result
template <typename StorageType>
constexpr auto move_if_value(StorageType const& other, bool move) noexcept(
    std::is_nothrow_move_constructible<typename StorageType::value_type>::value
) -> StorageType {
  if (move) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    return {in_place, std::move(other.value)};
  }
  return StorageType();
}
/// @brief Move the value from the other storage if required, otherwise
/// default-construct
/// @param other The source storage to transfer from
/// @param move True if the value should be transferred; false if the target should be empty
/// @return A new storage type holding the result
template <typename StorageType>
constexpr auto move_if_value(StorageType& other, bool move) noexcept(
    std::is_nothrow_move_constructible<typename StorageType::value_type>::value
) -> StorageType {
  if (move) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    return {in_place, std::move(other.value)};
  }
  return StorageType();
}

/// @brief Copy the value from the other storage if required, otherwise
/// default-construct
/// @param other The source storage to transfer from
/// @param copy True if the value should be transferred; false if the target should be empty
/// @return A new storage type holding the result
template <typename StorageType, typename OtherStorage>
constexpr auto copy_other_if_value(OtherStorage const& other, bool copy) -> StorageType {
  if (copy) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    return {in_place, other.value};
  }
  return StorageType();
}

/// @brief Move the value from the other storage if required, otherwise
/// default-construct
/// @param other The source storage to transfer from
/// @param move True if the value should be transferred; false if the target should be empty
/// @return A new storage type holding the result
template <typename StorageType, typename OtherStorage>
constexpr auto move_other_if_value(OtherStorage& other, bool move) -> StorageType {
  if (move) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    return {in_place, std::move(other.value)};
  }
  return StorageType();
}

// parasoft-begin-suppress AUTOSAR-A12_1_5-a-2 "False positive: No duplication between constructors"
/// @brief Base class for @c optional for the case where @c T has a trivial
/// destructor.  This is required to make @c optional<T> a literal type
/// when @c T has a trivial destructor, so we can use @c optional<T> in
/// @c constexpr contexts.
template <typename T, bool = std::is_trivially_destructible<T>::value>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class optional_base {
 protected:
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "Protected data members permitted by M11-0-1 Permit #1 v1.0.0"
  // parasoft-begin-suppress AUTOSAR-A9_5_1-a-2 "False positive: this is covered by the discriminated union exception"
  // parasoft-begin-suppress AUTOSAR-A9_5_1-b-2 "False positive: this is covered by the discriminated union exception"
  /// @brief Storage is either a T or a dummy value
  // NOLINTNEXTLINE(hicpp-special-member-functions)
  union storage_type {
    /// @brief The type of the value
    using value_type = T;
    /// @brief The actual stored object, when there is one
    value_type value;
    /// @brief An empty type for use where the optional is empty
    struct dummy_type {};
    /// @brief The dummy value for the case where the optional is empty
    dummy_type dummy;

    // parasoft-begin-suppress AUTOSAR-A12_7_1-a-2 "False positive: =default will have wrong semantics"
    /// @brief Construct an empty state
    constexpr storage_type() noexcept
        : dummy() {}
    // parasoft-end-suppress AUTOSAR-A12_7_1-a-2

    // parasoft-begin-suppress AUTOSAR-A12_8_6-a-2 "False positive: not a base class"
    /// @brief Copy constructor never used, always elided
    constexpr storage_type(storage_type const&) noexcept
        : storage_type() {
      ARENE_INVARIANT_UNREACHABLE("Copy constructor not elided as expected");
    }
    // parasoft-end-suppress AUTOSAR-A12_8_6-a-2
    /// @brief Default destructor
    ~storage_type() = default;

    /// @brief Deleted copy assignment: copying is handled by @c optional
    auto operator=(storage_type const&) -> storage_type& = delete;

    /// @brief Construct a T from the provided Args
    /// @tparam Args The types of the arguments to construct the value from
    /// @param args The arguments to construct the value from
    template <typename... Args>
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    constexpr storage_type(in_place_t, Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...)))
        : value(std::forward<Args>(args)...) {}
  };
  // parasoft-end-suppress AUTOSAR-A9_5_1-a-2
  // parasoft-end-suppress AUTOSAR-A9_5_1-b-2
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_7_1-a "Bases must be initialized explicitly to address a compiler bug"
  /// @brief Default construct to an empty state
  // Explicitly initializes bases rather than using @c =default to address a gcc compiler bug
  constexpr optional_base() noexcept
      : storage_{},
        has_value_{false} {}
  // parasoft-end-suppress AUTOSAR-A12_7_1-a

  /// @brief Trivial destructor
  ~optional_base() = default;

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: Constrained via SFINAE"
  // parasoft-begin-suppress AUTOSAR-A15_5_1-b-2 "False positive: Conditionally noxecept"
  /// @brief Copy construct the value if there is one
  /// @param other The source optional to construct the value from
  constexpr optional_base(optional_base const& other) noexcept(std::is_nothrow_constructible<T, T const&>::value)
      : storage_(copy_if_value(other.storage(), other.has_value())),
        has_value_(other.has_value()) {}
  // parasoft-end-suppress AUTOSAR-A15_5_1-b-2
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A15_5_1-b-2 "False positive: Conditionally noxecept"
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: Elements are moved"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: Elements are moved"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: Elements are moved"
  /// @brief Move construct the value if there is one
  /// @param other The source optional to construct the value from
  constexpr optional_base(optional_base&& other) noexcept(
      // NOLINTNEXTLINE(hicpp-noexcept-move)
      std::is_nothrow_constructible<T, T&&>::value
  )
      : storage_(move_if_value(other.storage(), other.has_value())),
        has_value_(other.has_value()) {}
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2
  // parasoft-end-suppress AUTOSAR-A15_5_1-b-2

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: Elements are moved"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: Elements are moved"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: Elements are moved"
  /// @brief Move construct the value if there is one
  /// @param other The source optional to construct the value from
  template <typename U>
  explicit constexpr optional_base(optional_base<U>&& other) noexcept(std::is_nothrow_constructible<T, T&&>::value)
      : storage_(move_other_if_value<storage_type>(other.storage(), other.has_value())),
        has_value_(other.has_value()) {}
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: Constrained via SFINAE"
  /// @brief Copy construct the value if there is one
  /// @param other The source optional to construct the value from
  template <typename U>
  explicit constexpr optional_base(optional_base<U> const& other) noexcept(std::is_nothrow_constructible<T, T&&>::value)
      : storage_(copy_other_if_value<storage_type>(other.storage(), other.has_value())),
        has_value_(other.has_value()) {}
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2
  /// @brief Construct from a value; don't use this for copy/move constructors
  /// @param source The source to construct the value from
  template <
      typename U,
      constraints<
          std::enable_if_t<std::is_constructible<T, U>::value>,
          std::enable_if_t<!std::is_same<remove_cvref_t<U>, optional<T>>::value>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions,bugprone-forwarding-reference-overload)
  constexpr optional_base(U&& source) noexcept(noexcept(T(std::forward<U>(source))))
      : storage_{in_place, std::forward<U>(source)},
        has_value_(true) {}

  /// @brief Construct from a set of arguments
  /// @tparam Args The types of the arguments to construct the value from
  /// @param args The arguments to construct the value from
  template <typename... Args>
  explicit constexpr optional_base(in_place_t, Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...)))
      : storage_{in_place, std::forward<Args>(args)...},
        has_value_(true) {}

  /// @brief Moving is managed explicitly in @c optional
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  auto operator=(optional_base&&) -> optional_base& = delete;
  /// @brief Copying is managed explicitly in @c optional
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  auto operator=(optional_base const&) -> optional_base& = delete;

 public:
  /// @brief Get whether the optional currently has a value.
  /// @return true if the optional has a value, false otherwise
  constexpr auto has_value() const noexcept -> bool { return has_value_; }
  /// @brief Set that the optional has a value.
  constexpr void set_has_value() noexcept { has_value_ = true; }

  /// @brief Access the storage.
  /// @return A reference to the storage
  constexpr auto storage() const noexcept -> storage_type const& { return storage_; }
  // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the storage"
  /// @brief Access the storage.
  /// @return A reference to the storage
  constexpr auto storage() noexcept -> storage_type& { return storage_; }
  // parasoft-end-suppress AUTOSAR-A9_3_1-a-2

  /// @brief Destroy the value if there is one
  constexpr void reset() noexcept {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    storage_.dummy = {};
    has_value_ = false;
  }

 private:
  /// @brief The storage
  storage_type storage_;
  /// @brief Do we have a value?
  bool has_value_;
};
// parasoft-end-suppress AUTOSAR-A12_1_5-a-2

// parasoft-begin-suppress AUTOSAR-A12_1_5-a-2 "False positive: No duplication between constructors"
/// @brief A specialization for the case where @c T does NOT have a trivial
/// destructor, so we have to explicitly call it in the @c reset member
/// function, and in the destructor of @c optional_base if there is a
/// stored value.  This should be identical to the trivial-destructor
/// specialization in all other ways.
template <typename T>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class optional_base<T, false> {
 protected:
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "This entire class is a private base class"
  // parasoft-begin-suppress AUTOSAR-A9_5_1-a-2 "False positive: this is covered by the discriminated union exception"
  // parasoft-begin-suppress AUTOSAR-A9_5_1-b-2 "False positive: this is covered by the discriminated union exception"
  /// @brief Our storage: either a T or a dummy value
  // NOLINTNEXTLINE(hicpp-special-member-functions)
  union storage_type {
    /// @brief The type of the stored value
    using value_type = T;
    /// @brief The actual stored object, when there is one
    value_type value;
    /// @brief An empty type for use where the optional is empty
    struct dummy_type {};
    /// @brief The dummy value for the case where the optional is empty
    dummy_type dummy;
    // parasoft-begin-suppress AUTOSAR-A12_7_1-a-2 "False positive: =default will have wrong semantics"
    /// @brief Construct an empty instance
    constexpr storage_type() noexcept
        : dummy() {}
    // parasoft-end-suppress AUTOSAR-A12_7_1-a-2
    /// @brief Copy constructor; never actually used, as copies are elided
    constexpr storage_type(storage_type const&) noexcept
        : storage_type() {
      ARENE_INVARIANT_UNREACHABLE("Copy constructor should be elided");
    }

    /// @brief Deleted copy assignment: copying is handled by @c optional
    auto operator=(storage_type const&) -> storage_type& = delete;

    /// @brief Construct a T from the provided Args
    /// @tparam Args The types of the arguments to construct the value from
    /// @param args The arguments to construct the value from
    template <typename... Args>
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    constexpr storage_type(in_place_t, Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...)))
        : value(std::forward<Args>(args)...) {}
    // parasoft-begin-suppress AUTOSAR-A12_7_1-a-2 "False positive: =default will have wrong semantics"
    /// @brief Must provide non-defaulted destructor for non-literal T.
    /// Don't do anything here, as we ensure storage is empty before destroying
    // NOLINTNEXTLINE(hicpp-use-equals-default)
    ~storage_type() noexcept {}
    // parasoft-end-suppress AUTOSAR-A12_7_1-a-2
  };
  // parasoft-end-suppress AUTOSAR-A9_5_1-a-2
  // parasoft-end-suppress AUTOSAR-A9_5_1-b-2
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  /// @brief Default construct to an empty state
  constexpr optional_base() noexcept = default;
  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: Constrained via SFINAE"
  /// @brief Copy construct the value if there is one
  /// @param other The source optional to construct the value from
  constexpr optional_base(optional_base const& other) noexcept(std::is_nothrow_constructible<T, T const&>::value)
      : storage_(copy_if_value(other.storage(), other.has_value())),
        has_value_(other.has_value()) {}
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A15_5_1-b-2 "False positive: Conditionally noxecept"
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: Elements are moved"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: Elements are moved"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: Elements are moved"
  /// @brief Move construct the value if there is one
  /// @param other The source optional to construct the value from
  constexpr optional_base(optional_base&& other) noexcept(
      // NOLINTNEXTLINE(hicpp-noexcept-move)
      std::is_nothrow_constructible<T, T&&>::value
  )
      : storage_(move_if_value(other.storage(), other.has_value())),
        has_value_(other.has_value()) {}
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2
  // parasoft-end-suppress AUTOSAR-A15_5_1-b-2

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: Constrained via SFINAE"
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: 'other' is moved via move_other_if_value"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: 'other' is moved via move_other_if_value"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: 'other' is moved via move_other_if_value"
  /// @brief Move construct the value if there is one
  /// @param other The source optional to construct the value from
  template <typename U>
  explicit constexpr optional_base(optional_base<U>&& other) noexcept(std::is_nothrow_constructible<T, T&&>::value)
      : storage_(move_other_if_value<storage_type>(other.storage(), other.has_value())),
        has_value_(other.has_value()) {}
  /// @brief Copy construct the value if there is one
  /// @param other The source optional to construct the value from
  template <typename U>
  explicit constexpr optional_base(optional_base<U> const& other) noexcept(std::is_nothrow_constructible<T, T&&>::value)
      : storage_(copy_other_if_value<storage_type>(other.storage(), other.has_value())),
        has_value_(other.has_value()) {}
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  /// @brief Construct from a value
  /// @param source The source to construct the value from
  template <
      typename U,
      constraints<
          std::enable_if_t<std::is_constructible<T, U>::value>,
          std::enable_if_t<!std::is_same<remove_cvref_t<U>, optional<T>>::value>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions,bugprone-forwarding-reference-overload)
  constexpr optional_base(U&& source) noexcept(noexcept(T(std::forward<U>(source))))
      : storage_{in_place, std::forward<U>(source)},
        has_value_(true) {}

  /// @brief Construct from a set of arguments
  /// @tparam Args The types of the arguments to construct the value from
  /// @param args The arguments to construct the value from
  template <typename... Args>
  explicit constexpr optional_base(in_place_t, Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...)))
      : storage_{in_place, std::forward<Args>(args)...},
        has_value_(true) {}

  /// @brief Moving is managed explicitly in @c optional
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  auto operator=(optional_base&&) -> optional_base& = delete;
  /// @brief Copying is managed explicitly in @c optional
  // NOLINTNEXTLINE(hicpp-use-equals-delete)
  auto operator=(optional_base const&) -> optional_base& = delete;

  /// @brief Destroy the value if there is one
  ~optional_base() noexcept { reset(); }

 public:
  /// @brief Get whether the optional currently has a value.
  /// @return true if the optional has a value, false otherwise
  constexpr auto has_value() const noexcept -> bool { return has_value_; }
  /// @brief Set that the optional has a value.
  constexpr void set_has_value() noexcept { has_value_ = true; }

  /// @brief Access the storage.
  /// @return A reference to the storage
  constexpr auto storage() const noexcept -> storage_type const& { return storage_; }

  // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the storage"
  /// @brief Access the storage.
  /// @return A reference to the storage
  constexpr auto storage() noexcept -> storage_type& { return storage_; }
  // parasoft-end-suppress AUTOSAR-A9_3_1-a-2

  /// @brief Destroy the value if there is one
  constexpr void reset() noexcept {
    if (has_value_) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
      storage_.value.~T();
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
      storage_.dummy = {};
      has_value_ = false;
    }
  }

 private:
  // parasoft-begin-suppress AUTOSAR-A12_1_2-a-2 "Specifying initializer here allows default constructor to be trivial"
  /// @brief The storage
  storage_type storage_{};
  /// @brief Do we have a value?
  bool has_value_{false};
  // parasoft-end-suppress AUTOSAR-A12_1_2-a-2
};
// parasoft-end-suppress AUTOSAR-A12_1_5-a-2

}  // namespace optional_detail
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_DETAIL_OPTIONAL_BASE_HPP_
