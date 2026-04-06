// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file result_base.hpp
/// @brief Implementation details for arene::base::result
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RESULT_DETAIL_RESULT_BASE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RESULT_DETAIL_RESULT_BASE_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/result/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/memory/construct_at.hpp"
#include "arene/base/monostate/monostate.hpp"
#include "arene/base/result/detail/in_place_error.hpp"
#include "arene/base/result/detail/in_place_value.hpp"
#include "arene/base/stdlib_choice/addressof.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_destructible.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_manipulation/give_qualifiers_to.hpp"
#include "arene/base/type_manipulation/non_constructible_dummy.hpp"
#include "arene/base/type_manipulation/static_if.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace result_detail {
/// @brief A flag type to indicate whether a result holds a value or error
enum class discriminator_type : std::uint8_t { holds_value, holds_error };

/// @brief An empty type for when the storage has neither a value or error
struct dummy_type {};

// parasoft-begin-suppress AUTOSAR-A8_4_2-a-2 "ARENE_INVARIANT_UNREACHABLE terminates, so does not need a return"
// parasoft-begin-suppress CERT_CPP-MSC52-a-2 "ARENE_INVARIANT_UNREACHABLE terminates, so does not need a return"
// parasoft-begin-suppress CERT_C-MSC37-a-2 "ARENE_INVARIANT_UNREACHABLE terminates, so does not need a return"
/// @brief Copy the value or error from the other storage as appropriate
/// @tparam StorageType The type storing the data
/// @tparam OtherStorageType The type of the source
/// @param other The source to copy from
/// @param discriminator The flag indicating whether a value or error is to be copied
/// @return StorageType holding a copy of the source
template <typename StorageType, typename OtherStorageType>
constexpr auto conditional_copy(OtherStorageType const& other, discriminator_type discriminator) noexcept(
    std::is_nothrow_constructible<
        typename remove_cvref_t<StorageType>::value_type,
        give_qualifiers_to<OtherStorageType, typename remove_cvref_t<OtherStorageType>::value_type>&>::value &&
    std::is_nothrow_constructible<
        typename remove_cvref_t<StorageType>::error_type,
        give_qualifiers_to<OtherStorageType, typename remove_cvref_t<OtherStorageType>::error_type>&>::value
) -> StorageType {
  // parasoft-begin-suppress AUTOSAR-M6_4_5-a-2 "All branches terminated as per permit M6-4-5#1"
  // parasoft-begin-suppress AUTOSAR-M6_4_3-a-2 "All branches terminated as per permit M6-4-3#1"
  switch (discriminator) {
    case discriminator_type::holds_value:
      return StorageType{in_place_value, other.unsafe_value()};
    case discriminator_type::holds_error:
      return StorageType{in_place_error, other.unsafe_error()};
    default:
      ARENE_INVARIANT_UNREACHABLE("Invalid discriminator");
  }
  // parasoft-end-suppress AUTOSAR-M6_4_5-a-2
  // parasoft-end-suppress AUTOSAR-M6_4_3-a-2
}
// parasoft-end-suppress CERT_C-MSC37-a-2
// parasoft-end-suppress CERT_CPP-MSC52-a-2
// parasoft-end-suppress AUTOSAR-A8_4_2-a-2

// parasoft-begin-suppress AUTOSAR-A8_4_2-a-2 "ARENE_INVARIANT_UNREACHABLE terminates, so does not need a return"
// parasoft-begin-suppress CERT_CPP-MSC52-a-2 "ARENE_INVARIANT_UNREACHABLE terminates, so does not need a return"
// parasoft-begin-suppress CERT_C-MSC37-a-2 "ARENE_INVARIANT_UNREACHABLE terminates, so does not need a return"
/// @brief Move the value or error from the other storage as appropriate
/// @tparam StorageType The type storing the data
/// @tparam OtherStorageType The type of the source
/// @param other The source to move from
/// @param discriminator The flag indicating whether a value or error is to be moved
/// @return StorageType holding a copy of the source
template <typename StorageType, typename OtherStorageType>
constexpr auto conditional_move(OtherStorageType& other, discriminator_type discriminator) noexcept(
    std::is_nothrow_constructible<
        typename remove_cvref_t<StorageType>::value_type,
        give_qualifiers_to<OtherStorageType, typename remove_cvref_t<OtherStorageType>::value_type>&&>::value &&
    std::is_nothrow_constructible<
        typename remove_cvref_t<StorageType>::error_type,
        give_qualifiers_to<OtherStorageType, typename remove_cvref_t<OtherStorageType>::error_type>&&>::value
) -> StorageType {
  // parasoft-begin-suppress AUTOSAR-M6_4_5-a-2 "All branches terminated as per permit M6-4-5#1"
  // parasoft-begin-suppress AUTOSAR-M6_4_3-a-2 "All branches terminated as per permit M6-4-3#1"
  switch (discriminator) {
    case discriminator_type::holds_value:
      return StorageType{in_place_value, std::move(other.unsafe_value())};
    case discriminator_type::holds_error:
      return StorageType{in_place_error, std::move(other.unsafe_error())};
    default:
      ARENE_INVARIANT_UNREACHABLE("Invalid discriminator");
  }
  // parasoft-end-suppress AUTOSAR-M6_4_5-a-2
  // parasoft-end-suppress AUTOSAR-M6_4_3-a-2
}
// parasoft-end-suppress CERT_C-MSC37-a-2
// parasoft-end-suppress CERT_CPP-MSC52-a-2
// parasoft-end-suppress AUTOSAR-A8_4_2-a-2

