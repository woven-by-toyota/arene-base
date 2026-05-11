// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/functional"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

// NOLINTBEGIN(readability-make-member-function-const) Functions are non-const for testing
// NOLINTBEGIN(hicpp-move-const-arg) Move is necessary for testing
// NOLINTBEGIN(misc-const-correctness) Variables are non-const for testing
// NOLINTBEGIN(readability-convert-member-functions-to-static) Member functions non-static for testing

////////////////////////////////////////
// Reference Wrap Object Types
////////////////////////////////////////

template <typename T>
class ObjectReferenceWrapperTests : public testing::Test {};

TYPED_TEST_SUITE(ObjectReferenceWrapperTests, testing::object_types, );

/// @brief Check getting, casting, type, and constructibility of a
/// reference_wrapped type
/// @tparam T The type of the object that is wrapped
/// @tparam WrappedType The type expected to be wrapped by @c Wrapper. Defaults to @c T
/// @param obj The wrapped object
/// @param ref_wrap The reference wrapper
template <typename T, typename WrappedType = T>
void check_wrapper(T const& obj, std::reference_wrapper<WrappedType> const& ref_wrap) {
  WrappedType& converted = ref_wrap;
  ASSERT_EQ(&obj, &ref_wrap.get());
  ASSERT_EQ(&obj, &converted);
  ASSERT_EQ(&obj, &static_cast<WrappedType&>(ref_wrap));
  ASSERT_TRUE(noexcept(ref_wrap.get()));
  ASSERT_TRUE(std::is_same_v<WrappedType, typename std::reference_wrapper<WrappedType>::type>);
  ASSERT_TRUE(std::is_nothrow_constructible_v<std::reference_wrapper<WrappedType>, T&>);
}

/// @test Basic construction of @c reference_wrapper
TYPED_TEST(ObjectReferenceWrapperTests, BasicConstructionTest) {
  TypeParam obj{};
  auto const ref_wrap = std::reference_wrapper<TypeParam>(obj);
  check_wrapper(obj, ref_wrap);

  std::reference_wrapper<TypeParam> const ref_wrap_copy = obj;
  ASSERT_EQ(&ref_wrap.get(), &ref_wrap_copy.get());
  ASSERT_TRUE(std::is_same_v<typename decltype(ref_wrap)::type, typename decltype(ref_wrap_copy)::type>);
}

/// @test @c ref will provide a @c reference_wrapper around a given object
TYPED_TEST(ObjectReferenceWrapperTests, BasicObjectWrapTest) {
  TypeParam obj{};
  auto const ref_wrap = std::ref(obj);
  check_wrapper(obj, ref_wrap);

  auto const ref_wrap_copy = std::ref(ref_wrap);
  ASSERT_EQ(&ref_wrap.get(), &ref_wrap_copy.get());
  ASSERT_TRUE(std::is_same_v<typename decltype(ref_wrap)::type, typename decltype(ref_wrap_copy)::type>);

  TypeParam obj_extra{};
  auto ref_wrap_assign = std::ref(obj_extra);
  ASSERT_NE(&ref_wrap.get(), &ref_wrap_assign.get());
  ref_wrap_assign = ref_wrap;
  ASSERT_EQ(&ref_wrap.get(), &ref_wrap_assign.get());
}

/// @test @c cref will provide a @c reference_wrapper around a given object
/// with added @c const qualification
TYPED_TEST(ObjectReferenceWrapperTests, BasicConstObjectWrapTest) {
  TypeParam obj{};
  auto const ref_wrap = std::cref(obj);
  check_wrapper<TypeParam, TypeParam const>(obj, ref_wrap);

  auto const ref_wrap_copy = std::ref(ref_wrap);
  ASSERT_EQ(&ref_wrap.get(), &ref_wrap_copy.get());
  ASSERT_TRUE(std::is_same_v<typename decltype(ref_wrap)::type, typename decltype(ref_wrap_copy)::type>);

  TypeParam obj_extra{};
  auto ref_wrap_assign = std::cref(obj_extra);
  ASSERT_NE(&ref_wrap.get(), &ref_wrap_assign.get());
  ref_wrap_assign = ref_wrap;
  ASSERT_EQ(&ref_wrap.get(), &ref_wrap_assign.get());
}

