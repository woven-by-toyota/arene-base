// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/functional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/utility/to_underlying.hpp"

namespace {

using ::arene::base::default_inline_function_size;
using ::arene::base::inline_function;
using ::arene::base::is_invocable_r_v;
using ::arene::base::is_invocable_v;
using ::arene::base::noexcept_inline_function;

/// @test `default_inline_function_size` is equivalent to `4*sizeof(void*)`.
TEST(DefaultInlineFunctionSize, IsFourTimesSizeOfPointer) {
  STATIC_ASSERT_EQ(default_inline_function_size, 4 * sizeof(void*));
}

/// @brief simple enum specifying if the function should be noexcept or not.
enum class noexcept_specifier : bool { is_noexcept = true, is_not_noexcept = false };

/// @brief A helper template for producing a `function_ref` specifier of a given noexcept qualification.
/// @tparam IsNoexcept If the function is noexcept or not
template <noexcept_specifier IsNoexcept>
struct inline_function_type {
  /// @brief Helper alias for producing an `inline_function` with a given signature whose noexcept-ness are set by the
  /// outer template.
  /// @tparam Signature The signature of the generated inline_function.
  /// @tparam BufferSize The buffer size of the generated inline_function.
  template <typename Signature, std::size_t BufferSize = default_inline_function_size>
  using inline_function =
      ::arene::base::inline_function<Signature, BufferSize, ::arene::base::to_underlying(IsNoexcept)>;
};

/// @brief A set of inline_function generators which includes all permutations of noexcept.
using all_inline_functions = ::testing::Types<
    inline_function_type<noexcept_specifier::is_noexcept>,
    inline_function_type<noexcept_specifier::is_not_noexcept>>;

template <typename T>
struct AllInlineFunctionsTest : public testing::Test {};

TYPED_TEST_SUITE(AllInlineFunctionsTest, all_inline_functions, );

/// @test Converting a default-constructed `inline_function` to `bool` gives `false`, to indicate that there is no
/// stored value
TYPED_TEST(AllInlineFunctionsTest, DefaultConstructedFunctionIsEmpty) {
  ASSERT_FALSE(typename TypeParam::template inline_function<void()>{});
}

/// @test Converting an `inline_function` that has been constructed with a function to `bool` gives `true`, to indicate
/// that there is a stored value
TYPED_TEST(AllInlineFunctionsTest, ConstructingWithAFunctionIsNotEmpty) {
  ASSERT_TRUE(typename TypeParam::template inline_function<void()>{[]() noexcept {}});
}

/// @test `inline_function` cannot be constructed with something that is not invocable with the desired signature.
TYPED_TEST(AllInlineFunctionsTest, CannotConstructWithNonInvocablesOrWrongSignature) {
  STATIC_ASSERT_FALSE(std::is_constructible<typename TypeParam::template inline_function<void()>, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<typename TypeParam::template inline_function<void()>, double>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<typename TypeParam::template inline_function<int()>, void (*)()>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<typename TypeParam::template inline_function<void()>, void (*)(int)>::value
  );
}

constexpr int forty_two = 42;
auto return_forty_two() noexcept -> int { return forty_two; }

/// @test Invoking an `inline_function` returns the same result as invoking the function that was supplied to the
/// constructor.
TYPED_TEST(AllInlineFunctionsTest, InvokingReturnsResultOfInvokingInitializer) {
  typename TypeParam::template inline_function<int()> func([]() noexcept { return return_forty_two(); });
  STATIC_ASSERT_TRUE(is_invocable_r_v<int, typename TypeParam::template inline_function<int()>>);

  ASSERT_TRUE(func);
  ASSERT_EQ(func(), forty_two);
}

/// @test Invoking an `inline_function` returns the same result as invoking the lambda that was supplied to the
/// constructor.
TYPED_TEST(AllInlineFunctionsTest, CanStoreALambda) {
  typename TypeParam::template inline_function<int()> func([]() noexcept { return forty_two * 2; });
  ASSERT_EQ(func(), forty_two * 2);
}

/// @test Invoking an `inline_function` with arguments returns the same result as invoking the invocable that was
/// supplied to the constructor, with those arguments
TYPED_TEST(AllInlineFunctionsTest, CanPassArguments) {
  int idx = 1;
  int const jdx = 2;

  struct foo {
    int idx;
  };

  typename TypeParam::template inline_function<int(int&, int, foo&&)> func(
      [](int& first, int second, foo&& third) noexcept { return first + second + third.idx; }
  );

  STATIC_ASSERT_TRUE(is_invocable_r_v<int, decltype(func), int&, int, foo&&>);
  STATIC_ASSERT_FALSE(is_invocable_r_v<int, decltype(func), int, int, foo&&>);
  STATIC_ASSERT_TRUE(is_invocable_r_v<int, decltype(func), int&, int&, foo&&>);
  STATIC_ASSERT_FALSE(is_invocable_r_v<int, decltype(func), int const&, int, foo&&>);
  STATIC_ASSERT_FALSE(is_invocable_r_v<int, decltype(func), int&, int, foo&>);

  ASSERT_EQ(func(idx, jdx, foo{3}), 6);
}

/// @test If an `inline_function` is constructed with an invocable object, then the function call operator is invoked
/// via a non-`const` reference if the function signature used as a template parameter for `inline_function` is not
/// `const`, and the function call operator is invoked via a `const` reference if the function signature used as a
/// template parameter for `inline_function` is `const`
TYPED_TEST(AllInlineFunctionsTest, ConstnessOfObjectAffectsCallOperatorInvoked) {
  struct functor {
    auto operator()() noexcept -> int { return 1; }
    auto operator()() const noexcept -> int { return 2; }
  };

  // NOLINTNEXTLINE(misc-const-correctness) demonstrating impact of non-constness
  typename TypeParam::template inline_function<int() const> func(functor{});

  auto const& cfunc = func;

  ASSERT_EQ(func(), 2);
  ASSERT_EQ(cfunc(), 2);

  typename TypeParam::template inline_function<int()> func2(functor{});
  auto const& cfunc2 = func2;

  ASSERT_EQ(func2(), 1);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(cfunc2)>);
}

