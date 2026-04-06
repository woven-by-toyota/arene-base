// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "Comparisons are against copied values, no maths performed");

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/decay.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/is_const.hpp"
#include "arene/base/stdlib_choice/is_rvalue_reference.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/max_value_overload.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/apply_all.hpp"
#include "arene/base/type_list/at.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/contains.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/variant/variant.hpp"
#include "arene/base/variant/visit.hpp"

namespace {

using ::arene::base::variant;
using ::arene::base::visit;

/// @brief An enum for expressing the qualification that is or should be applied to a type.
enum class qualification { l_value, cl_value, r_value, cr_value };

/// @brief Get the qualification for a given type.
/// @tparam T The type to get the qualification for.
/// @return qualification::l_value if T is an lvalue reference
/// @return qualification::cl_value if T is a const lvalue reference
/// @return qualification::r_value if T is an rvalue reference
/// @return qualification::cr_value if T is a const rvalue reference
template <typename T>
constexpr auto qualification_for() -> qualification {
  constexpr bool is_const_value = std::is_const<std::remove_reference_t<T>>::value;
  if (std::is_rvalue_reference<T>::value) {
    return is_const_value ? qualification::cr_value : qualification::r_value;
  }
  return is_const_value ? qualification::cl_value : qualification::l_value;
}

static_assert(qualification_for<int&>() == qualification::l_value, "");
static_assert(qualification_for<int const&>() == qualification::cl_value, "");
static_assert(qualification_for<int&&>() == qualification::r_value, "");
static_assert(qualification_for<int const&&>() == qualification::cr_value, "");

/// @brief A helper return type for visitors which stores a variant and associated qualifications.
template <typename VariantT>
struct sv_return {
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  sv_return(qualification vis_qual, qualification var_qual, VariantT val)
      : visitor_qualification(vis_qual),
        variant_qualification(var_qual),
        value(std::move(val)) {}
  qualification visitor_qualification;
  qualification variant_qualification;
  VariantT value;
};

/// @brief An enum for expressing if operations should throw or not.
enum noexceptness { all_noexcept, some_throwing };

/// @brief A visitor which permutes handlers for all alternatives of a variant across all ref qualifications of the
/// visitor, and tracks the qualification provided to both as well as stores the value that was passed.
template <typename VariantType, noexceptness AllNoexcept>
struct WhichWasCalledVisitor;

/// @brief A visitor which permutes handlers for all alternatives of a variant across all ref qualifications of the
/// visitor, and tracks the qualification provided to both as well as stores the value that was passed.
template <typename... Ts, noexceptness AllNoexcept>
struct WhichWasCalledVisitor<variant<Ts...>, AllNoexcept> {
  using variant_t = variant<Ts...>;
  using return_t = sv_return<variant_t>;

  template <
      typename U,
      ::arene::base::constraints<std::enable_if_t<
          ::arene::base::type_lists::contains_v<::arene::base::type_list<Ts...>, std::decay_t<U>>>> = nullptr>
  constexpr auto operator()(U&& value) & noexcept(AllNoexcept == noexceptness::all_noexcept) -> return_t const& {
    was_called.emplace(qualification::l_value, qualification_for<decltype(value)>(), variant_t{std::forward<U>(value)});
    return *was_called;
  }

  template <
      typename U,
      ::arene::base::constraints<std::enable_if_t<
          ::arene::base::type_lists::contains_v<::arene::base::type_list<Ts...>, std::decay_t<U>>>> = nullptr>
  constexpr auto operator()(U&& value) const& noexcept(AllNoexcept == noexceptness::all_noexcept) -> return_t const& {
    was_called
        .emplace(qualification::cl_value, qualification_for<decltype(value)>(), variant_t{std::forward<U>(value)});
    return *was_called;
  }