/// @test Cannot construct with an rvalue reference
TYPED_TEST(ObjectReferenceWrapperTests, RValueReferenceTest) {
  static_assert(
      !std::is_constructible_v<std::reference_wrapper<TypeParam>, TypeParam&&>,
      "Must not be constructible from rvalue reference"
  );
  static_assert(
      std::is_convertible_v<TypeParam&, std::reference_wrapper<TypeParam>>,
      "Must be convertable to lvalue reference"
  );

  auto const ref = FUNCTION_LIFT(std::ref);
  auto const cref = FUNCTION_LIFT(std::cref);
  ASSERT_FALSE(testing::simple_is_invocable_v<decltype(ref), TypeParam&&>);
  ASSERT_FALSE(testing::simple_is_invocable_v<decltype(cref), TypeParam&&>);
}

////////////////////////////////////////
// Reference Wrap Callable Types
////////////////////////////////////////

/// @brief An unused type to check invocability
struct dummy_type {};

/// @brief Do all of @c check_wrapper but also invoking and noexcept invocability
/// @tparam T The type of the object that is wrapped
/// @tparam Wrapper The type of the wrapper
/// @param function The wrapped callable
/// @param ref_wrap The reference wrapper
template <typename T, typename Wrapper>
void check_function(T const& function, Wrapper const& ref_wrap) {
  check_wrapper(function, ref_wrap);
  ASSERT_EQ(ref_wrap(2, 7), 9);

  ASSERT_TRUE(testing::simple_is_invocable_v<Wrapper, int, int>);
  ASSERT_FALSE(testing::simple_is_invocable_v<Wrapper, int>);
  ASSERT_FALSE(testing::simple_is_invocable_v<Wrapper, int, int, int>);
  ASSERT_FALSE(testing::simple_is_invocable_v<Wrapper, dummy_type, dummy_type>);
  ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<T, int, int>),
      (arene::base::is_nothrow_invocable_v<Wrapper, int, int>)
  );
}

/// @test Can wrap a lambda instance and call the underlying function
TEST(CallableReferenceWrapperTests, LambdaWrapTest) {
  auto lambda = [](int first, int second) -> int { return first + second; };
  auto const ref_wrap = std::ref(lambda);
  check_function(lambda, ref_wrap);
}

/// @test Can wrap a noexcept lambda instance and call the underlying function
TEST(CallableReferenceWrapperTests, NoexceptLambdaWrapTest) {
  auto lambda = [](int first, int second) noexcept -> int { return first + second; };
  auto const ref_wrap = std::ref(lambda);
  check_function(lambda, ref_wrap);
}

auto add_ints(int first, int second) -> int { return first + second; }

/// @test Can wrap a function and call it
TEST(CallableReferenceWrapperTests, FunctionWrapTest) {
  auto const ref_wrap = std::ref(add_ints);
  check_function(add_ints, ref_wrap);
}

/// @test Can wrap a function reference and call it
TEST(CallableReferenceWrapperTests, FunctionReferenceWrapTest) {
  int (&fun_ref)(int, int) = add_ints;
  auto const ref_wrap = std::ref(fun_ref);
  check_function(fun_ref, ref_wrap);
}

/// @test Can wrap a function pointer and call it
TEST(CallableReferenceWrapperTests, FunctionPointerWrapTest) {
  int (*fun_ptr)(int, int) = add_ints;
  auto const ref_wrap = std::ref(fun_ptr);
  check_function(fun_ptr, ref_wrap);
}

auto add_ints_noexcept(int first, int second) noexcept -> int { return first + second; }

/// @test Can wrap a noexcept function and call it
TEST(CallableReferenceWrapperTests, NoexceptFunctionWrapTest) {
  auto const ref_wrap = std::ref(add_ints_noexcept);
  check_function(add_ints_noexcept, ref_wrap);
}

/// @test Can wrap a noexcept function reference and call it
TEST(CallableReferenceWrapperTests, NoexceptFunctionReferenceWrapTest) {
  int (&fun_ref)(int, int) = add_ints_noexcept;
  auto const ref_wrap = std::ref(fun_ref);
  check_function(fun_ref, ref_wrap);
}

/// @test Can wrap a noexcept function pointer and call it
TEST(CallableReferenceWrapperTests, NoexceptFunctionPointerWrapTest) {
  int (*fun_ptr)(int, int) = add_ints_noexcept;
  auto const ref_wrap = std::ref(fun_ptr);
  check_function(fun_ptr, ref_wrap);
}

class with_callable {
 public:
  auto operator()(int first, int second) const -> int { return first + second; }
};

/// @test Can wrap a function object and call it
TEST(CallableReferenceWrapperTests, CallableWrapTest) {
  with_callable obj{};
  auto const ref_wrap = std::ref(obj);
  check_function(obj, ref_wrap);
}

class with_callable_noexcept {
 public:
  auto operator()(int first, int second) const noexcept -> int { return first + second; }
};