/// @test An `inline_function` can be constructed with a lambda that has captures, and that lambda is copied into the
/// `inline_function`, complete with captures.
TYPED_TEST(AllInlineFunctionsTest, CanStoreLambdasWithCaptures) {
  int idx = 3;

  typename TypeParam::template inline_function<int()> func([idx]() noexcept { return idx; });
  idx = 4;
  typename TypeParam::template inline_function<int()> func2([idx]() noexcept { return idx; });

  ASSERT_EQ(func(), 3);
  ASSERT_EQ(func2(), 4);
}

/// @test `inline_function` is move-constructible
TYPED_TEST(AllInlineFunctionsTest, CanMoveConstruct) {
  using func_t = typename TypeParam::template inline_function<int()>;
  STATIC_ASSERT_TRUE(std::is_move_constructible<func_t>::value);

  int idx = 3;
  func_t func([idx]() noexcept { return idx; });

  idx = 4;

  func_t func2(std::move(func));

  ASSERT_TRUE(func2);
  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
  ASSERT_FALSE(func);
  ASSERT_EQ(func2(), 3);
  ASSERT_EQ(idx, 4);
}

namespace {
class move_only_callable {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  unsigned& instance_count_;

 public:
  explicit move_only_callable(unsigned& instance_count)
      : instance_count_(instance_count) {
    ++instance_count;
  }

  move_only_callable(move_only_callable&& other) noexcept
      : instance_count_(other.instance_count_) {
    ++instance_count_;
  }
  auto operator=(move_only_callable&&) -> move_only_callable& = delete;
  move_only_callable(move_only_callable const&) = delete;
  auto operator=(move_only_callable const&) -> move_only_callable& = delete;

