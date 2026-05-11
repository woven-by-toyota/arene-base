// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/functional/invoke.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/detail/exceptions.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/make_tuple.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/reference_wrapper.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables) explicitly testing a global
unsigned increment_called = 0;
void nullary_function_that_increments_global() { ++increment_called; }

constexpr int simple_int_return = 42;
constexpr auto nullary_function_that_returns_int() -> int { return simple_int_return; }

constexpr float simple_float_return = 2.1F;
auto nullary_function_that_returns_float() -> float { return simple_float_return; }

struct some_class {
  int value;
};

constexpr some_class simple_class_return{123};

auto nullary_function_that_returns_some_class() -> some_class { return simple_class_return; }

template <typename... Ts>
using use_invoke = decltype(arene::base::invoke(std::declval<Ts>()...));

template <typename... Ts>
constexpr bool can_invoke = arene::base::substitution_succeeds<use_invoke, Ts...>;

/// @test `invoke` can call a simple function.
TEST(Invoke, InvokingSimpleFunctionCallsThatFunction) {
  increment_called = 0;
  arene::base::invoke(nullary_function_that_increments_global);
  ASSERT_EQ(increment_called, 1);
  ASSERT_TRUE(can_invoke<decltype(nullary_function_that_increments_global)>);
}

// IWYU pragma needed, possibly related to one of these issues:
// https://github.com/include-what-you-use/include-what-you-use/pull/1808
// https://github.com/include-what-you-use/include-what-you-use/issues/1831
// IWYU pragma: no_forward_declare some_string_class
class some_string_class {
  char const* str_;

 public:
  some_string_class() noexcept
      : str_("") {}
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  some_string_class(char const* source)
      : str_(source) {}

  friend auto operator==(some_string_class const& lhs, some_string_class const& rhs) {
    char const* lhs_ptr = lhs.c_str();
    char const* rhs_ptr = rhs.c_str();
    while ((*lhs_ptr != '\0') && (*lhs_ptr == *rhs_ptr)) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      ++lhs_ptr;
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      ++rhs_ptr;
    }
    return *lhs_ptr == *rhs_ptr;
  }

  auto c_str() const noexcept -> char const* { return str_; }
};

/// @test `invoke` cannot call functions with unmatching parameters.
TEST(Invoke, CannotInvokeFunctionWithWrongParameters) {
  ASSERT_FALSE((can_invoke<void (*)(), int>));
  ASSERT_FALSE((can_invoke<void (*)(some_string_class), int>));
  ASSERT_FALSE((can_invoke<void (*)(some_string_class)>));
  ASSERT_FALSE((can_invoke<void (*)(int, int), int>));
}

/// @test `invoke` call to a simple function returns the function's return value with the function's return type.
TEST(Invoke, InvokeOfSimpleFunctionPassesReturnValue) {
  static_assert(
      std::is_same<decltype(arene::base::invoke(nullary_function_that_increments_global)), void>::value,
      "Return type must match"
  );
  static_assert(
      std::is_same<decltype(arene::base::invoke(nullary_function_that_returns_int)), int>::value,
      "Return type must match"
  );
  static_assert(
      std::is_same<decltype(arene::base::invoke(nullary_function_that_returns_float)), float>::value,
      "Return type must match"
  );
  static_assert(
      std::is_same<decltype(arene::base::invoke(nullary_function_that_returns_some_class)), some_class>::value,
      "Return type must match"
  );

  ASSERT_EQ(arene::base::invoke(nullary_function_that_returns_int), simple_int_return);
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Testing for unmodified exact copy");
  ASSERT_EQ(arene::base::invoke(nullary_function_that_returns_float), simple_float_return);
  ARENE_IGNORE_END();
  ASSERT_EQ(arene::base::invoke(nullary_function_that_returns_some_class).value, simple_class_return.value);
}

