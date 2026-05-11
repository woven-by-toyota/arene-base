// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "Comparisons are against copied values, no maths performed");

#include <gtest/gtest.h>

#include "arene/base/inline_container/map.hpp"
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/monostate/monostate.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/max_value_overload.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/variant/variant.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/has_broken_less_than.hpp"

namespace {

/// @brief test string class with the same semantics as @c std::string but using @c inline_string
class test_string {
  arene::base::inline_string<50> str_;

 public:
  test_string() = default;
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  test_string(char const* source)
      : str_(source) {}
  test_string(std::initializer_list<char> source)
      : str_(source.begin(), source.end()) {}

  test_string(test_string&& other) noexcept
      : str_(other.str_) {
    other.str_.clear();
  }

  test_string(test_string const& other) = default;

  auto operator=(test_string const& other) -> test_string& = default;
  auto operator=(test_string&& other) noexcept -> test_string& {
    str_ = other.str_;
    other.str_.clear();
    return *this;
  }

  ~test_string() = default;

  friend auto operator<(test_string const& lhs, test_string const& rhs) noexcept -> bool { return lhs.str_ < rhs.str_; }
  friend auto operator==(test_string const& lhs, test_string const& rhs) noexcept -> bool {
    return lhs.str_ == rhs.str_;
  }
};

template <typename Type>
using test_vector = arene::base::inline_vector<Type, 10>;

template <typename Key, typename Value>
using test_map = arene::base::inline_map<Key, Value, 10>;

// NOLINTBEGIN(bugprone-exception-escape) clang-tidy false positives here for some reason.
struct person {
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  person(test_string name)
      : name_(std::move(name)) {}

  ~person() = default;

  // NOLINTNEXTLINE(bugprone-exception-escape)
  person(person&&) = default;
  // NOLINTNEXTLINE(bugprone-exception-escape)
  auto operator=(person&&) -> person& = default;

  person(person const&) = default;
  auto operator=(person const&) -> person& = default;