/// @test Can wrap a function object with noexcept call operator and call it
TEST(CallableReferenceWrapperTests, NoexceptCallableWrapTest) {
  with_callable_noexcept obj{};
  auto const ref_wrap = std::ref(obj);
  check_function(obj, ref_wrap);
}

////////////////////////////////////////
// Reference Wrap Member Pointers
////////////////////////////////////////

/// @brief Do all of @c check_wrapper but also invoking and noexcept invocability
/// on a member function pointer
/// @tparam T The type of the pointer that is wrapped
/// @tparam Object The type of the object that @c T is pointing to a member function of
/// @tparam BadObject Version of @c Object that does not match the expected qualifiers
/// @tparam Wrapper The type of the wrapper
/// @param fun_ptr The wrapped member function pointer
/// @param obj An object that @c fun_ptr can be called with
/// @param ref_wrap The reference wrapper
template <typename T, typename Object, typename BadObject, typename Wrapper>
void check_member_function_ptr(T const& fun_ptr, Object&& obj, BadObject&&, Wrapper const& ref_wrap) {
  check_wrapper(fun_ptr, ref_wrap);

  ASSERT_TRUE(testing::simple_is_invocable_v<Wrapper, Object, int, int>);
  ASSERT_FALSE(testing::simple_is_invocable_v<Wrapper, BadObject, int, int>);
  ASSERT_FALSE(testing::simple_is_invocable_v<Wrapper, Object, int>);
  ASSERT_FALSE(testing::simple_is_invocable_v<Wrapper, Object, int, int, int>);
  ASSERT_FALSE(testing::simple_is_invocable_v<Wrapper, Object, dummy_type, dummy_type>);
  ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<T, Object, int, int>),
      (arene::base::is_nothrow_invocable_v<Wrapper, Object, int, int>)
  );
  ASSERT_EQ(ref_wrap(std::forward<Object>(obj), 2, 7), 14);
}

/// @brief Do all of @c check_wrapper but also invoking and noexcept invocability
/// on a member function pointer
/// @tparam T The type of the pointer that is wrapped
/// @tparam Object The type of the object that @c T is pointing to a member function of
/// @tparam BadObject1 Version of @c Object that does not match the expected qualifiers
/// @tparam BadObject2 Version of @c Object that does not match the expected qualifiers
/// @tparam Wrapper The type of the wrapper
/// @param fun_ptr The wrapped member function pointer
/// @param obj An object that @c ptr can be called with
/// @param bad_obj An object that @c fun_ptr cannot be called with
/// @param ref_wrap The reference wrapper
template <typename T, typename Object, typename BadObject1, typename BadObject2, typename Wrapper>
void check_member_function_ptr(
    T const& fun_ptr,
    Object&& obj,
    BadObject1&& bad_obj,
    BadObject2&&,
    Wrapper const& ref_wrap
) {
  ASSERT_FALSE(testing::simple_is_invocable_v<Wrapper, BadObject2, int, int>);
  check_member_function_ptr(fun_ptr, std::forward<Object>(obj), std::forward<BadObject1>(bad_obj), ref_wrap);
}

class with_function {
 public:
  int i{5};

  auto add_ints(int first, int second) -> int { return first + second + i; }
  auto const_add_ints(int first, int second) const -> int { return first + second + i; }
};

/// @test Can wrap a function member pointer and call it
TEST(MemberPointerReferenceWrapperTests, FunctionMemberPointerWrapTest) {
  auto fun_ptr = &with_function::add_ints;
  auto const ref_wrap = std::ref(fun_ptr);
  with_function obj{};
  with_function const const_obj{};
  with_function volatile volatile_obj{};
  check_member_function_ptr(fun_ptr, obj, const_obj, volatile_obj, ref_wrap);
}

/// @test Can wrap a const function member pointer and call it
TEST(MemberPointerReferenceWrapperTests, ConstFunctionMemberPointerWrapTest) {
  auto fun_ptr = &with_function::const_add_ints;
  auto const ref_wrap = std::ref(fun_ptr);
  with_function const obj{};
  with_function volatile bad_obj{};
  check_member_function_ptr(fun_ptr, obj, bad_obj, ref_wrap);
}

class with_function_noexcept {
 public:
  int i{5};

  auto add_ints(int first, int second) noexcept -> int { return first + second + i; }
  auto const_add_ints(int first, int second) const noexcept -> int { return first + second + i; }
};

