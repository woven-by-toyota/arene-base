#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_CONFIGURABLE_SCALAR_VALUE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_CONFIGURABLE_SCALAR_VALUE_HPP_

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/type_traits/give_cv_to.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace arene {
namespace base {
namespace linalg_testing {

// The normal ::testing::configurable_value is unconditionally convertible to T, and making the conversion operator a
// template to disable it affects downstream overload resolution, so here's a miniature version that's not convertible.
template <typename T, ::testing::disable Disable = ::testing::disable::nothing>
class configurable_scalar_value {
  // This needs to be data member due to the following bug which affects GCC 9
  // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=103879
  ::testing::disable_mixin<Disable> disable_spec_;

 public:
  T value{};

  constexpr configurable_scalar_value() noexcept
      : disable_spec_{} {}

  template <class U, class = std::enable_if_t<std::is_constructible<T, U>::value>>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr configurable_scalar_value(U val) noexcept
      : disable_spec_{nullptr},
        value{static_cast<T>(val)} {}

  template <::testing::disable D = Disable, class = std::enable_if_t<!::testing::contains<D>(::testing::disable::plus)>>
  friend constexpr auto operator+(configurable_scalar_value const& lhs, configurable_scalar_value const& rhs) noexcept
      -> configurable_scalar_value {
    return configurable_scalar_value{lhs.value + rhs.value};
  }

  template <
      ::testing::disable D = Disable,
      class = std::enable_if_t<!::testing::contains<D>(::testing::disable::multiplies)>>
  friend constexpr auto operator*(configurable_scalar_value const& lhs, configurable_scalar_value const& rhs) noexcept
      -> configurable_scalar_value {
    return configurable_scalar_value{lhs.value * rhs.value};
  }

  template <
      ::testing::disable D = Disable,
      class = std::enable_if_t<!::testing::contains<D>(::testing::disable::plus_assign)>>
  constexpr auto operator+=(configurable_scalar_value const& rhs) noexcept -> configurable_scalar_value& {
    value += rhs.value;
    return *this;
  }
};

/// @brief Rebind the element type of an @c mdspan to a @c configurable_scalar_value with the given operators disabled
/// @tparam T The @c mdspan type to rebind the element of
/// @tparam Disable A specification of the functions to disable for the new element type
/// @note If @c T::element_type is CV-qualified, then the new element type will be
/// <c>configurable_scalar_value<El, Disable> CV</c> rather than <c>configurable_scalar_value<El CV, Disable></c>
/// @note Assumes that the accessor type is a template which takes @c element_type as its first argument
/// @{
template <class T, ::testing::disable Disable = ::testing::disable::nothing>
class rebind_to_configurable_scalar {};

template <class ElementType, class Extents, class Layout, template <class> class Accessor, ::testing::disable Disable>
class rebind_to_configurable_scalar<mdspan<ElementType, Extents, Layout, Accessor<ElementType>>, Disable> {
  using unqual_el_type = std::remove_cv_t<ElementType>;
  using new_el_type = give_cv_to_t<ElementType, configurable_scalar_value<unqual_el_type, Disable>>;

 public:
  using type = mdspan<new_el_type, Extents, Layout, Accessor<new_el_type>>;
};

template <class T, ::testing::disable Disable = ::testing::disable::nothing>
using rebind_to_configurable_scalar_t = typename rebind_to_configurable_scalar<T, Disable>::type;
/// @}

}  // namespace linalg_testing
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_CONFIGURABLE_SCALAR_VALUE_HPP_