  template <
      typename U,
      ::arene::base::constraints<std::enable_if_t<
          ::arene::base::type_lists::contains_v<::arene::base::type_list<Ts...>, std::decay_t<U>>>> = nullptr>
  constexpr auto operator()(U&& value) && noexcept(AllNoexcept == noexceptness::all_noexcept) -> return_t const& {
    was_called.emplace(qualification::r_value, qualification_for<decltype(value)>(), variant_t{std::forward<U>(value)});
    return *was_called;
  }

  template <
      typename U,
      ::arene::base::constraints<std::enable_if_t<
          ::arene::base::type_lists::contains_v<::arene::base::type_list<Ts...>, std::decay_t<U>>>> = nullptr>
  constexpr auto operator()(U&& value) const&& noexcept(AllNoexcept == noexceptness::all_noexcept) -> return_t const& {
    was_called
        .emplace(qualification::cr_value, qualification_for<decltype(value)>(), variant_t{std::forward<U>(value)});
    return *was_called;
  }

  /// @brief The content of the last call to the visitor.
  mutable ::arene::base::optional<return_t> was_called;
};

/// @brief A meta function which turns an invocation of visit into a SFAINE failure.
template <typename VisitorRefT, typename VariantRefT>
using use_visit_implicit_r = decltype(visit(std::declval<VisitorRefT>(), std::declval<VariantRefT>()));

/// @brief A trait which determins if a given visitor can be visited with a given variant using visit with a deduced
/// return type.
template <typename VisitorRefT, typename VariantRefT>
constexpr bool is_visit_implicit_r_invocable_with =
    ::arene::base::substitution_succeeds<use_visit_implicit_r, VisitorRefT, VariantRefT>;

/// @brief A helper metafunction for applying the specified @c qualification to a type.
template <qualification Qual>
struct apply_qualification;

/// @brief Specialization of @c apply_qualification for @c qualification::l_value.
template <>
struct apply_qualification<qualification::l_value> {
  static constexpr qualification qual = qualification::l_value;
  template <typename T>
  using type = T&;
};

/// @brief Specialization of @c apply_qualification for @c qualification::cl_value.
template <>
struct apply_qualification<qualification::cl_value> {
  static constexpr qualification qual = qualification::cl_value;
  template <typename T>
  using type = T const&;
};

/// @brief Specialization of @c apply_qualification for @c qualification::r_value.
template <>
struct apply_qualification<qualification::r_value> {
  static constexpr qualification qual = qualification::r_value;
  template <typename T>
  using type = T&&;
};

/// @brief Specialization of @c apply_qualification for @c qualification::cr_value.
template <>
struct apply_qualification<qualification::cr_value> {
  static constexpr qualification qual = qualification::cr_value;
  template <typename T>
  using type = T const&&;
};

static_assert(std::is_same<apply_qualification<qualification::l_value>::type<int>, int&>::value, "");
static_assert(std::is_same<apply_qualification<qualification::cl_value>::type<int>, int const&>::value, "");
static_assert(std::is_same<apply_qualification<qualification::r_value>::type<int>, int&&>::value, "");
static_assert(std::is_same<apply_qualification<qualification::cr_value>::type<int>, int const&&>::value, "");

/// @brief metafunction which, given two @c qualification values, produces a type_list of @c apply_qualification
/// instances.
template <qualification VisitorQualifier, qualification VariantQualifier>
using visitor_variant_qualifier_pair =
    ::arene::base::type_list<apply_qualification<VisitorQualifier>, apply_qualification<VariantQualifier>>;

/// @brief A type_list of all combinations of visitor and variant qualifications.
using visitor_variant_qualification_pairs = ::testing::Types<
    visitor_variant_qualifier_pair<qualification::l_value, qualification::l_value>,
    visitor_variant_qualifier_pair<qualification::l_value, qualification::cl_value>,
    visitor_variant_qualifier_pair<qualification::l_value, qualification::r_value>,
    visitor_variant_qualifier_pair<qualification::l_value, qualification::cr_value>,

