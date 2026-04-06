// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_FUNCTION_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_FUNCTION_HPP_

// IWYU pragma: private, include "arene/base/functional.hpp"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/array/array.hpp"
#include "arene/base/byte/byte.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/memory/construct_at.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/decay.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_base_of.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A15_4_3-a-2 "False positive: This file is the only declaration of these functions"

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-A5_1_1-a "False positive: The literal here is a scalar for a multiply."
/// @brief The default size of the buffer used for @c inline_function .
static constexpr std::size_t default_inline_function_size{4U * sizeof(void*)};
// parasoft-end-suppress AUTOSAR-A5_1_1-a

/// @brief A function wrapper akin to @c std::function that always stores the wrapped invocable internally.
/// @tparam Signature The signature of the function call operator.
/// @tparam BufferSize The size of the internal buffer. It defaults to 4 times the size of a pointer.
/// @tparam IsNoexcept If @c true , the function will have a @c noexcept qualified call operator. Otherwise, it will
///         not.
/// @note If the signature is const-qualified (e.g. <c> int (int,double) const</c> ) then the function call operator for
/// this class will also be const-qualified. If the signature is *not* const-qualified, then only non-const objects will
/// be invocable.
template <typename Signature, std::size_t BufferSize = default_inline_function_size, bool IsNoexcept = false>
class inline_function;

/// @brief A function wrapper to a noexcept-qualified invocable akin to @c std::function that always stores the wrapped
/// invocable internally.
/// @tparam Signature The signature of the function call operator.
/// @tparam BufferSize The size of the internal buffer. It defaults to 4 times the size of a pointer.
/// @note If the signature is const-qualified (e.g. <c> int (int,double) const</c> ) then the function call operator for
/// this class will also be const-qualified. If the signature is *not* const-qualified, then only non-const objects will
/// be invocable.
template <typename Signature, std::size_t BufferSize = default_inline_function_size>
using noexcept_inline_function = inline_function<Signature, BufferSize, true>;

namespace inline_function_detail {

// parasoft-begin-suppress AUTOSAR-A12_1_5-a-2 "False positive: delegating constructors are used"
/// @brief Base class for @c inline_function that provides the implementation details
/// @tparam BufferSize The size of the internal buffer.
/// @tparam IsConst Should this function have a const-qualified function call operator?
/// @tparam IsNoexcept Should this function require a noexcept-qualified callable?
/// @tparam ReturnType The return type of the function call operator
/// @tparam Args The argument types of the function call operator
template <std::size_t BufferSize, bool IsConst, bool IsNoexcept, typename ReturnType, typename... Args>
class inline_function_base {
  /// @brief The type of the @c self argument for the @c invoke function
  using invoke_self_type = std::conditional_t<IsConst, inline_function_base const&, inline_function_base&>;

  /// @brief The function signature of the @c invoke function
  using invoke_func_type = ReturnType(invoke_self_type self, Args&&... args);

  /// @brief A table of operations that can be applied to the wrapped invocable.
  /// The members are @c const so that the compiler can see the values don't change: there should be exactly one of
  /// these objects per type of callable.
  struct operation_functions {
    /// @brief A pointer to a function that destroys the wrapped invocable
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
    void (*const destroy)(inline_function_base& self) noexcept;
    /// @brief A pointer to a function that moves the wrapped invocable from @c source
    /// to @c target, leaving @c source empty
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
    void (*const move)(inline_function_base& target, inline_function_base& source) noexcept;
    /// @brief A pointer to a function that invokes the wrapped invocable with the
    /// supplied arguments
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
    invoke_func_type* const invoke;
  };

  /// @brief Destroy a wrapped invocable.
  /// @tparam T The type of the wrapped invocable
  /// @param self The wrapper object that holds the invocable
  template <typename T>
  static void do_destroy(inline_function_base& self) noexcept {
    // parasoft-begin-suppress AUTOSAR-M0_1_3-b-2 "False positive: The variable is used on the next line"
    T& wrapped_invocable{*self.template get_ptr<T>()};
    // parasoft-end-suppress AUTOSAR-M0_1_3-b-2
    wrapped_invocable.~T();
    self.operations_ = nullptr;
  }