/// @test `invoke` call to a simple lambda returns the lambda's return value with the lambda's return type.
TEST(Invoke, InvokeOfLambdaReturnsResult) {
  struct local {
    int i;
  };
  local lambda_result{simple_int_return};
  auto func = [&lambda_result]() -> local& { return lambda_result; };
  static_assert(std::is_same<decltype(arene::base::invoke(func)), local&>::value, "Must have correct return type");
  ASSERT_EQ(arene::base::invoke(func).i, simple_int_return);
}

/// @test `invoke` call to a lambda with arguments returns the lambda's return value with the lambda's return type.
TEST(Invoke, CanInvokeLambdaWithArguments) {
  int const first = 42;
  int const second = 99;
  struct local {
    int value;
  };
  auto func = [](int lhs, int rhs) { return local{lhs + rhs}; };
  static_assert(
      std::is_same<decltype(arene::base::invoke(func, first, second)), local>::value,
      "Must have correct return type"
  );
  ASSERT_EQ(arene::base::invoke(func, first, second).value, first + second);
}

/// @test `invoke` call to a function with arguments returns the function's return value with the function's return
/// type.
TEST(Invoke, CanInvokeFunctionWithArguments) {
  int const first = 42;
  int const second = 99;
  struct local {
    int value;

    static auto func(int lhs, int rhs) -> local { return local{lhs + rhs}; }
  };
  static_assert(
      std::is_same<decltype(arene::base::invoke(&local::func, first, second)), local>::value,
      "Must have correct return type"
  );
  ASSERT_EQ(arene::base::invoke(&local::func, first, second).value, first + second);
}

namespace {
// IWYU pragma: no_forward_declare category
enum class category { lvalue, rvalue, const_lvalue, const_rvalue };
}  // namespace

/// @test `invoke` calls the call operator preserving the callable _value category_ when passed a callable that
/// implements overloads for different _value categories_.
TEST(Invoke, ValueCategoryOfCallablePreserved) {
  struct callable {
    auto operator()() & -> category { return category::lvalue; }
    auto operator()() const& -> category { return category::const_lvalue; }
    auto operator()() && -> category { return category::rvalue; }
    auto operator()() const&& -> category { return category::const_rvalue; }
  };

  callable func;
  callable const cfunc{};
  ASSERT_EQ(arene::base::invoke(callable{}), category::rvalue);
  ASSERT_EQ(arene::base::invoke(func), category::lvalue);
  ASSERT_EQ(arene::base::invoke(cfunc), category::const_lvalue);
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  ASSERT_EQ(arene::base::invoke(std::move(func)), category::rvalue);
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  ASSERT_EQ(arene::base::invoke(std::move(cfunc)), category::const_rvalue);
}

/// @test `invoke` calls the call operator preserving the argument _value category_ when passed a callable that
/// implements overloads for different argument _value categories_.
TEST(Invoke, ValueCategoryOfArgsPreserved) {
  struct local {};
  struct callable {
    auto operator()(local&) -> category { return category::lvalue; }
    auto operator()(local const&) -> category { return category::const_lvalue; }
    auto operator()(local&&) -> category { return category::rvalue; }
    auto operator()(local const&&) -> category { return category::const_rvalue; }
  };

  local value;
  local const const_value{};

  ASSERT_EQ(arene::base::invoke(callable{}, local{}), category::rvalue);
  ASSERT_EQ(arene::base::invoke(callable{}, value), category::lvalue);
  ASSERT_EQ(arene::base::invoke(callable{}, const_value), category::const_lvalue);
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  ASSERT_EQ(arene::base::invoke(callable{}, std::move(value)), category::rvalue);
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  ASSERT_EQ(arene::base::invoke(callable{}, std::move(const_value)), category::const_rvalue);
}