  ~move_only_callable() { --instance_count_; }

  void operator()() const noexcept {}
};

}  // namespace

/// @test The stored object copied from the invocable passed to the `inline_function` constructor is destroyed when the
/// `inline_function` is destroyed.
TYPED_TEST(AllInlineFunctionsTest, StoredObjectDestroyedWhenWrapperDestroyed) {
  unsigned count = 0;

  {
    typename TypeParam::template inline_function<void()> const func(move_only_callable{count});
    ASSERT_EQ(count, 1);
  }
  ASSERT_EQ(count, 0);
}

/// @test When an `inline_function` is move-constructed from another `inline_function` instance, then the instance of
/// the invocable stored in the source object is destroyed.
TYPED_TEST(AllInlineFunctionsTest, MoveConstructionDestroysSource) {
  unsigned count = 0;

  {
    typename TypeParam::template inline_function<void()> func(move_only_callable{count});
    ASSERT_EQ(count, 1);
    {
      typename TypeParam::template inline_function<void()> const func2(std::move(func));
      ASSERT_EQ(count, 1);
    }
    ASSERT_EQ(count, 0);
  }
  ASSERT_EQ(count, 0);
}

/// @test When an `inline_function` is move-assigned from another `inline_function` instance, then the instance of
/// the invocable stored in the source object is destroyed, and the source is left empty, so conversion to `bool` yields
/// `false`.
TYPED_TEST(AllInlineFunctionsTest, CanMoveAssign) {
  unsigned count = 0;

  typename TypeParam::template inline_function<void()> func(move_only_callable{count});
  ASSERT_EQ(count, 1);
  typename TypeParam::template inline_function<void()> func2(std::move(func));
  ASSERT_EQ(count, 1);

  func = std::move(func2);
  ASSERT_TRUE(func);
  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
  ASSERT_FALSE(func2);
  ASSERT_EQ(count, 1);

  typename TypeParam::template inline_function<void()> func3{};
  func = std::move(func3);
  ASSERT_FALSE(func);
  ASSERT_EQ(count, 0);

  typename TypeParam::template inline_function<void()> func4{};
  func = std::move(func4);
  ASSERT_FALSE(func);
}

struct large_callable {
  double first;
  double second;
  double third;
  double forth;

  auto operator()() const noexcept -> double { return first + second + third + forth; }
};

/// @test `inline_function` is not constructible from an invocable that is too large for the internal buffer
TYPED_TEST(AllInlineFunctionsTest, CannotConstructIfBufferTooSmall) {
  STATIC_ASSERT_FALSE(std::is_constructible<
                      typename TypeParam::template inline_function<double(), sizeof(large_callable) - 1>,
                      large_callable>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<
                     typename TypeParam::template inline_function<double(), sizeof(large_callable)>,
                     large_callable>::value);
}

/// @test `inline_function` cannot be assigned to from an invocable that is too large for the internal buffer
TYPED_TEST(AllInlineFunctionsTest, CannotAssignIfBufferTooSmall) {
  STATIC_ASSERT_FALSE(std::is_assignable<
                      typename TypeParam::template inline_function<double(), sizeof(large_callable) - 1>,
                      large_callable>::value);
  STATIC_ASSERT_TRUE(std::is_assignable<
                     typename TypeParam::template inline_function<double(), sizeof(large_callable)>,
                     large_callable>::value);
}

/// @test `inline_function` is not constructible from an invocable with a throwing move constructor.
TYPED_TEST(AllInlineFunctionsTest, StoredFunctionMustHaveNoexceptMove) {
  struct throwing_move {
    throwing_move() = default;
    ~throwing_move() = default;
    throwing_move(throwing_move&&) noexcept(false) {}
    auto operator=(throwing_move&&) noexcept(false) -> throwing_move& { return *this; }
    throwing_move(throwing_move const&) = delete;
    auto operator=(throwing_move const&) -> throwing_move& = delete;

    void operator()() const noexcept {}
  };

  STATIC_ASSERT_FALSE(std::is_constructible<typename TypeParam::template inline_function<void()>, throwing_move>::value
  );
}