// parasoft-begin-suppress AUTOSAR-A14_5_1-a-2 "False Positive: Copy constructor is defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A12_0_1-a-2 "False Positive: Copy constructor is defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A12_1_5-a-2 "Delegating constructors would not reduce duplication"
// parasoft-begin-suppress AUTOSAR-A1_1_1-b-2 "False Positive: Copy constructor is defined or deleted as appropriate"
/// @brief Implementation helper for arene::base::result which provides discriminated union storage that properly
/// handles @c constexpr compatible vs. non @c constexpr compatible types.
///
/// @tparam ValueType the type to store in the value channel. Must not be @c void .
/// @tparam ErrorType the type to store in the error channel.
///
/// This specialization is for when @c ValueType and @c ErrorType have trivial destructors. @c result_storage thus also
/// has a trivial destructor and may be used in @c constexpr contexts.
template <
    typename ValueType,
    typename ErrorType,
    bool = std::is_trivially_destructible<ValueType>::value && std::is_trivially_destructible<ErrorType>::value>
// False positive: the class has a copy and move constructor, either defined, or declared deleted
// NOLINTNEXTLINE(hicpp-special-member-functions)
class result_storage {
 public:
  // parasoft-begin-suppress AUTOSAR-A9_5_1-a-2 "False positive: this is covered by the discriminated union exception"
  // parasoft-begin-suppress AUTOSAR-A9_5_1-b-2 "False positive: this is covered by the discriminated union exception"
  /// @brief Storage is either a ValueType or an ErrorType or a dummy value
  union storage_type {
   public:
    // parasoft-begin-suppress AUTOSAR-A2_10_1-d-2 "False Positive: This identifier does not hide anything"
    // @brief The type of the value
    using value_type = ValueType;
    // parasoft-end-suppress AUTOSAR-A2_10_1-d-2
    // @brief The type of the error
    using error_type = ErrorType;

   private:
    /// @brief The actual stored object, when there is one
    value_type value_;
    /// @brief The actual stored object, when there is one
    error_type error_;
    /// @brief The dummy value for the case where the result is empty
    dummy_type dummy_;

   public:
    // parasoft-begin-suppress AUTOSAR-A12_8_6-a-2 "False positive: not a base class"
    // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
    /// @brief Copy constructor never used, always elided
    constexpr storage_type(storage_type const&) noexcept
        : dummy_() {
      ARENE_INVARIANT_UNREACHABLE("Copy constructor not elided as expected");
    }
    // parasoft-end-suppress AUTOSAR-A12_8_6-a-2

    /// @brief Move constructor never used, always elided
    constexpr storage_type(storage_type&&) noexcept
        : dummy_() {
      ARENE_INVARIANT_UNREACHABLE("Move constructor not elided as expected");
    }
    // parasoft-end-suppress CERT_C-EXP37-a-3

    /// @brief Deleted copy assignment
    auto operator=(storage_type const& other) noexcept -> storage_type& = delete;
    /// @brief Deleted move assignment
    auto operator=(storage_type&& other) noexcept -> storage_type& = delete;

    /// @brief Default trivial destructor
    ~storage_type() = default;

    /// @brief Construct a ValueType from the provided ArgTypes
    /// @tparam ArgTypes The type of the initializers
    /// @param args The initializers
    template <typename... ArgTypes>
    explicit constexpr storage_type(in_place_value_t, ArgTypes&&... args) noexcept(
        noexcept(ValueType(std::forward<ArgTypes>(args)...))
    )
        : value_(std::forward<ArgTypes>(args)...) {}
    /// @brief Construct an ErrorType from the provided ArgTypes
    /// @tparam ArgTypes The type of the initializers
    /// @param args The initializers
    template <typename... ArgTypes>
    explicit constexpr storage_type(in_place_error_t, ArgTypes&&... args) noexcept(
        noexcept(ErrorType(std::forward<ArgTypes>(args)...))
    )
        : error_(std::forward<ArgTypes>(args)...) {}

    // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the value"
    /// @brief Access to the stored value.
    /// @pre Requires that the storage actually holds a value, else behavior is undefined.
    /// @return A reference to the stored value
    constexpr auto unsafe_value() & noexcept -> ValueType& { return value_; }
    // parasoft-end-suppress AUTOSAR-A9_3_1-a-2
    /// @brief Access to the stored value.
    /// @pre Requires that the storage actually holds a value, else behavior is undefined.
    /// @return A const reference to the stored value
    constexpr auto unsafe_value() const& noexcept -> ValueType const& { return value_; }
    /// @brief Access to the stored value.
    /// @pre Requires that the storage actually holds a value, else behavior is undefined.
    /// @return An rvalue reference to the stored value
    constexpr auto unsafe_value() && noexcept -> ValueType&& { return std::move(value_); }

    // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the value"
    /// @brief Access to the stored error.
    /// @pre Requires that the storage actually holds an error, else behavior is undefined.
    /// @return A reference to the stored error
    constexpr auto unsafe_error() & noexcept -> ErrorType& { return error_; }
    // parasoft-end-suppress AUTOSAR-A9_3_1-a-2
    /// @brief Access to the stored error.
    /// @pre Requires that the storage actually holds an error, else behavior is undefined.
    /// @return A const reference to the stored error
    constexpr auto unsafe_error() const& noexcept -> ErrorType const& { return error_; }
    /// @brief Access to the stored error.
    /// @pre Requires that the storage actually holds an error, else behavior is undefined.
    /// @return An rvalue reference to the stored error
    constexpr auto unsafe_error() && noexcept -> ErrorType&& { return std::move(error_); }

    /// @brief Set the dummy field as the active field
    constexpr void clear() { dummy_ = {}; }
  };
  // parasoft-end-suppress AUTOSAR-A9_5_1-a-2
  // parasoft-end-suppress AUTOSAR-A9_5_1-b-2

  /// @brief Default construct the value if possible
  /// @tparam ValueTypeParam A template parameter to allow this constructor to be constrained, always equal to @c
  /// ValueType
  template <
      class ValueTypeParam = ValueType,
      constraints<std::enable_if_t<std::is_default_constructible<ValueTypeParam>::value>> = nullptr>
  constexpr result_storage() noexcept(std::is_nothrow_default_constructible<ValueType>::value)
      : value_storage_(in_place_value),
        storage_discriminator_(discriminator_type::holds_value) {}

  /// @brief The type for what would be the real copy constructor: either @c result_storage, if we want copies, or @c
  /// non_constructible_dummy otherwise
  using const_copy_ctor_arg = std::conditional_t<
      std::is_constructible<ValueType, ValueType const&>::value &&
          std::is_constructible<ErrorType, ErrorType const&>::value,
      result_storage,
      non_constructible_dummy>;
  /// @brief The type for the "copy" constructor to delete: either @c result_storage, if we don't want copies, or @c
  /// non_constructible_dummy otherwise
  using deleted_const_copy_ctor_arg = std::conditional_t<
      std::is_constructible<ValueType, ValueType const&>::value &&
          std::is_constructible<ErrorType, ErrorType const&>::value,
      non_constructible_dummy,
      result_storage>;

  /// @brief The type for what would be the real move constructor: either @c result_storage, if we want moves, or @c
  /// non_constructible_dummy otherwise
  using non_const_move_ctor_arg = std::conditional_t<
      std::is_constructible<ValueType, ValueType&&>::value && std::is_constructible<ErrorType, ErrorType&&>::value,
      result_storage,
      non_constructible_dummy>;
  /// @brief The type for the "move" constructor to delete: either @c result_storage, if we don't want moves, or @c
  /// non_constructible_dummy otherwise
  using deleted_non_const_move_ctor_arg = std::conditional_t<
      std::is_constructible<ValueType, ValueType&&>::value && std::is_constructible<ErrorType, ErrorType&&>::value,
      non_constructible_dummy,
      result_storage>;

  /// @brief Deleted copy constructor if appropriate
  /// @param other the source to copy from
  constexpr result_storage(deleted_const_copy_ctor_arg const& other) = delete;
  /// @brief Deleted move constructor, if appropriate
  /// @param other the source to move from
  constexpr result_storage(deleted_non_const_move_ctor_arg&& other) = delete;