/// @test `invoke` call to a pointer to member function returns the member function's return value with the member
/// function's return type.
TEST(Invoke, CanInvokePointerToMemberFunctionWithObject) {
  class local {
   public:
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto int_member() -> int { return simple_int_return; }
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto float_member() -> float { return simple_float_return; }
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto class_member() -> some_class { return simple_class_return; }
  };

  local value;  // NOLINT(misc-const-correctness) clang-tidy doesn't see the non-const invocations
  static_assert(
      std::is_same<decltype(arene::base::invoke(&local::int_member, value)), int>::value,
      "Correct return type"
  );
  static_assert(
      std::is_same<decltype(arene::base::invoke(&local::float_member, value)), float>::value,
      "Correct return type"
  );
  static_assert(
      std::is_same<decltype(arene::base::invoke(&local::class_member, value)), some_class>::value,
      "Correct return type"
  );

  ASSERT_EQ(arene::base::invoke(&local::int_member, value), simple_int_return);
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Testing for unmodified exact copy");
  ASSERT_EQ(arene::base::invoke(&local::float_member, value), simple_float_return);
  ARENE_IGNORE_END();
  ASSERT_EQ(arene::base::invoke(&local::class_member, value).value, simple_class_return.value);
}

/// @test `invoke` call to a pointer to member function with arguments returns the member function's return value with
/// the member function's return type.
TEST(Invoke, CanInvokePointerToMemberFunctionWithObjectAndArgs) {
  class local {
   public:
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto func(int first, some_string_class const& second, double third, some_class fourth)
        -> std::tuple<int, some_string_class, double, some_class> {
      return std::make_tuple(first, second, third, fourth);
    }
  };

  local instance;  // NOLINT(misc-const-correctness) clang-tidy doesn't see that `func` requires non-const invocation
  int const value1 = -42;
  some_string_class const value2 = "hello";
  double const value3 = 3.141;
  some_class const value4{4321};

  auto res = arene::base::invoke(&local::func, instance, value1, value2, value3, value4);
  ASSERT_EQ(std::get<0>(res), value1);
  ASSERT_EQ(std::get<1>(res), value2);
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Testing for unmodified exact copy");
  ASSERT_EQ(std::get<2>(res), value3);
  ARENE_IGNORE_END();
  ASSERT_EQ(std::get<3>(res).value, value4.value);
}

/// @test `invoke` calls the pointer to member function preserving the callable _value category_ when passed a callable
/// that implements overloads for different _value categories_.
TEST(Invoke, ValueCategoryComesMemberFunctionPointer) {
  struct local {
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto func() & -> category { return category::lvalue; }
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto func() const& -> category { return category::const_lvalue; }
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto func() && -> category { return category::rvalue; }
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto func() const&& -> category { return category::const_rvalue; }
  };

  local instance;  // NOLINT(misc-const-correctness)
  ASSERT_EQ(arene::base::invoke(static_cast<category (local::*)()&>(&local::func), instance), category::lvalue);
  ASSERT_EQ(
      arene::base::invoke(static_cast<category (local::*)() const&>(&local::func), instance),
      category::const_lvalue
  );
  ASSERT_EQ(arene::base::invoke(static_cast<category (local::*)() &&>(&local::func), local{}), category::rvalue);
  ASSERT_EQ(
      arene::base::invoke(static_cast<category (local::*)() const&&>(&local::func), local{}),
      category::const_rvalue
  );
}

/// @test `invoke` cannot call a pointer to member function that is _rvalue_ qualified when passed an _lvalue_ object.
TEST(Invoke, CannotInvokeRvalueQualifiedMemberFunctionPointerWithLValue) {
  struct local {
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto func() && -> category { return category::rvalue; }
  };

  ASSERT_FALSE((arene::base::is_invocable_v<decltype(&local::func), local&>));
  ASSERT_FALSE((can_invoke<decltype(&local::func), local&>));
}