    visitor_variant_qualifier_pair<qualification::cl_value, qualification::l_value>,
    visitor_variant_qualifier_pair<qualification::cl_value, qualification::cl_value>,
    visitor_variant_qualifier_pair<qualification::cl_value, qualification::r_value>,
    visitor_variant_qualifier_pair<qualification::cl_value, qualification::cr_value>,

    visitor_variant_qualifier_pair<qualification::r_value, qualification::l_value>,
    visitor_variant_qualifier_pair<qualification::r_value, qualification::cl_value>,
    visitor_variant_qualifier_pair<qualification::r_value, qualification::r_value>,
    visitor_variant_qualifier_pair<qualification::r_value, qualification::cr_value>,

    visitor_variant_qualifier_pair<qualification::cr_value, qualification::l_value>,
    visitor_variant_qualifier_pair<qualification::cr_value, qualification::cl_value>,
    visitor_variant_qualifier_pair<qualification::cr_value, qualification::r_value>,
    visitor_variant_qualifier_pair<qualification::cr_value, qualification::cr_value>>;

/// @brief The base test fixture for visit tests, it provides a standard variant ad visitor type.
struct VisitTest : ::testing::Test {
  using variant_t = variant<int, float, arene::base::string_view>;
  template <noexceptness AllNoexcept = noexceptness::all_noexcept>
  using visitor_t = WhichWasCalledVisitor<variant_t, AllNoexcept>;
};

/// @brief The base test fixture for visit tests that need to apply qualifications. It consumes an element created by @c
/// visitor_variant_qualifier_pair , and uses it to produce appropriate reference qualifications of the visitor and
/// variant types from the base test fixture.
template <typename VisitorVariantQualifierPair>
struct QualifiedVisitTest : VisitTest {
  using apply_variant_qualifier = arene::base::type_lists::at_t<VisitorVariantQualifierPair, 1>;
  static constexpr qualification variant_qualifier = apply_variant_qualifier::qual;
  using variant_ref_t = typename apply_variant_qualifier::template type<variant_t>;

  using apply_visitor_qualifier = arene::base::type_lists::at_t<VisitorVariantQualifierPair, 0>;
  static constexpr qualification visitor_qualifier = apply_visitor_qualifier::qual;
  template <noexceptness AllNoexcept = noexceptness::all_noexcept>
  using visitor_ref_t = typename apply_visitor_qualifier::template type<visitor_t<AllNoexcept>>;

