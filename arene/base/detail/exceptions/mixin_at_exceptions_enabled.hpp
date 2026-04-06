// parasoft-begin-suppress AUTOSAR-A2_8_1-a "This is an implementation of mix_at so contains mixin_at_impl"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_MIXIN_AT_EXCEPTIONS_ENABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_MIXIN_AT_EXCEPTIONS_ENABLED_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/detail/exceptions/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/detail/exceptions/are_exceptions_enabled.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"

namespace arene {
namespace base {
namespace detail {

/// @brief Implementation helper for @c arene::base::detail::mixin_at which implements @c at().
/// @see arene::base::detail::mixin_at
///
/// @tparam ContainerType the type of the container class to mix into.
/// @tparam IndexType the type of the index used for the @c at() function.
template <typename ContainerType, typename IndexType>
class mixin_at_impl {
  static_assert(
      are_exceptions_enabled_v,
      "throwing mixin_at is only available when exceptions are enabled, this suggests a misconfiguration in build "
      "settings."
  );

  /// @brief Helper alias to detect if ContainerType has an index operator for IndexType.
  /// @tparam T the type to test.
  template <typename T>
  using use_index_operator = decltype(std::declval<T>()[std::declval<IndexType>()]);

  /// @brief Helper variable to detect if ContainerType has an index operator for IndexType.
  /// @tparam T the type to test.
  /// @return bool @c true if @c T has an index operator for @c IndexType , otherwise @c false .
  template <typename T>
  static constexpr bool has_index_operator_v{substitution_succeeds<use_index_operator, T>};

  /// @brief Casts this mixin to its container type.
  /// @return ContainerType& @c *this cast to the appropriate type.
  constexpr auto as_container() & noexcept -> ContainerType& { return static_cast<ContainerType&>(*this); }
  /// @brief Casts this mixin to its container type.
  /// @return ContainerType const& @c *this cast to the appropriate type.
  constexpr auto as_container() const& noexcept -> ContainerType const& {
    return static_cast<ContainerType const&>(*this);
  }
  /// @brief Casts this mixin to its container type.
  /// @return ContainerType& @c *this cast to the appropriate type.
  constexpr auto as_container() && noexcept -> ContainerType&& { return static_cast<ContainerType&&>(*this); }
  /// @brief Casts this mixin to its container type.
  /// @return ContainerType const& @c *this cast to the appropriate type.
  constexpr auto as_container() const&& noexcept -> ContainerType const&& {
    return static_cast<ContainerType const&&>(*this);
  }

  /// @brief Throws std::out_of_range if the provided index is out of range.
  /// @param idx the index to check.
  /// @throws std::out_of_range if @c idx>=this->size() .
  constexpr void throw_if_out_of_range(IndexType const idx) const {
    // The throw has to be guarded inside the condition due to a GCC8 bug in constexpr evaluation.
    if (idx >= as_container().size()) {
      throw std::out_of_range("Index out of range");
    }
  }

 public:
  /// @brief Accesses the value at the specified index.
  ///
  /// @tparam U SFINAE helper to ensure this overload is only available if the container type has a lvalue qualified
  /// index operator.
  /// @param idx The index of the element to access.
  /// @return The element at the specified index.
  /// @throws std::out_of_range if @c idx>=this->size() .
  template <typename U = ContainerType, constraints<std::enable_if_t<has_index_operator_v<U&>>> = nullptr>
  constexpr auto at(IndexType const idx) & -> decltype(auto) {
    throw_if_out_of_range(idx);
    return as_container()[idx];
  }
  /// @brief Accesses the value at the specified index.
  ///
  /// @tparam U SFINAE helper to ensure this overload is only available if the container type has a clvalue qualified
  /// index operator.
  /// @param idx The index of the element to access.
  /// @return The element at the specified index.
  /// @throws std::out_of_range if @c idx>=this->size() .
  template <typename U = ContainerType, constraints<std::enable_if_t<has_index_operator_v<U const&>>> = nullptr>
  constexpr auto at(IndexType const idx) const& -> decltype(auto) {
    throw_if_out_of_range(idx);
    return as_container()[idx];
  }
  /// @brief Accesses the value at the specified index.
  ///
  /// @tparam U SFINAE helper to ensure this overload is only available if the container type has a rvalue qualified
  /// index operator.
  /// @param idx The index of the element to access.
  /// @return The element at the specified index.
  /// @throws std::out_of_range if @c idx>=this->size() .
  template <typename U = ContainerType, constraints<std::enable_if_t<has_index_operator_v<U&&>>> = nullptr>
  constexpr auto at(IndexType const idx) && -> decltype(auto) {
    throw_if_out_of_range(idx);
    return (std::move(*this)).as_container()[idx];
  }
  /// @brief Accesses the value at the specified index.
  ///
  /// @tparam U SFINAE helper to ensure this overload is only available if the container type has a crvalue qualified
  /// index operator.
  /// @param idx The index of the element to access.
  /// @return The element at the specified index.
  /// @throws std::out_of_range if @c idx>=this->size() .
  template <typename U = ContainerType, constraints<std::enable_if_t<has_index_operator_v<U const&&>>> = nullptr>
  constexpr auto at(IndexType const idx) const&& -> decltype(auto) {
    throw_if_out_of_range(idx);
    return (std::move(*this)).as_container()[idx];
  }
};

}  // namespace detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_MIXIN_AT_EXCEPTIONS_ENABLED_HPP_
