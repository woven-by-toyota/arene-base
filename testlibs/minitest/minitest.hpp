// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_MINITEST_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_MINITEST_HPP_

#include "testlibs/minitest/cstring.hpp"
#include "testlibs/minitest/death_tests.hpp"      // IWYU pragma: export
#include "testlibs/minitest/functional.hpp"       // IWYU pragma: export
#include "testlibs/minitest/matcher.hpp"          // IWYU pragma: export
#include "testlibs/minitest/minitest_output.hpp"  // IWYU pragma: export
#include "testlibs/minitest/type_name.hpp"
#include "testlibs/minitest/utility.hpp"        // IWYU pragma: export
#include "testlibs/minitest/utility_types.hpp"  // IWYU pragma: export

/// @file minitest.hpp
///
/// "Mini" test framework intended to be API-compatible with googletest, but without requiring an existing C++ Standard
/// Library.

namespace testing {

template <typename T1, typename T2>
extern constexpr bool same_type_v = false;

template <typename T>
extern constexpr bool same_type_v<T, T> = true;

/// @brief Assert that the two type arguments are the same type
template <typename T1, typename T2>
// NOLINTNEXTLINE(readability-identifier-naming)
constexpr auto StaticAssertTypeEq() noexcept -> bool {
  static_assert(same_type_v<T1, T2>, "T1 and T2 are the same type");
  return true;
}

template <bool Value>
struct constant_flag {
  static constexpr bool value = Value;
};

class is_unambiguously_publicly_derived_from_impl {
 public:
  struct not_derived_result {};
  struct is_derived_result {
    not_derived_result dummy1;
    not_derived_result dummy2;
  };

  template <typename Base>
  static auto check(Base*) -> is_derived_result;
  template <typename Base>
  static auto check(void*) -> not_derived_result;
};

/// @brief Check if the @c Derived type is either the same as or unambiguously publicly derived from the @c Base type.
/// The value is @c true if this is the case, @c false otherwise.
template <typename Derived, typename Base>
extern constexpr bool is_unambiguously_publicly_derived_from_v =
    sizeof(is_unambiguously_publicly_derived_from_impl::check<Base>(static_cast<Derived*>(nullptr))) ==
    sizeof(is_unambiguously_publicly_derived_from_impl::is_derived_result);

namespace has_member_type_impl {
/// @brief This overload is ignored by SFINAE if @c U doesn't have a type member named @c type If it is called that
/// means there is one
/// @tparam U The type to check
/// @return true
template <typename U, typename = typename U::type>
static constexpr auto test(int) -> bool {
  return true;
}

/// @brief This overload is a fallback that is always valid, but @c ... has the lowest conversion rank, so this is
/// only called if there is no other valid overload, which means @c U does not have a type member named @c type
/// @tparam U The type to check
/// @return false
template <typename U>
static constexpr auto test(...) -> bool {
  return false;
}

}  // namespace has_member_type_impl

template <typename T>
extern constexpr bool has_member_type_v = has_member_type_impl::test<T>(0);

namespace simplified_is_invocable {

template <class...>
using void_t = void;

template <class...>
struct list {};

template <class F, class Args, class = void>
struct invoke_result_impl {};

template <class F, class... Args>
struct invoke_result_impl<F, list<Args...>, void_t<decltype(declval<F>()(declval<Args>()...))>> {
  using type = decltype(declval<F>()(declval<Args>()...));
};

template <class F, class... Args>
struct invoke_result : invoke_result_impl<F, list<Args...>> {};

}  // namespace simplified_is_invocable

// A simplified version of `std::is_invocable` which tests if `F(Args...)` is a valid expression.
// This only handles the base case for `INVOKE`: https://eel.is/c++draft/func.require#1.7
template <class F, class... Args>
constexpr bool simple_is_invocable_v = testing::has_member_type_v<simplified_is_invocable::invoke_result<F, Args...>>;

// Always returns true so that it can be used in a fake condition which obscures that the code after it is unreachable.
constexpr auto return_true() noexcept -> bool { return true; }

// Mark the current test as failed and log the assertion that caused it to fail
// Always returns true so that it can be used in a fake condition which obscures that the code after it is unreachable.
auto assertion_failure(char const* file, line_number line, assertion_strings const& assertion) noexcept -> bool;

// Mark the current test as skipped
// Always returns true so that it can be used in a fake condition which obscures that the code after it is unreachable.
auto skip_test(char const* file, line_number line) noexcept -> bool;

constexpr auto do_assert_true(bool check, char const* arg, char const* file, line_number line) noexcept -> bool {
  if (!check) {
    assertion_failure(file, line, {arg, " was false, expected to be true"});
  }
  return check;
}
constexpr auto do_assert_false(bool check, char const* arg, char const* file, line_number line) noexcept -> bool {
  if (check) {
    assertion_failure(file, line, {arg, " was true, expected to be false"});
  }
  return !check;
}
constexpr auto do_assert_comparison(
    bool check,
    char const* lhs,
    char const* rhs,
    char const* file,
    line_number line,
    char const* comparison_message
) noexcept -> bool {
  if (!check) {
    assertion_failure(file, line, {lhs, comparison_message, rhs});
  }
  return check;
}

/// @brief Check if two floating point arguments are not NaN and are sufficiently near each other
/// @tparam FloatingPoint Some floating point type
/// @param lhs The left-hand side of the comparison
/// @param rhs The right-hand side of the comparison
/// @param max_difference The largest permissible difference between @c lhs and @c rhs
/// @param lhs_text The text of @c lhs as entered into the macro
/// @param rhs_text The text of @c rhs as entered into the macro
/// @param file The file in which the assertion is taking place
/// @param line The line on which the assertion is taking place
/// @return @c true if the assertion passed, @c false if it didn't and a failure was logged
/// @pre @c max_difference must not be negative or NaN, otherwise this assertion always fails
template <typename FloatingPoint>
constexpr auto do_assert_near(
    FloatingPoint lhs,
    FloatingPoint rhs,
    FloatingPoint max_difference,
    char const* lhs_text,
    char const* rhs_text,
    char const* file,
    line_number line
) noexcept -> bool {
  if (is_nan(lhs) || is_nan(rhs)) {
    assertion_failure(file, line, {lhs_text, " expected to be near (but one was NaN) ", rhs_text});
    return false;
  }

  FloatingPoint const& lower = (lhs < rhs) ? lhs : rhs;
  FloatingPoint const& higher = (lhs < rhs) ? rhs : lhs;

  if ((higher - lower) <= max_difference) {
    return true;
  }

  // Do assertion failure in the default branch so that this catches NaNs generated by subtracting
  assertion_failure(file, line, {lhs_text, " expected to be near ", rhs_text});
  return false;
}

inline void do_assert_no_throw(char const* arg, char const* file, line_number line) noexcept {
  assertion_failure(file, line, {arg, " threw an exception when none was expected"});
}

inline void
do_assert_throw(bool thrown, char const* arg, char const* exception, char const* file, line_number line) noexcept {
  assertion_failure(
      file,
      line,
      {arg,
       thrown ? " threw a different type of exception, when it was expected to throw "
              : " did not throw an exception when it was expected to throw ",
       exception}
  );
}

/// @brief Base class for Test Cases
class test_base {
  test_base* next_ = nullptr;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
  static test_base* first_test;