/// @test Can wrap a noexcept function member pointer and call it
TEST(MemberPointerReferenceWrapperTests, NoexceptFunctionMemberPointerWrapTest) {
  auto fun_ptr = &with_function_noexcept::add_ints;
  auto const ref_wrap = std::ref(fun_ptr);
  with_function_noexcept obj{};
  with_function_noexcept const const_obj{};
  with_function_noexcept volatile volatile_obj{};
  check_member_function_ptr(fun_ptr, obj, const_obj, volatile_obj, ref_wrap);
}

/// @test Can wrap a const noexcept function member pointer and call it
TEST(MemberPointerReferenceWrapperTests, ConstNoexceptFunctionMemberPointerWrapTest) {
  auto fun_ptr = &with_function_noexcept::const_add_ints;
  auto const ref_wrap = std::ref(fun_ptr);
  with_function_noexcept const obj{};
  with_function_noexcept volatile bad_obj{};
  check_member_function_ptr(fun_ptr, obj, bad_obj, ref_wrap);
}

class with_function_lvalue {
 public:
  int i{5};

  auto add_ints(int first, int second) & -> int { return first + second + i; }
  auto const_add_ints(int first, int second) const& -> int { return first + second + i; }
};

/// @test Can wrap an lvalue function member pointer and call it
TEST(MemberPointerReferenceWrapperTests, LvalueFunctionMemberPointerWrapTest) {
  auto fun_ptr = &with_function_lvalue::add_ints;
  auto const ref_wrap = std::ref(fun_ptr);
  with_function_lvalue obj{};
  with_function_lvalue mv_obj{};
  with_function_lvalue const const_obj{};
  check_member_function_ptr(fun_ptr, obj, const_obj, std::move(mv_obj), ref_wrap);
}

/// @test Can wrap a const lvalue function member pointer and call it
TEST(MemberPointerReferenceWrapperTests, ConstLvalueFunctionMemberPointerWrapTest) {
  auto fun_ptr = &with_function_lvalue::const_add_ints;
  auto const ref_wrap = std::ref(fun_ptr);
  with_function_lvalue const obj{};
  with_function_lvalue const mv_obj{};
  check_member_function_ptr(fun_ptr, obj, std::move(mv_obj), ref_wrap);
}

class with_function_rvalue {
 public:
  int i{5};

  auto add_ints(int first, int second) && -> int { return first + second + i; }
  auto const_add_ints(int first, int second) const&& -> int { return first + second + i; }
};

/// @test Can wrap an rvalue function member pointer and call it
TEST(MemberPointerReferenceWrapperTests, RvalueFunctionMemberPointerWrapTest) {
  auto fun_ptr = &with_function_rvalue::add_ints;
  auto const ref_wrap = std::ref(fun_ptr);
  with_function_rvalue obj1{};
  with_function_rvalue obj2{};
  with_function_rvalue const const_obj{};
  check_member_function_ptr(fun_ptr, std::move(obj1), obj2, std::move(const_obj), ref_wrap);
}

/// @test Can wrap a const rvalue function member pointer and call it
TEST(MemberPointerReferenceWrapperTests, ConstRvalueFunctionMemberPointerWrapTest) {
  auto fun_ptr = &with_function_rvalue::const_add_ints;
  auto const ref_wrap = std::ref(fun_ptr);
  with_function_rvalue const obj1{};
  with_function_rvalue const obj2{};
  check_member_function_ptr(fun_ptr, std::move(obj1), obj2, ref_wrap);
}

struct with_int_member {
  int value{};
};

/// @test Check return type of invoked reference
TEST(MemberPointerReferenceWrapperTests, InvokeReturnType) {
  auto const value_ptr = &with_int_member::value;
  auto const ref_wrap = std::cref(value_ptr);

  static_assert(
      std::is_same_v<int&, decltype(ref_wrap(std::declval<with_int_member&>()))>,
      "Must return correct type on invoke"
  );
  static_assert(
      std::is_same_v<int const&, decltype(ref_wrap(std::declval<with_int_member const&>()))>,
      "Must return correct type on invoke"
  );
  static_assert(
      std::is_same_v<int&&, decltype(ref_wrap(std::declval<with_int_member&&>()))>,
      "Must return correct type on invoke"
  );
  static_assert(
      std::is_same_v<int const&&, decltype(ref_wrap(std::declval<with_int_member const&&>()))>,
      "Must return correct type on invoke"
  );
}

////////////////////////////////////////
// Member Type Checks
////////////////////////////////////////

template <class T, class = void>
struct has_result_type : std::false_type {};
template <class T>
struct has_result_type<T, std::void_t<typename T::result_type>> : std::true_type {};
template <class T>
constexpr auto has_result_type_v = has_result_type<T>::value;

