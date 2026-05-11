// Copyright 2024, Toyota Motor Corporation

// Examples from the documentation of the \ref compare subpackage

#include "arene/base/compare.hpp"
#include "arene/base/string_view.hpp"
#include "arene/base/type_traits.hpp"

namespace {
/// @brief A Dummy do-nothing comparison function, used to ensure that the examples below compile without requiring real
/// semantics
/// @tparam T The type of the first comparand
/// @tparam U The type of the second comparand
/// @return arene::base::strong_ordering holding the result (always equal)
template <typename T, typename U>
arene::base::strong_ordering real_comparison(T const&, U const&) noexcept {
  return arene::base::strong_ordering::equal;
}

/// @brief A Dummy do-nothing comparison function to check if two string-like types have the same size, used to ensure
/// that the examples below compile without requiring real semantics
/// @tparam T The type of the first comparand
/// @tparam U The type of the second comparand
/// @return bool holding the result (always @c false)
template <typename T, typename U>
bool same_size(T const&, U const&) noexcept {
  return false;
}

/// @brief Dummy string_view conversion function to make sure that the three-way-plus-equals example compiles
/// @tparam T The type to be "converted" to a @c arene::base::string_view
/// @return An @c arene::base::string_view pointing to an empty string
template <typename T>
arene::base::string_view to_string_view(T const&) noexcept {
  return "";
}

namespace just_three_way {
//! [string_like_generic_example]
/// @brief An example string-like class with generated comparison operations
class string_like : arene::base::generic_ordering_from_three_way_compare<string_like> {
 public:
  /// @brief a three-way ordering function for two @c string_like instances
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return arene::base::strong_ordering holding the result of the comparison
  ARENE_NODISCARD static arene::base::strong_ordering
  three_way_compare(string_like const& lhs, string_like const& rhs) noexcept {
    return real_comparison(lhs, rhs);
  }
  /// @brief a three-way ordering function for a @c string_like object with a @c arene::base::string_view object
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return arene::base::strong_ordering holding the result of the comparison
  ARENE_NODISCARD static arene::base::strong_ordering
  three_way_compare(string_like const& lhs, arene::base::string_view rhs) noexcept {
    return real_comparison(lhs, rhs);
  }

  /// @brief a three-way ordering function for a @c string_like object with a @c
  /// arene::base::null_terminated_string_view object
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return arene::base::strong_ordering holding the result of the comparison
  ARENE_NODISCARD static arene::base::strong_ordering
  three_way_compare(string_like const& lhs, arene::base::null_terminated_string_view rhs) noexcept {
    return real_comparison(lhs, rhs);
  }

  /// @brief A fast inequality check for two @c string_like types to avoid character-by-character comparisons if they
  /// have different lengths
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return arene::base::inequality_heuristic which is @c arene::base::inequality_heuristic::definitely_not_equal if
  /// the strings have different lengths and @c arene::base::inequality_heuristic::may_be_equal_or_not_equal otherwise
  ARENE_NODISCARD static arene::base::inequality_heuristic
  fast_inequality_check(string_like const& lhs, string_like const& rhs) noexcept {
    return same_size(lhs, rhs) ? arene::base::inequality_heuristic::may_be_equal_or_not_equal
                               : arene::base::inequality_heuristic::definitely_not_equal;
  }

  /// @brief A fast inequality check for two @c string_like types to avoid character-by-character comparisons if they
  /// have different lengths
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return arene::base::inequality_heuristic which is @c arene::base::inequality_heuristic::definitely_not_equal if
  /// the strings have different lengths and @c arene::base::inequality_heuristic::may_be_equal_or_not_equal otherwise
  ARENE_NODISCARD static arene::base::inequality_heuristic
  fast_inequality_check(string_like const& lhs, arene::base::string_view rhs) noexcept {
    return same_size(lhs, rhs) ? arene::base::inequality_heuristic::may_be_equal_or_not_equal
                               : arene::base::inequality_heuristic::definitely_not_equal;
  }
};
//! [string_like_generic_example]

// Assert that the comparison operations actually are present
static_assert(
    arene::base::is_equality_comparable_v<string_like, arene::base::string_view>,
    "Must be equality comparable"
);
static_assert(
    arene::base::is_equality_comparable_v<arene::base::string_view, string_like>,
    "Must be equality comparable"
);
static_assert(
    arene::base::is_inequality_comparable_v<string_like, arene::base::string_view>,
    "Must be inequality comparable"
);
static_assert(
    arene::base::is_inequality_comparable_v<arene::base::string_view, string_like>,
    "Must be inequality comparable"
);
static_assert(
    arene::base::is_less_than_comparable_v<string_like, arene::base::string_view>,
    "Must be less than comparable"
);
static_assert(
    arene::base::is_less_than_comparable_v<arene::base::string_view, string_like>,
    "Must be less than comparable"
);
static_assert(
    arene::base::is_less_than_or_equal_comparable_v<string_like, arene::base::string_view>,
    "Must be less than or equal comparable"
);
static_assert(
    arene::base::is_less_than_or_equal_comparable_v<arene::base::string_view, string_like>,
    "Must be less than or equal comparable"
);
static_assert(
    arene::base::is_greater_than_comparable_v<string_like, arene::base::string_view>,
    "Must be greater than comparable"
);
static_assert(
    arene::base::is_greater_than_comparable_v<arene::base::string_view, string_like>,
    "Must be greater than comparable"
);
static_assert(
    arene::base::is_greater_than_or_equal_comparable_v<string_like, arene::base::string_view>,
    "Must be greater than or equal comparable"
);
static_assert(
    arene::base::is_greater_than_or_equal_comparable_v<arene::base::string_view, string_like>,
    "Must be greater than or equal comparable"
);

/// @brief test function to actually use the comparisons, to avoid unused function errors from the compiler and ensure
/// they compile OK
ARENE_MAYBE_UNUSED void test() {
  string_like a;
  string_like b;
  std::ignore = a == b;
  std::ignore = a == arene::base::string_view{};
  std::ignore = arene::base::string_view{} == a;
  std::ignore = a != b;
  std::ignore = a != arene::base::string_view{};
  std::ignore = arene::base::string_view{} != a;
  std::ignore = arene::base::string_view{} > a;
  std::ignore = arene::base::string_view{} < a;
}

}  // namespace just_three_way

namespace three_way_and_equals {
//! [three_way_and_equals_mixin_example]
/// @brief An example string-like class with generated comparison operations
class string_like : arene::base::generic_ordering_from_three_way_compare_and_equals<string_like> {
 public:
  /// @brief a three-way ordering function for two @c string_like instances
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return arene::base::strong_ordering holding the result of the comparison
  ARENE_NODISCARD static arene::base::strong_ordering
  three_way_compare(string_like const& lhs, string_like const& rhs) noexcept {
    return real_comparison(lhs, rhs);
  }
  /// @brief a three-way ordering function for a @c string_like object with a @c arene::base::string_view object
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return arene::base::strong_ordering holding the result of the comparison
  ARENE_NODISCARD static arene::base::strong_ordering
  three_way_compare(string_like const& lhs, arene::base::string_view rhs) noexcept {
    return real_comparison(lhs, rhs);
  }

