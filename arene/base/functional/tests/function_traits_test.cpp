// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/functional/function_traits.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace {

using arene::base::type_list;

/// @brief Check a function's function traits
/// @tparam Fun Function type to check
/// @tparam Ret Expected return type
/// @tparam ExpectedArgs List of argument types expected to be returned by the function trait
template <typename Fun, typename Ret, typename ExpectedList>
void check_fun() {
  using fun_traits = arene::base::function_traits<Fun>;
  static_assert(
      std::is_same<typename fun_traits::return_type, Ret>::value,
      "function_traits_t::return_type not correct"
  );
  static_assert(std::is_same<arene::base::function_return_t<Fun>, Ret>::value, "function_return_t not correct");

  static_assert(
      std::is_same<typename fun_traits::argument_types, ExpectedList>::value,
      "function_traits_t::argument_types not correct"
  );
  static_assert(
      std::is_same<arene::base::function_arguments_t<Fun>, ExpectedList>::value,
      "function_arguments_t not correct"
  );
}

template <typename R, typename... Ts>
struct test_case {
  using ret = R;
  using args = type_list<Ts...>;
};

struct struct_case {};
enum class enum_case {};

using func_test_types = typename ::testing::Types<
    test_case<void>,
    test_case<int>,
    test_case<void, int>,
    test_case<int, int>,
    test_case<int, double, char>,
    test_case<double, char, int, float>,
    test_case<double, char, int, float, bool>,
    test_case<double*, char, int*>,
    test_case<double const*, char*, int const*>,
    test_case<double volatile*, char*, int volatile*>,
    test_case<double const volatile*, char const volatile*, int>,
    test_case<char&, double, int&>,
    test_case<char const&, double, int const&>,
    test_case<char volatile&, double, int volatile&>,
    test_case<char const volatile&, double, int const volatile&>,
    test_case<char&&, double, int&&>,
    test_case<char const&&, double, int const&&>,
    test_case<char volatile&&, double, int volatile&&>,
    test_case<char const volatile&&, double, int const volatile&&>,
    test_case<struct_case, int>,
    test_case<int, char, struct_case>,
    test_case<struct_case*, int>,
    test_case<int, char, struct_case*>,
    test_case<struct_case&, int>,
    test_case<int, char, struct_case&>,
    test_case<struct_case&&, int>,
    test_case<int, char, struct_case&&>,
    test_case<enum_case, int>,
    test_case<int, char, enum_case>,
    test_case<enum_case*, int>,
    test_case<int, char, enum_case*>,
    test_case<enum_case&, int>,
    test_case<int, char, enum_case&>,
    test_case<enum_case&&, int>,
    test_case<int, char, enum_case&&>,
    test_case<int (*)(int), int>,
    test_case<int, char, int (*)(int)>,
    test_case<int (&)(int), int>,
    test_case<int, char, int (&)(int)>,
    test_case<int (&&)(int), int>,
    test_case<int, char, int (&&)(int)>>;

template <typename T>
class FunctionTraitsTests : public ::testing::Test {
 public:
  using test_case = T;
};

TYPED_TEST_SUITE(FunctionTraitsTests, func_test_types, );

////////////////////////////////////////
// Function Types
////////////////////////////////////////

template <typename R, typename... Ts>
auto test_func(Ts...) -> R;

template <typename R, typename... Ts>
struct test_func_type;

template <typename R, typename... Ts>
struct test_func_type<R, type_list<Ts...>> {
  using type = decltype(test_func<R, Ts...>);
};

/// @test Test function traits on function types
TYPED_TEST(FunctionTraitsTests, FunctionTypesTest) {
  using func_type =
      typename test_func_type<typename TestFixture::test_case::ret, typename TestFixture::test_case::args>::type;
  check_fun<func_type, typename TestFixture::test_case::ret, typename TestFixture::test_case::args>();

  using fun_traits = arene::base::function_traits<func_type>;
  static_assert(
      fun_traits::member_function_cv_qualifier == arene::base::cv_qualifier::unqualified,
      "function_traits::member_function_cv_qualifier not correct"
  );
  static_assert(
      fun_traits::member_function_reference_qualifier == arene::base::reference_qualifier::unqualified,
      "function_traits::member_function_reference_qualifier not correct"
  );
}

////////////////////////////////////////
// Member Function Pointer Types
////////////////////////////////////////

template <typename R, typename Ts, bool Noexcept>
struct mem_funcs;

// NOLINTBEGIN(readability-make-member-function-const) Functions are non-const for testing
template <typename R, typename... Ts, bool Noexcept>
struct mem_funcs<R, type_list<Ts...>, Noexcept> {
  auto no_qual(Ts...) noexcept(Noexcept) -> R;
  auto const_qual(Ts...) const noexcept(Noexcept) -> R;
  auto volatile_qual(Ts...) volatile noexcept(Noexcept) -> R;
  auto const_volatile_qual(Ts...) const volatile noexcept(Noexcept) -> R;

  auto lval_qual(Ts...) & noexcept(Noexcept) -> R;
  auto const_lval_qual(Ts...) const& noexcept(Noexcept) -> R;
  auto volatile_lval_qual(Ts...) volatile& noexcept(Noexcept) -> R;
  auto const_volatile_lval_qual(Ts...) const volatile& noexcept(Noexcept) -> R;