  test_string name_;
  auto operator<(person const& other) const noexcept -> bool { return name_ < other.name_; }
  auto operator==(person const& other) const noexcept -> bool { return name_ == other.name_; }
};
// NOLINTEND(bugprone-exception-escape)

class non_default_constructible {
 public:
  non_default_constructible() = delete;
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  non_default_constructible(std::int32_t) {}
};

static_assert(
    !std::is_default_constructible<arene::base::variant<non_default_constructible, std::int32_t>>::value,
    "arene::base::variant<NonDefaultConstructible, std::int32_t> should "
    "not be default constructible"
);

class non_copyable {
 public:
  non_copyable() = default;
  ~non_copyable() = default;
  non_copyable(non_copyable const&) = delete;
  non_copyable(non_copyable&&) = default;
  auto operator=(non_copyable const&) -> non_copyable& = delete;
  auto operator=(non_copyable&&) -> non_copyable& = default;
};

class non_movable {
 public:
  non_movable() = default;
  ~non_movable() = default;
  non_movable(non_movable const&) = default;
  non_movable(non_movable&&) = delete;
  auto operator=(non_movable const&) -> non_movable& = default;
  auto operator=(non_movable&&) -> non_movable& = delete;
};

/// @test `emplace` is `noexcept` if and only if the selected constructor of the type being constructed is `noexcept`
TEST(VariantEmplace, IsNoexceptIfTypeCtorIsNoexcept) {
  using throwing_type = testing::
      configurable_value<test_vector<int>, testing::throws_on::copy_construct | testing::throws_on::value_construct>;

  using variant_t = arene::base::variant<std::int32_t, throwing_type>;
  {
    SCOPED_TRACE("type where all ctors are noexcept");
    STATIC_ASSERT_TRUE(noexcept(std::declval<variant_t>().emplace<std::int32_t>(std::declval<std::int32_t>())));
    STATIC_ASSERT_TRUE(noexcept(std::declval<variant_t>().emplace<0>(std::declval<std::int32_t>())));
  }
  {
    SCOPED_TRACE("type where some ctors are noexcept");
    STATIC_ASSERT_TRUE(noexcept(std::declval<variant_t>().emplace<throwing_type>(std::declval<throwing_type&&>())));
    STATIC_ASSERT_FALSE(noexcept(std::declval<variant_t>().emplace<throwing_type>(std::declval<throwing_type const&>()))
    );
    STATIC_ASSERT_TRUE(noexcept(std::declval<variant_t>().emplace<1>(std::declval<throwing_type&&>())));
    STATIC_ASSERT_FALSE(noexcept(std::declval<variant_t>().emplace<1>(std::declval<throwing_type const&>())));

    STATIC_ASSERT_FALSE(
        noexcept(std::declval<variant_t>().emplace<throwing_type>(std::declval<std::initializer_list<int>>()))
    );
    STATIC_ASSERT_FALSE(noexcept(std::declval<variant_t>().emplace<1>(std::declval<std::initializer_list<int>>())));
  }
}

/// @test A default-constructed `variant` has an index of 0, and contains a default-constructed instance of the first
/// type
TEST(Variant, DefaultVariant) {
  arene::base::variant<test_string, std::int32_t, float, double> const value;
  ASSERT_EQ(value.index(), 0);

  ASSERT_EQ(value.get<test_string>(), "");
  ASSERT_EQ(value.get<0>(), "");
}

/// @test If the first type in the type list of a `variant` is not default-constructible, then the `variant` type is not
/// default-constructible
TEST(Variant, NonDefaultConstructible) {
  // The next line should not compile
  // arene::base::variant<NonDefaultConstructible, int> value;

  arene::base::variant<test_string, non_default_constructible> const value;
  ASSERT_EQ(value.index(), 0);
  arene::base::variant<non_default_constructible, test_string> const value2{0};
  ASSERT_EQ(value.index(), 0);
}

/// @test After using `emplace`, or a constructor parameter, to set the value of a `variant` then the stored value has
/// the type used for `emplace`, and `index` returns the appropriate index value
TEST(Variant, GetSetPrimitiveType) {
  using variant_type = arene::base::variant<test_string, std::int32_t, float, double>;

  variant_type value(static_cast<std::int32_t>(10));  // Choose std::int32_t

  ASSERT_EQ(value.get<std::int32_t>(), 10);
  ASSERT_EQ(value.index(), 1);

  value.emplace<float>(10.0F);
  ASSERT_EQ(value.get<float>(), 10.0);
  ASSERT_EQ(value.index(), 2);

  value.emplace<double>(42.0);
  ASSERT_EQ(value.get<double>(), 42.0);
  ASSERT_EQ(value.index(), 3);

  value.emplace<test_string>("Magic String");
  ASSERT_EQ(value.get<test_string>(), "Magic String");
  ASSERT_EQ(value.index(), 0);
}

/// @test An object can be implicitly converted to a `variant` if exactly one of the element types can be constructed
/// from that object, or there is a clear "best match" conversion.
TEST(Variant, ConvertibleChecker) {
  {
    using variant_type = arene::base::variant<arene::base::monostate, person>;
    using type = test_string;
    using is_valid =
        decltype(arene::base::variant_detail::convertible_checker<variant_type>::run(std::declval<type>(), {}));
    static_assert(is_valid::value == 1, "Convertible checker failed");
  }

  {
    using variant_type = arene::base::variant<arene::base::monostate, person>;
    using type = person;
    using is_valid =
        decltype(arene::base::variant_detail::convertible_checker<variant_type>::run(std::declval<type>(), {}));
    static_assert(is_valid::value == 1, "Convertible checker failed");
  }

  {
    using variant_type = arene::base::variant<arene::base::monostate, person, test_string>;
    using type = test_string;
    using is_valid =
        decltype(arene::base::variant_detail::convertible_checker<variant_type>::run(std::declval<type>(), {}));
    static_assert(is_valid::value == 2,
                  "Convertible checker failed");  // test_string is a better match
  }

  {
    using variant_type = arene::base::variant<arene::base::monostate, person, test_string>;
    using type = char const*;
    using is_valid =
        decltype(arene::base::variant_detail::convertible_checker<variant_type>::run(std::declval<type>(), {}));
    static_assert(is_valid::value == 2,
                  "Convertible checker failed");  // test_string is a better match
  }

  // The following section will not compile.
  // There can be only one type that can be converted from T with the same
  // level of match.
  // {
  //   using variant_type = arene::base::variant<arene::base::monostate, Person, Car>;
  //   using type = test_string;
  //   using is_valid = decltype(arene::base::variant_detail::convertible_checker<variant_type>::run(
  //       std::declval<T>(), {}));
  //   static_assert(is_valid::value == 1, "Convertible checker failed");
  // }
}

/// @test `accepted_index` provides the index of the target type from an implicit conversion to a `variant`
TEST(Variant, AcceptedIndex) {
  {
    using variant_type = arene::base::variant<arene::base::monostate, person>;
    using type = test_string;
    constexpr auto is_valid = arene::base::variant_detail::accepted_index<variant_type, type>::type::value;
    static_assert(is_valid == 1, "Accepted Index failed");
  }

  {
    using variant_type = arene::base::variant<arene::base::monostate, person>;
    using type = person;
    constexpr auto is_valid = arene::base::variant_detail::accepted_index<variant_type, type>::type::value;
    static_assert(is_valid == 1, "Accepted Index failed");
  }

  {
    using variant_type = arene::base::variant<arene::base::monostate, person, test_string>;
    using type = test_string;
    constexpr auto is_valid = arene::base::variant_detail::accepted_index<variant_type, type>::type::value;
    static_assert(is_valid == 2,
                  "Accepted Index failed");  // test_string is a better match
  }

  {
    using variant_type = arene::base::variant<arene::base::monostate, person, test_string>;
    using type = char const*;
    constexpr auto is_valid = arene::base::variant_detail::accepted_index<variant_type, type>::type::value;
    static_assert(is_valid == 2,
                  "Accepted Index failed");  // test_string is a better match
  }

  // The following section will not compile.
  // There can be only one type that can be converted from T with the same
  // level of match.
  // {
  //   using variant_type = arene::base::variant<arene::base::monostate, Person, Car>;
  //   using type = test_string;
  //   constexpr auto is_valid = arene::base::variant_detail::accepted_index<variant_type,
  //   T>::type::value; static_assert(is_valid == 2,
  //                 "Accepted Index failed");  // test_string is a better match
  // }
}

/// @test Copying a `variant` copies the index and value of the stored object
TEST(Variant, CopyConstructor) {
  using variant_type = arene::base::variant<std::int32_t, float, double, test_string, person>;
  variant_type value;

  // Copy construct primitive type
  {
    value.emplace<std::int32_t>(10);
    variant_type const value2(value);
    ASSERT_EQ(value2.get<std::int32_t>(), 10);
    ASSERT_EQ(value2.index(), 0);
    ASSERT_EQ(value.get<std::int32_t>(), 10);
    ASSERT_EQ(value.index(), 0);
  }

  // Copy construct primitive (float) type
  {
    value.emplace<float>(10.2F);
    variant_type const value2(value);
    ASSERT_EQ(value2.get<float>(), 10.2F);
    ASSERT_EQ(value2.index(), 1);
    ASSERT_EQ(value.get<float>(), 10.2F);
    ASSERT_EQ(value.index(), 1);
  }

  // Copy construct primitive (double) type
  {
    value.emplace<double>(10.2);
    variant_type const value2(value);
    ASSERT_EQ(value2.get<double>(), 10.2);
    ASSERT_EQ(value2.index(), 2);
    ASSERT_EQ(value.get<double>(), 10.2);
    ASSERT_EQ(value.index(), 2);
  }

  // Copy Construct test_string
  {
    value.emplace<test_string>("Magic String");
    variant_type const value2(value);
    ASSERT_EQ(value2.get<test_string>(), "Magic String");
    ASSERT_EQ(value2.index(), 3);
    ASSERT_EQ(value.get<test_string>(), "Magic String");
    ASSERT_EQ(value.index(), 3);
  }

  // Copy Construct Person
  {
    value.emplace<person>("John Doe");
    variant_type const value2(value);
    ASSERT_EQ(value2.get<person>().name_, "John Doe");
    ASSERT_EQ(value2.index(), 4);
    ASSERT_EQ(value.get<person>().name_, "John Doe");
    ASSERT_EQ(value.index(), 4);
  }
}

/// @test Move-constructing a `variant` copies the index, and move-constructs the stored value
TEST(Variant, MoveConstructor) {
  using variant_type = arene::base::variant<std::int32_t, float, double, test_string, person>;
  variant_type value;

  // Move construct primitive type
  {
    value.emplace<std::int32_t>(10);
    variant_type value2(std::move(value));
    ASSERT_EQ(value2.get<std::int32_t>(), 10);
    ASSERT_EQ(value2.index(), 0);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    ASSERT_EQ(value.get<std::int32_t>(),
              10);  // Move semantic doesn't affect primitive types
    ASSERT_EQ(value.index(), 0);
  }

  // Move construct primitive (float) type
  {
    value.emplace<float>(10.2F);
    variant_type value2(std::move(value));
    ASSERT_EQ(value2.get<float>(), 10.2F);
    ASSERT_EQ(value2.index(), 1);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    ASSERT_EQ(value.get<float>(),
              10.2F);  // Move semantic doesn't affect primitive types
    ASSERT_EQ(value.index(), 1);
  }

  // Move construct primitive (float) type
  {
    value.emplace<double>(10.2);
    variant_type value2(std::move(value));
    ASSERT_EQ(value2.get<double>(), 10.2);
    ASSERT_EQ(value2.index(), 2);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    ASSERT_EQ(value.get<double>(),
              10.2);  // Move semantic doesn't affect primitive types
    ASSERT_EQ(value.index(), 2);
  }

  // Move Construct test_string
  {
    value.emplace<test_string>("Magic String");
    variant_type value2(std::move(value));
    ASSERT_EQ(value2.get<test_string>(), "Magic String");
    ASSERT_EQ(value2.index(), 3);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    ASSERT_EQ(value.get<test_string>(), "");
    ASSERT_EQ(value.index(), 3);
  }

  // Move Construct Person
  {
    value.emplace<person>("John Doe");
    variant_type value2(std::move(value));
    ASSERT_EQ(value2.get<person>().name_, "John Doe");
    ASSERT_EQ(value2.index(), 4);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    ASSERT_EQ(value.get<person>().name_, "");
    ASSERT_EQ(value.index(), 4);
  }
}

/// @test When a non-`variant` value is passed to the constructor of `variant`, the stored value is of the type with the
/// best match for the conversion.
TEST(Variant, ConvertConstructor) {
  using variant_type = arene::base::variant<std::int32_t, float, double, person, test_string>;

  // Convert construct std::int32_t
  {
    variant_type const value{static_cast<std::int32_t>(10)};
    ASSERT_EQ(value.index(), 0);
    ASSERT_EQ(value.get<std::int32_t>(), 10);
  }

  // Convert construct float
  {
    variant_type const value{42.0F};
    ASSERT_EQ(value.index(), 1);
    ASSERT_EQ(value.get<float>(), 42.0F);
  }

  // Convert construct double
  {
    variant_type const value{42.0};
    ASSERT_EQ(value.index(), 2);
    ASSERT_EQ(value.get<double>(), 42.0);
  }

  // Convert construct Person
  {
    variant_type const value{person{"John"}};
    ASSERT_EQ(value.index(), 3);
    ASSERT_EQ(value.get<person>().name_, "John");
  }

  // Convert construct test_string
  {
    test_string str{"John"};
    variant_type const value{std::move(str)};
    ASSERT_EQ(value.index(), 4);
    ASSERT_EQ(value.get<test_string>(), "John");
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    ASSERT_EQ(str, "");
  }

  // Convert construct test_string with string literal
  {
    variant_type const value{"John"};
    ASSERT_EQ(value.index(), 4);
    ASSERT_EQ(value.get<test_string>(), "John");
  }

  // The following section will not compile.
  // There can be only one type that can be converted from test_string with the
  // same level of match.
  // using V2 = arene::base::variant<arene::base::monostate, Person, Car>;
  // {
  //   V2 value{test_string{"John"}};
  //   ASSERT_EQ(value.index(), 1);
  //   ASSERT_EQ(value.get<Person>().name_, "John");
  // }
}

/// @test Copy assignment of a `variant` holding a value of the same type assigns the values
TEST(Variant, CopyAssignment) {
  using variant_type = arene::base::variant<std::int32_t, float, double, test_string, person>;
  variant_type value;

  // Copy assignment primitive type
  {
    value.emplace<std::int32_t>(10);
    variant_type value2;
    value2.emplace<std::int32_t>(0);  // to test assignment
    value2 = static_cast<variant_type const&>(value);
    ASSERT_EQ(value2.index(), 0);
    ASSERT_EQ(value2.get<std::int32_t>(), 10);
    ASSERT_EQ(value.index(), 0);
    ASSERT_EQ(value.get<std::int32_t>(), 10);
  }

  // Copy assignment primitive (float) type
  {
    value.emplace<float>(10.2F);
    variant_type value2;
    value2.emplace<float>(0.0F);  // to test assignment
    value2 = static_cast<variant_type const&>(value);
    ASSERT_EQ(value2.index(), 1);
    ASSERT_EQ(value2.get<float>(), 10.2F);
    ASSERT_EQ(value.index(), 1);
    ASSERT_EQ(value.get<float>(), 10.2F);
  }

  // Copy assignment primitive (double) type
  {
    value.emplace<double>(10.2);
    variant_type value2;
    value2.emplace<double>(0.0);  // to test assignment
    value2 = static_cast<variant_type const&>(value);
    ASSERT_EQ(value2.index(), 2);
    ASSERT_EQ(value2.get<double>(), 10.2);
    ASSERT_EQ(value.index(), 2);
    ASSERT_EQ(value.get<double>(), 10.2);
  }

  // Copy assignment test_string
  {
    value.emplace<test_string>("Magic String");
    variant_type value2;
    value2.emplace<test_string>("");  // to test assignment
    value2 = static_cast<variant_type const&>(value);
    ASSERT_EQ(value2.index(), 3);
    ASSERT_EQ(value2.get<test_string>(), "Magic String");
    value2.get<test_string>() = "Changed";
    ASSERT_EQ(value.index(), 3);
    ASSERT_EQ(value.get<test_string>(), "Magic String");
  }

  // Copy assignment Person
  {
    value.emplace<person>("John Doe");
    variant_type value2;
    value2.emplace<person>("None");  // to test assignment
    value2 = static_cast<variant_type const&>(value);
    ASSERT_EQ(value2.index(), 4);
    ASSERT_EQ(value2.get<person>().name_, "John Doe");
    value2.get<person>().name_ = "Changed";
    ASSERT_EQ(value.index(), 4);
    ASSERT_EQ(value.get<person>().name_, "John Doe");
  }
}

/// @test Copy assignment of a `variant` holding a value of a different type destroys the original value of the target,
/// and copy-constructs a new value from the source
TEST(Variant, CopyAssignmentToOtherType) {
  using variant_type = arene::base::variant<std::int32_t, float, double, test_string, person>;
  variant_type value;

  // Copy assignment primitive type
  {
    value.emplace<std::int32_t>(10);
    variant_type value2;
    value2.emplace<float>(0.0F);  // to test assignment to variant set to other type
    value2 = static_cast<variant_type const&>(value);
    ASSERT_EQ(value2.index(), 0);
    ASSERT_EQ(value2.get<std::int32_t>(), 10);
    ASSERT_EQ(value.index(), 0);
    ASSERT_EQ(value.get<std::int32_t>(), 10);
  }

  // Copy assignment primitive (float) type
  {
    value.emplace<float>(10.2F);
    variant_type value2;
    value2.emplace<double>(0.0);  // to test assignment to variant set to other type
    value2 = static_cast<variant_type const&>(value);
    ASSERT_EQ(value2.index(), 1);
    ASSERT_EQ(value2.get<float>(), 10.2F);
    ASSERT_EQ(value.index(), 1);
    ASSERT_EQ(value.get<float>(), 10.2F);
  }

  // Copy assignment primitive (double) type
  {
    value.emplace<double>(10.2);
    variant_type value2;
    value2.emplace<test_string>("empty");  // to test assignment to variant set to other type
    value2 = static_cast<variant_type const&>(value);
    ASSERT_EQ(value2.index(), 2);
    ASSERT_EQ(value2.get<double>(), 10.2);
    ASSERT_EQ(value.index(), 2);
    ASSERT_EQ(value.get<double>(), 10.2);
  }

  // Copy assignment test_string
  {
    value.emplace<test_string>("Magic String");
    variant_type value2;
    value2.emplace<person>("None");  // to test assignment to variant set to other type
    value2 = static_cast<variant_type const&>(value);
    ASSERT_EQ(value2.index(), 3);
    ASSERT_EQ(value2.get<test_string>(), "Magic String");
    value2.get<test_string>() = "Changed";
    ASSERT_EQ(value.index(), 3);
    ASSERT_EQ(value.get<test_string>(), "Magic String");
  }

  // Copy assignment Person
  {
    value.emplace<person>("John Doe");
    variant_type value2;
    value2.emplace<std::int32_t>(0);  // to test assignment to variant set to other type
    value2 = static_cast<variant_type const&>(value);
    ASSERT_EQ(value2.index(), 4);
    ASSERT_EQ(value2.get<person>().name_, "John Doe");
    value2.get<person>().name_ = "Changed";
    ASSERT_EQ(value.index(), 4);
    ASSERT_EQ(value.get<person>().name_, "John Doe");
  }
}

/// @test Move assignment of a `variant` to a target with the same stored type move-assigns the stored value
TEST(Variant, MoveAssignment) {
  using variant_type = arene::base::variant<std::int32_t, float, double, test_string, person>;
  variant_type value;

  // Move assignment primitive type
  {
    value.emplace<std::int32_t>(10);
    variant_type value2;
    value2.emplace<std::int32_t>(0);  // same type
    value2 = std::move(value);
    ASSERT_EQ(value2.get<std::int32_t>(), 10);
    ASSERT_EQ(value2.index(), 0);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    ASSERT_EQ(value.get<std::int32_t>(),
              10);  // Move semantic doesn't affect primitive types
    ASSERT_EQ(value.index(), 0);
  }

  // Move assignment primitive type
  {
    value.emplace<float>(10.2F);
    variant_type value2;
    value2.emplace<float>(0.0F);  // same type
    value2 = std::move(value);
    ASSERT_EQ(value2.get<float>(), 10.2F);
    ASSERT_EQ(value2.index(), 1);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    ASSERT_EQ(value.get<float>(),
              10.2F);  // Move semantic doesn't affect primitive types
    ASSERT_EQ(value.index(), 1);
  }

  // Move assignment primitive type
  {
    value.emplace<double>(10.2);
    variant_type value2;
    value2.emplace<double>(0.0);  // same type
    value2 = std::move(value);
    ASSERT_EQ(value2.get<double>(), 10.2);
    ASSERT_EQ(value2.index(), 2);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    ASSERT_EQ(value.get<double>(),
              10.2);  // Move semantic doesn't affect primitive types
    ASSERT_EQ(value.index(), 2);
  }

  // Move assignment test_string
  {
    value.emplace<test_string>("Magic String");
    variant_type value2;
    value2.emplace<test_string>("");  // same type
    value2 = std::move(value);
    ASSERT_EQ(value2.get<test_string>(), "Magic String");
    ASSERT_EQ(value2.index(), 3);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    ASSERT_EQ(value.get<test_string>(), "");
    ASSERT_EQ(value.index(), 3);
  }

  // Move assignment Person
  {
    value.emplace<person>("John Doe");
    variant_type value2;
    value2.emplace<person>("none");  // same type
    value2 = std::move(value);
    ASSERT_EQ(value2.get<person>().name_, "John Doe");
    ASSERT_EQ(value2.index(), 4);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    ASSERT_EQ(value.get<person>().name_, "");
    ASSERT_EQ(value.index(), 4);
  }
}

/// @test Move assignment of a `variant` to a target with a different stored type destroys the old stored value of the
/// target and move-constructs a new value from the source
TEST(Variant, MoveAssignmentToDifferentType) {
  using variant_type = arene::base::variant<std::int32_t, float, double, test_string, person>;
  variant_type value;

  // Move assignment primitive type
  {
    value.emplace<std::int32_t>(10);
    variant_type value2;
    value2.emplace<float>(0.0F);  // different type
    value2 = std::move(value);
    ASSERT_EQ(value2.get<std::int32_t>(), 10);
    ASSERT_EQ(value2.index(), 0);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    ASSERT_EQ(value.get<std::int32_t>(),
              10);  // Move semantic doesn't affect primitive types
    ASSERT_EQ(value.index(), 0);
  }

  // Move assignment primitive type
  {
    value.emplace<float>(10.2F);
    variant_type value2;
    value2.emplace<double>(0.0);  // different type
    value2 = std::move(value);
    ASSERT_EQ(value2.get<float>(), 10.2F);
    ASSERT_EQ(value2.index(), 1);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    ASSERT_EQ(value.get<float>(),
              10.2F);  // Move semantic doesn't affect primitive types
    ASSERT_EQ(value.index(), 1);
  }

  // Move assignment primitive type
  {
    value.emplace<double>(10.2);
    variant_type value2;
    value2.emplace<test_string>("");  // different type
    value2 = std::move(value);
    ASSERT_EQ(value2.get<double>(), 10.2);
    ASSERT_EQ(value2.index(), 2);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    ASSERT_EQ(value.get<double>(),
              10.2);  // Move semantic doesn't affect primitive types
    ASSERT_EQ(value.index(), 2);
  }

  // Move assignment test_string
  {
    value.emplace<test_string>("Magic String");
    variant_type value2;
    value2.emplace<person>("none");  // different type
    value2 = std::move(value);
    ASSERT_EQ(value2.get<test_string>(), "Magic String");
    ASSERT_EQ(value2.index(), 3);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    ASSERT_EQ(value.get<test_string>(), "");
    ASSERT_EQ(value.index(), 3);
  }

  // Move assignment Person
  {
    value.emplace<person>("John Doe");
    variant_type value2;
    value2.emplace<std::int32_t>(0);  // different type
    value2 = std::move(value);
    ASSERT_EQ(value2.get<person>().name_, "John Doe");
    ASSERT_EQ(value2.index(), 4);
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    ASSERT_EQ(value.get<person>().name_, "");
    ASSERT_EQ(value.index(), 4);
  }
}

/// @test If any of the element types are not copyable, then `variant` is not copyable
TEST(Variant, NonCopyable) {
  using variant_type = arene::base::variant<std::int32_t, non_copyable>;
  static_assert(!std::is_copy_constructible<variant_type>::value, "V should not be copy constructible");
  static_assert(!std::is_copy_assignable<variant_type>::value, "V should not be copy assignable");
}

/// @test If any of the element types are not movable, then `variant` is not movable
TEST(Variant, NonMovable) {
  using variant_type = arene::base::variant<std::int32_t, non_movable>;
  static_assert(!std::is_move_constructible<variant_type>::value, "V should not be move constructible");
  static_assert(!std::is_move_assignable<variant_type>::value, "V should not be move assignable");
}

/// @test Self-assignment of a `variant` does nothing
TEST(Variant, SelfAssignWillNotDestroyStoredObject) {
  using variant_type = arene::base::variant<std::int32_t, float, double, test_string>;
  variant_type value{static_cast<std::int32_t>(42)};
  variant_type& reference_to_same = value;

  value = reference_to_same;
  ASSERT_EQ(value.index(), 0);
  ASSERT_EQ(value.get<0>(), 42);

  value = std::move(reference_to_same);
  ASSERT_EQ(value.index(), 0);
  ASSERT_EQ(value.get<0>(), 42);
}

/// @test Given two instances of `variant<Ts...>` `lhs` and `rhs`, and given equality comparison is defined for all
/// `Ts`, and given `lhs.index() != rhs.index()`, when the instances are compared for equality, then they always compare
/// non-equal.
TEST(Variant, VariantsWithDifferentActiveAlternativesAreNotEqual) {
  using variant_type = arene::base::variant<std::int32_t, float, double, test_string>;
  variant_type value1{static_cast<std::int32_t>(42)};
  variant_type const value2{20.3F};
  ASSERT_FALSE(value1 == value2);
  ASSERT_TRUE(value1 != value2);

  ASSERT_FALSE(value2 == value1);
  ASSERT_TRUE(value2 != value1);

  value1.emplace<test_string>("Very long string that disables SSO.");
  ASSERT_FALSE(value1 == value2);
  ASSERT_TRUE(value1 != value2);

  ASSERT_FALSE(value2 == value1);
  ASSERT_TRUE(value2 != value1);
}

/// @test Given two instances of `variant<Ts...>` `lhs` and `rhs`, and given equality comparison is defined for all
/// `Ts`, and given `lhs.index() == rhs.index()`, when the instances are compared for equality, then the result is
/// equivalent to comparing the underlying values.
TEST(Variant, VariantsWithSameActiveAlternativesCompareEqualIfUnderlyingValuesAreEqual) {
  using variant_type = arene::base::variant<std::int32_t, float, double, test_string>;
  variant_type value1;
  variant_type value2;
  ASSERT_TRUE(value1 == value2);
  ASSERT_FALSE(value1 != value2);

  value1.emplace<double>(42.0);
  value2.emplace<double>(42.0);
  ASSERT_TRUE(value1 == value2);
  ASSERT_FALSE(value1 != value2);
  value2.emplace<double>(43.0);
  ASSERT_FALSE(value1 == value2);
  ASSERT_TRUE(value1 != value2);

  value1.emplace<test_string>("Hello World");
  value2.emplace<test_string>("Hello World");
  ASSERT_TRUE(value1 == value2);
  ASSERT_FALSE(value1 != value2);
  value2.emplace<test_string>("Very long string that disables SSO.");
  ASSERT_FALSE(value1 == value2);
  ASSERT_TRUE(value1 != value2);
  value1.emplace<test_string>("Very long string that disables SSO.");
  ASSERT_TRUE(value1 == value2);
  ASSERT_FALSE(value1 != value2);
}

/// @test Given a `variant<Ts...>` where at least one `Ts` has a comparison operator which is declared but for which
/// instantiating it would be malformed, then the equality comparison operators for the variant are still validly
/// instantiable and behave as normal.
TEST(Variant, EqualityComparisonIsValidEvenIfRelationalComparisonIsNotValidlyInstantiable) {
  using variant_type = arene::base::variant<std::int32_t, testing::has_broken_less_than>;
  variant_type value1;
  variant_type value2;
  ASSERT_EQ(value1, value2);

  value1.emplace<std::int32_t>(42);
  value2 = value1;
  ASSERT_EQ(value1, value2);

  value2.emplace<std::int32_t>(43);
  ASSERT_NE(value1, value2);

  value1.emplace<testing::has_broken_less_than>(43);
  ASSERT_NE(value1, value2);

  value2 = value1;
  ASSERT_EQ(value1, value2);

  value1.emplace<testing::has_broken_less_than>(44);
  ASSERT_NE(value1, value2);
}

/// @test One `variant` compares less than another if the index of the stored type is less, or they have the same index,
/// and the stored value of the first compares less than the stored value of the second
TEST(Variant, CompareLessThan) {
  // Different index
  {
    using variant_type = arene::base::variant<std::int32_t, float, double, test_string>;
    variant_type const value1{static_cast<std::int32_t>(42)};
    variant_type const value2{42.0F};
    ASSERT_EQ(value1.index(), 0);
    ASSERT_EQ(value2.index(), 1);

    ASSERT_TRUE(value1 < value2);
    ASSERT_FALSE(value2 < value1);
  }
  // Same index, different value
  {
    using variant_type = arene::base::variant<std::int32_t, float, double, test_string>;

    {  // int32_t
      variant_type const value1{static_cast<std::int32_t>(41)};
      variant_type const value2{static_cast<std::int32_t>(43)};
      ASSERT_EQ(value1.index(), 0);
      ASSERT_EQ(value2.index(), 0);

      ASSERT_TRUE(value1 < value2);
      ASSERT_FALSE(value2 < value1);
    }

    {  // float
      variant_type const value1{42.6F};
      variant_type const value2{43.8F};
      ASSERT_EQ(value1.index(), 1);
      ASSERT_EQ(value2.index(), 1);

      ASSERT_TRUE(value1 < value2);
      ASSERT_FALSE(value2 < value1);
    }

    {  // double
      variant_type const value1{static_cast<double>(72.6)};
      variant_type const value2{static_cast<double>(93.8)};
      ASSERT_EQ(value1.index(), 2);
      ASSERT_EQ(value2.index(), 2);

      ASSERT_TRUE(value1 < value2);
      ASSERT_FALSE(value2 < value1);
    }

    {  // double
      variant_type const value1{"aloha"};
      variant_type const value2{"hello"};
      ASSERT_EQ(value1.index(), 3);
      ASSERT_EQ(value2.index(), 3);

      ASSERT_TRUE(value1 < value2);
      ASSERT_FALSE(value2 < value1);
    }
  }
}

/// @test One `variant` compares greater than another if the index of the stored type is greater, or they have the same
/// index,
/// and the stored value of the first compares greater than the stored value of the second
TEST(Variant, CompareGreaterThan) {
  // Different index
  {
    using variant_type = arene::base::variant<std::int32_t, float, double, test_string>;
    variant_type const value1{static_cast<std::int32_t>(42)};
    variant_type const value2{42.0F};
    ASSERT_EQ(value1.index(), 0);
    ASSERT_EQ(value2.index(), 1);

    ASSERT_FALSE(value1 > value2);
    ASSERT_TRUE(value2 > value1);
  }
  // Same index, different value
  {
    using variant_type = arene::base::variant<std::int32_t, float, double, test_string>;
    variant_type const value1{static_cast<std::int32_t>(41)};
    variant_type const value2{static_cast<std::int32_t>(43)};
    ASSERT_EQ(value1.index(), 0);
    ASSERT_EQ(value2.index(), 0);

    ASSERT_FALSE(value1 > value2);
    ASSERT_TRUE(value2 > value1);
  }
}

// This is a compile time check to ensure that the operation
// only accepts types stored in this variant.
// TEST(Variant, GetSetNonExistingType) {
//   arene::base::variant<std::int32_t, float, double, test_string> value;
//   value.emplace<char>('a');
// }

// Utility class to record the number of calls to the constructor
// and destructor.
// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables) explicit global tracker
class VariantDestructorTest : public ::testing::Test {
 public:
  struct counts_ctor_dtor {
   public:
    counts_ctor_dtor() { ++count_ctor; }
    ~counts_ctor_dtor() { ++count_dtor; }
    counts_ctor_dtor(counts_ctor_dtor const&) = default;
    counts_ctor_dtor(counts_ctor_dtor&&) = default;
    auto operator=(counts_ctor_dtor const&) -> counts_ctor_dtor& = default;
    auto operator=(counts_ctor_dtor&&) -> counts_ctor_dtor& = default;