template <class T, class = void>
struct has_argument_type : std::false_type {};
template <class T>
struct has_argument_type<T, std::void_t<typename T::argument_type>> : std::true_type {};
template <class T>
constexpr auto has_argument_type_v = has_argument_type<T>::value;

template <class T, class = void>
struct has_first_argument_type : std::false_type {};
template <class T>
struct has_first_argument_type<T, std::void_t<typename T::first_argument_type>> : std::true_type {};
template <class T>
constexpr auto has_first_argument_type_v = has_first_argument_type<T>::value;

template <class T, class = void>
struct has_second_argument_type : std::false_type {};
template <class T>
struct has_second_argument_type<T, std::void_t<typename T::second_argument_type>> : std::true_type {};
template <class T>
constexpr auto has_second_argument_type_v = has_second_argument_type<T>::value;

////////////////////////////////////////
// Wrapped Function Member Types
////////////////////////////////////////

/// @brief Check member types of a reference_wrapped function with no arguments
/// Also relevant for more than two arguments as no argument types are set in
/// that case
/// @tparam Wrapper Type of the wrapper
template <typename Wrapper>
void check_no_arg_fun() {
  ASSERT_TRUE(has_result_type_v<Wrapper>);
  ASSERT_FALSE(has_argument_type_v<Wrapper>);
  ASSERT_FALSE(has_first_argument_type_v<Wrapper>);
  ASSERT_FALSE(has_second_argument_type_v<Wrapper>);

  ASSERT_TRUE(std::is_same_v<typename Wrapper::result_type, int>);
}

/// @brief Check member types of a reference_wrapped function with one argument
/// @tparam Wrapper Type of the wrapper
template <typename Wrapper>
void check_one_arg_fun() {
  ASSERT_TRUE(has_result_type_v<Wrapper>);
  ASSERT_TRUE(has_argument_type_v<Wrapper>);
  ASSERT_FALSE(has_first_argument_type_v<Wrapper>);
  ASSERT_FALSE(has_second_argument_type_v<Wrapper>);

  ASSERT_TRUE(std::is_same_v<typename Wrapper::result_type, int>);
  ASSERT_TRUE(std::is_same_v<typename Wrapper::argument_type, char>);
}

/// @brief Check member types of a reference_wrapped function with two arguments
/// @tparam Wrapper Type of the wrapper
template <typename Wrapper>
void check_two_arg_fun() {
  ASSERT_TRUE(has_result_type_v<Wrapper>);
  ASSERT_FALSE(has_argument_type_v<Wrapper>);
  ASSERT_TRUE(has_first_argument_type_v<Wrapper>);
  ASSERT_TRUE(has_second_argument_type_v<Wrapper>);

  ASSERT_TRUE(std::is_same_v<typename Wrapper::result_type, int>);
  ASSERT_TRUE(std::is_same_v<typename Wrapper::first_argument_type, char>);
  ASSERT_TRUE(std::is_same_v<typename Wrapper::second_argument_type, double>);
}

/// @test Check member types of a wrapped function with no arguments
TEST(FunctionMemberTypesReferenceWrapperTests, FunctionNoArgTypesTest) {
  check_no_arg_fun<std::reference_wrapper<auto()->int>>();
  check_no_arg_fun<std::reference_wrapper<auto (*)()->int>>();
}

/// @test Check member types of a wrapped function with one argument
TEST(FunctionMemberTypesReferenceWrapperTests, FunctionOneArgTypesTest) {
  check_one_arg_fun<std::reference_wrapper<auto(char)->int>>();
  check_one_arg_fun<std::reference_wrapper<auto (*)(char)->int>>();
}

/// @test Check member types of a wrapped function with two arguments
TEST(FunctionMemberTypesReferenceWrapperTests, FunctionTwoArgTypesTest) {
  check_two_arg_fun<std::reference_wrapper<auto(char, double)->int>>();
  check_two_arg_fun<std::reference_wrapper<auto (*)(char, double)->int>>();
}

/// @test Check member types of a wrapped function with three arguments
TEST(FunctionMemberTypesReferenceWrapperTests, FunctionThreeArgTypesTest) {
  check_no_arg_fun<std::reference_wrapper<auto(char, double, int)->int>>();
  check_no_arg_fun<std::reference_wrapper<auto (*)(char, double, int)->int>>();
}

/// @test Check member types of a wrapped noexcept function
TEST(FunctionMemberTypesReferenceWrapperTests, NoexceptFunctionTypesTest) {
  check_one_arg_fun<std::reference_wrapper<auto(char) noexcept -> int>>();
  check_one_arg_fun<std::reference_wrapper<auto (*)(char) noexcept -> int>>();
}

