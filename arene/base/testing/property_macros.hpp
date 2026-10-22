// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_MACROS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_MACROS_HPP_

#include <gtest/gtest.h>

#include "arene/base/compiler_support/preprocessor/concatenate.hpp"
#include "arene/base/testing/detail/any_prng_ref.hpp"         // IWYU pragma: keep
#include "arene/base/testing/detail/property_trampoline.hpp"  // IWYU pragma: keep
#include "arene/base/testing/property_config.hpp"             // IWYU pragma: keep
#include "arene/base/testing/property_summary.hpp"            // IWYU pragma: keep
#include "arene/base/testing/run_property.hpp"                // IWYU pragma: keep

// IWYU pragma: no_include "arene/base/optional/optional.hpp"
// IWYU pragma: no_include "arene/base/testing/property_error.hpp"

/// @brief unique closure type name for a property test
/// @param TestSuite test suite name
/// @param TestCase test case name
/// @return property_closure_<TestSuite>_<TestCase>
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_INTERNAL_PROPERTY_CLOSURE_NAME(TestSuite, TestCase) \
  ARENE_CONCATENATE(ARENE_CONCATENATE(property_closure_, TestSuite), ARENE_CONCATENATE(_, TestCase))

/// @brief three-argument form of @c ARENE_PROPERTY
/// @param TestSuite test suite name
/// @param TestCase test case name
/// @param Params parenthesized parameter list for the property body
///
/// Generates the closure type, the GoogleTest/Minitest @c TEST registration,
/// and the out-of-class definition stub of the property's @c operator(). The user's
/// braced block that follows the macro invocation becomes the body of that @c
/// operator().
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_INTERNAL_PROPERTY3(TestSuite, TestCase, Params)             \
  struct ARENE_INTERNAL_PROPERTY_CLOSURE_NAME(TestSuite, TestCase) {      \
    ::arene::base::testing::detail::any_prng_ref prng;                    \
    auto operator() Params const -> void;                                 \
  };                                                                      \
  TEST(TestSuite, TestCase) {                                             \
    auto const summary = ::arene::base::testing::run_property(            \
        ::arene::base::testing::detail::property_trampoline<              \
            ARENE_INTERNAL_PROPERTY_CLOSURE_NAME(TestSuite, TestCase)>{}, \
        ::arene::base::testing::read_property_config()                    \
    );                                                                    \
    if (summary.failed) {                                                 \
      FAIL() << summary;                                                  \
    }                                                                     \
  }                                                                       \
  auto ARENE_INTERNAL_PROPERTY_CLOSURE_NAME(TestSuite, TestCase)::operator() Params const -> void

/// @brief two-argument form of @c ARENE_PROPERTY
/// @param TestSuite test suite name
/// @param TestCase test case name
///
/// Forwards to @c ARENE_INTERNAL_PROPERTY3 with an empty parameter list,
/// producing a property whose body takes no generated arguments.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_INTERNAL_PROPERTY2(TestSuite, TestCase) ARENE_INTERNAL_PROPERTY3(TestSuite, TestCase, ())

/// @brief helper macro to dispatch to the correct internal property macro
/// @param _1   first positional slot (unused)
/// @param _2   second positional slot (unused)
/// @param _3   third positional slot (unused)
/// @param NAME the selected macro name to return
/// @param ...  any trailing tokens (ignored)
///
/// Used as an argument-count dispatcher for @c ARENE_PROPERTY or
/// @c ARENE_TYPED_PROPERTY. The positional arguments @c _1, @c _2, @c _3, @c NAME
/// capture tokens from the caller, and the selected @c NAME is returned. The
/// variadic tail (@c ...) absorbs any additional tokens so that the dispatch works
/// regardless of overall argument count.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_INTERNAL_PROPERTY_PICK(_1, _2, _3, NAME, ...) NAME

/// @brief define a property-based test
/// Arguments: test suite name, test case name, and an optional parenthesized parameter list for the property body,
///   e.g. @c (int a, int b). When omitted, the body takes no generated arguments.
///
/// Expands to a @c TEST case whose body is executed many times by @c
/// ::arene::base::testing::run_property, with pseudorandom arguments drawn
/// according to @c ::arene::base::testing::read_property_config.
///
/// The macro has two forms:
///
/// @code{.cpp}
/// ARENE_PROPERTY(TestSuite, TestCase) { /* body */ }
/// ARENE_PROPERTY(TestSuite, TestCase, Params) { /* body */ }
/// @endcode
///
/// The first form declares a property with no generated arguments. The
/// second form accepts a parenthesized parameter list @c Params that
/// specifies the types (and optional names) of the values the framework
/// should generate and pass into the body each iteration.
///
/// Inside the body, the identifier @c prng refers to a type-erased PRNG, which
/// may be used to manually produce additional random values if needed.
///
/// Example:
/// @snippet docs/examples/property_examples.cpp arene_property
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_PROPERTY(...)          \
  ARENE_INTERNAL_PROPERTY_PICK(      \
      __VA_ARGS__,                   \
      ARENE_INTERNAL_PROPERTY3,      \
      ARENE_INTERNAL_PROPERTY2,      \
      unused_token_to_match_ellipses \
  )                                  \
  (__VA_ARGS__)