  /// @brief Move a wrapped invocable from one wrapper to another and leaves the source
  /// empty.
  /// @tparam T The type of the wrapped invocable
  /// @param source The wrapper object that holds the source invocable
  /// @param target The wrapper object that will hold the moved invocable
  template <typename T>
  static void do_move(inline_function_base& target, inline_function_base& source) noexcept {
    target.construct_from(std::move(*source.template get_ptr<T>()));
    target.operations_ = source.operations_;
    do_destroy<T>(source);
  }

  /// @brief Invoke the wrapped invocable as a const or non-const object as applicable,
  /// with the supplied arguments.
  /// @tparam T The type of the wrapped invocable
  /// @param self The wrapper object that holds the invocable
  /// @param args The function call arguments
  /// @return The result of the invocation
  template <typename T>
  static auto do_invoke(invoke_self_type self, Args&&... args) -> ReturnType {
    auto& wrapped_invocable = *self.template get_ptr<T>();
    // parasoft-begin-suppress AUTOSAR-A18_9_2-a-2 "Use of 'forward' here ensures rvalues are correctly moved"
    return static_cast<ReturnType>(wrapped_invocable(std::forward<Args>(args)...));
    // parasoft-end-suppress AUTOSAR-A18_9_2-a-2
  }

  /// @brief The table of function pointers for a given invocable type
  /// @tparam T The type of the wrapped invocable
  template <typename T>
  static constexpr operation_functions call_table{&do_destroy<T>, &do_move<T>, &do_invoke<T>};

 protected:
  /// @brief Trait to determine if a functor is invocable with matching constness, noexcept, return and arguments.
  /// @tparam T the function type to test.
  /// @return If @c IsNoexcept , then equivalent to @c is_nothrow_invocable_r_v<ReturnType,T,Args...> . Otherwise,
  ///         equivalent to @c is_invocable_r_v<ReturnType,T,Args...>
  template <typename T>
  static constexpr bool
      is_invocable_n_r_v{IsNoexcept ? ::arene::base::is_nothrow_invocable_r_v<ReturnType, T, Args...> : ::arene::base::is_invocable_r_v<ReturnType, T, Args...>};

 public:
  /// @brief Default construct with no wrapped invocable
  inline_function_base() noexcept
      : buffer_{},
        operations_{nullptr} {}

  /// @brief Not copyable
  inline_function_base(inline_function_base const&) = delete;
  /// @brief Not copyable
  auto operator=(inline_function_base const&) -> inline_function_base& = delete;

  /// @brief Construct with a wrapped invocable
  /// @tparam T The type of the wrapped invocable
  /// @param invocable The invocable to wrap
  /// @pre The supplied invocable must be invocable with @c Args, with a return
  /// type that can be converted to @c ReturnType, both as a const and non-const
  /// object. The invocable must also be nothrow-move-constructible, and must
  /// fit in the buffer.
  template <
      typename T,
      constraints<
          std::enable_if_t<(sizeof(std::decay_t<T>) <= BufferSize)>,
          std::enable_if_t<std::is_nothrow_move_constructible<std::decay_t<T>>::value>,
          std::enable_if_t<is_invocable_n_r_v<std::conditional_t<IsConst, std::decay_t<T> const, std::decay_t<T>>>>> =
          nullptr>
  // NOLINTNEXTLINE(bugprone-forwarding-reference-overload,google-explicit-constructor,hicpp-explicit-conversions)
  inline_function_base(T&& invocable) noexcept
      : buffer_{},
        operations_(&call_table<std::decay_t<T>>) {
    construct_from(std::forward<T>(invocable));
  }

  /// @brief Deleted constructor for an invocable that is too large for the buffer. The
  /// template parameter and parameter names are specified to make the error
  /// message clearer.
  /// @tparam InvocableTooBigForBuffer The type of the supplied invocable
  /// @param buffer_too_small The supplied invocable
  template <
      typename InvocableTooBigForBuffer,
      constraints<
          std::enable_if_t<(sizeof(std::decay_t<InvocableTooBigForBuffer>) > BufferSize)>,
          std::enable_if_t<!std::is_base_of<inline_function_base, std::decay_t<InvocableTooBigForBuffer>>::value>> =
          nullptr>
  // NOLINTNEXTLINE(bugprone-forwarding-reference-overload,google-explicit-constructor,hicpp-explicit-conversions)
  inline_function_base(InvocableTooBigForBuffer&& buffer_too_small) = delete;