////////////////////////////////////////
// Wrapped Member Function Pointer Member Types
////////////////////////////////////////

/// @brief Check member types of a reference_wrapped member function pointer
/// with no arguments
/// @tparam Wrapper Type of the wrapper
/// @tparam MemClass Type of the class the function is a member of
template <typename Wrapper, typename MemClass>
void check_no_arg_mem_ptr() {
  ASSERT_TRUE(has_result_type_v<Wrapper>);
  ASSERT_TRUE(has_argument_type_v<Wrapper>);
  ASSERT_FALSE(has_first_argument_type_v<Wrapper>);
  ASSERT_FALSE(has_second_argument_type_v<Wrapper>);

  ASSERT_TRUE(std::is_same_v<typename Wrapper::result_type, int>);
  ASSERT_TRUE(std::is_same_v<typename Wrapper::argument_type, MemClass>);
}

/// @brief Check member types of a reference_wrapped member function pointer
/// with one argument
/// @tparam Wrapper Type of the wrapper
/// @tparam MemClass Type of the class the function is a member of
template <typename Wrapper, typename MemClass>
void check_one_arg_mem_ptr() {
  ASSERT_TRUE(has_result_type_v<Wrapper>);
  ASSERT_FALSE(has_argument_type_v<Wrapper>);
  ASSERT_TRUE(has_first_argument_type_v<Wrapper>);
  ASSERT_TRUE(has_second_argument_type_v<Wrapper>);

  ASSERT_TRUE(std::is_same_v<typename Wrapper::result_type, int>);
  ASSERT_TRUE(std::is_same_v<typename Wrapper::first_argument_type, MemClass>);
  ASSERT_TRUE(std::is_same_v<typename Wrapper::second_argument_type, char>);
}

/// @brief Check member types of a reference_wrapped member function pointer
/// with two arguments
/// @tparam Wrapper Type of the wrapper
/// @tparam MemClass Type of the class the function is a member of
template <typename Wrapper, typename MemClass>
void check_two_arg_mem_ptr() {
  ASSERT_TRUE(has_result_type_v<Wrapper>);
  ASSERT_FALSE(has_argument_type_v<Wrapper>);
  ASSERT_FALSE(has_first_argument_type_v<Wrapper>);
  ASSERT_FALSE(has_second_argument_type_v<Wrapper>);

  ASSERT_TRUE(std::is_same_v<typename Wrapper::result_type, int>);
}

/// @test Check member types of a wrapped member function pointer with no arguments
TEST(PointerMemberTypesReferenceWrapperTests, MemberFunctionPointerNoArgTypesTest) {
  class mem_fun_class {};

  check_no_arg_mem_ptr<std::reference_wrapper<auto (mem_fun_class::*)()->int>, mem_fun_class*>();
  check_no_arg_mem_ptr<std::reference_wrapper<auto (mem_fun_class::*)() const->int>, mem_fun_class const*>();
  check_no_arg_mem_ptr<std::reference_wrapper<auto (mem_fun_class::*)() volatile->int>, mem_fun_class volatile*>();
  check_no_arg_mem_ptr<
      std::reference_wrapper<auto (mem_fun_class::*)() const volatile->int>,
      mem_fun_class const volatile*>();
}

/// @test Check member types of a wrapped member function pointer with one argument
TEST(PointerMemberTypesReferenceWrapperTests, MemberFunctionPointerOneArgTypesTest) {
  class mem_fun_class {};

  check_one_arg_mem_ptr<std::reference_wrapper<auto (mem_fun_class::*)(char)->int>, mem_fun_class*>();
  check_one_arg_mem_ptr<std::reference_wrapper<auto (mem_fun_class::*)(char) const->int>, mem_fun_class const*>();
  check_one_arg_mem_ptr<std::reference_wrapper<auto (mem_fun_class::*)(char) volatile->int>, mem_fun_class volatile*>();
  check_one_arg_mem_ptr<
      std::reference_wrapper<auto (mem_fun_class::*)(char) const volatile->int>,
      mem_fun_class const volatile*>();
}

/// @test Check member types of a wrapped member function pointer with two arguments
TEST(PointerMemberTypesReferenceWrapperTests, MemberFunctionPointerTwoArgTypesTest) {
  class mem_fun_class {};

  check_two_arg_mem_ptr<std::reference_wrapper<auto (mem_fun_class::*)(char, double)->int>, mem_fun_class*>();
  check_two_arg_mem_ptr<std::reference_wrapper<auto (mem_fun_class::*)(char, double) const->int>, mem_fun_class const*>(
  );
  check_two_arg_mem_ptr<
      std::reference_wrapper<auto (mem_fun_class::*)(char, double) volatile->int>,
      mem_fun_class volatile*>();
  check_two_arg_mem_ptr<
      std::reference_wrapper<auto (mem_fun_class::*)(char, double) const volatile->int>,
      mem_fun_class const volatile*>();
}

