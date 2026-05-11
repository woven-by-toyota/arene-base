// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "stdlib/include/iterator"
#include "stdlib/include/memory"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"

// NOLINTBEGIN(readability-identifier-length)

namespace {

/// @brief a simple user defined type to use in this test file
///
struct user_defined_type {};

/// @brief a type that only satisfies the syntactic requirements of a container
///
struct bad_container {
  // container value type
  using value_type = int;
};

/// @brief a type that mocks the push_back operation
///
struct mock_container {
  // container value type
  using value_type = int;

  // push_back state
  enum class state {
    unpushed,
    moved,
    copied,
  };

  // state of the latest push_back operation
  state latest{state::unpushed};

  // record push_back invoked with rvalue reference overload
  constexpr auto push_back(int&&) noexcept -> void { latest = state::moved; }
  // record push_back invoked with lvalue reference overload
  constexpr auto push_back(int const&) noexcept -> void { latest = state::copied; }
  // disable the ability to use '&' to obtain the address
  auto operator&() = delete;  // NOLINT(google-runtime-operator)
};

/// @brief a type that acts as an proxy container and has const qualified push_back members
///
struct proxy_container {
  // container value type
  using value_type = typename mock_container::value_type;

  // indirect container
  mock_container& container;

  // rvalue reference overload
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  constexpr auto push_back(int&& value) const noexcept -> void { container.push_back(std::move(value)); }
  // lvalue reference overload
  constexpr auto push_back(int const& value) const noexcept -> void { container.push_back(value); }
};

using container_types = ::testing::Types<
    bad_container,
    mock_container,
    proxy_container,
    bad_container const,
    mock_container const,
    proxy_container const>;

template <class T>
struct BackInsertIterator : testing::Test {};

TYPED_TEST_SUITE(BackInsertIterator, container_types, );

/// @test Check that @c back_insert_operator publicly derives from @c iterator.
///
TYPED_TEST(BackInsertIterator, PubliclyDerivesFromIterator) {
  using testing::is_unambiguously_publicly_derived_from_v;

  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<
              std::back_insert_iterator<TypeParam>,
              std::iterator<std::output_iterator_tag, void, void, void, void>>);
}

/// @test Check that @c back_insert_operator defines iterator member type aliases
///
TYPED_TEST(BackInsertIterator, DefinesMemberTypeAliases) {
  ASSERT_TRUE(std::is_same_v<
              std::output_iterator_tag,
              typename std::back_insert_iterator<TypeParam>::iterator_category>);
  ASSERT_TRUE(std::is_same_v<void, typename std::back_insert_iterator<TypeParam>::value_type>);
  ASSERT_TRUE(std::is_same_v<void, typename std::back_insert_iterator<TypeParam>::difference_type>);
  ASSERT_TRUE(std::is_same_v<void, typename std::back_insert_iterator<TypeParam>::reference>);
  ASSERT_TRUE(std::is_same_v<void, typename std::back_insert_iterator<TypeParam>::pointer>);
}

/// @test Check that @c back_insert_operator models @c OutputIterator.
///
TYPED_TEST(BackInsertIterator, ModelsOutputIterator) {
  using arene::base::is_output_iterator_v;

  ASSERT_TRUE(is_output_iterator_v<std::back_insert_iterator<TypeParam>>);
}

/// @test Given non-const container types, check that a back insert iterator is
/// only constructible with a non-const lvalue reference
///
TYPED_TEST(BackInsertIterator, ConstructibleWithOnlyNonConstLvalueReference) {
  using container_type = std::remove_const_t<TypeParam>;
  using back_insert_iterator_type = std::back_insert_iterator<container_type>;

  ASSERT_TRUE(std::is_constructible_v<back_insert_iterator_type, container_type&>);

  ASSERT_FALSE(std::is_constructible_v<back_insert_iterator_type, container_type const&>);
  ASSERT_FALSE(std::is_constructible_v<back_insert_iterator_type, container_type&&>);
  ASSERT_FALSE(std::is_constructible_v<back_insert_iterator_type, container_type const&&>);
}

template <class T>
struct BackInsertIteratorAssign : testing::Test {};

/// @brief function object that forwards an lvalue reference
///
struct as_lvalue {
  template <class T>
  constexpr auto operator()(T& ref) const noexcept -> T& {
    return ref;
  }
};

/// @brief function object that converts an lvalue reference to an rvalue reference
///
struct as_rvalue {
  template <class T>
  constexpr auto operator()(T& ref) const noexcept -> T&& {
    return std::move(ref);
  }
};