  /// @brief Assign from an invocable
  /// @tparam T The type of the invocable
  /// @param invocable The invocable to wrap
  /// @pre The supplied invocable must be invocable with @c Args, with a return
  /// type that can be converted to @c ReturnType, both as a const and non-const
  /// object. The invocable must also be nothrow-move-constructible, and must
  /// fit in the buffer.
  template <
      typename T,
      constraints<
          std::enable_if_t<(sizeof(std::decay_t<T>) <= BufferSize)>,
          std::enable_if_t<std::is_nothrow_move_constructible<std::decay_t<T>>::value>,
          std::enable_if_t<is_invocable_n_r_v<std::conditional_t<IsConst, std::decay_t<T> const, std::decay_t<T>>>>> =
          nullptr>
  void assign_from(T&& invocable) noexcept {
    destroy();
    operations_ = &call_table<std::decay_t<T>>;
    construct_from(std::forward<T>(invocable));
  }

  /// @brief Construct from an invocable
  /// @tparam T The type of the invocable
  /// @param invocable The invocable to wrap
  /// @pre The supplied invocable must be invocable with @c Args, with a return
  /// type that can be converted to @c ReturnType, both as a const and non-const
  /// object. The invocable must also be nothrow-move-constructible, and must
  /// fit in the buffer.
  template <
      typename T,
      constraints<
          std::enable_if_t<(sizeof(std::decay_t<T>) <= BufferSize)>,
          std::enable_if_t<std::is_nothrow_move_constructible<std::decay_t<T>>::value>,
          std::enable_if_t<is_invocable_n_r_v<std::conditional_t<IsConst, std::decay_t<T> const, std::decay_t<T>>>>> =
          nullptr>
  void construct_from(T&& invocable) noexcept {
    construct_at(get_ptr<std::decay_t<T>>(), std::forward<T>(invocable));
  }

  /// @brief Destroy the wrapped invocable
  ~inline_function_base() { destroy(); }

  /// @brief Is this function initialized with an invocable?
  /// @return @c true if the object is initialized, @c false otherwise
  auto is_initialized() const noexcept -> bool { return operations_ != nullptr; }

  /// @brief Get a pointer to the invoke function
  /// @return A pointer to the function
  auto get_invoke() const noexcept -> invoke_func_type* {
    ARENE_PRECONDITION(is_initialized());
    return operations_->invoke;
  }

  /// @brief Check if there is a wrapped invocable
  /// @return true If there is a wrapped invocable.
  /// @return false Otherwise.
  explicit operator bool() const noexcept { return is_initialized(); }

 protected:
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: The data is moved via the 'move_from' function"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: The data is moved via the 'move_from' function"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: The data is moved via the 'move_from' function"
  /// @brief Construct a new object that takes ownership of the invocable held by the
  /// source. Leaves the source without an invocable.
  /// @param other The object to move from
  inline_function_base(inline_function_base&& other) noexcept
      : inline_function_base{} {
    move_from(other);
  }
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: The data is moved via the 'move_from' function"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: The data is moved via the 'move_from' function"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: this is not a move constructor"
  /// @brief Transfer ownership of an invocable from another object. Leaves the source
  /// without an invocable.
  /// @param other The object to move from
  /// @return A reference to @c *this
  auto operator=(inline_function_base&& other) noexcept -> inline_function_base& {
    if (&other != this) {
      destroy();
      move_from(other);
    }
    return *this;
  }
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2

 private:
  /// @brief Destroy the invocable if there is one
  void destroy() noexcept {
    if (is_initialized()) {
      operations_->destroy(*this);
    }
  }

  /// @brief Transfer ownership of an invocable from another object, if it has one
  /// @param other The object to move from
  void move_from(inline_function_base& other) {
    if (other.is_initialized()) {
      other.operations_->move(*this, other);
    }
  }

  /// @brief Get a pointer to the wrapped invocable or its storage (if not yet
  /// constructed)
  /// @tparam T The type of the wrapped invocable
  /// @return A pointer to the object or storage
  template <typename T>
  auto get_ptr() noexcept -> T* {
    return static_cast<T*>(static_cast<void*>(buffer_.data()));
  }

  /// @brief Get a const pointer to the wrapped invocable
  /// @tparam T The type of the wrapped invocable
  /// @return A pointer to the object
  template <typename T>
  auto get_ptr() const noexcept -> T const* {
    return static_cast<T const*>(static_cast<void const*>(buffer_.data()));
  }