  /// @brief a three-way ordering function for a @c string_like object with a @c
  /// arene::base::null_terminated_string_view object
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return arene::base::strong_ordering holding the result of the comparison
  ARENE_NODISCARD static arene::base::strong_ordering
  three_way_compare(string_like const& lhs, arene::base::null_terminated_string_view rhs) noexcept {
    return real_comparison(lhs, rhs);
  }

  /// @brief A fast inequality check for two @c string_like types to avoid character-by-character comparisons if they
  /// have different lengths
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return arene::base::inequality_heuristic which is @c arene::base::inequality_heuristic::definitely_not_equal if
  /// the strings have different lengths and @c arene::base::inequality_heuristic::may_be_equal_or_not_equal otherwise
  ARENE_NODISCARD friend bool operator==(string_like const& lhs, arene::base::string_view rhs) noexcept {
    return to_string_view(lhs) == rhs;
  }

  /// @brief A fast inequality check for two @c string_like types to avoid character-by-character comparisons if they
  /// @param lhs The first object being compared
  /// @param rhs The second object being compared
  /// @return arene::base::inequality_heuristic which is @c arene::base::inequality_heuristic::definitely_not_equal if
  /// the strings have different lengths and @c arene::base::inequality_heuristic::may_be_equal_or_not_equal otherwise
  ARENE_NODISCARD friend bool operator==(string_like const& lhs, string_like const& rhs) noexcept {
    return lhs == to_string_view(rhs);
  }
};
//! [three_way_and_equals_mixin_example]

// Assert that the comparison operations actually are present
static_assert(
    arene::base::is_equality_comparable_v<string_like, arene::base::string_view>,
    "Must be equality comparable"
);
static_assert(
    arene::base::is_equality_comparable_v<arene::base::string_view, string_like>,
    "Must be equality comparable"
);
static_assert(
    arene::base::is_inequality_comparable_v<string_like, arene::base::string_view>,
    "Must be inequality comparable"
);
static_assert(
    arene::base::is_inequality_comparable_v<arene::base::string_view, string_like>,
    "Must be inequality comparable"
);
static_assert(
    arene::base::is_less_than_comparable_v<string_like, arene::base::string_view>,
    "Must be less than comparable"
);
static_assert(
    arene::base::is_less_than_comparable_v<arene::base::string_view, string_like>,
    "Must be less than comparable"
);
static_assert(
    arene::base::is_less_than_or_equal_comparable_v<string_like, arene::base::string_view>,
    "Must be less than or equal comparable"
);
static_assert(
    arene::base::is_less_than_or_equal_comparable_v<arene::base::string_view, string_like>,
    "Must be less than or equal comparable"
);
static_assert(
    arene::base::is_greater_than_comparable_v<string_like, arene::base::string_view>,
    "Must be greater than comparable"
);
static_assert(
    arene::base::is_greater_than_comparable_v<arene::base::string_view, string_like>,
    "Must be greater than comparable"
);
static_assert(
    arene::base::is_greater_than_or_equal_comparable_v<string_like, arene::base::string_view>,
    "Must be greater than or equal comparable"
);
static_assert(
    arene::base::is_greater_than_or_equal_comparable_v<arene::base::string_view, string_like>,
    "Must be greater than or equal comparable"
);

/// @brief test function to actually use the comparisons, to avoid unused function errors from the compiler and ensure
/// they compile OK
ARENE_MAYBE_UNUSED void test() {
  string_like a;
  string_like b;
  std::ignore = a == b;
  std::ignore = a == arene::base::string_view{};
  std::ignore = arene::base::string_view{} == a;
  std::ignore = a != b;
  std::ignore = a != arene::base::string_view{};
  std::ignore = arene::base::string_view{} != a;
  std::ignore = arene::base::string_view{} > a;
  std::ignore = arene::base::string_view{} < a;
}

}  // namespace three_way_and_equals

}  // namespace