 public:
  test_base() noexcept
      : next_(first_test) {
    first_test = this;
  }
  virtual ~test_base() = default;

  test_base(test_base const&) = delete;
  test_base(test_base&&) = delete;
  auto operator=(test_base const&) -> test_base& = delete;
  auto operator=(test_base&&) -> test_base& = delete;

  static auto run_tests() -> bool;

  virtual auto get_context() const noexcept -> test_context = 0;
  virtual auto do_test() -> void = 0;

  // These two functions are GoogleTest's test start/end customization points, so they follow Google's style.
  // These can *not* be run before/after CONSTEXPR_ tests, so you probably should not customize them when defining a
  // fixture for CONSTEXPR_ tests, though it's technically possible. In that case they will only be run at runtime.
  virtual auto SetUp() -> void {}     // NOLINT(readability-identifier-naming)
  virtual auto TearDown() -> void {}  // NOLINT(readability-identifier-naming)

 protected:
  /// @brief Manually flush the output buffer; useful when an assertion may cause the process to be duplicated or abort.
  static auto flush_output() noexcept -> void;
};

/// @brief Alias for @c test_base, for compatibility with GoogleTest
// NOLINTNEXTLINE(readability-identifier-naming)
using Test = test_base;

struct dummy_result {};

/// @brief A list of types for a @c TYPED_TEST_SUITE
template <typename... TypeParams>
// NOLINTNEXTLINE(readability-identifier-naming)
class Types {};

/// @brief no-op function used to require a semicolon at the end of an ASSERT macro
constexpr auto require_semicolon() noexcept -> void {}

/// @brief Dummy type that supports stream insertion and discards the values
class stream_insertion_sink {
 public:
  template <typename T>
  constexpr auto operator<<(T&&) const noexcept -> stream_insertion_sink const& {
    return *this;
  }
  constexpr auto operator+() const noexcept -> stream_insertion_sink const& { return *this; }
};

template <template <class> class Case, class Params>
struct typed_test_registration;
template <template <class> class Case, template <class...> class List, class... Param>
struct typed_test_registration<Case, List<Param...>> : Case<Param>... {};

template <typename SuiteNameHolder, template <typename> class BaseTestCase>
class derived_test_case_wrapper {
 public:
  template <typename TypeParam>
  class derived_test_case : public BaseTestCase<TypeParam> {
    auto get_suite_name_string() const noexcept -> char const* override { return SuiteNameHolder::get_suite_name(); }
  };
};

template <class... Ts>
struct inherits_from_all : Ts... {};

template <typename TypeParams, typename SuiteNameHolder, template <typename> class TestCase>
auto register_parameterized_test_with_params() -> typed_test_registration<
    derived_test_case_wrapper<SuiteNameHolder, TestCase>::template derived_test_case,
    TypeParams>;

template <typename TypeParams, typename SuiteNameHolder, template <typename> class... TestCases>
auto register_parameterized_tests_with_params(
) -> inherits_from_all<decltype(register_parameterized_test_with_params<TypeParams, SuiteNameHolder, TestCases>())...>;

}  // namespace testing