using assign_ref_qual_types = ::testing::Types<as_lvalue, as_rvalue>;

TYPED_TEST_SUITE(BackInsertIteratorAssign, assign_ref_qual_types, );

/// @test Check that both lvalue and rvalue ref qualified assignment invokes
/// push_back on the underlying container with an rvalue reference
///
CONSTEXPR_TYPED_TEST(BackInsertIteratorAssign, InvokesPushBackWithRvalueReference) {
  auto container = mock_container{};
  auto it = std::back_insert_iterator<decltype(container)>{container};

  ASSERT_EQ(mock_container::state::unpushed, container.latest);

  using as_lvalue_or_rvalue = TypeParam;
  as_lvalue_or_rvalue{}(it) = int{};

  ASSERT_EQ(mock_container::state::moved, container.latest);
}

/// @test Check that both lvalue and rvalue ref qualified assignment invokes
/// push_back on the underlying container with an lvalue reference
///
CONSTEXPR_TYPED_TEST(BackInsertIteratorAssign, InvokesPushBackWithLvalueReference) {
  auto container = mock_container{};
  auto it = std::back_insert_iterator<decltype(container)>{container};

  ASSERT_EQ(mock_container::state::unpushed, container.latest);

  using as_lvalue_or_rvalue = TypeParam;
  auto value = int{};
  as_lvalue_or_rvalue{}(it) = value;

  ASSERT_EQ(mock_container::state::copied, container.latest);
}

/// @test Check that both lvalue and rvalue ref qualified assignment invokes
/// push_back on a const proxy container with an rvalue reference
///
CONSTEXPR_TYPED_TEST(BackInsertIteratorAssign, InvokesPushBackWithRvalueReferenceAndProxyContainer) {
  mock_container container{};
  proxy_container const proxy{container};
  auto it = std::back_insert_iterator<decltype(proxy)>{proxy};

  ASSERT_EQ(mock_container::state::unpushed, container.latest);

  using as_lvalue_or_rvalue = TypeParam;
  as_lvalue_or_rvalue{}(it) = int{};

  ASSERT_EQ(mock_container::state::moved, container.latest);
}

/// @test Check that both lvalue and rvalue ref qualified assignment invokes
/// push_back on a const proxy container with an lvalue reference
///
CONSTEXPR_TYPED_TEST(BackInsertIteratorAssign, InvokesPushBackWithLvalueReferenceAndProxyContainer) {
  mock_container container{};
  proxy_container const proxy{container};
  auto it = std::back_insert_iterator<decltype(proxy)>{proxy};

  ASSERT_EQ(mock_container::state::unpushed, container.latest);

  using as_lvalue_or_rvalue = TypeParam;
  auto value = int{};
  as_lvalue_or_rvalue{}(it) = value;

  ASSERT_EQ(mock_container::state::copied, container.latest);
}

/// @brief helper container used to test noexcept-ness of back_insert_iterator's assignment operator
/// @tparam T container value type
/// @tparam IsMoveNoexcept determines if push_back with move is noexcept
/// @tparam IsCopyNoexcept determines if push_back with copy is noexcept
///
template <class T, bool IsMoveNoexcept, bool IsCopyNoexcept>
struct nothrow_test_container {
  // container value type
  using value_type = T;

  // push_back invoked with rvalue reference overload
  constexpr auto push_back(T&&) noexcept(IsMoveNoexcept) -> void {}
  // push_back invoked with lvalue reference overload
  constexpr auto push_back(T const&) noexcept(IsCopyNoexcept) -> void {}
};

using noexcept_container_types = ::testing::Types<
    nothrow_test_container<int, true, true>,
    nothrow_test_container<int, true, false>,
    nothrow_test_container<int, false, false>,
    nothrow_test_container<user_defined_type, true, true>,
    nothrow_test_container<user_defined_type, true, false>,
    nothrow_test_container<user_defined_type, false, false>>;

template <class T>
struct BackInsertIteratorNoexcept : testing::Test {};

TYPED_TEST_SUITE(BackInsertIteratorNoexcept, noexcept_container_types, );

