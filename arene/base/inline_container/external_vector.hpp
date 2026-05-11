// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_EXTERNAL_VECTOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_EXTERNAL_VECTOR_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/byte/byte.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/wrapped_iterator.hpp"  // IWYU pragma: keep
#include "arene/base/inline_container/detail/comparison_interface.hpp"
#include "arene/base/inline_container/detail/try_construct_interface.hpp"
#include "arene/base/inline_container/detail/unsafe_element_storage.hpp"
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/iterator/advance.hpp"
#include "arene/base/memory/construct_at.hpp"
#include "arene/base/memory/destroy_at.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/move_iterator.hpp"
#include "arene/base/type_manipulation/non_constructible_dummy.hpp"
#include "arene/base/type_traits/conditional.hpp"
#include "arene/base/type_traits/denotes_range.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/utility/alignment.hpp"
#include "arene/base/utility/forward_like.hpp"
#include "arene/base/utility/swap.hpp"

// parasoft-end-suppress AUTOSAR-A16_2_2-a

// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-A1_1_1-b-2 "False positive: This is all conforming code"

namespace arene {
namespace base {

/// @brief a container similar to @c std::vector<T> that uses an external buffer
///   for storage
/// @tparam T value type of the container
///
/// A container where storage for the values is provided as a buffer, which is
/// external to the class. The lifetime of the external buffer must bound the
/// lifetime of @c external_vector (i.e. the external buffer must outlive the
/// @c external_vector that uses it).
///
template <typename T>
class external_vector;

namespace external_vector_detail {

/// @brief @c try_construct_policy for @c external_vector
/// @tparam T container value type
///
template <class T>
class try_construct_policy {
 public:
  /// @brief value type stored by the @c external_vector
  using value_type = T;
  /// @brief size type of the @c external_vector
  using size_type = std::size_t;

  /// @brief obtain the value capacity of a byte buffer
  /// @param buffer external buffer of bytes
  /// @pre the alignment of @c buffer matches the alignment of @c value_type
  /// @pre the size of @c buffer is an exact multiple of @c sizeof(value_type)
  /// @return number of values that can be placed in @c buffer
  ///
  static auto capacity_of(span<byte> buffer) noexcept -> size_type {
    ARENE_PRECONDITION(is_aligned<alignof(value_type)>(buffer.data()));
    ARENE_PRECONDITION((buffer.size() % sizeof(value_type)) == 0U);

    return buffer.size() / sizeof(value_type);
  }
};

/// @brief storage base class for @c external_vector
/// @tparam T value type of the @c external_vector
///
/// Handles element construction and destruction for an @c external_vector into
/// an externally provided buffer. This class provides a minimal interface to
/// the underlying storage; @c vector_interface provides the bulk of
/// "vector"-type functionality for @c external_vector.
///
template <class T>
class storage_base {
  /// @brief tag type used to protect construction of iterators
  class iterator_passkey {
   public:
    /// @brief default constructor
    explicit iterator_passkey() = default;
  };

 public:
  /// @brief value type stored by this class
  using value_type = T;
  /// @brief size type
  using size_type = std::size_t;
  /// @brief type of a pointer to a stored value
  using pointer = value_type*;
  /// @brief type of a const pointer to a stored value
  using const_pointer = value_type const*;
  /// @brief iterator type
  using iterator = detail::wrapped_iterator<pointer, iterator_passkey>;
  /// @brief const iterator type
  using const_iterator = detail::wrapped_iterator<const_pointer, iterator_passkey>;
  /// @brief difference type
  using difference_type = typename std::iterator_traits<iterator>::difference_type;

 private:
  /// @brief underlying element type that allows a value to exist or not exist
  using element_type = inline_container::detail::unsafe_element_storage<value_type>;

  /// @brief view to external storage
  span<element_type> storage_;
  /// @brief number of active values
  size_type size_;