  auto rval_qual(Ts...) && noexcept(Noexcept) -> R;
  auto const_rval_qual(Ts...) const&& noexcept(Noexcept) -> R;
  auto volatile_rval_qual(Ts...) volatile&& noexcept(Noexcept) -> R;
  auto const_volatile_rval_qual(Ts...) const volatile&& noexcept(Noexcept) -> R;
};
// NOLINTEND(readability-make-member-function-const)

/// @brief Run tests on a member function pointer test case
/// @tparam R Return type
/// @tparam ArgsList A type_list of arguments
/// @tparam Noexcept Whether the member functions should be @c noexcept
template <typename R, typename ArgsList, bool Noexcept>
struct check_mem_fun_test_case {
  using test_type = mem_funcs<R, ArgsList, Noexcept>;
  using fun_cv = arene::base::cv_qualifier;
  using fun_ref = arene::base::reference_qualifier;

  /// @brief Check a member function pointer's function traits
  /// @tparam MemFun Member function pointer to check
  /// @tparam CVQual cv-qualifier enum
  /// @tparam RefQual reference-qualifier enum
  /// @tparam ClassType Class with matching cv and reference qualifiers
  template <typename MemFun, fun_cv CVQual, fun_ref RefQual, typename ClassType>
  void check_mem_fun() {
    using ptr_traits = arene::base::member_pointer_traits<MemFun>;
    static_assert(
        std::is_same<typename ptr_traits::class_type, test_type>::value,
        "member_pointee_traits::class_type not correct"
    );
    check_fun<typename ptr_traits::pointee_type, R, ArgsList>();

    using fun_ptr_traits = arene::base::member_function_pointer_traits<MemFun>;
    static_assert(
        std::is_same<typename fun_ptr_traits::class_type, test_type>::value,
        "member_function_pointee_traits::class_type not correct"
    );
    check_fun<typename fun_ptr_traits::pointee_type, R, ArgsList>();
    static_assert(
        std::is_same<typename fun_ptr_traits::return_type, R>::value,
        "member_function_pointer_traits_t::return_type not correct"
    );
    static_assert(
        std::is_same<typename fun_ptr_traits::argument_types, ArgsList>::value,
        "member_function_pointer_traits_t::argument_types not correct"
    );
    static_assert(
        std::is_same<typename fun_ptr_traits::implicit_argument_type, ClassType>::value,
        "member_function_pointer_traits_t::implicit_argument_type not correct"
    );

    static_assert(
        std::is_same<arene::base::member_function_return_t<MemFun>, R>::value,
        "member_function_return_t not correct"
    );
    static_assert(
        std::is_same<arene::base::member_function_arguments_t<MemFun>, ArgsList>::value,
        "member_function_arguments_t not correct"
    );

    static_assert(arene::base::member_function_cv<MemFun>::value == CVQual, "member_function_cv not correct");
    static_assert(
        arene::base::member_function_reference<MemFun>::value == RefQual,
        "member_function_reference not correct"
    );
    static_assert(arene::base::member_function_cv_v<MemFun> == CVQual, "member_function_cv_v not correct");
    static_assert(
        arene::base::member_function_reference_v<MemFun> == RefQual,
        "member_function_reference_v not correct"
    );
  }

  /// @brief Constructor runs checks on all member function pointers
  check_mem_fun_test_case() {
    check_mem_fun<decltype(&test_type::no_qual), fun_cv::unqualified, fun_ref::unqualified, test_type>();
    check_mem_fun<decltype(&test_type::const_qual), fun_cv::const_qualified, fun_ref::unqualified, test_type const>();
    check_mem_fun<
        decltype(&test_type::volatile_qual),
        fun_cv::volatile_qualified,
        fun_ref::unqualified,
        test_type volatile>();
    check_mem_fun<
        decltype(&test_type::const_volatile_qual),
        fun_cv::const_volatile_qualified,
        fun_ref::unqualified,
        test_type const volatile>();

    check_mem_fun<decltype(&test_type::lval_qual), fun_cv::unqualified, fun_ref::lvalue_qualified, test_type&>();
    check_mem_fun<
        decltype(&test_type::const_lval_qual),
        fun_cv::const_qualified,
        fun_ref::lvalue_qualified,
        test_type const&>();
    check_mem_fun<
        decltype(&test_type::volatile_lval_qual),
        fun_cv::volatile_qualified,
        fun_ref::lvalue_qualified,
        test_type volatile&>();
    check_mem_fun<
        decltype(&test_type::const_volatile_lval_qual),
        fun_cv::const_volatile_qualified,
        fun_ref::lvalue_qualified,
        test_type const volatile&>();

    check_mem_fun<decltype(&test_type::rval_qual), fun_cv::unqualified, fun_ref::rvalue_qualified, test_type&&>();
    check_mem_fun<
        decltype(&test_type::const_rval_qual),
        fun_cv::const_qualified,
        fun_ref::rvalue_qualified,
        test_type const&&>();
    check_mem_fun<
        decltype(&test_type::volatile_rval_qual),
        fun_cv::volatile_qualified,
        fun_ref::rvalue_qualified,
        test_type volatile&&>();
    check_mem_fun<
        decltype(&test_type::const_volatile_rval_qual),
        fun_cv::const_volatile_qualified,
        fun_ref::rvalue_qualified,
        test_type const volatile&&>();
  }
};

/// @test Test function traits on member function pointer types
TYPED_TEST(FunctionTraitsTests, MemberFunctionTypesTest) {
  check_mem_fun_test_case<typename TestFixture::test_case::ret, typename TestFixture::test_case::args, true>{};
  check_mem_fun_test_case<typename TestFixture::test_case::ret, typename TestFixture::test_case::args, false>{};
}

}  // namespace