/// @brief macro to concat string
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MINITEST_INTERNAL_CONCAT_IMPL(X, Y) X##Y
/// @brief macro to concat string
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MINITEST_INTERNAL_CONCAT(X, Y) MINITEST_INTERNAL_CONCAT_IMPL(X, Y)

/// @brief macro to obtain the name of a test case
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, Case) Suite##_##Case

/// @brief macro to obtain the name of the test suite type parameters
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MINITEST_INTERNAL_TESTSUITE_TYPE_PARAMS(Suite) MINITEST_INTERNAL_CONCAT(Suite, _test_params)

/// @brief macro to define the test context
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MINITEST_INTERNAL_TESTCASE_CONTEXT(Suite, Case, Type) \
  ::testing::test_context { #Suite, #Case, __FILE__, __LINE__, Type }

/// @brief underlying macro for test registration
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MINITEST_INTERNAL_REGISTER_IMPL(...) \
  __VA_ARGS__ MINITEST_INTERNAL_CONCAT(test_instance_, __LINE__) {}

/// @brief macro to register a test case
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MINTEST_REGISTER_TEST_CASE(CaseType) MINITEST_INTERNAL_REGISTER_IMPL(CaseType)

/// @brief macro to register a suite of typed tests
// clang-format off
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MINITEST_INTERNAL_REGISTER_TYPED_TESTSUITE_TESTS(TypedCase, StoredParams) \
  MINITEST_INTERNAL_REGISTER_IMPL(::testing::typed_test_registration<TypedCase, StoredParams>) /* NOLINT(fuchsia-statically-constructed-objects,cppcoreguidelines-avoid-non-const-global-variables) */
// clang-format on

/// @brief Macro to introduce a test case
// clang-format off
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TEST(Suite, TestCase)                                                                     \
  class MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)                                     \
      : public ::testing::test_base {                                                             \
   public:                                                                                        \
    auto get_context() const noexcept -> ::testing::test_context override {                       \
      return MINITEST_INTERNAL_TESTCASE_CONTEXT(Suite, TestCase, "");                             \
    }                                                                                             \
    auto do_test() -> void override;                                                              \
  };                                                                                              \
  MINTEST_REGISTER_TEST_CASE(MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)); /* NOLINT(fuchsia-statically-constructed-objects,cppcoreguidelines-avoid-non-const-global-variables) */ \
  void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)::do_test()
// clang-format on

/// @brief Macro to introduce a test case using a user-provided test fixture
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TEST_F(Suite, TestCase)                                                     \
  class Suite##_##TestCase : public Suite { /* NOLINT(bugprone-macro-parentheses)*/ \
   public:                                                                          \
    auto get_context() const noexcept -> ::testing::test_context override {         \
      return {#Suite, #TestCase, __FILE__, __LINE__, ""};                           \
    }                                                                               \
    auto do_test() -> void override;                                                \
  };                                                                                \
  Suite##_##TestCase Suite##_##TestCase##_instance{}; /*NOLINT*/                    \
  void Suite##_##TestCase::do_test()

/// @brief Macro to introduce a test suite parameterized with a list of types
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TYPED_TEST_SUITE(Suite, TypeParams, ignored) using MINITEST_INTERNAL_TESTSUITE_TYPE_PARAMS(Suite) = TypeParams

// NOLINTNEXTLINE(readability-identifier-naming)
using MINITEST_INTERNAL_DUMMY_TESTSUITE_TYPE_PARAMS = ::testing::Types<void>;

/// @brief Macro to introduce a test case from a typed test suite
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TYPED_TEST(Suite, TestCase)                                                                           \
  template <typename TypeParam>                                                                               \
  class MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)                                                 \
      : public Suite<TypeParam> { /* NOLINT(bugprone-macro-parentheses)*/                                     \
   public:                                                                                                    \
    using TestFixture = MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase);                                \
    auto get_context() const noexcept -> ::testing::test_context override {                                   \
      return MINITEST_INTERNAL_TESTCASE_CONTEXT(Suite, TestCase, ::testing::type_name::type_name<TypeParam>); \
    }                                                                                                         \
    auto do_test() -> void override;                                                                          \
  };                                                                                                          \
  MINITEST_INTERNAL_REGISTER_TYPED_TESTSUITE_TESTS(                                                           \
      MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase),                                                  \
      MINITEST_INTERNAL_TESTSUITE_TYPE_PARAMS(Suite)                                                          \
  );                                                                                                          \
  template <typename TypeParam>                                                                               \
  void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)<TypeParam>::do_test()

