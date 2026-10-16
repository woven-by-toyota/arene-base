// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_LAPACK_OVERLOAD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_LAPACK_OVERLOAD_HPP_

#include "arene/base/stdlib_choice/complex.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_const.hpp"
#include "arene/base/stdlib_choice/remove_pointer.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/type_list/index_of.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/disjunction.hpp"
#include "arene/base/type_traits/type_identity.hpp"

namespace arene {
namespace base {
namespace linalg_testing {

/// @brief Determine whether @c T is a LAPACK data type
/// @tparam T The type to test
///
/// Provides a member constant @c value equal to @c true if @c T is @c float or
/// @c double , and @c false otherwise.
///
/// @note The set of LAPACK data types also includes @c std::complex<float> and
/// @c std::complex<double>, but the real types (S and D) are currently enabled.
template <class T>
struct is_lapack_data_type
    : ::arene::base::disjunction<
          std::is_same<T, float>,
          std::is_same<T, double>,
          std::is_same<T, std::complex<float>>,
          std::is_same<T, std::complex<double>>> {};

/// @brief Helper variable template for @c is_lapack_data_type
/// @relates is_lapack_data_type
/// @tparam T The type to test
template <class T>
constexpr auto is_lapack_data_type_v = is_lapack_data_type<T>::value;

/// @brief Determine the LAPACK element type from a set of argument types
/// @tparam Args Argument types passed to a LAPACK overload object
///
/// Defines the member typedef @c type to be the first argument type @c T (with
/// any pointer and top-level @c const stripped) for which
/// @c is_lapack_data_type_v<T> is @c true .
///
/// @note This assumes the underlying routine is homogeneous in its element type,
/// i.e. the element type is the first LAPACK-typed argument. This holds for the
/// standard BLAS/LAPACK data-type families (xDOT, xAXPY, xCOPY, ...). Routines
/// that mix element types in their arguments (for example @c csscal , which takes
/// both a real scalar and complex vectors) are not supported by this deduction.
template <class... Args>
struct lapack_overload_type;

template <class A0, class... An>
struct lapack_overload_type<A0, An...>
    : std::conditional_t<
          is_lapack_data_type_v<std::remove_const_t<std::remove_pointer_t<A0>>>,
          ::arene::base::type_identity<std::remove_const_t<std::remove_pointer_t<A0>>>,
          lapack_overload_type<An...>> {};

/// @brief Helper template alias for @c lapack_overload_type
/// @relates lapack_overload_type
/// @tparam Args Argument types passed to a LAPACK overload object
template <class... Args>
using lapack_overload_type_t = typename lapack_overload_type<Args...>::type;

/// @brief A group of LAPACK functions or subroutines keyed by element type
/// @tparam S Real (single-precision, @c float ) function pointer type
/// @tparam D Double-precision (@c double ) function pointer type
/// @tparam C Single-precision complex (@c std::complex<float> ) function pointer type
/// @tparam Z Double-precision complex (@c std::complex<double> ) function pointer type
///
/// A LAPACK overload object holds pointers to the underlying LAPACK functions or
/// subroutines. Invoking the object deduces the element type from the argument
/// pointers and dispatches to the matching underlying function, so the caller
/// never has to name the element type.
template <class S, class D, class C, class Z>
class lapack_overload {
  /// @brief The LAPACK element types in the same order as the stored overloads
  using element_types = ::arene::base::type_list<float, double, std::complex<float>, std::complex<double>>;

  /// @brief The underlying function pointers keyed positionally by @c element_types
  std::tuple<S, D, C, Z> overloads_;

  /// @brief Select the underlying function pointer for element type @c T
  /// @tparam T LAPACK element type to dispatch on
  /// @return The function pointer matching @c T
  template <class T>
  constexpr auto get() const noexcept {
    return std::get<::arene::base::type_lists::index_of_v<element_types, T>>(overloads_);
  }

 public:
  /// @brief Construct from the underlying function pointers
  /// @param float_fn Real (@c float ) function pointer
  /// @param double_fn Double-precision (@c double ) function pointer
  /// @param complex_float_fn Single-precision complex (@c std::complex<float> ) function pointer
  /// @param complex_double_fn Double-precision complex (@c std::complex<double> ) function pointer
  constexpr lapack_overload(S float_fn, D double_fn, C complex_float_fn, Z complex_double_fn) noexcept
      : overloads_{float_fn, double_fn, complex_float_fn, complex_double_fn} {}

  /// @brief Call the underlying LAPACK overload
  /// @tparam Args Argument pointee types
  /// @param args Arguments passed to the underlying LAPACK overload
  /// @return The result of the underlying LAPACK overload
  ///
  /// Dispatches to the LAPACK overload for the element type deduced from
  /// @c Args... as either @c float or @c double .
  ///
  /// @note Arguments follow the FORTRAN calling convention and must be passed by
  /// address.
  ///
  /// @see
  /// https://www.intel.com/content/www/us/en/docs/onemkl/developer-guide-linux/2023-0/call-lapack-blas-and-cblas-from-c-language-env.html
  template <class... Args>
  auto operator()(Args... args) const noexcept {
    return get<lapack_overload_type_t<std::remove_pointer_t<Args>...>>()(args...);
  }
};

/// @cond
constexpr struct {
  template <class... Args>
  auto operator()(Args const&...) const = delete;
} unspecified_function{};
/// @endcond

/// @brief Construct a LAPACK overload object
/// @tparam S Real (single-precision, @c float ) function pointer type
/// @tparam D Double-precision (@c double ) function pointer type
/// @param float_fn Real (@c float ) function pointer
/// @param double_fn Double-precision (@c double ) function pointer
/// @return A @c lapack_overload holding @c float_fn, @c double_fn
///
/// Constructs a LAPACK overload object holding pointers to the underlying LAPACK
/// functions or subroutines. The appropriate overload is invoked by passing
/// arguments to the returned object. Complex functions are uninvocable function objects.
template <class S, class D>
constexpr auto make_lapack_overload(S float_fn, D double_fn) noexcept
    -> lapack_overload<S, D, decltype(unspecified_function), decltype(unspecified_function)> {
  return {float_fn, double_fn, unspecified_function, unspecified_function};
}

/// @brief Construct a LAPACK overload object
/// @tparam S Real (single-precision, @c float ) function pointer type
/// @tparam D Double-precision (@c double ) function pointer type
/// @tparam C Single-precision complex (@c std::complex<float> ) function pointer type
/// @tparam Z Double-precision complex (@c std::complex<double> ) function pointer type
/// @param float_fn Real (@c float ) function pointer
/// @param double_fn Double-precision (@c double ) function pointer
/// @param complex_float_fn Single-precision complex (@c std::complex<float> ) function pointer
/// @param complex_double_fn Double-precision complex (@c std::complex<double> ) function pointer
/// @return A @c lapack_overload holding @c float_fn, @c double_fn, @c complex_float_fn, @c complex_double_fn
///
/// Constructs a LAPACK overload object holding pointers to the underlying LAPACK
/// functions or subroutines. The appropriate overload is invoked by passing
/// arguments to the returned object.
template <class S, class D, class C, class Z>
constexpr auto make_lapack_overload(S float_fn, D double_fn, C complex_float_fn, Z complex_double_fn) noexcept
    -> lapack_overload<S, D, C, Z> {
  return {float_fn, double_fn, complex_float_fn, complex_double_fn};
}

}  // namespace linalg_testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_LAPACK_OVERLOAD_HPP_