/// @test `invoke` can call a pointer to member function when passed an object of a derived class, preserving the
/// object's _value category_.
TEST(Invoke, CanInvokeMemberFunctionPointerWithObjectOfDerivedClass) {
  struct local {
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto func() & -> category { return category::lvalue; }
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto func() const& -> category { return category::const_lvalue; }
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto func() && -> category { return category::rvalue; }
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto func() const&& -> category { return category::const_rvalue; }
  };

  class derived : public local {};

  derived instance;  // NOLINT(misc-const-correctness) clang-tidy doesn't see the non-const invocations here
  ASSERT_EQ(arene::base::invoke(static_cast<category (local::*)()&>(&local::func), instance), category::lvalue);
  ASSERT_EQ(
      arene::base::invoke(static_cast<category (local::*)() const&>(&local::func), instance),
      category::const_lvalue
  );
  ASSERT_EQ(arene::base::invoke(static_cast<category (local::*)() &&>(&local::func), derived{}), category::rvalue);
  ASSERT_EQ(
      arene::base::invoke(static_cast<category (local::*)() const&&>(&local::func), derived{}),
      category::const_rvalue
  );
}

/// @test `invoke` can call a pointer to member function when passed an object dereferencable to the object implementing
/// the member function, preserving the object's _value category_.
TEST(Invoke, CanInvokeMemberFunctionPointerWithDereferencableObject) {
  struct local {
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto func() & -> category { return category::lvalue; }
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto func() const& -> category { return category::const_lvalue; }
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto func() && -> category { return category::rvalue; }
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto func() const&& -> category { return category::const_rvalue; }
  };

  class other {
    local internal_;

   public:
    auto operator*() & -> local& { return internal_; }
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    auto operator*() && -> local&& { return std::move(internal_); }
  };

  other instance{};
  ASSERT_EQ(arene::base::invoke(static_cast<category (local::*)()&>(&local::func), instance), category::lvalue);
  ASSERT_EQ(
      arene::base::invoke(static_cast<category (local::*)() const&>(&local::func), instance),
      category::const_lvalue
  );
  ASSERT_EQ(arene::base::invoke(static_cast<category (local::*)() &&>(&local::func), other{}), category::rvalue);
  ASSERT_EQ(
      arene::base::invoke(static_cast<category (local::*)() const&&>(&local::func), other{}),
      category::const_rvalue
  );
}

/// @test `invoke` call to a pointer to member function with arguments returns the member function's return value with
/// the member function's return type, when passed an `std::ref` to the object implementing the member function.
TEST(Invoke, CanInvokeMemberFunctionPointerWithReferenceWrapperObject) {
  class local {
   public:
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto func(int first, some_string_class const& second, double third, some_class fourth)
        -> std::tuple<int, some_string_class, double, some_class> {
      return std::make_tuple(first, second, third, fourth);
    }
  };

  local instance;
  int const value1 = -42;
  some_string_class const value2 = "hello";
  double const value3 = 3.141;
  some_class const value4{4321};

  auto res = arene::base::invoke(&local::func, std::ref(instance), value1, value2, value3, value4);
  ASSERT_EQ(std::get<0>(res), value1);
  ASSERT_EQ(std::get<1>(res), value2);
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Testing for unmodified exact copy");
  ASSERT_EQ(std::get<2>(res), value3);
  ARENE_IGNORE_END();
  ASSERT_EQ(std::get<3>(res).value, value4.value);
}

/// @test `invoke` cannot call a pointer to member function with unmatching parameters.
TEST(Invoke, CannotInvokeMemberFunctionWithWrongArguments) {
  class local {
   public:
    void func(int, some_string_class const&, double, some_class) {}
  };

  ASSERT_FALSE((can_invoke<decltype(&local::func), local&, int, int, int, int>));
  ASSERT_FALSE((can_invoke<decltype(&local::func), local&, int, some_string_class, double, some_class, int>));
}

/// @test `invoke` can be called on a pointer to a data member of an object together with the object reference,
/// returning the data member.
TEST(Invoke, CanInvokePointerToDataMemberWithObject) {
  struct local {
    int data;
  };

  constexpr int data = 42;
  local const instance{data};

  static_assert(arene::base::is_invocable_v<decltype(&local::data), local&>, "Data member pointer is invocable");
  auto res = arene::base::invoke(&local::data, instance);
  ASSERT_EQ(res, data);
}