  /// @brief The storage for the wrapped invocable
  alignas(std::max_align_t) array<byte, BufferSize> buffer_;

  /// @brief The pointer to the operation table
  operation_functions const* operations_;
};
// parasoft-end-suppress AUTOSAR-A12_1_5-a-2

/// @brief Out-of-line definition for the operation table
/// @tparam ReturnType The return type of the function call operator
/// @tparam Args The argument types of the function call operator
/// @tparam BufferSize The size of the internal buffer.
/// @tparam T The type of the wrapped invocable
template <std::size_t BufferSize, bool IsConst, bool IsNoexcept, typename ReturnType, typename... Args>
template <typename T>
constexpr typename inline_function_base<BufferSize, IsConst, IsNoexcept, ReturnType, Args...>::operation_functions
    inline_function_base<BufferSize, IsConst, IsNoexcept, ReturnType, Args...>::call_table;

}  // namespace inline_function_detail

/// @brief A function wrapper akin to @c std::function that always stores the
/// wrapped invocable internally. This specialization is for where
/// the function type is not @c const qualified
/// @tparam ReturnType The return type of the function call operator
/// @tparam Args The argument types of the function call operator
/// @tparam BufferSize The size of the internal buffer.
/// @tparam IsNoexcept If @c true , the function will have a @c noexcept qualified call operator. Otherwise, it will
///         not.
template <typename ReturnType, typename... Args, std::size_t BufferSize, bool IsNoexcept>
class inline_function<ReturnType(Args...), BufferSize, IsNoexcept>
    : inline_function_detail::inline_function_base<BufferSize, false, IsNoexcept, ReturnType, Args...> {
  /// @brief The type of the base class
  using base_class = inline_function_detail::inline_function_base<BufferSize, false, IsNoexcept, ReturnType, Args...>;

 public:
  /// @brief Inherit the base class converting constructors
  using base_class::base_class;

  /// @brief Default construct with no stored invocable
  inline_function() noexcept = default;

  /// @brief Transfer ownership of an invocable from another object. Leaves the source
  /// without an invocable.
  /// @param other The object to move from
  inline_function(inline_function&& other) noexcept = default;

  /// @brief Transfer ownership of an invocable from another object. Leaves the source
  /// without an invocable.
  /// @param other The object to move from
  /// @return A reference to @c *this
  auto operator=(inline_function&& other) noexcept -> inline_function& = default;

  /// @brief Assign from a wrapped invocable
  /// @tparam T The type of the wrapped invocable
  /// @param invocable The invocable to wrap
  /// @return A reference to @c *this
  /// @pre The supplied invocable must be invocable with @c Args, with a return
  /// type that can be converted to @c ReturnType, both as a const and non-const
  /// object. The invocable must also be nothrow-move-constructible, and must
  /// fit in the buffer.
  template <
      typename T,
      constraints<
          std::enable_if_t<(sizeof(std::decay_t<T>) <= BufferSize)>,
          std::enable_if_t<std::is_nothrow_move_constructible<std::decay_t<T>>::value>,
          std::enable_if_t<base_class::template is_invocable_n_r_v<std::decay_t<T>>>> = nullptr>
  auto operator=(T&& invocable) noexcept -> inline_function& {
    this->assign_from(std::forward<T>(invocable));
    return *this;
  }

  /// @brief Deleted assignment operator for an invocable that is too large for the
  /// buffer. The template parameter and parameter names are specified to make
  /// the error message clearer.
  /// @tparam InvocableTooBigForBuffer The type of the supplied invocable
  /// @param buffer_too_small The supplied invocable
  template <
      typename InvocableTooBigForBuffer,
      constraints<
          std::enable_if_t<(sizeof(std::decay_t<InvocableTooBigForBuffer>) > BufferSize)>,
          std::enable_if_t<!std::is_same<inline_function, std::decay_t<InvocableTooBigForBuffer>>::value>> = nullptr>
  auto operator=(InvocableTooBigForBuffer&& buffer_too_small) -> inline_function& = delete;

  /// @brief Not copyable
  inline_function(inline_function const&) = delete;
  /// @brief Not copyable
  auto operator=(inline_function const&) -> inline_function& = delete;

  /// @brief Destroy the wrapped invocable
  ~inline_function() = default;

  /// @brief Inherit the base class boolean conversion operator
  using base_class::operator bool;

  /// @brief Invoke the wrapped invocable as a non-const object, with the supplied
  /// arguments
  /// @param args The arguments for the call
  /// @return The value returned from the call
  auto operator()(Args... args) noexcept(IsNoexcept) -> ReturnType {
    ARENE_PRECONDITION(this->is_initialized());
    // parasoft-begin-suppress AUTOSAR-A18_9_2-a-2 "Use of 'forward' here ensures rvalues are correctly moved"
    return this->get_invoke()(*this, std::forward<Args>(args)...);
    // parasoft-end-suppress AUTOSAR-A18_9_2-a-2
  }
};