    static std::int32_t count_ctor;
    static std::int32_t count_dtor;
  };
};

std::int32_t VariantDestructorTest::counts_ctor_dtor::count_ctor{0};
std::int32_t VariantDestructorTest::counts_ctor_dtor::count_dtor{0};
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables) explicit global tracker

/// @test `emplace` destroys the existing element, even if it is the same type
TEST_F(VariantDestructorTest, Destructor) {
  arene::base::variant<counts_ctor_dtor, std::int32_t, float> value;

  // variant is initialized to its first type
  ASSERT_EQ(counts_ctor_dtor::count_ctor, 1);
  ASSERT_EQ(counts_ctor_dtor::count_dtor, 0);

  counts_ctor_dtor const obj{};
  ASSERT_EQ(counts_ctor_dtor::count_ctor, 2);
  value.emplace<counts_ctor_dtor>(obj);
  ASSERT_EQ(counts_ctor_dtor::count_ctor, 2);
  ASSERT_EQ(counts_ctor_dtor::count_dtor, 1);  // Destructor is called once

  value.emplace<std::int32_t>(42);
  ASSERT_EQ(counts_ctor_dtor::count_ctor, 2);
  ASSERT_EQ(counts_ctor_dtor::count_dtor, 2);  // The copy is destroyed
                                               // by the util in variant
}