  using noexcept_visitor_ref_t = visitor_ref_t<noexceptness::all_noexcept>;
  using throwing_visitor_ref_t = visitor_ref_t<noexceptness::some_throwing>;
};

template <typename VisitorVariantQualifierPair>
constexpr qualification QualifiedVisitTest<VisitorVariantQualifierPair>::variant_qualifier;
template <typename VisitorVariantQualifierPair>
constexpr qualification QualifiedVisitTest<VisitorVariantQualifierPair>::visitor_qualifier;

/// @brief The test fixture for implicit return visit tests.
template <typename VisitorVariantQualifierPair>
struct VisitImplicitReturnTest : QualifiedVisitTest<VisitorVariantQualifierPair> {};

TYPED_TEST_SUITE(VisitImplicitReturnTest, visitor_variant_qualification_pairs, );

/// @test Given `variant<Ts...>`, `Visitor`,
/// When `Ret = invoke_result_t<Visitor, T0>` is well-formed and where `T0` is the first element in `Ts...` and
//       `is_invocable_v<Visitor, Ts> && ... == true` and `is_same_v<Ret, invoke_result_t<Visitor, Ts>> && ... == true`
///      for a given cref qualification of `Visitor` and `variant<Ts...>`,
/// Then `is_invocable_v<visit<Visitor, variant<Ts...>>, Visitor, variant<Ts...>> == true` and
///      `is_same_v<Ret, invoke_result_t<visit<Visitor, variant<Ts...>>, Visitor, variant<Ts...>>> == true` for the same
///      cref qualification of `Visitor` and `variant<Ts...>`.

TYPED_TEST(VisitImplicitReturnTest, IsInvocableIFFVisitorHasOverloadForAllAlternativesAndAllReturnSameType) {
  STATIC_ASSERT_TRUE(is_visit_implicit_r_invocable_with<
                     typename TestFixture::noexcept_visitor_ref_t,
                     typename TestFixture::variant_ref_t>);
}

/// @test Given `variant<Ts...>` and a valid `Visitor`,
/// When `is_nothrow_invocable_v<Visitor, Ts> && ... == true` for a given cref qualification of `Visitor` and
///      `variant<Ts...>`,
/// Then `is_nothrow_invocable_v<visit<Visitor, variant<Ts...>>, Visitor, variant<Ts...>> == true` for the same cref
///      qualification of `Visitor` and `variant<Ts...>`.
TYPED_TEST(VisitImplicitReturnTest, IsNoexceptIfAllVisitorOverloadsAreNoexceptForAGivenRefQualificationOfInputs) {
  STATIC_ASSERT_TRUE(noexcept(visit(
      std::declval<typename TestFixture::noexcept_visitor_ref_t>(),
      std::declval<typename TestFixture::variant_ref_t>()
  )));
  STATIC_ASSERT_FALSE(noexcept(visit(
      std::declval<typename TestFixture::throwing_visitor_ref_t>(),
      std::declval<typename TestFixture::variant_ref_t>()
  )));
}

/// @brief A helper meta-function which consumes type_list<visitor_variant_qualifier_pair> and variant<Alternative...>
/// and produces type_list<type_list<Alternative, visitor_variant_qualifier_pair>...>
template <typename TypeList, typename Variant>
struct apply_alternatives;

/// @brief A helper meta-function which consumes type_list<visitor_variant_qualifier_pair> and variant<Alternative...>
/// and produces type_list<type_list<Alternative, visitor_variant_qualifier_pair>...>
template <typename TypeList, typename... Ts>
struct apply_alternatives<TypeList, variant<Ts...>> {
  template <typename T>
  struct apply_alternative {
    template <typename... Us>
    struct apply {
      using type = arene::base::type_list<arene::base::type_list<T, Us>...>;
    };
    using type = typename arene::base::type_lists::apply_all_t<TypeList, apply>::type;
  };
  using type = arene::base::type_lists::concat_t<typename apply_alternative<Ts>::type...>;
};

/// @brief A test fixture for visit tests where there is a need to produce an alternative for the variant. It consumes a
/// @c type_list<Alternative,visitor_variant_qualifier_pair> .
template <typename AlternativeAndVisitorVariantQualifierPair>
struct VisitImplicitReturnInvokeTest
    : VisitImplicitReturnTest<arene::base::type_lists::at_t<AlternativeAndVisitorVariantQualifierPair, 1>> {
  using alternative_t = arene::base::type_lists::at_t<AlternativeAndVisitorVariantQualifierPair, 0>;
  static constexpr auto alternative_value() noexcept -> alternative_t { return alternative_t{}; }
};

// This will be a type_list<type_list<Alternative, visitor_variant_qualifier_pair>...>
using alternative_and_visitor_variant_qualifier_pairs = ::arene::base::type_lists::
    apply_all_t<apply_alternatives<visitor_variant_qualification_pairs, VisitTest::variant_t>::type, ::testing::Types>;

TYPED_TEST_SUITE(VisitImplicitReturnInvokeTest, alternative_and_visitor_variant_qualifier_pairs, );

/// @test Given a `variant<Ts...> vrnt` which is not valueless by exception and a valid `Visitor vst`,
/// When `visit(vst, vrnt)` is invoked for a given cref qualification of `vst` and `vrnt`,
/// Then `vst` is invoked with the active alternative in `vrnt`, preserving the cref qualification of `vst` and `vrnt`.
TYPED_TEST(VisitImplicitReturnInvokeTest, InvokesVisitorWithActiveAlternativeInVariantMaintainingQualification) {
  auto visitor = typename TestFixture::template visitor_t<>{};
  auto vrnt = typename TestFixture::variant_t{this->alternative_value()};
  std::ignore = visit(
      static_cast<typename TestFixture::noexcept_visitor_ref_t>(visitor),
      static_cast<typename TestFixture::variant_ref_t>(vrnt)
  );
  ASSERT_TRUE(visitor.was_called);
  EXPECT_EQ(visitor.was_called->visitor_qualification, TestFixture::visitor_qualifier);
  EXPECT_EQ(visitor.was_called->variant_qualification, TestFixture::variant_qualifier);
  EXPECT_EQ(visitor.was_called->value, this->alternative_value());
}

/// @test Given a `variant<Ts...> vrnt` which is not valueless by exception and a valid `Visitor vst`,
/// When `visit(vst, vrnt)` is invoked for a given cref qualification of `vst` and `vrnt`,
/// Then the return value is the return from invoking `vst` with the active alternative in `vrnt`.
TYPED_TEST(VisitImplicitReturnInvokeTest, ReturnFromVisitIsReturnFromSelectedVariantAlternative) {
  auto visitor = typename TestFixture::template visitor_t<>{};
  auto vrnt = typename TestFixture::variant_t{this->alternative_value()};
  auto const result = visit(
      static_cast<typename TestFixture::noexcept_visitor_ref_t>(visitor),
      static_cast<typename TestFixture::variant_ref_t>(vrnt)
  );
  EXPECT_EQ(result.visitor_qualification, TestFixture::visitor_qualifier);
  EXPECT_EQ(result.variant_qualification, TestFixture::variant_qualifier);
  EXPECT_EQ(result.value, this->alternative_value());
}

/// @brief A helper type which is implicitly convertible from a `sv_return<VariantT>`, and the noexceptness of that
/// conversion can be specified.
template <typename VariantT, noexceptness NoExceptConversion = noexceptness::all_noexcept>
struct converted_sv_return : private sv_return<VariantT> {
  // NOLINTNEXTLINE(hicpp-explicit-conversions) Must be implicitly convertible to satisfy visit requirements
  converted_sv_return(sv_return<VariantT> val) noexcept(NoExceptConversion == noexceptness::all_noexcept)
      : sv_return<VariantT>(val) {}
  ~converted_sv_return() = default;
  converted_sv_return(converted_sv_return const&) noexcept(NoExceptConversion == noexceptness::all_noexcept) = default;
  // NOLINTNEXTLINE(hicpp-noexcept-move) Explicitly testing throwing move behavior
  converted_sv_return(converted_sv_return&&) noexcept(NoExceptConversion == noexceptness::all_noexcept) = default;
  auto operator=(converted_sv_return const&) noexcept(NoExceptConversion == noexceptness::all_noexcept)
      -> converted_sv_return& = default;
  // NOLINTNEXTLINE(hicpp-noexcept-move) Explicitly testing throwing move behavior
  auto operator=(converted_sv_return&&) noexcept(NoExceptConversion == noexceptness::all_noexcept)
      -> converted_sv_return& = default;
  using sv_return<VariantT>::variant_qualification;
  using sv_return<VariantT>::visitor_qualification;
  using sv_return<VariantT>::value;
};

/// @brief A helper metafunction for deducing if visit can be invoked with a given explicit return type, variant, and
/// visitor.
template <typename Ret, typename VisitorRefT, typename VariantRefT>
using use_visit_explicit_r = decltype(visit<Ret>(std::declval<VisitorRefT>(), std::declval<VariantRefT>()));

/// @brief A trait which determines if a given visitor can be visited with a given variant using visit with an explicit
/// return type.
template <typename Ret, typename VisitorRefT, typename VariantRefT>
constexpr bool is_visit_explicit_r_invocable_with =
    ::arene::base::substitution_succeeds<use_visit_explicit_r, Ret, VisitorRefT, VariantRefT>;

/// @brief A test fixture for visit tests with an explicit return type. It consumes a
/// @c type_list<visitor_variant_qualifier_pair> and produces appropriate reference qualifications of the visitor and
/// variant types from the base test fixture. It also provides a type alias for a suitable return type for the visit
/// invocation.
template <typename VisitorVariantQualifierPair>
struct VisitExplicitReturnTest : QualifiedVisitTest<VisitorVariantQualifierPair> {
  template <noexceptness AllNoexcept = noexceptness::all_noexcept>
  using converted_ret_t = converted_sv_return<VisitTest::variant_t, AllNoexcept>;
};

TYPED_TEST_SUITE(VisitExplicitReturnTest, visitor_variant_qualification_pairs, );

/// @test Given `variant<Ts...>`, `Visitor`, and an expected return type `Ret`,
/// When `is_invocable_r_v<Ret, Visitor, Ts> && ... == true` for a given cref qualification of
//       `Visitor` and `variant<Ts...>`,
/// Then `is_invocable_v<Ret, visit<Ret, Visitor, variant<Ts...>>, Visitor, variant<Ts...>> == true` and
///      `is_same_v<Ret, invoke_result_t<visit<Ret, Visitor, variant<Ts...>>, Visitor, variant<Ts...>>> == true` for the
///      same cref qualification of `Visitor` and `variant<Ts...>`.
TYPED_TEST(VisitExplicitReturnTest, IsInvocableIFFVisitorHasOverloadForAllAlternativesAndAllReturnConvertibleToRet) {
  STATIC_ASSERT_TRUE(is_visit_explicit_r_invocable_with<
                     typename TestFixture::template converted_ret_t<>,
                     typename TestFixture::noexcept_visitor_ref_t,
                     typename TestFixture::variant_ref_t>);
}

/// @test Given `variant<Ts...>`, a valid `Visitor`, and an expected return type `Ret`,
/// When `is_nothrow_invocable_r_v<Ret, Visitor, Ts> && ... == true` for a given cref qualification of `Visitor` and
///      `variant<Ts...>`,
/// Then `is_nothrow_invocable_v<visit<Ret, Visitor, variant<Ts...>>, Visitor, variant<Ts...>> == true` for the same
///      cref qualification of `Visitor` and `variant<Ts...>`.
TYPED_TEST(
    VisitExplicitReturnTest,
    IsNoexceptIfAllVisitorOverloadsAreNoexceptForAGivenRefQualificationOfInputsAndConversionToReturnIsNoexcept
) {
  ////// Nothing throws

  STATIC_ASSERT_TRUE(noexcept(visit<typename TestFixture::template converted_ret_t<noexceptness::all_noexcept>>(
      std::declval<typename TestFixture::noexcept_visitor_ref_t>(),
      std::declval<typename TestFixture::variant_ref_t>()
  )));

  ///// Visitor may throw

  STATIC_ASSERT_FALSE(noexcept(visit<typename TestFixture::template converted_ret_t<noexceptness::all_noexcept>>(
      std::declval<typename TestFixture::throwing_visitor_ref_t>(),
      std::declval<typename TestFixture::variant_ref_t>()
  )));

  ///// Return Conversion may throw

  STATIC_ASSERT_FALSE(noexcept(visit<typename TestFixture::template converted_ret_t<noexceptness::some_throwing>>(
      std::declval<typename TestFixture::noexcept_visitor_ref_t>(),
      std::declval<typename TestFixture::variant_ref_t>()
  )));
}

/// @brief A test fixture for visit tests with an explicit return type where there is a need to produce an alternative
/// for the variant. It consumes a @c type_list<Alternative,visitor_variant_qualifier_pair> .
template <typename AlternativeAndVisitorVariantQualifierPair>
struct VisitExplicitReturnInvokeTest
    : VisitExplicitReturnTest<::arene::base::type_lists::at_t<AlternativeAndVisitorVariantQualifierPair, 1>> {
  using alternative_t = arene::base::type_lists::at_t<AlternativeAndVisitorVariantQualifierPair, 0>;
  static constexpr auto alternative_value() noexcept -> alternative_t { return alternative_t{}; }
};

TYPED_TEST_SUITE(VisitExplicitReturnInvokeTest, alternative_and_visitor_variant_qualifier_pairs, );

/// @test Given a `variant<Ts...> vrnt` which is not valueless by exception, a valid `Visitor vst`, and a return type
/// `Ret`, When `visit<Ret>(vst, vrnt)` is invoked for a given cref qualification of `vst` and `vrnt`, Then `vst` is
/// invoked with the active alternative in `vrnt`, preserving the cref qualification of `vst` and `vrnt`.
TYPED_TEST(VisitExplicitReturnInvokeTest, InvokesVisitorWithActiveAlternativeInVariantMaintainingQualification) {
  auto visitor = typename TestFixture::template visitor_t<>{};
  auto vrnt = typename TestFixture::variant_t{this->alternative_value()};
  std::ignore = visit<typename TestFixture::template converted_ret_t<>>(
      static_cast<typename TestFixture::noexcept_visitor_ref_t>(visitor),
      static_cast<typename TestFixture::variant_ref_t>(vrnt)
  );
  ASSERT_TRUE(visitor.was_called);
  EXPECT_EQ(visitor.was_called->visitor_qualification, TestFixture::visitor_qualifier);
  EXPECT_EQ(visitor.was_called->variant_qualification, TestFixture::variant_qualifier);
  EXPECT_EQ(visitor.was_called->value, this->alternative_value());
}

/// @test Given a `variant<Ts...> vrnt` which is not valueless by exception, a valid `Visitor vst`, and a return type
/// `Ret`, When `visit<Ret>(vst, vrnt)` is invoked for a given cref qualification of `vst` and `vrnt`, Then the return
/// value is the return from invoking `vst` with the active alternative in `vrnt` implicitly converted to `Ret`.
TYPED_TEST(VisitExplicitReturnInvokeTest, ReturnFromVisitIsReturnFromSelectedVariantAlternative) {
  auto visitor = typename TestFixture::template visitor_t<>{};
  auto vrnt = typename TestFixture::variant_t{this->alternative_value()};
  auto const result = visit<typename TestFixture::template converted_ret_t<>>(
      static_cast<typename TestFixture::noexcept_visitor_ref_t>(visitor),
      static_cast<typename TestFixture::variant_ref_t>(vrnt)
  );

  EXPECT_EQ(result.visitor_qualification, TestFixture::visitor_qualifier);
  EXPECT_EQ(result.variant_qualification, TestFixture::variant_qualifier);
  EXPECT_EQ(result.value, this->alternative_value());
}

#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // Only relevant if exceptions are enabled.
/// @brief A type which is used to test that visit aborts when a variant is valueless by exception. It has a copy
/// constructor and copy assignment operator which throw an exception when invoked.
class exploding_copy {
  bool should_throw_;