/// @test `invoke` cannot be called on a pointer to a data member of an object, with the object reference and additional
/// arguments.
TEST(Invoke, CannotInvokePointerToDataMemberWithObjectAndArgs) {
  struct local {
    int data;
  };

  static_assert(
      !arene::base::is_invocable_v<decltype(&local::data), local&, int>,
      "Data member pointer is not invocable"
  );
  static_assert(!can_invoke<decltype(&local::data), local&, int>, "Data member pointer is not invocable");
}

/// @test `invoke` can be called on a const pointer to a data member of an object together with the object reference,
/// returning a reference to the data member.
TEST(Invoke, CanInvokeConstPointerToDataMemberWithObject) {
  struct local {
    int data;
  };

  constexpr int data = 42;
  local instance{data};
  auto const ptr = &local::data;

  static_assert(arene::base::is_invocable_v<decltype(ptr), local&>, "Data member pointer is invocable");
  auto& res = arene::base::invoke(ptr, instance);
  ASSERT_EQ(res, data);
  ASSERT_EQ(&res, &instance.data);
  static_assert(std::is_same<decltype(res), int&>::value, "Pointer to non-const");
}

/// @test `invoke` can be called on a pointer to a data member of an object together with the object reference,
/// returning the data member with the same _value category_ as the object.
TEST(Invoke, ValueCategoryComesFromDataMemberPointer) {
  struct local {
    int data;
  };

  constexpr int val = -123;
  local instance{val};  // NOLINT(misc-const-correctness) explicitly want non-const lvalue here
  local const const_instance{val};
  ASSERT_EQ(arene::base::invoke(static_cast<int(local::*)>(&local::data), instance), val);
  static_assert(
      std::is_same<decltype(arene::base::invoke(static_cast<int(local::*)>(&local::data), instance)), int&>::value,
      "Must return lvalue reference"
  );
  ASSERT_EQ(arene::base::invoke(static_cast<int const(local::*)>(&local::data), instance), val);
  constexpr int val2 = 5678;
  ASSERT_EQ(arene::base::invoke(static_cast<int(local::*)>(&local::data), local{val2}), val2);
  static_assert(
      std::is_same<decltype(arene::base::invoke(static_cast<int(local::*)>(&local::data), local{val2})), int&&>::value,
      "Must return rvalue reference"
  );
  ASSERT_EQ(arene::base::invoke(static_cast<int const(local::*)>(&local::data), instance), val);
  static_assert(
      std::is_same<
          decltype(arene::base::invoke(static_cast<int const(local::*)>(&local::data), instance)),
          int const&>::value,
      "Must return lvalue reference"
  );
  ASSERT_FALSE((!can_invoke<int(local::*), local const&>));
  ASSERT_EQ(arene::base::invoke(static_cast<int(local::*)>(&local::data), const_instance), val);
  ASSERT_EQ(arene::base::invoke(static_cast<int const(local::*)>(&local::data), const_instance), val);
  static_assert(
      std::is_same<
          decltype(arene::base::invoke(static_cast<int const(local::*)>(&local::data), const_instance)),
          int const&>::value,
      "Must return lvalue reference"
  );
}

/// @test `invoke` can be called on a pointer to a data member of an object together with a derived object reference,
/// returning the data member.
TEST(Invoke, CanInvokeDataMemberPointerWithObjectOfDerivedClass) {
  constexpr int val = -999;
  struct local {
    int data = val;
  };

  class derived : public local {};

  derived const instance;
  ASSERT_EQ(arene::base::invoke(&local::data, instance), val);
}