/// @brief Macro to introduce a test case from a typed test suite that is only run if a condition is true
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CONDITIONAL_TYPED_TEST(Suite, TestCase, Condition)                                                    \
  template <typename TypeParam>                                                                               \
  class MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)                                                 \
      : public Suite<TypeParam> { /* NOLINT(bugprone-macro-parentheses)*/                                     \
   public:                                                                                                    \
    using TestFixture = MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase);                                \
    auto get_context() const noexcept -> ::testing::test_context override {                                   \
      return MINITEST_INTERNAL_TESTCASE_CONTEXT(Suite, TestCase, ::testing::type_name::type_name<TypeParam>); \
    }                                                                                                         \
    auto do_test() -> void override { this->do_conditional_test(::testing::constant_flag<(Condition)>{}); }   \
    auto do_conditional_test(::testing::constant_flag<false>) -> void {}                                      \
    auto do_conditional_test(::testing::constant_flag<true>) -> void;                                         \
  };                                                                                                          \
  MINITEST_INTERNAL_REGISTER_TYPED_TESTSUITE_TESTS(                                                           \
      MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase),                                                  \
      MINITEST_INTERNAL_TESTSUITE_TYPE_PARAMS(Suite)                                                          \
  );                                                                                                          \
  template <typename TypeParam>                                                                               \
  void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(                                                                  \
      Suite,                                                                                                  \
      TestCase                                                                                                \
  )<TypeParam>::do_conditional_test(::testing::constant_flag<true>)

/// @brief Macro to introduce a constexpr test case from a typed test suite
/// @note A constexpr test case may not reference @c this
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CONSTEXPR_TYPED_TEST(Suite, TestCase)                                                                 \
  template <typename TypeParam>                                                                               \
  class MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)                                                 \
      : public Suite<TypeParam> { /* NOLINT(bugprone-macro-parentheses)*/                                     \
    static constexpr void do_test_impl();                                                                     \
                                                                                                              \
   public:                                                                                                    \
    using TestFixture = MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase);                                \
    auto get_context() const noexcept -> ::testing::test_context override {                                   \
      return MINITEST_INTERNAL_TESTCASE_CONTEXT(Suite, TestCase, ::testing::type_name::type_name<TypeParam>); \
    }                                                                                                         \
    auto do_test() -> void override;                                                                          \
  };                                                                                                          \
  template <typename TypeParam>                                                                               \
  void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)<TypeParam>::do_test() {                          \
    static_assert((do_test_impl(), true), "");                                                                \
    do_test_impl();                                                                                           \
  }                                                                                                           \
  MINITEST_INTERNAL_REGISTER_TYPED_TESTSUITE_TESTS(                                                           \
      MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase),                                                  \
      MINITEST_INTERNAL_TESTSUITE_TYPE_PARAMS(Suite)                                                          \
  );                                                                                                          \
  template <typename TypeParam>                                                                               \
  constexpr void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)<TypeParam>::do_test_impl()

/// @brief Macro to introduce a conditional constexpr test case from a typed test suite
/// @note A constexpr test case may not reference @c this
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CONDITIONALLY_CONSTEXPR_TYPED_TEST(Suite, TestCase, CheckIfShouldRunInConstexpr)                      \
  template <typename TypeParam>                                                                               \
  class MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)                                                 \
      : public Suite<TypeParam> { /* NOLINT(bugprone-macro-parentheses)*/                                     \
    static constexpr void do_test_impl();                                                                     \
    void maybe_do_test_in_constexpr(::testing::constant_flag<true>);                                          \
    void maybe_do_test_in_constexpr(::testing::constant_flag<false>);                                         \
                                                                                                              \
   public:                                                                                                    \
    using TestFixture = MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase);                                \
    auto get_context() const noexcept -> ::testing::test_context override {                                   \
      return MINITEST_INTERNAL_TESTCASE_CONTEXT(Suite, TestCase, ::testing::type_name::type_name<TypeParam>); \
    }                                                                                                         \
    auto do_test() -> void override;                                                                          \
  };                                                                                                          \
  template <typename TypeParam>                                                                               \
  void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(                                                                  \
      Suite,                                                                                                  \
      TestCase                                                                                                \
  )<TypeParam>::maybe_do_test_in_constexpr(::testing::constant_flag<true>) {                                  \
    static_assert((do_test_impl(), true), "");                                                                \
    do_test_impl();                                                                                           \
  }                                                                                                           \
  template <typename TypeParam>                                                                               \
  void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(                                                                  \
      Suite,                                                                                                  \
      TestCase                                                                                                \
  )<TypeParam>::maybe_do_test_in_constexpr(::testing::constant_flag<false>) {                                 \
    do_test_impl();                                                                                           \
  }                                                                                                           \
  template <typename TypeParam>                                                                               \
  void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)<TypeParam>::do_test() {                          \
    maybe_do_test_in_constexpr(::testing::constant_flag<(CheckIfShouldRunInConstexpr)>{});                    \
  }                                                                                                           \
  MINITEST_INTERNAL_REGISTER_TYPED_TESTSUITE_TESTS(                                                           \
      MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase),                                                  \
      MINITEST_INTERNAL_TESTSUITE_TYPE_PARAMS(Suite)                                                          \
  );                                                                                                          \
  template <typename TypeParam>                                                                               \
  constexpr void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)<TypeParam>::do_test_impl()