/// @test Check member types of a wrapped noexcept member function pointer
TEST(PointerMemberTypesReferenceWrapperTests, NoexceptMemberFunctionPointerTypesTest) {
  class mem_fun_class {};

  check_one_arg_mem_ptr<std::reference_wrapper<auto (mem_fun_class::*)(char) noexcept -> int>, mem_fun_class*>();
  check_one_arg_mem_ptr<
      std::reference_wrapper<auto (mem_fun_class::*)(char) const noexcept -> int>,
      mem_fun_class const*>();
  check_one_arg_mem_ptr<
      std::reference_wrapper<auto (mem_fun_class::*)(char) volatile noexcept -> int>,
      mem_fun_class volatile*>();
  check_one_arg_mem_ptr<
      std::reference_wrapper<auto (mem_fun_class::*)(char) const volatile noexcept -> int>,
      mem_fun_class const volatile*>();
}

/// @test Check member types of a wrapped lvalue reference member function pointer
TEST(PointerMemberTypesReferenceWrapperTests, LvalueReferenceMemberFunctionPointerTypesTest) {
  class mem_fun_class {};

  check_one_arg_mem_ptr<std::reference_wrapper<auto (mem_fun_class::*)(char)&->int>, mem_fun_class*>();
  check_one_arg_mem_ptr<std::reference_wrapper<auto (mem_fun_class::*)(char) const&->int>, mem_fun_class const*>();
  check_one_arg_mem_ptr<std::reference_wrapper<auto (mem_fun_class::*)(char) volatile&->int>, mem_fun_class volatile*>(
  );
  check_one_arg_mem_ptr<
      std::reference_wrapper<auto (mem_fun_class::*)(char) const volatile&->int>,
      mem_fun_class const volatile*>();
}

/// @test Check member types of a wrapped lvalue reference noexcept member function pointer
TEST(PointerMemberTypesReferenceWrapperTests, LvalueReferenceNoexceptMemberFunctionPointerTypesTest) {
  class mem_fun_class {};

  check_one_arg_mem_ptr<std::reference_wrapper<auto (mem_fun_class::*)(char) & noexcept -> int>, mem_fun_class*>();
  check_one_arg_mem_ptr<
      std::reference_wrapper<auto (mem_fun_class::*)(char) const & noexcept -> int>,
      mem_fun_class const*>();
  check_one_arg_mem_ptr<
      std::reference_wrapper<auto (mem_fun_class::*)(char) volatile & noexcept -> int>,
      mem_fun_class volatile*>();
  check_one_arg_mem_ptr<
      std::reference_wrapper<auto (mem_fun_class::*)(char) const volatile & noexcept -> int>,
      mem_fun_class const volatile*>();
}

/// @test Check member types of a wrapped rvalue reference member function pointer
TEST(PointerMemberTypesReferenceWrapperTests, RvalueReferenceMemberFunctionPointerTypesTest) {
  class mem_fun_class {};

  check_one_arg_mem_ptr<std::reference_wrapper<auto (mem_fun_class::*)(char)&&->int>, mem_fun_class*>();
  check_one_arg_mem_ptr<std::reference_wrapper<auto (mem_fun_class::*)(char) const&&->int>, mem_fun_class const*>();
  check_one_arg_mem_ptr<std::reference_wrapper<auto (mem_fun_class::*)(char) volatile&&->int>, mem_fun_class volatile*>(
  );
  check_one_arg_mem_ptr<
      std::reference_wrapper<auto (mem_fun_class::*)(char) const volatile&&->int>,
      mem_fun_class const volatile*>();
}

/// @test Check member types of a wrapped rvalue reference noexcept member function pointer
TEST(PointerMemberTypesReferenceWrapperTests, RvalueReferenceNoexceptMemberFunctionPointerTypesTest) {
  class mem_fun_class {};

  check_one_arg_mem_ptr<std::reference_wrapper<auto (mem_fun_class::*)(char) && noexcept -> int>, mem_fun_class*>();
  check_one_arg_mem_ptr<
      std::reference_wrapper<auto (mem_fun_class::*)(char) const && noexcept -> int>,
      mem_fun_class const*>();
  check_one_arg_mem_ptr<
      std::reference_wrapper<auto (mem_fun_class::*)(char) volatile && noexcept -> int>,
      mem_fun_class volatile*>();
  check_one_arg_mem_ptr<
      std::reference_wrapper<auto (mem_fun_class::*)(char) const volatile && noexcept -> int>,
      mem_fun_class const volatile*>();
}