 protected:
  /// @brief Copy construct the value or error as appropriate
  /// @param other the source to copy from
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr result_storage(const_copy_ctor_arg const& other) noexcept(
      std::is_nothrow_constructible<ValueType, ValueType const&>::value &&
      std::is_nothrow_constructible<ErrorType, ErrorType const&>::value
  )
      : value_storage_(conditional_copy<storage_type>(other.storage(), other.discriminator())),
        storage_discriminator_(other.discriminator()) {}

  // parasoft-begin-suppress AUTOSAR-A15_5_1-b-2 "False positive: Conditionally noxecept"
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: The data is moved via the 'conditional_move' function"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: The data is moved via the 'conditional_move' function"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: The data is moved via the 'conditional_move' function"
  /// @brief Move construct the value or error as appropriate
  /// @param other the source to move from
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr result_storage(non_const_move_ctor_arg&& other) noexcept(
      // NOLINTNEXTLINE(hicpp-noexcept-move)
      std::is_nothrow_constructible<ValueType, ValueType&&>::value &&
      std::is_nothrow_constructible<ErrorType, ErrorType&&>::value
  )
      : value_storage_(conditional_move<storage_type>(other.storage(), other.discriminator())),
        storage_discriminator_(other.discriminator()) {}
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2
  // parasoft-end-suppress AUTOSAR-A15_5_1-b-2

 public:
  /// @brief Move construct the value or error as appropriate
  /// @tparam SourceValueType The Value type of the source
  /// @tparam SourceErrorType The Error type of the source
  /// @param other the source to move from
  template <typename SourceValueType, typename SourceErrorType>
  explicit constexpr result_storage(result_storage<SourceValueType, SourceErrorType>&& other) noexcept(
      std::is_nothrow_constructible<ValueType, SourceValueType&&>::value &&
      std::is_nothrow_constructible<ErrorType, SourceErrorType&&>::value
  )
      : value_storage_(conditional_move<storage_type>(other.storage(), other.discriminator())),
        storage_discriminator_(other.discriminator()) {}

  /// @brief Copy construct the value or error as appropriate
  /// @tparam SourceValueType The Value type of the source
  /// @tparam SourceErrorType The Error type of the source
  /// @param other the source to copy from
  template <typename SourceValueType, typename SourceErrorType>
  explicit constexpr result_storage(result_storage<SourceValueType, SourceErrorType> const& other) noexcept(
      std::is_nothrow_constructible<ValueType, SourceValueType const&>::value &&
      std::is_nothrow_constructible<ErrorType, SourceErrorType const&>::value
  )
      : value_storage_(conditional_copy<storage_type>(other.storage(), other.discriminator())),
        storage_discriminator_(other.discriminator()) {}

  /// @brief Construct a value from a set of arguments
  /// @tparam ArgTypes The types of the arguments
  /// @param args The arguments
  template <typename... ArgTypes>
  explicit constexpr result_storage(in_place_value_t, ArgTypes&&... args) noexcept(
      noexcept(ValueType(std::forward<ArgTypes>(args)...))
  )
      : value_storage_{in_place_value, std::forward<ArgTypes>(args)...},
        storage_discriminator_(discriminator_type::holds_value) {}

  /// @brief Construct an error from a set of arguments
  /// @tparam ArgTypes The types of the arguments
  /// @param args The arguments
  template <typename... ArgTypes>
  explicit constexpr result_storage(in_place_error_t, ArgTypes&&... args) noexcept(
      noexcept(ErrorType(std::forward<ArgTypes>(args)...))
  )
      : value_storage_{in_place_error, std::forward<ArgTypes>(args)...},
        storage_discriminator_(discriminator_type::holds_error) {}

  // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the value"
  /// @brief Get whether the result currently has a value.
  /// @return discriminator_type The discriminator
  constexpr auto discriminator() noexcept -> discriminator_type& { return storage_discriminator_; }
  // parasoft-end-suppress AUTOSAR-A9_3_1-a-2
  /// @brief Get whether the result currently has a value.
  /// @return discriminator_type The discriminator
  constexpr auto discriminator() const noexcept -> discriminator_type { return storage_discriminator_; }

  // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the value"
  /// @brief Access the storage.
  /// @return A reference to the storage
  constexpr auto storage() noexcept -> storage_type& { return value_storage_; }
  // parasoft-end-suppress AUTOSAR-A9_3_1-a-2
  /// @brief Access the storage.
  /// @return A reference to the storage
  constexpr auto storage() const noexcept -> storage_type const& { return value_storage_; }

  /// @brief Destroy the currently held element
  constexpr void reset() noexcept { value_storage_.clear(); }

  /// @brief Trivial destructor.
  /// The destructor must use @c =default to maintain triviality
  ~result_storage() = default;

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Deleted copy
  auto operator=(result_storage const&) -> result_storage& = delete;
  /// @brief Deleted move assignment
  auto operator=(result_storage&&) -> result_storage& = delete;
  // parasoft-end-suppress CERT_C-EXP37-a-3