/// @test One of the element types of a `variant` can be another `variant`.
TEST(Variant, EmbeddedVariant) {
  using variant_type_1 = arene::base::variant<std::int32_t, float>;
  using variant_type_2 = arene::base::variant<variant_type_1, test_string>;

  variant_type_2 value;
  ASSERT_EQ(value.index(), 0);

  // emplace top level type to recursive variant
  value.emplace<variant_type_1>();
  ASSERT_EQ(value.index(), 0);

  auto view_v1 = value.get<variant_type_1>();
  ASSERT_EQ(view_v1.index(), 0);

  view_v1.emplace<std::int32_t>(42);
  ASSERT_EQ(view_v1.index(), 0);
  ASSERT_EQ(view_v1.get<std::int32_t>(), 42);

  view_v1.emplace<float>(42.0F);
  ASSERT_EQ(view_v1.index(), 1);
  ASSERT_EQ(view_v1.get<float>(), 42.0);

  // emplace top level type to string
  value.emplace<test_string>("Hello World");
  ASSERT_EQ(value.index(), 1);
  ASSERT_EQ(value.get<test_string>(), "Hello World");
}

/// @test One of the element types of a `variant` can be a `test_vector`
TEST(Variant, VectorVariant) {
  using str = test_string;
  using vector_str = test_vector<test_string>;
  using type = arene::base::variant<str, vector_str>;

  type value;
  ASSERT_EQ(value.index(), 0);

  value.emplace<1>(2U, "Hello");
  ASSERT_EQ(value.index(), 1);

  auto& vec = value.get<vector_str>();
  static_assert(std::is_same<std::remove_reference_t<decltype(vec)>, vector_str>::value, "Type is not V_STR");
  ASSERT_EQ(vec.size(), 2);
  ASSERT_EQ(vec[0], "Hello");
  ASSERT_EQ(vec[1], "Hello");

  auto& vec2 = value.get<vector_str>();
  vec2[0] = "Hi";
  ASSERT_EQ(vec[0], "Hi");
}

