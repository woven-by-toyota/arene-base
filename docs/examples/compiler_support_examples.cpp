// Copyright 2024, Toyota Motor Corporation

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

//! [arene_guarantee_internal_definition_usage_example]
#define FOO_CONFIG_I_ ARENE_ON_BY_DEFAULT

static_assert(ARENE_GUARANTEE_INTERNAL_DEFINITION(FOO_CONFIG), "FOO_CONFIG must be defined");
//! [arene_guarantee_internal_definition_usage_example]

//! [arene_cpp14_inline_variable_fo]
namespace foo {
namespace fancy_add_impl {
/// @brief A function object that adds two values.
class fancy_add {
 public:
  /// @brief Adds two values.
  /// @tparam T The type of the values to add.
  /// @param lhs The left-hand side of the addition.
  /// @param rhs The right-hand side of the addition.
  /// @return The result of the addition.
  ///
  /// Note that we use the @c def and @c copydoc tags to copy the documentation of the overload and attach it to the
  /// definition of the inline variable produced by the macro in order to make it display correctly in doxygen.
  template <typename T>
  constexpr auto operator()(T const& lhs, T const& rhs) const -> decltype(lhs + rhs) {
    return lhs + rhs;
  }
};
}  // namespace fancy_add_impl

/// @def foo::fancy_add
/// @copydoc fancy_add_impl::fancy_add::operator()
ARENE_CPP14_INLINE_VARIABLE(fancy_add_impl::fancy_add, fancy_add);
}  // namespace foo

//! [arene_cpp14_inline_variable_fo]