  /// @brief convert a span of bytes into a span of elements
  /// @param buffer external buffer of bytes
  ///
  /// Starts the lifetime of elements in the external buffer of bytes.
  ///
  /// @return span of elements
  ///
  static auto make_storage_array(span<byte> buffer) noexcept -> span<element_type> {
    auto const count = try_construct_policy<T>::capacity_of(buffer);

    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto* const start = (count == 0U) ? nullptr : new (buffer.data()) element_type[count]{};

    return {start, count};
  }

 public:
  /// @brief construct an empty @c storage_base
  /// @param buffer external buffer of bytes
  /// @post <tt> size() == 0 </tt>
  ///
  /// Initializes a @c storage_base to enable storage of @c value_type values in
  /// @c buffer.
  ///
  explicit storage_base(span<byte> buffer) noexcept
      : storage_{make_storage_array(buffer)},
        size_{0U} {}

  /// @brief destroy values stored in the external buffer
  ///
  ~storage_base() { storage_shrink_to(0U); }

  /// @brief copying is managed externally from this type
  storage_base(storage_base const&) = delete;
  /// @brief moving is managed externally from this type
  storage_base(storage_base&&) = delete;
  /// @brief copying is managed externally from this type
  auto operator=(storage_base const&) -> storage_base& = delete;
  /// @brief moving is managed externally from this type
  auto operator=(storage_base&&) -> storage_base& = delete;

  /// @brief Obtain a span over the stored values
  /// @return A @c span<value_type> for the initialized values
  auto storage_values() noexcept -> span<value_type> {
    if (storage_capacity() == 0U) {
      return {};
    }
    return {&storage_.front().object, size_};
  }
  /// @brief Obtain a @c const span over the stored values
  /// @return A <c>span<const value_type></c> for the initialized values
  auto storage_values() const noexcept -> span<value_type const> {
    if (storage_capacity() == 0U) {
      return {};
    }
    return {&storage_.front().object, size_};
  }

  /// @brief obtain the number of values that can be stored
  /// @return number of values that can be stored
  ///
  auto storage_capacity() const noexcept -> size_type { return storage_.size(); }

  /// @brief obtain the number of values active in storage
  /// @return number of values active in storage
  ///
  auto stored_size() const noexcept -> size_type { return size_; }

  /// @brief append a value to the end, constructed in place from the
  ///   supplied arguments
  /// @tparam Args argument types
  /// @param args constructor arguments
  /// @pre <tt> size() < capacity() </tt>
  template <class... Args, constraints<std::enable_if_t<std::is_constructible<value_type, Args...>::value>> = nullptr>
  void storage_emplace_back(Args&&... args) noexcept(std::is_nothrow_constructible<value_type, Args...>::value) {
    ARENE_PRECONDITION(size_ < storage_capacity());

    auto& location = storage_[size_];
    destroy_at(&location.dummy);
    std::ignore = construct_at(&location.object, std::forward<Args>(args)...);

    ++size_;
  }

  /// @brief reduce size, removing values at the back
  /// @param count target number of values
  /// @pre <tt> count <= size() </tt>
  /// @post <tt> size() == count </tt>
  ///
  auto storage_shrink_to(std::size_t count) noexcept -> void {
    ARENE_PRECONDITION(count <= size_);

    for (auto& location : storage_.subspan(count, size_ - count)) {
      destroy_at(&location.object);
      std::ignore = construct_at(&location.dummy);
    }

    size_ = count;
  }

  /// @brief swap contents with a different @c storage_base
  /// @param other @c storage_base to swap contents with
  /// @note does not swap individual elements but rather the underlying buffer
  ///
  auto swap(storage_base& other) noexcept -> void {
    arene::base::swap(this->storage_, other.storage_);
    arene::base::swap(this->size_, other.size_);
  }