/// @test One of the element types of a `variant` can be a `test_map`
TEST(Variant, MapVariant) {
  using str = test_string;
  using map_str_int = test_map<test_string, std::int32_t>;
  using type = arene::base::variant<str, map_str_int>;

  type value;
  ASSERT_EQ(value.index(), 0);

  std::initializer_list<std::pair<test_string const, std::int32_t>> const data{{"Hello", 0}, {"World", 1}};
  auto& map = value.emplace<1>(data);

  static_assert(std::is_same<std::remove_reference_t<decltype(map)>, map_str_int>::value, "Type is not MAP_STR_INT");
  ASSERT_EQ(value.index(), 1);

  auto& map_get = value.get<map_str_int>();

  static_assert(
      std::is_same<std::remove_reference_t<decltype(map_get)>, map_str_int>::value,
      "Type is not MAP_STR_INT"
  );

  ASSERT_EQ(map_get.size(), 2);
  ASSERT_EQ(map_get["Hello"], 0);
  ASSERT_EQ(map_get["World"], 1);

  auto& map2 = value.get<map_str_int>();
  map2["Hello"] = 2;
  ASSERT_EQ(map["Hello"], 2);
}

/// @test One of the element types of a `variant` can be a `test_string`
TEST(Variant, String) {
  using variant_type = arene::base::variant<test_string, std::int32_t>;
  variant_type value;
  value.emplace<test_string>("Hello");

  variant_type const value2(std::move(value));
  ASSERT_TRUE(arene::base::holds_alternative<test_string>(value2));
  ASSERT_EQ(value2.get<test_string>(), "Hello");
}

