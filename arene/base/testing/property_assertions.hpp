#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_ASSERTIONS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_ASSERTIONS_HPP_

// IWYU pragma: begin_keep
#include "arene/base/testing/detail/property_context.hpp"  // IWYU pragma: export
#include "arene/base/testing/property_error.hpp"           // IWYU pragma: export
// IWYU pragma: end_keep

/// @brief Macro to assert that a property is @c true
/// @param expr property to check
/// If the property encoded in @c expr is @c false, returns a @c property_outcome with <c>status == failure</c>.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_PROP_ASSERT(...)                                                                                \
  do { /* NOLINT(cppcoreguidelines-avoid-do-while) */                                                         \
    if (!static_cast<bool>(__VA_ARGS__) && !::arene::base::testing::detail::property_context().has_value()) { \
      ::arene::base::testing::detail::property_context().emplace(::arene::base::testing::property_error{      \
          ::arene::base::testing::property_error::error_code::failure,                                        \
          {__FILE__, __LINE__},                                                                               \
          {#__VA_ARGS__}                                                                                      \
      });                                                                                                     \
      return;                                                                                                 \
    }                                                                                                         \
  } while (false)

/// @brief Macro to assert that a property is @c true
/// @param expr property to check
/// If the property encoded in @c expr is @c false, returns a @c property_outcome with <c>status == failure</c>.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_PROP_ASSERT_TRUE(...) ARENE_PROP_ASSERT(__VA_ARGS__)

/// @brief Macro to assert that a property is @c false
/// @param expr property to check
/// If the property encoded in @c expr is @c true, returns a @c property_outcome with <c>status == failure</c>.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_PROP_ASSERT_FALSE(...) ARENE_PROP_ASSERT(!(__VA_ARGS__))

/// @brief Macro to discard a specific run
/// @param expr property to check
/// If @c expr is @c false, returns a @c property_outcome with <c>status == discard</c>.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_PROP_PRE(...)                                                                                   \
  do { /* NOLINT(cppcoreguidelines-avoid-do-while) */                                                         \
    if (!static_cast<bool>(__VA_ARGS__) && !::arene::base::testing::detail::property_context().has_value()) { \
      ::arene::base::testing::detail::property_context().emplace(::arene::base::testing::property_error{      \
          ::arene::base::testing::property_error::error_code::discard,                                        \
          {__FILE__, __LINE__},                                                                               \
          {#__VA_ARGS__}                                                                                      \
      });                                                                                                     \
      return;                                                                                                 \
    }                                                                                                         \
  } while (false)
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_ASSERTIONS_HPP_
