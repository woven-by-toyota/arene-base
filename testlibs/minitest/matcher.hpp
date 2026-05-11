// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_MATCHER_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_MATCHER_HPP_

// Float comparisons are all from user-specified types
#if defined(__clang__)
_Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wfloat-equal\"") static_assert(true, "");
#elif defined(__GNUC__)
_Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wfloat-equal\"") static_assert(true, "");
#endif

namespace testing {

/// @brief declaration of the Eq matcher
template <class T>
// NOLINTNEXTLINE(readability-identifier-naming)
constexpr auto Eq(T&&);

namespace matcher_detail {

/// @brief declaration use to obtain a type without qualifiers
template <class T>
constexpr auto decay_copy(T const&) -> T;

/// @brief declaration use to obtain a type without qualifiers
template <class Ret, typename... Args>
constexpr auto decay_copy(Ret (*)(Args...)) -> Ret (*)(Args...);

/// @brief tag used to classify arguments as matchers
struct matcher_tag {};

/// @brief matcher or promote a value to an Eq matcher
///
/// @{
template <template <class, class> class Matcher, class... Values>
constexpr auto matcher_else_eq(Matcher<matcher_tag, Values...> const& arg) -> auto& {
  return arg;
}
template <class T>
constexpr auto matcher_else_eq(T const& arg) {
  return ::testing::Eq(arg);
}
/// @}

}  // namespace matcher_detail
}  // namespace testing

/// @brief macro to define a unary matcher
/// @param matcher_name name of the resulting matcher
/// @param arg_name name of the matcher's single argument
/// @param description description of the matcher
///
/// Returns a matcher, a function object which is a unary predicate, which is
/// closure that decay copies the arguments.
///
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MATCHER_P(matcher_name, arg_name, description)                                                    \
  template <class Tag, class Value>                                                                       \
  struct matcher_name##_testing_matcher1 {                                                                \
    Value arg_name;                                                                                       \
                                                                                                          \
    template <class Arg>                                                                                  \
    constexpr auto operator()(Arg const& arg) const -> bool;                                              \
  };                                                                                                      \
  template <class T>                                                                                      \
  constexpr auto matcher_name(T&& arg_name) /* NOLINT(bugprone-macro-parentheses)*/ {                     \
    using value_type = decltype(::testing::matcher_detail::decay_copy(arg_name));                         \
    return matcher_name##_testing_matcher1<::testing::matcher_detail::matcher_tag, value_type>{arg_name}; \
  }                                                                                                       \
  template <class Tag, class Value>                                                                       \
  template <class Arg>                                                                                    \
  constexpr auto matcher_name##_testing_matcher1<Tag, Value>::operator()(Arg const& arg) const -> bool

/// @brief macro to define a binary matcher
/// @param matcher_name name of the resulting matcher
/// @param arg1 name of the matcher's first argument
/// @param arg2 name of the matcher's second argument
/// @param description description of the matcher
///
/// Returns a matcher, a function object which is a unary predicate, which is
/// closure that decay copies the arguments.
///
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MATCHER_P2(matcher_name, arg1, arg2, description)                                                     \
  template <class Tag, class Value1, class Value2>                                                            \
  struct matcher_name##_testing_matcher2 {                                                                    \
    Value1 arg1;                                                                                              \
    Value2 arg2;                                                                                              \
                                                                                                              \
    template <class Arg>                                                                                      \
    constexpr auto operator()(Arg const& arg) const -> bool;                                                  \
  };                                                                                                          \
  template <class T1, class T2>                                                                               \
  constexpr auto matcher_name(T1&& arg1, T2&& arg2) /* NOLINT(bugprone-macro-parentheses)*/ {                 \
    using value_type1 = decltype(::testing::matcher_detail::decay_copy(arg1));                                \
    using value_type2 = decltype(::testing::matcher_detail::decay_copy(arg2));                                \
    return matcher_name##_testing_matcher2<::testing::matcher_detail::matcher_tag, value_type1, value_type2>{ \
        arg1,                                                                                                 \
        arg2                                                                                                  \
    };                                                                                                        \
  }                                                                                                           \
  template <class Tag, class Value1, class Value2>                                                            \
  template <class Arg>                                                                                        \
  constexpr auto matcher_name##_testing_matcher2<Tag, Value1, Value2>::operator()(Arg const& arg) const -> bool

namespace testing {

MATCHER_P(Eq, value, "argument == value") { return arg == value; }

MATCHER_P(
    Each,
    expr,
    "argument is a container where every element matches e, which can be either a value or a matcher."
) {
  auto const& matcher = ::testing::matcher_detail::matcher_else_eq(expr);

  for (auto const& elem : arg) {
    if (!matcher(elem)) {
      return false;
    }
  }
  return true;
}

MATCHER_P2(
    ElementsAreArray,
    begin,
    end,
    "The same as ElementsAre() except that the expected element values/matchers come from an initializer list, "
    "STL-style container, iterator range, or C-style array."
) {
  auto iter = begin;

  for (auto const& elem : arg) {
    if (iter == end) {
      return false;
    }
    if (!(*iter == elem)) {
      return false;
    }
    ++iter;
  }
  return iter == end;
}
MATCHER_P(
    ElementsAreArray,
    a_container,
    "The same as ElementsAre() except that the expected element values/matchers come from an initializer list, "
    "STL-style container, iterator range, or C-style array."
) {
  return ElementsAreArray(a_container.begin(), a_container.end())(arg);
}
/// @brief overload for C-arrays
template <class T, decltype(sizeof(int)) N>
// NOLINTNEXTLINE(readability-identifier-naming,hicpp-avoid-c-arrays)
constexpr auto ElementsAreArray(T (&arr)[N]) {
  return ElementsAreArray(arr + 0, arr + N);
}
/// @brief overload that handles initializer lists
template <class T, decltype(sizeof(int)) N>
// NOLINTNEXTLINE(readability-identifier-naming,hicpp-avoid-c-arrays)
constexpr auto ElementsAreArray(T const (&arr)[N]) {
  return ElementsAreArray(arr + 0, arr + N);
}

}  // namespace testing

#if defined(__clang__)
_Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
_Pragma("GCC diagnostic pop")
#endif

#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_MATCHER_HPP_