/// @test A `monostate` can be used as the element type of a `variant`
TEST(Variant, monostate) {
  // The following line will not compile because arene::base::variant does not
  // support empty types. Use monostate instead.
  // arene::base::variant<> value;
  arene::base::variant<arene::base::monostate> const value;
  ASSERT_EQ(value.index(), 0);
}

/// @test A `variant` can have a type list with a `monostate` and another type
TEST(Variant, monostateAndOtherTypeCanCoexists) {
  arene::base::variant<arene::base::monostate, test_string> const value;
  ASSERT_EQ(value.index(), 0);

  arene::base::variant<arene::base::monostate, test_string> const string_value{"hello"};
  ASSERT_EQ(string_value.index(), 1);

  ASSERT_EQ(value, value);
  ASSERT_EQ(string_value, string_value);

  ASSERT_LE(value, value);
  ASSERT_LE(string_value, string_value);

  ASSERT_LE(value, string_value);
  ASSERT_NE(value, string_value);
}

/// @test `holds_alternative` returns `true` if and only if the variant holds a value of the specified type
TEST(Variant, HoldsAlternative) {
  using variant_type = arene::base::variant<std::int32_t, float>;

  variant_type value{static_cast<std::int32_t>(10)};
  ASSERT_EQ(value.index(), 0);
  ASSERT_TRUE(arene::base::holds_alternative<std::int32_t>(value));
  ASSERT_FALSE(arene::base::holds_alternative<float>(value));
  value.emplace<float>(10.0F);
  ASSERT_FALSE(arene::base::holds_alternative<std::int32_t>(value));
  ASSERT_TRUE(arene::base::holds_alternative<float>(value));
}