/// @test `invoke` can be called on a pointer to a data member of an object together with an object dereferencable to
/// the object containing the data member, returning the data member and preserving the object's _value category_.
TEST(Invoke, CanInvokeDataMemberPointerWithDereferenceableObject) {
  constexpr int val = -999;
  struct local {
    int data = val;
  };

  class other {
    local internal_;

   public:
    auto operator*() & -> local& { return internal_; }
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    auto operator*() && -> local&& { return std::move(internal_); }

    other() = default;
    other(other const&) = delete;
    auto operator=(other const&) -> other& = delete;
    other(other const&&) = delete;
    auto operator=(other const&&) -> other& = delete;
    ~other() = default;
  };
  other instance;
  ASSERT_EQ(arene::base::invoke(&local::data, instance), val);
  static_assert(std::is_same<decltype(arene::base::invoke(&local::data, instance)), int&>::value, "Must be lvalue ref");
  ASSERT_EQ(&arene::base::invoke(&local::data, instance), &(*instance).data);
  static_assert(
      std::is_same<decltype(arene::base::invoke(&local::data, std::move(instance))), int&&>::value,
      "Must be rvalue ref"
  );
  ASSERT_EQ(arene::base::invoke(&local::data, std::move(instance)), val);
}

/// @test `invoke` can be called on a pointer to a data member of an object together with a reference wrapper to the
/// object, returning the data member and preserving the object's _value category_.
TEST(Invoke, CanInvokeDataMemberPointerWithReferenceWrapper) {
  struct local {
    int data;
  };

  constexpr int val = -123;
  local instance{val};
  auto ref_to_instance = std::ref(instance);
  auto const_ref_to_instance = std::cref(instance);
  ASSERT_EQ(arene::base::invoke(static_cast<int(local::*)>(&local::data), ref_to_instance), val);
  static_assert(
      std::is_same<decltype(arene::base::invoke(static_cast<int(local::*)>(&local::data), ref_to_instance)), int&>::
          value,
      "Must return lvalue reference"
  );
  ASSERT_EQ(arene::base::invoke(static_cast<int const(local::*)>(&local::data), ref_to_instance), val);
  constexpr int val2 = 5678;
  ASSERT_EQ(arene::base::invoke(static_cast<int(local::*)>(&local::data), local{val2}), val2);
  static_assert(
      std::is_same<decltype(arene::base::invoke(static_cast<int(local::*)>(&local::data), local{val2})), int&&>::value,
      "Must return rvalue reference"
  );
  ASSERT_EQ(arene::base::invoke(static_cast<int const(local::*)>(&local::data), ref_to_instance), val);
  static_assert(
      std::is_same<
          decltype(arene::base::invoke(static_cast<int const(local::*)>(&local::data), ref_to_instance)),
          int const&>::value,
      "Must return lvalue reference"
  );
  ASSERT_TRUE((can_invoke<int(local::*), decltype(const_ref_to_instance)&>));
  ASSERT_EQ(arene::base::invoke(static_cast<int(local::*)>(&local::data), const_ref_to_instance), val);
  ASSERT_EQ(arene::base::invoke(static_cast<int const(local::*)>(&local::data), const_ref_to_instance), val);
  static_assert(
      std::is_same<
          decltype(arene::base::invoke(static_cast<int const(local::*)>(&local::data), const_ref_to_instance)),
          int const&>::value,
      "Must return lvalue reference"
  );
}

/// @test `invoke` cannot be called on a pointer to a data member of an object, with a reference wrapper to the object
/// and additional arguments.
TEST(Invoke, CannotInvokePointerToDataMemberWithReferenceWrapperAndArgs) {
  struct local {
    int data;
  };

  static_assert(
      !arene::base::is_invocable_v<decltype(&local::data), std::reference_wrapper<local>, int>,
      "Data member pointer is not invocable"
  );
  static_assert(
      !can_invoke<decltype(&local::data), std::reference_wrapper<local>, int>,
      "Data member pointer is not invocable"
  );
}