////////////////////////////////////////
// Wrapped Class Member Types
////////////////////////////////////////

/// @test Check member types of a wrapped class with @c result_type
TEST(ClassMemberTypesReferenceWrapperTests, ClassResultTypeTest) {
  class has_result {
   public:
    using result_type = int;
  };

  using Wrapper = std::reference_wrapper<has_result>;

  ASSERT_TRUE(has_result_type_v<Wrapper>);
  ASSERT_FALSE(has_argument_type_v<Wrapper>);
  ASSERT_FALSE(has_first_argument_type_v<Wrapper>);
  ASSERT_FALSE(has_second_argument_type_v<Wrapper>);

  ASSERT_TRUE(std::is_same_v<Wrapper::result_type, int>);
}

/// @test Check member types of a wrapped class with @c argument_type
TEST(ClassMemberTypesReferenceWrapperTests, ClassArgumentTypeTest) {
  class has_argument {
   public:
    using argument_type = double;
  };

  using Wrapper = std::reference_wrapper<has_argument>;

  ASSERT_FALSE(has_result_type_v<Wrapper>);
  ASSERT_TRUE(has_argument_type_v<Wrapper>);
  ASSERT_FALSE(has_first_argument_type_v<Wrapper>);
  ASSERT_FALSE(has_second_argument_type_v<Wrapper>);

  ASSERT_TRUE(std::is_same_v<Wrapper::argument_type, double>);
}

/// @test Check member types of a wrapped class with @c first_argument_type
/// and @c second_argument_type
TEST(ClassMemberTypesReferenceWrapperTests, ClassFirstSecondArgumentTypeTest) {
  class has_first_second_argument {
   public:
    using first_argument_type = char;
    using second_argument_type = float;
  };

  using Wrapper = std::reference_wrapper<has_first_second_argument>;

  ASSERT_FALSE(has_result_type_v<Wrapper>);
  ASSERT_FALSE(has_argument_type_v<Wrapper>);
  ASSERT_TRUE(has_first_argument_type_v<Wrapper>);
  ASSERT_TRUE(has_second_argument_type_v<Wrapper>);

  ASSERT_TRUE(std::is_same_v<Wrapper::first_argument_type, char>);
  ASSERT_TRUE(std::is_same_v<Wrapper::second_argument_type, float>);
}

/// @test Check member types of a wrapped class with @c result_type and
/// @c argument_type
TEST(ClassMemberTypesReferenceWrapperTests, ClassResultAndArgumentTypeTest) {
  class has_result_and_argument {
   public:
    using result_type = int;
    using argument_type = double;
  };

  using Wrapper = std::reference_wrapper<has_result_and_argument>;

  ASSERT_TRUE(has_result_type_v<Wrapper>);
  ASSERT_TRUE(has_argument_type_v<Wrapper>);
  ASSERT_FALSE(has_first_argument_type_v<Wrapper>);
  ASSERT_FALSE(has_second_argument_type_v<Wrapper>);

  ASSERT_TRUE(std::is_same_v<Wrapper::result_type, int>);
  ASSERT_TRUE(std::is_same_v<Wrapper::argument_type, double>);
}

/// @test Check member types of a wrapped class with @c result_type,
/// @c first_argument_type, and @c second_argument_type
TEST(ClassMemberTypesReferenceWrapperTests, ClassResultAndFirstSecondArgumentTypeTest) {
  class has_result_and_first_second_argument {
   public:
    using result_type = int;
    using first_argument_type = char;
    using second_argument_type = float;
  };

  using Wrapper = std::reference_wrapper<has_result_and_first_second_argument>;

  ASSERT_TRUE(has_result_type_v<Wrapper>);
  ASSERT_FALSE(has_argument_type_v<Wrapper>);
  ASSERT_TRUE(has_first_argument_type_v<Wrapper>);
  ASSERT_TRUE(has_second_argument_type_v<Wrapper>);

  ASSERT_TRUE(std::is_same_v<Wrapper::result_type, int>);
  ASSERT_TRUE(std::is_same_v<Wrapper::first_argument_type, char>);
  ASSERT_TRUE(std::is_same_v<Wrapper::second_argument_type, float>);
}

// NOLINTEND(readability-convert-member-functions-to-static)
// NOLINTEND(misc-const-correctness)
// NOLINTEND(hicpp-move-const-arg)
// NOLINTEND(readability-make-member-function-const)

}  // namespace