/// @test `get_if` returns a `const` pointer to the stored value from a `const` `variant` if the type is correct,
/// `nullptr` otherwise.
TEST(Variant, GetIfForConstVariant) {
  using variant_type = arene::base::variant<std::int32_t, float>;
  {
    variant_type const value{static_cast<std::int32_t>(10)};
    ASSERT_EQ(value.index(), 0);
    auto const* ptr_int = arene::base::get_if<0>(&value);
    static_assert(
        std::is_same<decltype(ptr_int), std::int32_t const*>::value,
        "Type of result is not std::int32_t const*"
    );
    ASSERT_EQ(*ptr_int, 10);

    // Get it as a different type.
    auto const* ptr_float = arene::base::get_if<1>(&value);
    static_assert(std::is_same<decltype(ptr_float), float const*>::value, "Type of result is not float const*");
    ASSERT_EQ(ptr_float, nullptr);
  }

  {
    variant_type const value{static_cast<std::int32_t>(10)};
    ASSERT_EQ(value.index(), 0);
    auto const* ptr_int = arene::base::get_if<std::int32_t>(&value);
    static_assert(
        std::is_same<decltype(ptr_int), std::int32_t const*>::value,
        "Type of result is not std::int32_t const*"
    );
    ASSERT_EQ(*ptr_int, 10);

    // Get it as a different type.
    auto const* ptr_float = arene::base::get_if<float>(&value);
    static_assert(std::is_same<decltype(ptr_float), float const*>::value, "Type of result is not float const*");
    ASSERT_EQ(ptr_float, nullptr);
  }
}

/// @test `get_if` returns a non-`const` pointer to the stored value from a non-`const` `variant` if the type is
/// correct, `nullptr` otherwise.
TEST(Variant, GetIfForNonConstVariant) {
  using variant_type = arene::base::variant<std::int32_t, float>;
  {
    variant_type value{static_cast<std::int32_t>(10)};
    ASSERT_EQ(value.index(), 0);
    auto* ptr_int = arene::base::get_if<0>(&value);
    static_assert(std::is_same<decltype(ptr_int), std::int32_t*>::value, "Type of result is not std::int32_t*");
    ASSERT_EQ(*ptr_int, 10);

    // Get it as a different type.
    auto* ptr_float = arene::base::get_if<1>(&value);
    static_assert(std::is_same<decltype(ptr_float), float*>::value, "Type of result is not float*");
    ASSERT_EQ(ptr_float, nullptr);
  }

  {
    variant_type value{static_cast<std::int32_t>(10)};
    ASSERT_EQ(value.index(), 0);
    auto* ptr_int = arene::base::get_if<std::int32_t>(&value);
    static_assert(std::is_same<decltype(ptr_int), std::int32_t*>::value, "Type of result is not std::int32_t*");
    ASSERT_EQ(*ptr_int, 10);

    // Get it as a different type.
    auto* ptr_float = arene::base::get_if<float>(&value);
    static_assert(std::is_same<decltype(ptr_float), float*>::value, "Type of result is not float*");
    ASSERT_EQ(ptr_float, nullptr);
  }
}