/// @test `invoke` can be called in a `constexpr` context.
TEST(Invoke, CanInvokeInConstantExpression) {
  STATIC_ASSERT_EQ(arene::base::invoke(nullary_function_that_returns_int), simple_int_return);
  class local {
   public:
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    constexpr auto func() const -> int { return simple_int_return; }

    int data{simple_int_return};
  };
  static constexpr local instance{};
  STATIC_ASSERT_EQ(arene::base::invoke(&local::func, instance), simple_int_return);
  // C++14 std::reference_wrapper cannot be used in constexpr
  // static_assert(
  //     arene::base::invoke(&local::func, std::ref(instance)) == simple_int_return,
  //     "Can invoke in constexpr");
  STATIC_ASSERT_EQ(arene::base::invoke(&local::func, &instance), simple_int_return);
  STATIC_ASSERT_EQ(arene::base::invoke(&local::data, instance), simple_int_return);
  // C++14 std::reference_wrapper cannot be used in constexpr
  // static_assert(
  //     arene::base::invoke(&local::xdata, std::ref(instance)) == simple_int_return,
  //     "Can invoke in constexpr");
  STATIC_ASSERT_EQ(arene::base::invoke(&local::data, &instance), simple_int_return);
}

/// @test `invoke` is `noexcept` when the underlying calls are `noexcept`.
TEST(Invoke, InvokeIsNoexceptIfCallableIs) {
  class callable {
   public:
    void operator()() noexcept {}
    void operator()(int) noexcept {}
    void operator()(double) {}
    void operator()(some_string_class const&) noexcept {}

    int data;
  };

#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
  class local_error {};

  class throwing_dereferenceable {
   public:
    auto operator*() -> callable& {
      // NOLINTNEXTLINE(hicpp-exception-baseclass)
      throw local_error{};
    }
  };
#endif

  class nothrow_dereferenceable {
    callable data_;

   public:
    auto operator*() noexcept -> callable& { return data_; }
  };

  static_assert(noexcept(arene::base::invoke(callable{})), "Must be noexcept");
  EXPECT_NO_FATAL_FAILURE(arene::base::invoke(callable{}););
  static_assert(noexcept(arene::base::invoke(callable{}, 3)), "Must be noexcept");
  EXPECT_NO_FATAL_FAILURE(arene::base::invoke(callable{}, 3););
  static_assert(!noexcept(arene::base::invoke(callable{}, 2.1)), "Must not be noexcept");
  EXPECT_NO_FATAL_FAILURE(arene::base::invoke(callable{}, 2.1););
  some_string_class const value1;
  static_assert(noexcept(arene::base::invoke(callable{}, value1)), "Must be noexcept");
  EXPECT_NO_FATAL_FAILURE(arene::base::invoke(callable{}, value1););
  static_assert(!noexcept(arene::base::invoke(callable{}, "hello")), "Must not be noexcept");
  EXPECT_NO_FATAL_FAILURE(arene::base::invoke(callable{}, "hello"););
  static_assert(noexcept(arene::base::invoke(&callable::data, callable{})), "Must be noexcept");
  EXPECT_NO_FATAL_FAILURE(arene::base::invoke(&callable::data, callable{}););
  ;
  static_assert(noexcept(arene::base::invoke(&callable::data, nothrow_dereferenceable{})), "Must be noexcept");
  EXPECT_NO_FATAL_FAILURE(arene::base::invoke(&callable::data, nothrow_dereferenceable{}););

#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
  static_assert(!noexcept(arene::base::invoke(&callable::data, throwing_dereferenceable{})), "Must not be noexcept");
  ASSERT_THROW(arene::base::invoke(&callable::data, throwing_dereferenceable{}), local_error);
#endif

  auto nothrow_lambda = []() noexcept {};
  static_assert(noexcept(arene::base::invoke(nothrow_lambda)), "Must be noexcept");
  EXPECT_NO_FATAL_FAILURE(arene::base::invoke(nothrow_lambda););

#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)

  auto throwing_lambda = []() {
    // NOLINTNEXTLINE(hicpp-exception-baseclass)
    throw local_error();
  };
  static_assert(!noexcept(arene::base::invoke(throwing_lambda)), "Must not be noexcept");
  ASSERT_THROW(arene::base::invoke(throwing_lambda), local_error);
#endif

  class class_with_func {
   public:
    void nothrow() noexcept {}

#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    ARENE_NORETURN void throwing() {
      // NOLINTNEXTLINE(hicpp-exception-baseclass)
      throw local_error{};
    }
#endif
  };

  // C++14 doesn't preserve noexcept property of member functions in member
  // function pointer types
  // static_assert(
  //     noexcept(arene::base::invoke(&class_with_func::nothrow, class_with_func{})),
  //     "Must be noexcept");
  EXPECT_NO_FATAL_FAILURE(arene::base::invoke(&class_with_func::nothrow, class_with_func{}););