 public:
  explicit exploding_copy(bool should_throw = true)
      : should_throw_(should_throw) {}
  ~exploding_copy() = default;
  exploding_copy(exploding_copy const& copy)
      : should_throw_(copy.should_throw_) {
    if (should_throw_) {
      throw std::runtime_error{"boom copy ctor"};
    }
  }
  exploding_copy(exploding_copy&&) = default;
  auto operator=(exploding_copy const& copy) -> exploding_copy& {
    should_throw_ = copy.should_throw_;
    if (should_throw_) {
      throw std::runtime_error{"boom copy assign"};
    }
    return *this;
  }
  auto operator=(exploding_copy&&) -> exploding_copy& = default;
};

/// @brief A dummy visitor which can be used in tests to avoid having to define a real visitor. It returns 1 for any
/// input.
auto dummy_visitor() {
  return [](auto&&) -> int { return 1; };
}
#endif  // ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) Only relevant if exceptions are enabled.

/// @brief A test fixture for visit tests which validate that visit aborts when a valueless-by-exception variant is
/// given. It consumes a @c visitor_variant_qualifier_pair .
template <typename VisitorVariantQualifierPair>
struct VisitDeathTest : QualifiedVisitTest<VisitorVariantQualifierPair> {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // Only relevant if exceptions are enabled.
  using variant_t = variant<exploding_copy>;
  using apply_variant_qualifier = arene::base::type_lists::at_t<VisitorVariantQualifierPair, 1>;
  static constexpr qualification variant_qualifier = apply_variant_qualifier::qual;
  using variant_ref_t = typename apply_variant_qualifier::template type<variant_t>;