/// @brief Macro to introduce a constexpr test case from a typed test suite that is only compiled and run if a condition
/// is true
/// @note A constexpr test case may not reference @c this
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CONDITIONAL_CONSTEXPR_TYPED_TEST(Suite, TestCase, CheckIfShouldRun)                                   \
  template <typename TypeParam>                                                                               \
  class MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)                                                 \
      : public Suite<TypeParam> { /* NOLINT(bugprone-macro-parentheses)*/                                     \
    static constexpr void do_test_impl();                                                                     \
    void maybe_do_test(::testing::constant_flag<true>);                                                       \
    void maybe_do_test(::testing::constant_flag<false>);                                                      \
                                                                                                              \
   public:                                                                                                    \
    using TestFixture = MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase);                                \
    auto get_context() const noexcept -> ::testing::test_context override {                                   \
      return MINITEST_INTERNAL_TESTCASE_CONTEXT(Suite, TestCase, ::testing::type_name::type_name<TypeParam>); \
    }                                                                                                         \
    auto do_test() -> void override;                                                                          \
  };                                                                                                          \
                                                                                                              \
  template <typename TypeParam>                                                                               \
  void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(                                                                  \
      Suite,                                                                                                  \
      TestCase                                                                                                \
  )<TypeParam>::maybe_do_test(::testing::constant_flag<true>) {                                               \
    static_assert((do_test_impl(), true), "");                                                                \
    do_test_impl();                                                                                           \
  }                                                                                                           \
                                                                                                              \
  template <typename TypeParam>                                                                               \
  void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(                                                                  \
      Suite,                                                                                                  \
      TestCase                                                                                                \
  )<TypeParam>::maybe_do_test(::testing::constant_flag<false>) {                                              \
    GTEST_SKIP() << "Disabled by conditional";                                                                \
  }                                                                                                           \
                                                                                                              \
  template <typename TypeParam>                                                                               \
  void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)<TypeParam>::do_test() {                          \
    maybe_do_test(::testing::constant_flag<(CheckIfShouldRun)>{});                                            \
  }                                                                                                           \
                                                                                                              \
  MINITEST_INTERNAL_REGISTER_TYPED_TESTSUITE_TESTS(                                                           \
      MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase),                                                  \
      MINITEST_INTERNAL_TESTSUITE_TYPE_PARAMS(Suite)                                                          \
  );                                                                                                          \
                                                                                                              \
  template <typename TypeParam>                                                                               \
  constexpr void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)<TypeParam>::do_test_impl()

/// @brief Macro to introduce a constexpr test case
/// @note A constexpr test case may not reference @c this
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CONSTEXPR_TEST(Suite, TestCase)                                              \
  template <typename>                                                                \
  class MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)                        \
      : public ::testing::test_base {                                                \
    static constexpr void do_test_impl();                                            \
                                                                                     \
   public:                                                                           \
    auto get_context() const noexcept -> ::testing::test_context override {          \
      return MINITEST_INTERNAL_TESTCASE_CONTEXT(Suite, TestCase, "");                \
    }                                                                                \
    auto do_test() -> void override;                                                 \
  };                                                                                 \
  template <typename TypeParam>                                                      \
  void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)<TypeParam>::do_test() { \
    static_assert((do_test_impl(), true), "");                                       \
    do_test_impl();                                                                  \
  }                                                                                  \
  MINITEST_INTERNAL_REGISTER_TYPED_TESTSUITE_TESTS(                                  \
      MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase),                         \
      MINITEST_INTERNAL_DUMMY_TESTSUITE_TYPE_PARAMS                                  \
  );                                                                                 \
  template <typename TypeParam>                                                      \
  constexpr void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)<TypeParam>::do_test_impl()

/// @brief Macro to introduce a conditional constexpr test case
/// @note A constexpr test case may not reference @c this
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CONDITIONALLY_CONSTEXPR_TEST(Suite, TestCase, CheckIfShouldRunInConstexpr)         \
  template <typename>                                                                      \
  class MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)                              \
      : public ::testing::test_base {                                                      \
    static constexpr void do_test_impl();                                                  \
    void maybe_do_test_in_constexpr(::testing::constant_flag<true>);                       \
    void maybe_do_test_in_constexpr(::testing::constant_flag<false>);                      \
                                                                                           \
   public:                                                                                 \
    auto get_context() const noexcept -> ::testing::test_context override {                \
      return MINITEST_INTERNAL_TESTCASE_CONTEXT(Suite, TestCase, "");                      \
    }                                                                                      \
    auto do_test() -> void override;                                                       \
  };                                                                                       \
  template <typename TypeParam>                                                            \
  void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(                                               \
      Suite,                                                                               \
      TestCase                                                                             \
  )<TypeParam>::maybe_do_test_in_constexpr(::testing::constant_flag<true>) {               \
    static_assert((do_test_impl(), true), "");                                             \
    do_test_impl();                                                                        \
  }                                                                                        \
  template <typename TypeParam>                                                            \
  void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(                                               \
      Suite,                                                                               \
      TestCase                                                                             \
  )<TypeParam>::maybe_do_test_in_constexpr(::testing::constant_flag<false>) {              \
    do_test_impl();                                                                        \
  }                                                                                        \
  template <typename TypeParam>                                                            \
  void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)<TypeParam>::do_test() {       \
    maybe_do_test_in_constexpr(::testing::constant_flag<(CheckIfShouldRunInConstexpr)>{}); \
  }                                                                                        \
  MINITEST_INTERNAL_REGISTER_TYPED_TESTSUITE_TESTS(                                        \
      MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase),                               \
      MINITEST_INTERNAL_DUMMY_TESTSUITE_TYPE_PARAMS                                        \
  );                                                                                       \
  template <typename TypeParam>                                                            \
  constexpr void MINITEST_INTERNAL_TESTCASE_TYPE_NAME(Suite, TestCase)<TypeParam>::do_test_impl()