#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
  static_assert(!noexcept(arene::base::invoke(&class_with_func::throwing, class_with_func{})), "Must not be noexcept");
  ASSERT_THROW(arene::base::invoke(&class_with_func::throwing, class_with_func{}), local_error);
#endif
}

/// @test `invoke` with a callable implementing call operator overloads will call the appropriate overload when passed a
/// convertible type as argument.
TEST(Invoke, OverloadResolutionAndImplicitConversions) {
  class callable {
   public:
    auto operator()(std::uint32_t) -> std::uint32_t { return 0; }
    auto operator()(double) -> double { return 0; }
    auto operator()(some_string_class const&) -> some_string_class { return {}; }
  };

  class convertible {
   public:
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    operator std::uint32_t() { return {}; }
  };

  static_assert(
      std::is_same<decltype(arene::base::invoke(callable{}, convertible{})), std::uint32_t>::value,
      "convertible -> unsigned"
  );
  ASSERT_EQ(arene::base::invoke(callable{}, convertible{}), 0);
  static_assert(std::is_same<decltype(arene::base::invoke(callable{}, 0.F)), double>::value, "float -> double");
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Testing for unmodified exact copy");
  ASSERT_EQ(arene::base::invoke(callable{}, 0.F), 0);
  ARENE_IGNORE_END();
  static_assert(
      std::is_same<decltype(arene::base::invoke(callable{}, "hello")), some_string_class>::value,
      "const char* -> some_string_class"
  );
  ASSERT_EQ(arene::base::invoke(callable{}, "hello"), "");
}

/// @test `invoke` can call a non-movable callable.
TEST(Invoke, CanInvokeANonMovableCallable) {
  class callable {
   public:
    auto operator()() const -> int { return simple_int_return; }

    callable() = default;
    ~callable() = default;
    callable(callable const&) = delete;
    callable(callable&&) = delete;
    auto operator=(callable const&) -> callable& = delete;
    auto operator=(callable&&) -> callable& = delete;
  };

  callable const instance;

  ASSERT_EQ(arene::base::invoke(instance), simple_int_return);
  ASSERT_EQ(arene::base::invoke(callable{}), simple_int_return);
}

/// @test `invoke` can call a lambda taking non-movable arguments.
TEST(Invoke, CanPassNonMovableArguments) {
  class non_movable {
    int data_;

   public:
    explicit non_movable(int data)
        : data_(data) {}
    auto get_data() const noexcept -> int { return data_; }

    ~non_movable() = default;
    non_movable(non_movable const&) = delete;
    non_movable(non_movable&&) = delete;
    auto operator=(non_movable const&) -> non_movable& = delete;
    auto operator=(non_movable&&) -> non_movable& = delete;
  };

  non_movable instance{simple_int_return};

  ASSERT_EQ(arene::base::invoke([](non_movable& arg) { return arg.get_data(); }, instance), simple_int_return);

  ASSERT_EQ(
      arene::base::invoke([](non_movable&& arg) { return arg.get_data(); }, non_movable{simple_int_return * 2}),
      simple_int_return * 2
  );
}

}  // namespace