  using apply_visitor_qualifier = arene::base::type_lists::at_t<VisitorVariantQualifierPair, 0>;
  static constexpr qualification visitor_qualifier = apply_visitor_qualifier::qual;
  using visitor_ref_t = typename apply_visitor_qualifier::template type<decltype(dummy_visitor())>;

  static auto make_valueless_by_exception_variant() -> variant_t {
    variant_t vrnt{};
    try {
      exploding_copy const not_poison_pill{false};
      exploding_copy const poison_pill{true};
      vrnt.template emplace<0>(not_poison_pill);
      vrnt.template emplace<0>(poison_pill);
    } catch (std::runtime_error const&) {
    }
    return vrnt;
  }
#endif  // ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) Only relevant if exceptions are enabled.
};

TYPED_TEST_SUITE(VisitDeathTest, visitor_variant_qualification_pairs, );

/// @test Given a `variant<Ts...> vrnt` which is valueless by exception and a valid `Visitor vst`,
/// When `visit(vst, vrnt)` is invoked for a given cref qualification of `vst` and `vrnt`,
/// Then it is an `ARENE_PRECONDITION` violation.
TYPED_TEST(VisitDeathTest, VisitIROnVariantWhichIsValuelessByExceptionIsPreconditionViolation) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // Only relevant if exceptions are enabled.
  auto visitor = dummy_visitor();
  auto valid_variant = typename TestFixture::variant_t{};
  auto exploding_variant = this->make_valueless_by_exception_variant();
  // we have to add this to avoid uncovered code in visit since the dispatch function is unconditionally generated.
  EXPECT_NO_FATAL_FAILURE(std::ignore = visit(
                              static_cast<typename TestFixture::visitor_ref_t>(visitor),
                              static_cast<typename TestFixture::variant_ref_t>(valid_variant)
                          ););
  ASSERT_DEATH(
      visit(
          static_cast<typename TestFixture::visitor_ref_t>(visitor),
          static_cast<typename TestFixture::variant_ref_t>(exploding_variant)
      ),
      "Precondition"
  );
#else
  GTEST_SKIP() << "Exceptions are disabled, skipping test.";
#endif  // ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) Only relevant
}