/// @test Check that assignment is noexcept if the underlying call to push_back is noexcept
///
TYPED_TEST(BackInsertIteratorNoexcept, AssignmentIsConditionallyNoexcept) {
  using container_type = TypeParam;
  using back_insert_iterator_type = std::back_insert_iterator<container_type>;
  using value_type = typename TypeParam::value_type;

  ASSERT_EQ(
      noexcept(std::declval<container_type&>().push_back(std::declval<value_type&&>())),
      (std::is_nothrow_assignable_v<back_insert_iterator_type, value_type&&>)
  );
  ASSERT_EQ(
      noexcept(std::declval<container_type&>().push_back(std::declval<value_type const&>())),
      (std::is_nothrow_assignable_v<back_insert_iterator_type, value_type const&>)
  );
}

/// @brief function object that returns a member named container
///
struct container_member_accessible_fn {
  template <class I>
  constexpr auto operator()(I it) -> decltype(it.container);
};

/// @brief helper type to obtain the protected member of the @c back_insert_iterator
///
template <class I>
struct container_member_getter : I {
  constexpr explicit container_member_getter(I base)
      : I{base} {}

  constexpr auto get() const { return this->container; }
};

/// @test Check that the back_insert_iterator defines a protected member named @c container
CONSTEXPR_TEST(BackInsertIterator, HasProtectedContainerMember) {
  auto container = mock_container{};

  using back_insert_iterator_type = std::back_insert_iterator<decltype(container)>;
  auto const it = back_insert_iterator_type{container};

  ASSERT_FALSE(::testing::simple_is_invocable_v<container_member_accessible_fn, back_insert_iterator_type>);

  ASSERT_EQ(std::addressof(container), container_member_getter<back_insert_iterator_type>{it}.get());
}

/// @test Check that @c back_inserter creates a @c back_insert_iterator
///
TEST(BackInserter, CreatesBackInserter) {
  using container_type = mock_container;

  ASSERT_TRUE(std::is_same_v<
              std::back_insert_iterator<container_type>,
              decltype(std::back_inserter(std::declval<container_type&>()))>);
  ASSERT_TRUE(std::is_same_v<
              std::back_insert_iterator<container_type const>,
              decltype(std::back_inserter(std::declval<container_type const&>()))>);
}

/// @test Check that @c back_inserter is not invocable with a non-const rvalue reference
///
TEST(BackInserter, NotInvocableWithNonConstRvalueReference) {
  using container_type = mock_container;

  auto const back_inserter = FUNCTION_LIFT(std::back_inserter);

  ASSERT_FALSE(::testing::simple_is_invocable_v<decltype(back_inserter), container_type&&>);
}

template <class T>
struct BackInserter : testing::Test {};

TYPED_TEST_SUITE(BackInserter, container_types, );

/// @test Check that @c back_inserter is noexcept
///
TYPED_TEST(BackInserter, BackInsertIsAlwaysNoexcept) {
  using container_type = TypeParam;

  ASSERT_TRUE(noexcept(std::back_inserter(std::declval<container_type&>())));
}

/// @brief helper function to test equality of a @c back_insert_iterator
///
template <class Container>
constexpr auto equal(std::back_insert_iterator<Container> lhs, std::back_insert_iterator<Container> rhs) -> bool {
  using container_getter_type = container_member_getter<std::back_insert_iterator<Container>>;

  auto lhs_pointer = container_getter_type{lhs}.get();
  auto rhs_pointer = container_getter_type{rhs}.get();

  // NOLINTNEXTLINE(bugprone-sizeof-expression)
  static_assert(sizeof(lhs_pointer) == sizeof(lhs), "this 'equal' function is not valid if sizes are not equal");
  static_assert(
      alignof(decltype(lhs_pointer)) == alignof(decltype(lhs)),
      "this 'equal' function is not valid if alignments are not equal"
  );

  return lhs_pointer == rhs_pointer;
}

/// @test Check that the return value of @c back_inserter is equivalent to
/// constructing a @c back_insert_iterator from the same container.
///
CONSTEXPR_TEST(BackInserter, ReturnedIteratorEquivalentToBackInsertIterator) {
  auto container = mock_container{};

  auto it1 = std::back_insert_iterator<decltype(container)>{container};
  auto it2 = std::back_inserter(container);

  ASSERT_TRUE(equal(it1, it2));
}

/// @test Check that @c back_inserter can be used with an rvalue reference to a
/// const container
///
TEST(BackInserter, InvocableWithConstRvalueReferenceContainer) {
  auto const container = mock_container{};

  auto it1 = std::back_insert_iterator<decltype(container)>{container};
  auto it2 = std::back_inserter(std::move(container));  // NOLINT(hicpp-move-const-arg)

  ASSERT_TRUE(equal(it1, it2));
}

}  // namespace

// NOLINTEND(readability-identifier-length)