 private:
  /// @brief The storage
  storage_type value_storage_;
  /// @brief Do we have a value?
  discriminator_type storage_discriminator_;
};
// parasoft-end-suppress AUTOSAR-A14_5_1-a-2
// parasoft-end-suppress AUTOSAR-A12_0_1-a-2
// parasoft-end-suppress AUTOSAR-A12_1_5-a-2
// parasoft-end-suppress AUTOSAR-A1_1_1-b-2

// parasoft-begin-suppress AUTOSAR-A1_1_1-b-2 "False Positive: Copy constructor is defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A12_0_1-a-2 "False Positive: Copy constructor is defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A12_1_5-a-2 "Delegating constructors would not reduce duplication"
// parasoft-begin-suppress AUTOSAR-A14_5_1-a-2 "False Positive: Copy constructor is defined or deleted as appropriate"
/// @brief Implementation helper for arene::base::result which provides discriminated union storage that properly
/// handles @c constexpr compatible vs. non @c constexpr compatible types.
///
/// @tparam ValueType the type to store in the value channel. Must not be @c void .
/// @tparam ErrorType the type to store in the error channel.
///
/// This specialization is for when @c ValueType and @c ErrorType do not have trivial destructors
template <typename ValueType, typename ErrorType>
// False positive: the class has a copy and move constructor, either defined, or declared deleted
// NOLINTNEXTLINE(hicpp-special-member-functions)
class result_storage<ValueType, ErrorType, false> {
 public:
  // parasoft-begin-suppress AUTOSAR-A9_5_1-a-2 "False positive: this is covered by the discriminated union exception"
  // parasoft-begin-suppress AUTOSAR-A9_5_1-b-2 "False positive: this is covered by the discriminated union exception"
  /// @brief Our storage: either a ValueType, an ErrorType or a dummy value
  union storage_type {
   public:
    // parasoft-begin-suppress AUTOSAR-A2_10_1-d-2 "False Positive: This identifier does not hide anything"
    /// @brief The type of the stored value
    using value_type = ValueType;
    // parasoft-end-suppress AUTOSAR-A2_10_1-d-2
    /// @brief The actual stored object, when there is one
    /// @brief The type of the stored error
    using error_type = ErrorType;

   private:
    /// @brief The stored value, when there is one
    value_type value_;
    /// @brief The stored error, when there is one
    error_type error_;
    /// @brief The dummy value
    dummy_type dummy_;

   public:
    // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
    /// @brief Copy constructor never used, always elided
    constexpr storage_type(storage_type const&) noexcept
        : dummy_() {
      ARENE_INVARIANT_UNREACHABLE("Copy constructor not elided as expected");
    }

    /// @brief Move constructor never used, always elided
    constexpr storage_type(storage_type&&) noexcept
        : dummy_() {
      ARENE_INVARIANT_UNREACHABLE("Move constructor not elided as expected");
    }
    // parasoft-end-suppress CERT_C-EXP37-a-3

    /// @brief Deleted copy assignment
    auto operator=(storage_type const& other) noexcept -> storage_type& = delete;
    /// @brief Deleted move assignment
    auto operator=(storage_type&& other) noexcept -> storage_type& = delete;

    /// @brief Construct a ValueType from the provided ArgTypes
    /// @tparam ArgTypes The type of the initializers
    /// @param args The initializers
    template <typename... ArgTypes>
    explicit constexpr storage_type(in_place_value_t, ArgTypes&&... args) noexcept(
        noexcept(ValueType(std::forward<ArgTypes>(args)...))
    )
        : value_(std::forward<ArgTypes>(args)...) {}
    /// @brief Construct an ErrorType from the provided ArgTypes
    /// @tparam ArgTypes The type of the initializers
    /// @param args The initializers
    template <typename... ArgTypes>
    explicit constexpr storage_type(in_place_error_t, ArgTypes&&... args) noexcept(
        noexcept(ErrorType(std::forward<ArgTypes>(args)...))
    )
        : error_(std::forward<ArgTypes>(args)...) {}

    // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the value"
    /// @brief Access to the stored value.
    /// @pre Requires that the storage actually holds a value, else behavior is undefined.
    /// @return A reference to the stored value
    constexpr auto unsafe_value() & noexcept -> ValueType& { return value_; }
    // parasoft-end-suppress AUTOSAR-A9_3_1-a-2
    /// @brief Access to the stored value.
    /// @pre Requires that the storage actually holds a value, else behavior is undefined.
    /// @return A const reference to the stored value
    constexpr auto unsafe_value() const& noexcept -> ValueType const& { return value_; }
    /// @brief Access to the stored value.
    /// @pre Requires that the storage actually holds a value, else behavior is undefined.
    /// @return An rvalue reference to the stored value
    constexpr auto unsafe_value() && noexcept -> ValueType&& { return std::move(value_); }

    // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the value"
    /// @brief Access to the stored error.
    /// @pre Requires that the storage actually holds an error, else behavior is undefined.
    /// @return A reference to the stored error
    constexpr auto unsafe_error() & noexcept -> ErrorType& { return error_; }
    // parasoft-end-suppress AUTOSAR-A9_3_1-a-2
    /// @brief Access to the stored error.
    /// @pre Requires that the storage actually holds an error, else behavior is undefined.
    /// @return A const reference to the stored error
    constexpr auto unsafe_error() const& noexcept -> ErrorType const& { return error_; }
    /// @brief Access to the stored error.
    /// @pre Requires that the storage actually holds an error, else behavior is undefined.
    /// @return An rvalue reference to the stored error
    constexpr auto unsafe_error() && noexcept -> ErrorType&& { return std::move(error_); }

    /// @brief Set the dummy field as the active field
    constexpr void clear() { dummy_ = {}; }

    // parasoft-begin-suppress AUTOSAR-A12_7_1-a-2 "False positive: =default will have wrong semantics"
    /// @brief Must provide non-defaulted destructor for non-literal ValueType.
    /// Don't do anything here, as we ensure storage is empty before destroying
    // NOLINTNEXTLINE(hicpp-use-equals-default)
    ~storage_type() noexcept {}
    // parasoft-end-suppress AUTOSAR-A12_7_1-a-2
  };
  // parasoft-end-suppress AUTOSAR-A9_5_1-a-2
  // parasoft-end-suppress AUTOSAR-A9_5_1-b-2

  /// @brief Default construct the value if possible
  /// @tparam ValueTypeParam A template parameter to allow this constructor to be constrained, always equal to @c
  /// ValueType
  template <
      class ValueTypeParam = ValueType,
      constraints<std::enable_if_t<std::is_default_constructible<ValueTypeParam>::value>> = nullptr>
  constexpr result_storage() noexcept(std::is_nothrow_default_constructible<ValueType>::value)
      : value_storage_(in_place_value),
        storage_discriminator_(discriminator_type::holds_value) {}

  /// @brief The type for what would be the real copy constructor: either @c result_storage, if we want copies, or @c
  /// non_constructible_dummy otherwise
  using const_copy_ctor_arg = std::conditional_t<
      std::is_constructible<ValueType, ValueType const&>::value &&
          std::is_constructible<ErrorType, ErrorType const&>::value,
      result_storage,
      non_constructible_dummy>;
  /// @brief The type for the "copy" constructor to delete: either @c result_storage, if we don't want copies, or @c
  /// non_constructible_dummy otherwise
  using deleted_const_copy_ctor_arg = std::conditional_t<
      std::is_constructible<ValueType, ValueType const&>::value &&
          std::is_constructible<ErrorType, ErrorType const&>::value,
      non_constructible_dummy,
      result_storage>;

  /// @brief The type for what would be the real move constructor: either @c result_storage, if we want moves, or @c
  /// non_constructible_dummy otherwise
  using non_const_move_ctor_arg = std::conditional_t<
      std::is_constructible<ValueType, ValueType&&>::value && std::is_constructible<ErrorType, ErrorType&&>::value,
      result_storage,
      non_constructible_dummy>;
  /// @brief The type for the "move" constructor to delete: either @c result_storage, if we don't want moves, or @c
  /// non_constructible_dummy otherwise
  using deleted_non_const_move_ctor_arg = std::conditional_t<
      std::is_constructible<ValueType, ValueType&&>::value && std::is_constructible<ErrorType, ErrorType&&>::value,
      non_constructible_dummy,
      result_storage>;

  /// @brief Deleted copy constructor if appropriate
  /// @param other the source to copy from
  constexpr result_storage(deleted_const_copy_ctor_arg const& other) = delete;
  /// @brief Deleted move constructor, if appropriate
  /// @param other the source to move from
  constexpr result_storage(deleted_non_const_move_ctor_arg&& other) = delete;