/// @test The move constructor of `inline_function` is `noexcept`
TYPED_TEST(AllInlineFunctionsTest, MoveIsNoexcept) {
  using func = typename TypeParam::template inline_function<void()>;
  STATIC_ASSERT_TRUE(std::is_nothrow_move_constructible<func>::value);
  STATIC_ASSERT_TRUE(std::is_nothrow_move_assignable<func>::value);
}

/// @test Given an instance from something that can be invoked with the specified
/// signature, or from `{}`, which is the same as default construction
TYPED_TEST(AllInlineFunctionsTest, CanAssignFromCallable) {
  using function_type = typename TypeParam::template inline_function<int()>;
  {
    SCOPED_TRACE("Assignment from a callable stores the callable in the functor");
    function_type func;
    int idx = 2;
    func = [idx]() noexcept { return idx; };
    ASSERT_TRUE(func);

    ASSERT_EQ(func(), 2);
    idx = 3;
    ASSERT_EQ(func(), 2);
    ASSERT_EQ(idx, 3);

    func = {};
    ASSERT_FALSE(func);
  }

  {
    SCOPED_TRACE("Assignment from a callable results in only a single move");
    function_type func;
    struct move_counter {
      unsigned count{0};

      move_counter() noexcept = default;

      move_counter(move_counter&& other) noexcept
          : count(other.count) {
        ++count;
      }
      move_counter(move_counter const&) = delete;
      auto operator=(move_counter const&) -> move_counter& = delete;
      auto operator=(move_counter&&) -> move_counter& = delete;
      ~move_counter() = default;

      auto operator()() const noexcept -> unsigned { return count; }
    };

    func = move_counter();

    ASSERT_EQ(func(), 1);
  }
}

/// @test An instance of `inline_function` can be safely assigned to from itself without resulting in a move.
TYPED_TEST(AllInlineFunctionsTest, CanSafelySelfAssign) {
  using function_type = typename TypeParam::template inline_function<int()>;
  {
    SCOPED_TRACE("The functor remains valid after self-assignment");
    int const idx = 2;
    function_type func{[=]() noexcept { return idx; }};
    ARENE_IGNORE_START();
    ARENE_IGNORE_WSELFMOVE("Test code: self-assignment is on purpose");
    func = std::move(func);
    ARENE_IGNORE_END();
    ASSERT_TRUE(func);

    ASSERT_EQ(func(), 2);
  }

  {
    SCOPED_TRACE("Self assignment does not move the functor");
    struct move_counter {
      unsigned count{0};

      move_counter() noexcept = default;

      move_counter(move_counter&& other) noexcept
          : count(other.count) {
        ++count;
      }
      move_counter(move_counter const&) = delete;
      auto operator=(move_counter const&) -> move_counter& = delete;
      auto operator=(move_counter&&) -> move_counter& = delete;
      ~move_counter() = default;

      auto operator()() const noexcept -> unsigned { return count; }
    };

    function_type func{move_counter()};
    ARENE_IGNORE_START();
    ARENE_IGNORE_WSELFMOVE("Test code: self-assignment is on purpose");
    func = std::move(func);
    ARENE_IGNORE_END();
    ASSERT_EQ(func(), 1);
  }
}

/// @test An `inline_function` with a function signature that is specified to be `const` cannot be assigned from an
/// invocable that is only non-`const`-invocable
TYPED_TEST(AllInlineFunctionsTest, CannotAssignConstFunctionFromNonConstCallable) {
  struct callable {
    void operator()() noexcept {}
  };

  STATIC_ASSERT_FALSE(std::is_assignable<typename TypeParam::template inline_function<void() const>, callable>::value);
}