/// @brief A function wrapper akin to @c std::function that always stores the
/// wrapped invocable internally. This specialization is for where the
/// function type is @c const qualified
/// @tparam ReturnType The return type of the function call operator
/// @tparam Args The argument types of the function call operator
/// @tparam BufferSize The size of the internal buffer.
/// @tparam IsNoexcept If @c true , the function will have a @c noexcept qualified call operator. Otherwise, it will
///         not.
template <typename ReturnType, typename... Args, std::size_t BufferSize, bool IsNoexcept>
class inline_function<ReturnType(Args...) const, BufferSize, IsNoexcept>
    : inline_function_detail::inline_function_base<BufferSize, true, IsNoexcept, ReturnType, Args...> {
  /// @brief Internal type alias for the base class
  using base_class = inline_function_detail::inline_function_base<BufferSize, true, IsNoexcept, ReturnType, Args...>;

 public:
  /// @brief Inherit the base class converting constructors
  using base_class::base_class;

  /// @brief Default construct with no stored invocable
  inline_function() noexcept = default;

  /// @brief Transfer ownership of an invocable from another object. Leaves the source
  /// without an invocable.
  /// @param other The object to move from
  inline_function(inline_function&& other) noexcept = default;

  /// @brief Transfer ownership of an invocable from another object. Leaves the source
  /// without an invocable.
  /// @param other The object to move from
  /// @return A reference to @c *this
  auto operator=(inline_function&& other) noexcept -> inline_function& = default;

  /// @brief Assign from a wrapped invocable
  /// @tparam T The type of the wrapped invocable
  /// @param invocable The invocable to wrap
  /// @return A reference to @c *this
  /// @pre The supplied invocable must be invocable with @c Args, with a return
  /// type that can be converted to @c ReturnType, both as a const and non-const
  /// object. The invocable must also be nothrow-move-constructible, and must
  /// fit in the buffer.
  template <
      typename T,
      constraints<
          std::enable_if_t<(sizeof(std::decay_t<T>) <= BufferSize)>,
          std::enable_if_t<std::is_nothrow_move_constructible<std::decay_t<T>>::value>,
          std::enable_if_t<base_class::template is_invocable_n_r_v<std::decay_t<T> const>>> = nullptr>
  auto operator=(T&& invocable) noexcept -> inline_function& {
    this->assign_from(std::forward<T>(invocable));
    return *this;
  }

  /// @brief Deleted assignment operator for an invocable that is too large for the
  /// buffer. The template parameter and parameter names are specified to make
  /// the error message clearer.
  /// @tparam InvocableTooBigForBuffer The type of the supplied invocable
  /// @param buffer_too_small The supplied invocable
  template <
      typename InvocableTooBigForBuffer,
      constraints<
          std::enable_if_t<(sizeof(std::decay_t<InvocableTooBigForBuffer>) > BufferSize)>,
          std::enable_if_t<!std::is_same<inline_function, std::decay_t<InvocableTooBigForBuffer>>::value>> = nullptr>
  auto operator=(InvocableTooBigForBuffer&& buffer_too_small) -> inline_function& = delete;

  /// @brief Not copyable
  inline_function(inline_function const&) = delete;
  /// @brief Not copyable
  auto operator=(inline_function const&) -> inline_function& = delete;

  /// @brief Destroy the wrapped invocable
  ~inline_function() = default;

  /// @brief Inherit the base class boolean conversion operator
  using base_class::operator bool;

  /// @brief Invoke the wrapped invocable as a const object, with the supplied
  /// arguments
  /// @param args The arguments for the call
  /// @return The value returned from the call
  auto operator()(Args... args) const noexcept(IsNoexcept) -> ReturnType {
    ARENE_PRECONDITION(this->is_initialized());
    return this->get_invoke()(*this, std::forward<Args>(args)...);
  }
};

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_FUNCTION_HPP_