/// @brief Macro to introduce a test suite that can be later parameterized with a list of types
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TYPED_TEST_SUITE_P(Suite) \
  class Suite##_TypedTestSuitePClass {}

/// @brief Macro to introduce a test case from a parameterized test suite
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TYPED_TEST_P(Suite, TestCase)                                                                                      \
  namespace Suite##_##TypedSuiteP {                                                                                        \
    template <typename TypeParam>                                                                                          \
    class TestCase : public Suite<TypeParam> { /* NOLINT(bugprone-macro-parentheses)*/                                     \
      virtual auto get_suite_name_string() const noexcept -> const char* = 0;                                              \
      using TestFixture = Suite<TypeParam>; /* NOLINT(bugprone-macro-parentheses)*/                                        \
     public:                                                                                                               \
      auto get_context() const noexcept -> ::testing::test_context override {                                              \
        return {this->get_suite_name_string(), #TestCase, __FILE__, __LINE__, ::testing::type_name::type_name<TypeParam>}; \
      }                                                                                                                    \
      auto do_test() -> void override;                                                                                     \
    };                                                                                                                     \
  }                                                                                                                        \
  template <typename TypeParam>                                                                                            \
  void Suite##_##TypedSuiteP::TestCase<TypeParam>::do_test()

/// @brief Macro to introduce a test case from a parameterized test suite that is only run if a condition is true
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CONDITIONAL_TYPED_TEST_P(Suite, TestCase, Condition)                                                               \
  namespace Suite##_##TypedSuiteP {                                                                                        \
    template <typename TypeParam>                                                                                          \
    class TestCase : public Suite<TypeParam> { /* NOLINT(bugprone-macro-parentheses)*/                                     \
      virtual auto get_suite_name_string() const noexcept -> const char* = 0;                                              \
                                                                                                                           \
     public:                                                                                                               \
      using TestFixture = Suite<TypeParam>; /* NOLINT(bugprone-macro-parentheses)*/                                        \
      auto get_context() const noexcept -> ::testing::test_context override {                                              \
        return {this->get_suite_name_string(), #TestCase, __FILE__, __LINE__, ::testing::type_name::type_name<TypeParam>}; \
      }                                                                                                                    \
      auto do_test() -> void override { this->do_conditional_test(::testing::constant_flag<(Condition)>{}); }              \
      auto do_conditional_test(::testing::constant_flag<false>) -> void {                                                  \
        static_cast<void>(::testing::skip_test(__FILE__, __LINE__));                                                       \
      }                                                                                                                    \
      auto do_conditional_test(::testing::constant_flag<true>) -> void;                                                    \
    };                                                                                                                     \
  }                                                                                                                        \
  template <typename TypeParam>                                                                                            \
  void Suite##_##TypedSuiteP::TestCase<TypeParam>::do_conditional_test(::testing::constant_flag<true>)

/// @brief Macro to register a test suite and the tests in it for later parameterization with a list of types
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define REGISTER_TYPED_TEST_SUITE_P(Suite, ...)                                                                      \
  namespace Suite##_##TypedSuiteP {                                                                                  \
    template <typename TypeParams, typename SuiteNameHolder>                                                         \
    class TestRunner {                                                                                               \
     public:                                                                                                         \
      using test_registration_type =                                                                                 \
          decltype(::testing::register_parameterized_tests_with_params<TypeParams, SuiteNameHolder, __VA_ARGS__>()); \
    };                                                                                                               \
  }                                                                                                                  \
  class Suite##_TypedTestSuitePRegistration {}