/// @test An `inline_function` with a function signature that is specified to be non-`const` cannot be assigned from an
/// invocable that is only invocable for a `const`-qualified object
TYPED_TEST(AllInlineFunctionsTest, CannotAssignNonConstFunctionFromConstOnlyCallable) {
  struct callable {
    void operator()() noexcept = delete;
    void operator()() const noexcept {}
  };

  STATIC_ASSERT_FALSE(std::is_assignable<typename TypeParam::template inline_function<void()>, callable>::value);
}

/// @test An `inline_function` with a function signature that is specified to be non-`const` can be assigned from an
/// invocable that is only non-`const`-invocable
TYPED_TEST(AllInlineFunctionsTest, CanAssignNonConstFunctionFromNonConstOnlyCallable) {
  struct callable {
    void operator()() noexcept {}
    void operator()() const noexcept = delete;
  };

  STATIC_ASSERT_TRUE(std::is_assignable<typename TypeParam::template inline_function<void()>, callable>::value);
}

/// @test @c noexcept_inline_function is an alias for @c inline_function with @c IsNoexcept set @c true
TEST(NoexceptInlineFunction, IsAnAliasForInlineFunctionWithIsNoExceptSetTrue) {
  ::testing::StaticAssertTypeEq<
      noexcept_inline_function<void(void)>,
      inline_function<void(void), default_inline_function_size, true>>();
  ::testing::StaticAssertTypeEq<
      noexcept_inline_function<int(void)>,
      inline_function<int(void), default_inline_function_size, true>>();
  ::testing::StaticAssertTypeEq<
      noexcept_inline_function<void(int)>,
      inline_function<void(int), default_inline_function_size, true>>();
  ::testing::StaticAssertTypeEq<
      noexcept_inline_function<int(int)>,
      inline_function<int(int), default_inline_function_size, true>>();
  ::testing::StaticAssertTypeEq<
      noexcept_inline_function<void(void) const>,
      inline_function<void(void) const, default_inline_function_size, true>>();
  ::testing::StaticAssertTypeEq<
      noexcept_inline_function<int(void) const>,
      inline_function<int(void) const, default_inline_function_size, true>>();
  ::testing::StaticAssertTypeEq<
      noexcept_inline_function<void(int) const>,
      inline_function<void(int) const, default_inline_function_size, true>>();
  ::testing::StaticAssertTypeEq<
      noexcept_inline_function<int(int) const>,
      inline_function<int(int) const, default_inline_function_size, true>>();
}

/// @test A @c noexcept_inline_function 's call operator is noexcept
TEST(NoexceptInlineFunction, TheCallOperatorIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<noexcept_inline_function<void()>>()()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<noexcept_inline_function<void(int)>>()(int{})));
  STATIC_ASSERT_TRUE(noexcept(std::declval<noexcept_inline_function<int()>>()()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<noexcept_inline_function<int(int)>>()(int{})));
  STATIC_ASSERT_TRUE(noexcept(std::declval<noexcept_inline_function<void() const>>()()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<noexcept_inline_function<void(int) const>>()(int{})));
  STATIC_ASSERT_TRUE(noexcept(std::declval<noexcept_inline_function<int() const>>()()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<noexcept_inline_function<int(int) const>>()(int{})));
}

/// @test A @c noexcept_inline_function cannot be instantiated by a callable which does not satisfy nothrow_invocable .
TEST(NoexceptInlineFunction, CannotBeConstructedFromFunctorWhichIsNotNoexcept) {
  auto const not_noexcept = []() noexcept(false) {};
  STATIC_ASSERT_FALSE(std::is_constructible<noexcept_inline_function<void()>, decltype(not_noexcept)>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<noexcept_inline_function<void() const>, decltype(not_noexcept)>::value);
}

}  // namespace