  /// @brief check that buffer is not already used by this storage
  /// @param buffer buffer to check against
  ///
  /// Some of the external_vector constructors have the precondition that there is no aliasing, but there is not a
  /// portable way to check for aliasing in C++ today. This function performs a best-effort check that the data pointer
  /// of the given buffer is not equal to the storage's data pointer, intended to help catch the case of a pointer/span
  /// being accidentally reused.
  ///
  /// If a more expansive check is required, it will need to be platform-dependent and should be implemented in the
  /// compiler_support package.
  ///
  /// @pre @c buffer.data() != storage_.data()
  /// @return @c buffer
  ///
  auto verify_not_already_in_use(span<byte> buffer) const noexcept -> span<byte> {
    ARENE_PRECONDITION(static_cast<void*>(buffer.data()) != static_cast<void*>(storage_.data()));
    return buffer;
  }
};

/// @brief Base class for @c vector_interface that redirects via the @c storage_base base class of the derived class to
/// access the external storage
/// @tparam ValueType The type of the value stored in the container
template <typename ValueType>
class external_storage_base {
 protected:
  /// @brief Passkey to allow access to the storage
  class storage_access_tag {
   public:
    /// @brief Explicit default constructor to require named initialization
    explicit storage_access_tag() = default;
  };

 private:
  /// @brief The type of the derived class
  using derived_external_vector = external_vector<ValueType>;

  /// @brief Get the storage from the derived @c external_vector
  /// @return A reference to the storage class
  auto get_storage() const noexcept -> storage_base<ValueType> const& {
    return derived_external_vector::get_storage(storage_access_tag{}, this);
  }

  /// @brief Get the storage from the derived @c external_vector
  /// @return A reference to the storage class
  auto get_storage() noexcept -> storage_base<ValueType>& {
    return derived_external_vector::get_storage(storage_access_tag{}, this);
  }

 public:
  /// @brief The type of the stored values
  using value_type = ValueType;
  /// @brief The type used for sizes of the vector
  using size_type = std::size_t;

 protected:
  /// @brief Get the maximum number of items that can be stored in this vector
  /// @return The maximum number of items that can be stored in this vector
  auto capacity() const noexcept -> size_type { return get_storage().storage_capacity(); }
  /// @brief Get the maximum number of items that can be stored in this vector
  /// @return The maximum number of items that can be stored in this vector
  auto max_size() const noexcept -> size_type { return capacity(); }

  /// @brief Get a @c span for the initialized values in the vector
  /// @return A @c span for the initialized values in the vector
  auto values() noexcept -> span<value_type> { return get_storage().storage_values(); }
  /// @brief Get a @c span for the initialized values in the vector
  /// @return A @c span for the initialized values in the vector
  auto values() const noexcept -> span<value_type const> { return get_storage().storage_values(); }

  /// @brief Construct a new element at the end of the existing range, from the supplied arguments.
  /// @tparam Args The types of the arguments
  /// @param args The constructor arguments
  template <typename... Args>
  void internal_emplace_back(Args&&... args) noexcept(std::is_nothrow_constructible<ValueType, Args...>::value) {
    get_storage().storage_emplace_back(std::forward<Args>(args)...);
  }