/// @test `get_if` returns a null pointer if the supplied pointer-to-variant is a null pointer
TEST(Variant, GetIfWithNull) {
  using variant_type = arene::base::variant<std::int32_t, float>;
  {
    auto const* ptr_int = arene::base::get_if<0>(static_cast<variant_type*>(nullptr));
    ASSERT_EQ(ptr_int, nullptr);
  }

  {
    auto const* ptr_int = arene::base::get_if<1>(static_cast<variant_type*>(nullptr));
    ASSERT_EQ(ptr_int, nullptr);
  }

  {
    auto const* ptr_int = arene::base::get_if<0>(static_cast<variant_type const*>(nullptr));
    ASSERT_EQ(ptr_int, nullptr);
  }

  {
    auto const* ptr_int = arene::base::get_if<1>(static_cast<variant_type const*>(nullptr));
    ASSERT_EQ(ptr_int, nullptr);
  }
}

/// @test `get_if` returns a non-`const` pointer to the stored value from a non-`const` `variant` if the type is
/// correct, `nullptr` otherwise.
TEST(Variant, GetIfWithWrongId) {
  using variant_type = arene::base::variant<std::int32_t, float>;
  {
    auto var = variant_type{static_cast<std::int32_t>(42)};
    auto const* correct = arene::base::get_if<0>(&var);
    ASSERT_NE(correct, nullptr);
    ASSERT_EQ(*correct, 42);
    auto const* missed = arene::base::get_if<1>(&var);
    ASSERT_EQ(missed, nullptr);
  }

  {
    auto var = variant_type{42.3F};
    auto const* correct = arene::base::get_if<1>(&var);
    ASSERT_NE(correct, nullptr);
    ASSERT_EQ(*correct, 42.3F);
    auto const* missed = arene::base::get_if<0>(&var);
    ASSERT_EQ(missed, nullptr);
  }

  {
    auto const var = variant_type{static_cast<std::int32_t>(42)};
    auto const* correct = arene::base::get_if<0>(&var);
    ASSERT_NE(correct, nullptr);
    ASSERT_EQ(*correct, 42);
    auto const* missed = arene::base::get_if<1>(&var);
    ASSERT_EQ(missed, nullptr);
  }

  {
    auto const var = variant_type{42.3F};
    auto const* correct = arene::base::get_if<1>(&var);
    ASSERT_NE(correct, nullptr);
    ASSERT_EQ(*correct, 42.3F);
    auto const* missed = arene::base::get_if<0>(&var);
    ASSERT_EQ(missed, nullptr);
  }
}

/// @test `variant` can be used as a key in `test_map`
TEST(Variant, AsMapKey) {
  using variant_type = arene::base::variant<int, float, person>;
  test_map<variant_type, test_string> map;
  map[42] = "42";
  map[43] = "43";
  map[44.0F] = "44.0";
  map[person{"45"}] = "45";
  ASSERT_EQ(map.size(), 4);
  ASSERT_EQ(map[42], "42");
  ASSERT_EQ(map[43], "43");
  ASSERT_EQ(map[44.0F], "44.0");
  ASSERT_EQ(map[person{"45"}], "45");
}

/// @test A `variant` can be moved; the result has the same type index as the source
TEST(VariantCoverage, MapKeysCanBeSafelyMoved) {
  using variant_type = arene::base::variant<int, float>;
  auto orig = variant_type{1};
  variant_type moved;
  moved = std::move(orig);
  ASSERT_EQ(moved.index(), 0);

  variant_type moved2{std::move(moved)};
  ASSERT_EQ(moved2.index(), 0);

  variant_type target{42.3F};
  target = std::move(moved2);
  ASSERT_EQ(target.index(), 0);
}

/// @test `emplace` with an index correctly stores the value with the correct index, even if all elements have the same
/// type
TEST(Variant, EmplaceNumberWillRespectPostConditionAndStoreProperIndexValue) {
  using variant_type = arene::base::variant<int, int, int>;
  auto var = variant_type{};
  ASSERT_EQ(var.index(), 0);
  ASSERT_EQ(var.get<0>(), 0);

  var.emplace<0>(10);
  ASSERT_EQ(var.index(), 0);
  ASSERT_EQ(var.get<0>(), 10);
  var.emplace<1>(20);
  ASSERT_EQ(var.index(), 1);
  ASSERT_EQ(var.get<1>(), 20);
  var.emplace<2>(30);
  ASSERT_EQ(var.index(), 2);
  ASSERT_EQ(var.get<2>(), 30);
}

/// @test Copy construction of a `variant` with multiple elements of the same type correctly preserves the index
TEST(Variant, AssignFromVariantWithSameTypesMustRememberIndex) {
  using variant_type = arene::base::variant<int, int, int>;
  auto var = variant_type{};

  var.emplace<2>(30);
  ASSERT_EQ(var.index(), 2);
  ASSERT_EQ(var.get<2>(), 30);

  auto var2 = variant_type{std::move(var)};
  ASSERT_EQ(var2.index(), 2);
  ASSERT_EQ(var2.get<2>(), 30);
}

/// @test Specifying an index for `emplace` when using an initializer list correctly initializes the value and stores
/// the right index, even if multiple elements have the same type
TEST(Variant, EmplaceWithInitializerListNumberWillRespectPostConditionAndStoreProperIndexValue) {
  using variant_type = arene::base::variant<test_string, test_string, test_string>;
  auto var = variant_type{};
  ASSERT_EQ(var.index(), 0);
  ASSERT_EQ(var.get<0>(), "");

  var.emplace<0>({'a', 'b', 'c'});
  ASSERT_EQ(var.index(), 0);
  ASSERT_EQ(var.get<0>(), "abc");
  var.emplace<1>({'d', 'e'});
  ASSERT_EQ(var.index(), 1);
  ASSERT_EQ(var.get<1>(), "de");
  var.emplace<2>({'x'});
  ASSERT_EQ(var.index(), 2);
  ASSERT_EQ(var.get<2>(), "x");

  auto var2 = std::move(var);
  ASSERT_EQ(var2.index(), 2);
  ASSERT_EQ(var2.get<2>(), "x");
}

}  // namespace
ARENE_IGNORE_END();