 protected:
  /// @brief Copy construct the value or error as appropriate
  /// @param other the source to copy from
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr result_storage(const_copy_ctor_arg const& other) noexcept(
      std::is_nothrow_constructible<ValueType, ValueType const&>::value &&
      std::is_nothrow_constructible<ErrorType, ErrorType const&>::value
  )
      : value_storage_(conditional_copy<storage_type>(other.storage(), other.discriminator())),
        storage_discriminator_(other.discriminator()) {}

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: The data is moved via the 'conditional_move' function"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: The data is moved via the 'conditional_move' function"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: The data is moved via the 'conditional_move' function"
  /// @brief Move construct the value or error as appropriate
  /// @param other the source to move from
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr result_storage(non_const_move_ctor_arg&& other) noexcept(
      // NOLINTNEXTLINE(hicpp-noexcept-move)
      std::is_nothrow_constructible<ValueType, ValueType&&>::value &&
      std::is_nothrow_constructible<ErrorType, ErrorType&&>::value
  )
      : value_storage_(conditional_move<storage_type>(other.storage(), other.discriminator())),
        storage_discriminator_(other.discriminator()) {}
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2

 public:
  /// @brief Move construct the value or error as appropriate
  /// @tparam SourceValueType The Value type of the source
  /// @tparam SourceErrorType The Error type of the source
  /// @param other the source to move from
  template <typename SourceValueType, typename SourceErrorType>
  explicit constexpr result_storage(result_storage<SourceValueType, SourceErrorType>&& other) noexcept(
      std::is_nothrow_constructible<ValueType, SourceValueType&&>::value &&
      std::is_nothrow_constructible<ErrorType, SourceErrorType&&>::value
  )
      : value_storage_(conditional_move<storage_type>(other.storage(), other.discriminator())),
        storage_discriminator_(other.discriminator()) {}
  /// @brief Copy construct the value or error as appropriate
  /// @tparam SourceValueType The Value type of the source
  /// @tparam SourceErrorType The Error type of the source
  /// @param other the source to copy from
  template <typename SourceValueType, typename SourceErrorType>
  explicit constexpr result_storage(result_storage<SourceValueType, SourceErrorType> const& other) noexcept(
      std::is_nothrow_constructible<ValueType, SourceValueType const&>::value &&
      std::is_nothrow_constructible<ErrorType, SourceErrorType const&>::value
  )
      : value_storage_(conditional_copy<storage_type>(other.storage(), other.discriminator())),
        storage_discriminator_(other.discriminator()) {}

  /// @brief Construct a value from a set of arguments
  /// @tparam ArgTypes The types of the arguments
  /// @param args The arguments
  template <typename... ArgTypes>
  explicit constexpr result_storage(in_place_value_t, ArgTypes&&... args) noexcept(
      noexcept(ValueType(std::forward<ArgTypes>(args)...))
  )
      : value_storage_{in_place_value, std::forward<ArgTypes>(args)...},
        storage_discriminator_(discriminator_type::holds_value) {}
  /// @brief Construct an error from a set of arguments
  /// @tparam ArgTypes The types of the arguments
  /// @param args The arguments
  template <typename... ArgTypes>
  explicit constexpr result_storage(in_place_error_t, ArgTypes&&... args) noexcept(
      noexcept(ErrorType(std::forward<ArgTypes>(args)...))
  )
      : value_storage_{in_place_error, std::forward<ArgTypes>(args)...},
        storage_discriminator_(discriminator_type::holds_error) {}

  /// @brief Destroy the currently held element
  ~result_storage() noexcept { reset(); }

  // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the value"
  /// @brief Get whether the result currently has a value.
  /// @return discriminator_type The discriminator
  constexpr auto discriminator() noexcept -> discriminator_type& { return storage_discriminator_; }
  // parasoft-end-suppress AUTOSAR-A9_3_1-a-2
  /// @brief Get whether the result currently has a value.
  /// @return discriminator_type The discriminator
  constexpr auto discriminator() const noexcept -> discriminator_type { return storage_discriminator_; }

  // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the value"
  /// @brief Access the storage.
  /// @return A reference to the storage
  constexpr auto storage() noexcept -> storage_type& { return value_storage_; }
  // parasoft-end-suppress AUTOSAR-A9_3_1-a-2
  /// @brief Access the storage.
  /// @return A reference to the storage
  constexpr auto storage() const noexcept -> storage_type const& { return value_storage_; }