  /// @brief If the current size is more than @c new_size, reduce the size to @c new_size.
  /// @param new_size The new size limit
  void shrink_to(size_type new_size) noexcept { get_storage().storage_shrink_to(new_size); }
};

}  // namespace external_vector_detail

// parasoft-begin-suppress AUTOSAR-A10_1_1-a "False positive: Only inherits from a single non-interface base classes"
// parasoft-begin-suppress AUTOSAR-M14_5_3-a "False positive: Copy assignment is defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A12_1_6-a "False positive: storage_base called as appropriate"

// parasoft-begin-suppress AUTOSAR-A7_3_1-a "'storage_base::swap' is not part
// of the public interface and is not hidden. 'storage_base' is a private base.

// parasoft-begin-suppress AUTOSAR-A10_2_1-b "'storage_base::swap' is not part
// of the public interface and is not hidden. 'storage_base' is a private base.

/// @brief a container similar to @c std::vector<T> that uses an external buffer
///   for storage
/// @tparam T value type of the container
///
/// A container where storage for the values is provided as a buffer, which is
/// external to the class. The lifetime of the external buffer must bound the
/// lifetime of @c external_vector (i.e. the external buffer must outlive the
/// @c external_vector that uses it).
///
template <class T>
// NOLINTNEXTLINE(hicpp-special-member-functions) : false positive
class external_vector
    : external_vector_detail::storage_base<T>
    , public inline_vector_detail::vector_interface<external_vector_detail::external_storage_base<T>>
    , public inline_container::detail::comparison_interface<external_vector<T>>
    , public inline_container_detail::try_construct_interface<
          external_vector<T>,
          external_vector_detail::try_construct_policy<T>,
          inline_container_detail::try_construct_head_args<span<byte>>> {
  /// @brief storage base type
  using storage_base = external_vector_detail::storage_base<T>;

  /// @brief The type of the interface base class
  using interface_base = inline_vector_detail::vector_interface<external_vector_detail::external_storage_base<T>>;

  /// @brief comparison base type
  using comparison_base = inline_container::detail::comparison_interface<external_vector<T>>;

  /// @brief try_construct interface base type
  using try_construct_interface = inline_container_detail::try_construct_interface<
      external_vector,
      external_vector_detail::try_construct_policy<T>,
      inline_container_detail::try_construct_head_args<span<byte>>>;

  /// @brief helper that provides a type to use in defining move or copy assignment
  /// @tparam Enable @c true if defining the actual move/copy assignment
  ///   operator, @c false for the deleted overload
  /// @tparam U qualified value type, <tt> T const & </tt> for copy assignment,
  ///   <tt> T&& </tt> for move assignment
  /// @tparam Pred defaulted parameter that determines the result type
  ///
  /// Trait used when defining move or copy assignment operators.
  ///
  /// If @c Enable is @c true and the value type is copy/move
  /// constructible and assignable, the result is @c external_vector.
  /// If @c Enable is @c true and the value type is *not* copy/move
  /// constructible and assignable, the result is @c non_constructible_dummy.
  ///
  /// If @c Enable is @c false and the value type is copy/move
  /// constructible and assignable, the result is @c non_constructible_dummy.
  /// If @c Enable is @c false and the value type is *not* copy/move
  /// constructible and assignable, the result is @c external_vector.
  ///
  /// @note The result type has the reference and qualifiers properties of @c U.
  ///
  template <bool Enable, class U, bool Pred = std::is_constructible<T, U>::value && std::is_assignable<T&, U>::value>
  using enable_if_assignable_t = decltype(arene::base::forward_like<U>(  //
      std::declval<conditional_t<
          Enable,
          conditional_t<Pred, external_vector, non_constructible_dummy>,
          conditional_t<Pred, non_constructible_dummy, external_vector>>>()
  ));

 public:
  // Alias in typedefs from base class
  using typename interface_base::const_iterator;
  using typename interface_base::const_pointer;
  using typename interface_base::const_reference;
  using typename interface_base::const_reverse_iterator;
  using typename interface_base::difference_type;
  using typename interface_base::iterator;
  using typename interface_base::pointer;
  using typename interface_base::reference;
  using typename interface_base::reverse_iterator;
  using typename interface_base::size_type;
  using typename interface_base::value_type;

  using interface_base::assign;
  using interface_base::capacity;
  using interface_base::max_size;

  /// @brief default destructor
  ///
  /// Detroys values stored in the external buffer.
  ///
  ~external_vector() = default;

  /// @brief construct an empty @c external_vector
  /// @param buffer external buffer of bytes
  /// @pre the alignment of @c buffer matches the alignment of @c value_type
  /// @pre the size of @c buffer is an exact multiple of @c sizeof(value_type)
  /// @post <tt> size() == 0 </tt>
  /// @post <tt> capacity() == buffer.size() / sizeof(value_type) </tt>
  ///
  /// Constructs an empty @c external_vector that is configured to use @c buffer
  /// as external storage.
  ///
  explicit external_vector(span<byte> buffer) noexcept
      : storage_base{buffer},
        interface_base{},
        comparison_base{this},
        try_construct_interface{this} {}

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: All these constructors delegate to another, which does
  // initialize the base class"

  /// @brief construct an empty @c external_vector
  /// @post <tt> size() == 0 </tt>
  /// @post <tt> capacity() == 0 </tt>
  ///
  /// Constructs an empty @c external_vector without initializing any external
  /// storage.
  ///
  external_vector() noexcept
      : external_vector{span<byte>{}} {}

  /// @brief construct an @c external_vector from a range
  /// @tparam I input iterator type
  /// @param buffer external buffer of bytes
  /// @param first beginning of the range of values
  /// @param last end of the range of values
  /// @pre the alignment of @c buffer matches the alignment of @c value_type
  /// @pre the size of @c buffer is an exact multiple of @c sizeof(value_type)
  /// @pre <tt> std::distance(first, last) </tt> does not exceed the number of
  ///   values that can be stored in @c buffer
  ///
  /// Constructs an @c external_vector from range <tt> [first, last) </tt>.
  ///
  template <
      class I,
      constraints<
          std::enable_if_t<is_input_iterator_v<I>>,
          std::enable_if_t<std::is_constructible<value_type, typename std::iterator_traits<I>::reference>::value>> =
          nullptr>
  external_vector(span<byte> buffer, I first, I last) noexcept(
      denotes_nothrow_iterable_range_v<I> &&
      std::is_nothrow_constructible<value_type, typename std::iterator_traits<I>::reference>::value
  )
      : external_vector{buffer} {
    // this is likely to be replaced with 'vector_interface'
    while (first != last) {
      std::ignore = this->emplace_back(*first);
      arene::base::advance(first, typename std::iterator_traits<I>::difference_type{1});
    }
  }

  /// @brief construct an @c external_vector with default constructed values
  /// @tparam U dummy used to constrain this constructor to value types that are
  ///   default constructible
  /// @param buffer external buffer of bytes
  /// @param count size of the @c external_vector
  /// @pre the alignment of @c buffer matches the alignment of @c value_type
  /// @pre the size of @c buffer is an exact multiple of @c sizeof(value_type)
  /// @pre @c count does not exceed the number of values that can be stored in
  ///   @c buffer
  ///
  /// Constructs an @c external_vector containing @c count number of default
  /// constructed values.
  ///
  template <class U = value_type, constraints<std::enable_if_t<std::is_default_constructible<U>::value>> = nullptr>
  external_vector(span<byte> buffer, size_type count) noexcept(std::is_nothrow_default_constructible<value_type>::value)
      : external_vector{buffer} {
    while (count != 0U) {
      std::ignore = this->emplace_back();
      --count;
    }
  }

  /// @brief construct an @c external_vector with copies of a value
  /// @tparam U dummy used to constrain this constructor to value types that are
  ///   copy constructible
  /// @param buffer external buffer of bytes
  /// @param count size of the @c external_vector
  /// @param value value to copy into the @c external_vector
  /// @pre the alignment of @c buffer matches the alignment of @c value_type
  /// @pre the size of @c buffer is an exact multiple of @c sizeof(value_type)
  /// @pre @c count does not exceed the number of values that can be stored in
  ///   @c buffer
  ///
  /// Constructs an @c external_vector containing @c count number of copies of
  /// @c value.
  ///
  template <class U = value_type, constraints<std::enable_if_t<std::is_copy_constructible<U>::value>> = nullptr>
  external_vector(span<byte> buffer, size_type count, value_type const& value) noexcept(
      std::is_nothrow_copy_constructible<value_type>::value
  )
      : external_vector{buffer} {
    while (count != 0U) {
      std::ignore = this->emplace_back(value);
      --count;
    }
  }

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a "False positive: contents of 'other' are moved via use of 'move_iterator'"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a "False positive: contents of 'other' are moved via use of 'move_iterator'"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a "False positive: contents of 'other' are moved via use of
  // 'move_iterator'"

  /// @brief construct an @c external_vector from another @c external_vector
  /// @tparam U dummy used to constrain this constructor to value types that are
  ///   move constructible
  /// @param buffer external buffer of bytes
  /// @param other another @c external_vector
  /// @pre the alignment of @c buffer matches the alignment of @c value_type
  /// @pre the size of @c buffer is an exact multiple of @c sizeof(value_type)
  /// @pre @c other.size() does not exceed the number of values that can be
  ///   stored in @c buffer
  /// @pre @c buffer does not alias with the buffer managed by @c other
  ///
  /// Constructs an @c external_vector from @c other, moving values from the
  /// external buffer managed by @c other to @c buffer.
  ///
  /// @post @c other is in a valid but unspecified state
  ///
  template <class U = value_type, constraints<std::enable_if_t<std::is_move_constructible<U>::value>> = nullptr>
  external_vector(span<byte> buffer, external_vector&& other) noexcept(
      std::is_nothrow_move_constructible<value_type>::value
  )
      : external_vector{
            other.verify_not_already_in_use(buffer),
            std::make_move_iterator(other.begin()),
            std::make_move_iterator(other.end())
        } {}

  // parasoft-end-suppress AUTOSAR-A8_4_6-a
  // parasoft-end-suppress AUTOSAR-A8_4_5-a
  // parasoft-end-suppress AUTOSAR-A12_8_4-a

  /// @brief construct an @c external_vector from another @c external_vector
  /// @tparam U dummy used to constrain this constructor to value types that are
  ///   copy constructible
  /// @param buffer external buffer of bytes
  /// @param other another @c external_vector
  /// @pre the alignment of @c buffer matches the alignment of @c value_type
  /// @pre the size of @c buffer is an exact multiple of @c sizeof(value_type)
  /// @pre @c other.size() does not exceed the number of values that can be
  ///   stored in @c buffer
  /// @pre @c buffer does not alias with the buffer managed by @c other
  ///
  /// Constructs an @c external_vector from @c other, copying values from the
  /// external buffer managed by @c other to @c buffer.
  ///
  /// @post <tt> std::equal(this->begin(), this->end(), other.begin(), other.end()) </tt> is @c true
  ///
  template <class U = value_type, constraints<std::enable_if_t<std::is_copy_constructible<U>::value>> = nullptr>
  external_vector(span<byte> buffer, external_vector const& other) noexcept(
      std::is_nothrow_copy_constructible<value_type>::value
  )
      : external_vector{other.verify_not_already_in_use(buffer), other.begin(), other.end()} {}

  /// @brief construct an @c external_vector from a list of values
  /// @tparam U dummy used to constrain this constructor to value types that are
  ///   copy constructible
  /// @param buffer external buffer of bytes
  /// @param init_list values to copy
  /// @pre the alignment of @c buffer matches the alignment of @c value_type
  /// @pre the size of @c buffer is an exact multiple of @c sizeof(value_type)
  /// @pre @c init_list.size() does not exceed the number of values that can be
  ///   stored in @c buffer
  ///
  /// Constructs an @c external_vector with copies of values in @c init_list.
  ///
  template <class U = value_type, constraints<std::enable_if_t<std::is_copy_constructible<U>::value>> = nullptr>
  external_vector(span<byte> buffer, std::initializer_list<value_type> init_list) noexcept(
      std::is_nothrow_copy_constructible<value_type>::value
  )
      : external_vector{buffer, init_list.begin(), init_list.end()} {}

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a "The contents of 'other' are semantically moved via use of
  // 'move_iterator'" parasoft-begin-suppress AUTOSAR-A8_4_5-a "The contents of 'other' are semantically moved via use
  // of 'move_iterator'" parasoft-begin-suppress AUTOSAR-A12_8_4-a "The contents of 'other' are semantically moved via
  // use of 'move_iterator'"

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a "The contents of 'other' are semantically moved via swap"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a "The contents of 'other' are semantically moved via swap"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a "The contents of 'other' are semantically moved via swap"

  /// @brief construct an @c external_vector from another @c external_vector
  /// @param other @c external_vector to move from
  ///
  /// Constructs an @c external_vector by moving contents from @c other. This
  /// does not invoke any constructors of @c value_type.
  ///
  /// @post @c other is in a valid but unspecified state
  ///
  external_vector(external_vector&& other) noexcept
      : external_vector{} {
    external_vector::swap(other);
  }

  // parasoft-end-suppress AUTOSAR-A8_4_6-a
  // parasoft-end-suppress AUTOSAR-A8_4_5-a
  // parasoft-end-suppress AUTOSAR-A12_8_4-a

  /// @brief deleted copy constructor
  external_vector(external_vector const&) = delete;

  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  /// @brief deleted move assignment operator
  /// @note The argument type resolves to @c external_vector&& if
  ///   <tt> std::is_move_constructible<value_type>::value && std::is_move_assignable<value_type>::value </tt>
  ///   is @c false
  /// @note The move assignment operator is deleted if
  ///   <tt> std::is_move_constructible<value_type>::value && std::is_move_assignable<value_type>::value </tt>
  ///   is @c false
  ///
  auto operator=(enable_if_assignable_t<false, value_type&&>) -> external_vector& = delete;

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a "The contents of 'other' are semantically moved via make_move_iterator"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a "The contents of 'other' are semantically moved via make_move_iterator"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a "The contents of 'other' are semantically moved via make_move_iterator"

  /// @brief move assignment operator
  /// @param other @c external_vector to move values from
  /// @return @c *this
  ///
  /// Replaces the contents of the container with values from @c other. The
  /// first <tt> std::min(size(), other.size()) </tt> values are move assigned;
  /// remaining values are move constructed.
  ///
  /// @pre <tt> capacity() <= other.size() </tt>
  /// @post @c other is in a valid but unspecified state
  ///
  /// @note The argument type resolves to @c external_vector&& if
  ///   <tt> std::is_move_constructible<value_type>::value && std::is_move_assignable<value_type>::value </tt>
  ///   is @c true
  /// @note This assignment operator is deleted if
  ///   <tt> std::is_move_constructible<value_type>::value && std::is_move_assignable<value_type>::value </tt>
  ///   is @c false
  ///
  auto operator=(enable_if_assignable_t<true, value_type&&> other) noexcept(
      std::is_nothrow_move_constructible<value_type>::value && std::is_nothrow_move_assignable<value_type>::value
  ) -> external_vector& {
    this->assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
    return *this;
  }

  // parasoft-end-suppress AUTOSAR-A8_4_6-a
  // parasoft-end-suppress AUTOSAR-A8_4_5-a
  // parasoft-end-suppress AUTOSAR-A12_8_4-a

  /// @brief deleted copy assignment operator
  /// @note The argument type resolves to <tt> external_vector const& </tt> if
  ///   <tt> std::is_copy_constructible<value_type>::value && std::is_copy_assignable<value_type>::value </tt>
  ///   is @c false
  /// @note The copy assignment operator is deleted if
  ///   <tt> std::is_copy_constructible<value_type>::value && std::is_copy_assignable<value_type>::value </tt>
  ///   is @c false
  ///
  auto operator=(enable_if_assignable_t<false, value_type const&>) -> external_vector& = delete;

  /// @brief copy assignment operator
  /// @param other @c external_vector to copy values from
  /// @return @c *this
  ///
  /// Replaces the contents of the container with values from @c other. The
  /// first <tt> std::min(size(), other.size()) </tt> values are copy assigned;
  /// remaining values are copy constructed.
  ///
  /// @pre <tt> capacity() <= other.size() </tt>
  /// @post <tt> std::equal(this->begin(), this->end(), other.begin(), other.end()) </tt> is @c true
  ///
  /// @note The argument type resolves to <tt> external_vector const& </tt> if
  ///   <tt> std::is_copy_constructible<value_type>::value && std::is_copy_assignable<value_type>::value </tt>
  ///   is @c true
  /// @note This assignment operator is deleted if
  ///   <tt> std::is_copy_constructible<value_type>::value && std::is_copy_assignable<value_type>::value </tt>
  ///   is @c false
  ///
  auto operator=(enable_if_assignable_t<true, value_type const&> other) noexcept(
      std::is_nothrow_copy_constructible<value_type>::value && std::is_nothrow_copy_assignable<value_type>::value
  ) -> external_vector& {
    this->assign(other.begin(), other.end());
    return *this;
  }

  /// @brief assignment from a list of values
  /// @tparam U dummy used to constrain this assignment operator to value types
  ///   that are copy constructible and copy assignable
  /// @param init_list values to copy
  /// @return @c *this
  ///
  /// Replaces the contents of the container with values from @c other. The
  /// first <tt> std::min(size(), other.size()) </tt> values are copy assigned;
  /// remaining values are copy constructed.
  ///
  /// @pre <tt> capacity() <= other.size() </tt>
  /// @post <tt> std::equal(this->begin(), this->end(), other.begin(), other.end()) </tt> is @c true
  ///
  template <
      class U = value_type,
      constraints<std::enable_if_t<std::is_copy_constructible<U>::value && std::is_copy_assignable<U>::value>> =
          nullptr>
  auto operator=(std::initializer_list<value_type> init_list) noexcept(
      std::is_nothrow_copy_constructible<value_type>::value && std::is_nothrow_copy_assignable<value_type>::value
  ) -> external_vector& {
    // GCC8 constraint failure without instantiating the trait here
    // parasoft-begin-suppress CERT_C-PRE31-c "False positive: static_assert is
    // a compile-time assert and can have no side-effects"
    static_assert(is_random_access_iterator_v<decltype(init_list.begin())>, "");
    // parasoft-end-suppress CERT_C-PRE31-c
    this->assign(init_list.begin(), init_list.end());
    return *this;
  }

  /// @brief swap contents with a different @c external_vector
  /// @param other @c external_vector to swap contents with
  /// @note does not swap individual elements but rather the underlying buffer
  ///   associated with each @c external_vector
  ///
  auto swap(external_vector& other) noexcept -> void { storage_base::swap(other); }

  // parasoft-begin-suppress AUTOSAR-A7_1_1-a "Declaring 'lhs' or 'rhs' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-M7_1_2-c "Declaring 'lhs' or 'rhs' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-A8_4_9-a "Declaring 'lhs' or 'rhs' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2"
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: 'swap' is namespace scope and used in other
  // translation units"

  // parasoft-begin-suppress AUTOSAR-A2_7_2-a "False positive: no commented-out code, this is documentation"
  /// @brief swaps all the elements between two @c external_vector values
  /// @param lhs the left hand vector to swap
  /// @param rhs the right hand vector to swap
  /// @post equivalent to having called @c lhs.swap(rhs);
  /// @see external_vector::swap.
  ///
  // parasoft-end-suppress AUTOSAR-A2_7_2-a
  friend auto swap(external_vector& lhs, external_vector& rhs) noexcept -> void { lhs.swap(rhs); }

  // parasoft-end-suppress AUTOSAR-A7_1_1-a
  // parasoft-end-suppress AUTOSAR-M7_1_2-c
  // parasoft-end-suppress AUTOSAR-A8_4_9-a
  // parasoft-end-suppress AUTOSAR-A11_3_1-a
  // parasoft-end-suppress AUTOSAR-A0_1_3-a

  /// @brief Internal function to get access to the storage for an @c external_vector
  /// @param self The pointer to the internal base class
  /// @return A reference to the storage class
  static auto get_storage(
      typename external_vector_detail::external_storage_base<T>::storage_access_tag,
      external_vector_detail::external_storage_base<T> const* const self
  ) noexcept -> storage_base const& {
    return *static_cast<storage_base const*>(static_cast<external_vector const*>(self));
  }

  /// @brief Internal function to get access to the storage for an @c external_vector
  /// @param self The pointer to the internal base class
  /// @return A reference to the storage class
  static auto get_storage(
      typename external_vector_detail::external_storage_base<T>::storage_access_tag,
      external_vector_detail::external_storage_base<T>* const self
  ) noexcept -> storage_base& {
    return *static_cast<storage_base*>(static_cast<external_vector*>(self));
  }
};
// parasoft-end-suppress AUTOSAR-A10_1_1-a
// parasoft-end-suppress AUTOSAR-M14_5_3-a
// parasoft-end-suppress AUTOSAR-A7_3_1-a
// parasoft-end-suppress AUTOSAR-A10_2_1-b
// parasoft-end-suppress AUTOSAR-A12_1_6-a

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_EXTERNAL_VECTOR_HPP_
