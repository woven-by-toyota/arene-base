// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

// IWYU pragma: no_include "arene/base/functional.hpp"
#include "arene/base/inline_container/function.hpp"  // IWYU pragma: keep
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/utility/to_underlying.hpp"

namespace {

using ::arene::base::default_inline_function_size;
using ::arene::base::inline_function;

/// @brief simple enum specifying if the function should be noexcept or not.
enum class noexcept_specifier : bool { is_noexcept = true, is_not_noexcept = false };

/// @brief A helper template for producing a `function_ref` specifier of a given noexcept qualification.
/// @tparam IsNoexcept If the function is noexcept or not
template <noexcept_specifier IsNoexcept>
struct inline_function_type {
  /// @brief Helper alias for producing an `inline_function` with a given signature whose noexcept-ness are set by the
  /// outer template.
  /// @tparam Signature The signature of the generated inline_function.
  /// @tparam BufferSize The buffer size of the generated inline_function.
  template <typename Signature, std::size_t BufferSize = default_inline_function_size>
  using inline_function =
      ::arene::base::inline_function<Signature, BufferSize, ::arene::base::to_underlying(IsNoexcept)>;
};

/// @brief A set of inline_function generators which includes all permutations of noexcept.
using all_inline_functions = ::testing::Types<
    inline_function_type<noexcept_specifier::is_noexcept>,
    inline_function_type<noexcept_specifier::is_not_noexcept>>;

template <typename T>
struct AllInlineFunctionsDeathTest : public testing::Test {};

TYPED_TEST_SUITE(AllInlineFunctionsDeathTest, all_inline_functions, );

/// @test Invoking a default-constructed `inline_function` terminates the program with a precondition violation
TYPED_TEST(AllInlineFunctionsDeathTest, InvokingAnEmptyInlineFunctionIsAPreconditionViolation) {
  ASSERT_DEATH(typename TypeParam::template inline_function<void()>{}(), "Precondition violation");
  ASSERT_DEATH(typename TypeParam::template inline_function<void() const>{}(), "Precondition violation");
}

}  // namespace