/// @brief Macro to instantiate a test suite with a list of types
// clang-format off
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define INSTANTIATE_TYPED_TEST_SUITE_P(InstantiationName, Suite, TypeParams, ignored)                         \
  static_assert(sizeof(#ignored) == 1, "Must only provide 3 parameters and a trailing comma");                \
  class InstantiationName##_##Suite##_TestRunner_suite_name_holder {                                          \
   public:                                                                                                    \
    static auto get_suite_name() noexcept -> const char* { return #InstantiationName "/" #Suite; }            \
  };                                                                                                          \
  Suite##_##TypedSuiteP::TestRunner<TypeParams, InstantiationName##_##Suite##_TestRunner_suite_name_holder>::test_registration_type InstantiationName##_##Suite##_TestRunner_instance{}; /* NOLINT(fuchsia-statically-constructed-objects,cppcoreguidelines-avoid-non-const-global-variables) */ \
  class InstantiationName##_##Suite##_TypedTestSuitePInstantiation {}
// clang-format on

/// @brief Macro to cause a test to immediately succeed
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SUCCEED()                 \
  if (::testing::return_true()) { \
    return;                       \
  }                               \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to skip the current test (the name with GTEST_ is the only one provided by GoogleTest, which we match)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define GTEST_SKIP()                              \
  if (::testing::skip_test(__FILE__, __LINE__)) { \
    return;                                       \
  }                                               \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to cause a test to immediately fail
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FAIL()                                                                                           \
  if (::testing::assertion_failure(__FILE__, __LINE__, {"Execution reached an invocation of FAIL()"})) { \
    return;                                                                                              \
  }                                                                                                      \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to add a failure to a test without immediately causing the test to end
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ADD_FAILURE()                                                                                          \
  static_cast<void>(::testing::assertion_failure(__FILE__, __LINE__, {"Execution reached an ADD_FAILURE()"})); \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that an expression is @c true
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_TRUE(...)                                                                              \
  if (!::testing::do_assert_true(static_cast<bool>(__VA_ARGS__), #__VA_ARGS__, __FILE__, __LINE__)) { \
    return;                                                                                           \
  }                                                                                                   \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that an expression is @c true
/// NOTE: Usable within a constant expression
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CONSTEXPR_ASSERT(...) ASSERT_TRUE(__VA_ARGS__)

/// @brief Macro to assert that an expression is @c false
/// NOTE: Usable within a constant expression
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CONSTEXPR_ASSERT_FALSE(...) ASSERT_FALSE(__VA_ARGS__)

/// @brief Macro to assert that two expressions are equal
/// NOTE: Usable within a constant expression
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CONSTEXPR_ASSERT_EQ(lhs, rhs) CONSTEXPR_ASSERT(((lhs) == (rhs)))

/// @brief Macro to assert that an expression is @c false
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_FALSE(...)                                                                              \
  if (!::testing::do_assert_false(static_cast<bool>(__VA_ARGS__), #__VA_ARGS__, __FILE__, __LINE__)) { \
    return;                                                                                            \
  }                                                                                                    \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that two expressions compare equal
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_EQ(lhs, rhs)                                                                                     \
  if (!::testing::do_assert_comparison(((lhs) == (rhs)), #lhs, #rhs, __FILE__, __LINE__, " expected to be ")) { \
    return;                                                                                                     \
  }                                                                                                             \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that two string expressions compare equal
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_STREQ(lhs, rhs)                     \
  if (!::testing::do_assert_comparison(            \
          ::testing::cstring::equal((lhs), (rhs)), \
          #lhs,                                    \
          #rhs,                                    \
          __FILE__,                                \
          __LINE__,                                \
          " expected to be "                       \
      )) {                                         \
    return;                                        \
  }                                                \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that two string expressions compare unequal
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_STRNE(lhs, rhs)                      \
  if (!::testing::do_assert_comparison(             \
          !::testing::cstring::equal((lhs), (rhs)), \
          #lhs,                                     \
          #rhs,                                     \
          __FILE__,                                 \
          __LINE__,                                 \
          " expected to be different from "         \
      )) {                                          \
    return;                                         \
  }                                                 \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that two expressions do not compare equal
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_NE(lhs, rhs)                                                                                         \
  if (!::testing::do_assert_comparison(((lhs) != (rhs)), #lhs, #rhs, __FILE__, __LINE__, " expected to not be ")) { \
    return;                                                                                                         \
  }                                                                                                                 \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that two expressions compare less than
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_LT(lhs, rhs)             \
  if (!::testing::do_assert_comparison( \
          ((lhs) < (rhs)),              \
          #lhs,                         \
          #rhs,                         \
          __FILE__,                     \
          __LINE__,                     \
          " expected to be less than "  \
      )) {                              \
    return;                             \
  }                                     \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that two expressions compare less than or equal
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_LE(lhs, rhs)                        \
  if (!::testing::do_assert_comparison(            \
          ((lhs) <= (rhs)),                        \
          #lhs,                                    \
          #rhs,                                    \
          __FILE__,                                \
          __LINE__,                                \
          " expected to be less than or equal to " \
      )) {                                         \
    return;                                        \
  }                                                \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that two expressions compare greater than
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_GT(lhs, rhs)               \
  if (!::testing::do_assert_comparison(   \
          ((lhs) > (rhs)),                \
          #lhs,                           \
          #rhs,                           \
          __FILE__,                       \
          __LINE__,                       \
          " expected to be greater than " \
      )) {                                \
    return;                               \
  }                                       \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that two expressions compare greater than or equal
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_GE(lhs, rhs)                           \
  if (!::testing::do_assert_comparison(               \
          ((lhs) >= (rhs)),                           \
          #lhs,                                       \
          #rhs,                                       \
          __FILE__,                                   \
          __LINE__,                                   \
          " expected to be greater than or equal to " \
      )) {                                            \
    return;                                           \
  }                                                   \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that two expressions compare greater than or equal
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_NEAR(lhs, rhs, max_difference)                                                       \
  static_assert((max_difference) >= 0, "max_difference must not be a negative value or NaN");       \
  if (!::testing::do_assert_near((lhs), (rhs), (max_difference), #lhs, #rhs, __FILE__, __LINE__)) { \
    return;                                                                                         \
  }                                                                                                 \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that applying a matcher to a value returns @c true
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_THAT(value_expression, matcher) ASSERT_TRUE((matcher)((value_expression)));

#ifdef __EXCEPTIONS
/// @brief Macro to assert that an expression does not throw an exception
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_NO_THROW(...)                                         \
  try {                                                              \
    static_cast<void>(__VA_ARGS__);                                  \
  } catch (...) {                                                    \
    ::testing::do_assert_no_throw(#__VA_ARGS__, __FILE__, __LINE__); \
    return;                                                          \
  }                                                                  \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that an expression throws an exception with a specific type. The assertion fails if the
/// expression does not throw any exceptions, or throws an exception of a different type.
/// @param expression The expression to check
/// @param exception The type of exception it is expected to throw
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_THROW(expression, exception)                                         \
  try {                                                                             \
    static_cast<void>(expression);                                                  \
    ::testing::do_assert_throw(false, #expression, #exception, __FILE__, __LINE__); \
    return;                                                                         \
  } catch (exception const&) {                                                      \
  } catch (...) {                                                                   \
    ::testing::do_assert_throw(true, #expression, #exception, __FILE__, __LINE__);  \
    return;                                                                         \
  }                                                                                 \
  (+::testing::stream_insertion_sink{})
#else  // __EXCEPTIONS
/// @brief Macro to assert that an expression does not throw an exception
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_NO_THROW(...)          \
  { static_cast<void>(__VA_ARGS__); } \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that an expression throws an exception with a specific type. The assertion fails if the
/// expression does not throw any exceptions, or throws an exception of a different type.
/// @param expression The expression to check
/// @param exception The type of exception it is expected to throw
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_THROW(expression, exception)                                          \
  static_assert(false, "ASSERT_THROW is only meaningful if exceptions are enabled"); \
  (+::testing::stream_insertion_sink{})
#endif  // __EXCEPTIONS

/// @brief Macro to assert that an expression is @c true
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EXPECT_TRUE(...)                                                                                           \
  static_cast<void>(!::testing::do_assert_true(static_cast<bool>(__VA_ARGS__), #__VA_ARGS__, __FILE__, __LINE__)); \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that an expression is @c false
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EXPECT_FALSE(...)                                                                                           \
  static_cast<void>(!::testing::do_assert_false(static_cast<bool>(__VA_ARGS__), #__VA_ARGS__, __FILE__, __LINE__)); \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that two expressions compare equal
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EXPECT_EQ(lhs, rhs)                                                                                 \
  static_cast<void>(                                                                                        \
      ::testing::do_assert_comparison(((lhs) == (rhs)), #lhs, #rhs, __FILE__, __LINE__, " expected to be ") \
  );                                                                                                        \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that two expressions do not compare equal
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EXPECT_NE(lhs, rhs)                                                                                 \
  static_cast<void>(                                                                                        \
      ::testing::do_assert_comparison(((lhs) != (rhs)), #lhs, #rhs, __FILE__, __LINE__, " expected to be ") \
  );                                                                                                        \
  (+::testing::stream_insertion_sink{})

/// @brief Macro to assert that an expression does not cause a fatal failure
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EXPECT_NO_FATAL_FAILURE(statement) {statement}(+::testing::stream_insertion_sink{})

/// @brief Macro to check that applying a matcher to a value returns @c true
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EXPECT_THAT(value_expression, matcher) EXPECT_TRUE((matcher)((value_expression)));

/// @brief Internal macro to simplify forwarding a set of arguments
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define INTERNAL_FWD(...) static_cast<decltype(__VA_ARGS__)>(__VA_ARGS__)

/// @brief Macro to lift a function into a function object using a generic lambda.
///
/// @note This will not preserve constexpr
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FUNCTION_LIFT(X)                                                                                  \
  [](auto&&... args) noexcept(noexcept(X(INTERNAL_FWD(args)...))) -> decltype(X(INTERNAL_FWD(args)...)) { \
    return X(INTERNAL_FWD(args)...);                                                                      \
  }

/// @brief Macro to cause the test to panic with a message and abort
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MINITEST_PANIC(msg) ::testing::panic(__FILE__, __LINE__, (msg))

/// @brief Macro to add the current file name, line number, and the given message message failure message for each
/// assertion failure that occurs in the scope.
///
/// Provided for compatibility with GoogleTest, currently unimplemented.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SCOPED_TRACE(...)

#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_MINITEST_HPP_