/// @test Given a `variant<Ts...> vrnt` which is valueless by exception, a valid `Visitor vst`, and a return type `Ret`,
/// When `visit(vst, vrnt)` is invoked for a given cref qualification of `vst` and `vrnt`,
/// Then it is an `ARENE_PRECONDITION` violation.
TYPED_TEST(VisitDeathTest, VisitEROnVariantWhichIsValuelessByExceptionIsPreconditionViolation) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)  // Only relevant if exceptions are enabled.
  auto visitor = dummy_visitor();
  auto valid_variant = typename TestFixture::variant_t{};
  auto exploding_variant = this->make_valueless_by_exception_variant();
  EXPECT_NO_FATAL_FAILURE(std::ignore = visit<std::int64_t>(
                              static_cast<typename TestFixture::visitor_ref_t>(visitor),
                              static_cast<typename TestFixture::variant_ref_t>(valid_variant)
                          ););
  ASSERT_DEATH(
      visit<std::int64_t>(
          static_cast<typename TestFixture::visitor_ref_t>(visitor),
          static_cast<typename TestFixture::variant_ref_t>(exploding_variant)
      ),
      "Precondition"
  );
#else
  GTEST_SKIP() << "Exceptions are disabled, skipping test.";
#endif  // ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) Only relevant
}

}  // namespace

ARENE_IGNORE_END();