  // parasoft-begin-suppress AUTOSAR-A0_1_2-a-2 "False positive: return value used to call destructor"
  // parasoft-begin-suppress CERT_C-POS54-a-1 "False positive: return value used to call destructor"
  // parasoft-begin-suppress CERT_C-ERR33-a-1 "False positive: return value used to call destructor"
  /// @brief Destroy the currently held element
  constexpr void reset() noexcept {
    // parasoft-begin-suppress AUTOSAR-M6_4_5-a-2 "All branches terminated as per permit M6-4-5#1"
    // parasoft-begin-suppress AUTOSAR-M6_4_3-a-2 "All branches terminated as per permit M6-4-3#1"
    switch (storage_discriminator_) {
      case discriminator_type::holds_value:
        value_storage_.unsafe_value().~ValueType();
        break;
      case discriminator_type::holds_error:
        value_storage_.unsafe_error().~ErrorType();
        break;
      default:
        ARENE_INVARIANT_UNREACHABLE("Invalid discriminator");
    }
    // parasoft-end-suppress AUTOSAR-M6_4_5-a-2
    // parasoft-end-suppress AUTOSAR-M6_4_3-a-2
    value_storage_.clear();
  }
  // parasoft-end-suppress CERT_C-ERR33-a-1
  // parasoft-end-suppress CERT_C-POS54-a-1
  // parasoft-end-suppress AUTOSAR-A0_1_2-a-2

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Deleted copy
  auto operator=(result_storage const&) -> result_storage& = delete;
  /// @brief Deleted move assignment
  auto operator=(result_storage&&) -> result_storage& = delete;
  // parasoft-end-suppress CERT_C-EXP37-a-3

 private:
  /// @brief The storage
  storage_type value_storage_;
  /// @brief Do we have a value?
  discriminator_type storage_discriminator_;
};
// parasoft-end-suppress AUTOSAR-A14_5_1-a-2
// parasoft-end-suppress AUTOSAR-A12_0_1-a-2
// parasoft-end-suppress AUTOSAR-A12_1_5-a-2
// parasoft-end-suppress AUTOSAR-A1_1_1-b-2

///
/// @brief Implementation helper for arene::base::result which contains logic that does not depend on @c value_type
///
/// @tparam ValueType the type to store in the value channel of the result
/// @tparam ErrorType the type to store in the error channel of the result
///
template <typename ValueType, typename ErrorType>
// False positive: the class has a copy and move constructor, either defined, or declared deleted
// NOLINTNEXTLINE(hicpp-special-member-functions)
class result_base : public result_storage<monostate_identity<ValueType>, ErrorType> {
  /// @brief The type of the internal storage
  using storage_type = result_storage<monostate_identity<ValueType>, ErrorType>;

 public:
  /// @brief alias in constructors from storage type
  using storage_type::storage_type;

  ///
  /// @brief checks if the value channel is populated.
  ///
  /// @return true if the value channel is populated.
  /// @return false if the error channel is populated.
  ARENE_NODISCARD constexpr auto has_value() const noexcept -> bool {
    return this->discriminator() == result_detail::discriminator_type::holds_value;
  }

  ///
  /// @brief checks if the error channel is populated.
  ///
  /// @return true if the error channel is populated.
  /// @return false if the value channel is populated.
  ARENE_NODISCARD constexpr auto has_error() const noexcept -> bool { return !has_value(); }

  ///
  /// @brief Checks if the error channel contains a specific error
  ///
  /// @param err The error to check for.
  /// @return true If @c has_error() and @c error()==err .
  /// @return false Otherwise.
  template <typename U = ErrorType, constraints<std::enable_if_t<is_equality_comparable_v<U>>> = nullptr>
  ARENE_NODISCARD constexpr auto has_error(ErrorType const& err) const
      noexcept(noexcept(err == std::declval<ErrorType>())) -> bool {
    if (has_error()) {
      return err == this->storage().unsafe_error();
    }
    return false;
  }

  ///
  /// @brief implicit conversion to bool, equivalent to @c has_value()
  ///
  /// @return true if the value channel is populated.
  /// @return false if the error channel is populated.
  ARENE_NODISCARD constexpr explicit operator bool() const noexcept { return has_value(); }

  // parasoft-begin-suppress AUTOSAR-A9_3_1-a-2 "False positive: This class is a container of the value"
  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Emplaces an error into the result
  ///
  /// @tparam ArgTypes the types of the arguments to construct the error with
  /// @param args The arguments to construct the error with
  /// @return A reference to the newly emplaced error
  /// @post @c has_error() returns @c true
  /// @post The content of @c error() will be equivalent to a @c error_type direct-initialized from @c args .
  ///
  template <
      typename... ArgTypes,
      constraints<std::enable_if_t<std::is_nothrow_constructible<ErrorType, ArgTypes&&...>::value>> = nullptr>
  auto emplace(in_place_error_t, ArgTypes&&... args) noexcept -> ErrorType& {
    this->reset();
    this->discriminator() = discriminator_type::holds_error;
    return *::arene::base::construct_at(
        std::addressof(this->storage().unsafe_error()),
        std::forward<ArgTypes>(args)...
    );
  }
  // parasoft-end-suppress CERT_C-EXP37-a-3
  // parasoft-end-suppress AUTOSAR-A9_3_1-a-2
};

}  // namespace result_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RESULT_DETAIL_RESULT_BASE_HPP_