/// @brief three-argument form of @c ARENE_TYPED_PROPERTY
/// @param TestSuite typed test suite name
/// @param TestCase test case name
/// @param Params parenthesized parameter list for the property body
///
/// Generates the closure type template (parameterized by the GoogleTest
/// @c TypeParam), the @c TYPED_TEST registration, and the out-of-class
/// definition stub of the property's @c operator(). The user's braced
/// block that follows the macro invocation becomes the body of that
/// @c operator(). Inside the body, @c TypeParam refers to the current
/// type from the type list bound to the typed test suite.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_INTERNAL_TYPED_PROPERTY3(TestSuite, TestCase, Params)                      \
  template <class TypeParam>                                                             \
  struct ARENE_INTERNAL_PROPERTY_CLOSURE_NAME(TestSuite, TestCase) {                     \
    ::arene::base::testing::detail::any_prng_ref prng;                                   \
    auto operator() Params const -> void;                                                \
  };                                                                                     \
  TYPED_TEST(TestSuite, TestCase) {                                                      \
    auto const summary = ::arene::base::testing::run_property(                           \
        ::arene::base::testing::detail::property_trampoline <                            \
            ARENE_INTERNAL_PROPERTY_CLOSURE_NAME(TestSuite, TestCase) < TypeParam >> {}, \
        ::arene::base::testing::read_property_config()                                   \
    );                                                                                   \
    if (summary.failed) {                                                                \
      FAIL() << summary;                                                                 \
    }                                                                                    \
  }                                                                                      \
  template <class TypeParam>                                                             \
  auto ARENE_INTERNAL_PROPERTY_CLOSURE_NAME(TestSuite, TestCase)<TypeParam>::operator() Params const -> void

/// @brief two-argument form of @c ARENE_TYPED_PROPERTY
/// @param TestSuite typed test suite name
/// @param TestCase test case name
///
/// Forwards to @c ARENE_INTERNAL_TYPED_PROPERTY3 with an empty parameter list,
/// producing a typed property whose body takes no generated arguments.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_INTERNAL_TYPED_PROPERTY2(TestSuite, TestCase) ARENE_INTERNAL_TYPED_PROPERTY3(TestSuite, TestCase, ())

/// @brief define a typed property-based test
/// Arguments: typed test suite name, test case name, and an optional parenthesized parameter list for the property
/// body,
///   e.g. @c (TypeParam a, TypeParam b). When omitted, the body takes no
///   generated arguments.
///
/// Expands to a @c TYPED_TEST case whose body is executed many times by
/// @c ::arene::base::testing::run_property, with pseudorandom arguments drawn
/// according to @c ::arene::base::testing::read_property_config. The body is
/// instantiated for each type in the type list bound to @c TestSuite.
///
/// The macro has two forms:
///
/// @code{.cpp}
/// ARENE_TYPED_PROPERTY(TestSuite, TestCase) { /* body */ }
/// ARENE_TYPED_PROPERTY(TestSuite, TestCase, Params) { /* body */ }
/// @endcode
///
/// The first form declares a typed property with no generated arguments.
/// The second form accepts a parenthesized parameter list @c Params that
/// specifies the types (and optional names) of the values the framework
/// should generate and pass into the body each iteration. The identifier
/// @c TypeParam is available inside @c Params and the body, referring to
/// the current type under test.
///
/// Inside the body, the identifier @c prng refers to a type-erased PRNG, which
/// may be used to manually produce additional random values if needed.
///
/// Example:
/// @snippet docs/examples/property_examples.cpp arene_typed_property
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_TYPED_PROPERTY(...)     \
  ARENE_INTERNAL_PROPERTY_PICK(       \
      __VA_ARGS__,                    \
      ARENE_INTERNAL_TYPED_PROPERTY3, \
      ARENE_INTERNAL_TYPED_PROPERTY2, \
      unused_token_to_match_ellipses  \
  )                                   \
  (__VA_ARGS__)

/// @brief run an inline property check
/// @param Description string literal describing the property. Currently
///   unused by the expansion; reserved for future labeling and reporting.
/// @param Params parenthesized parameter list for the property body,
///   e.g. @c (int a, int b). Use @c () for a property that takes no
///   generated arguments.
/// @param ... braced block forming the body of the property's @c operator().
///   The block is passed through as variadic tokens so that it may contain
///   commas (e.g. inside initializer lists or template arguments).
/// @return @c proprety_summary of the run
///
/// Runs the property body repeatedly with pseudorandom arguments drawn
/// according to @c ::arene::base::testing::read_property_config, and
/// yields the resulting @c property_summary.
///
/// Unlike @c ARENE_PROPERTY and @c ARENE_TYPED_PROPERTY, this macro does
/// not register a test case. It is intended for use inside an existing
/// test body or other context where the caller wants direct access to
/// the summary, for example to inspect statistics or to assert custom
/// conditions on the outcome.
///
/// Inside the body, the identifier @c prng refers to a type-erased PRNG,
/// which may be used to manually produce additional random values if
/// needed.
///
/// Example:
/// @snippet docs/examples/property_examples.cpp arene_prop_check
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_PROP_CHECK(Description, Params, ...)                                     \
  [] {                                                                                 \
    struct local_proprety_closure {                                                    \
      ::arene::base::testing::detail::any_prng_ref prng;                               \
      auto operator() Params const -> void __VA_ARGS__                                 \
    };                                                                                 \
    return ::arene::base::testing::run_property(                                       \
        ::arene::base::testing::detail::property_trampoline<local_proprety_closure>{}, \
        ::arene::base::testing::read_property_config()                                 \
    );                                                                                 \
  }()

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_MACROS_HPP_
