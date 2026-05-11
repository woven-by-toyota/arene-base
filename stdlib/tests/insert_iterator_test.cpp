// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
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

  // iterator type
  using iterator = int*;
};

/// @brief a type that mocks the insert operation
///
struct mock_container {
  // container value type
  using value_type = int;

  // iterator type
  using iterator = int*;

  // insert state
  enum class state {
    unpushed,
    moved,
    copied,
  };

  // state of the latest insert operation
  state latest{state::unpushed};

  // record insert invoked with rvalue reference overload
  constexpr auto insert(iterator it, value_type&&) noexcept -> iterator {
    latest = state::moved;
    return it;
  }
  // record insert invoked with lvalue reference overload
  constexpr auto insert(iterator it, value_type const&) noexcept -> iterator {
    latest = state::copied;
    return it;
  }
  // disable the ability to use '&' to obtain the address
  auto operator&() = delete;  // NOLINT(google-runtime-operator)
};

/// @brief a type that acts as an proxy container and has const qualified insert protected_members
///
struct proxy_container {
  // container value type
  using value_type = typename mock_container::value_type;

  // iterator type
  using iterator = int*;

  // indirect container
  mock_container& container;

  // NOLINTBEGIN(hicpp-move-const-arg)

  // rvalue reference overload
  constexpr auto insert(iterator it, value_type&& value) const noexcept -> iterator {
    return container.insert(std::move(it), std::move(value));
  }
  // lvalue reference overload
  constexpr auto insert(iterator it, value_type const& value) const noexcept -> iterator {
    return container.insert(std::move(it), value);
  }

  // NOLINTEND(hicpp-move-const-arg)
};

/// @brief helper container used to test noexcept-ness of insert_iterator's assignment operator
/// @tparam T container value type
/// @tparam IsMoveNoexcept determines if insert with move is noexcept
/// @tparam IsCopyNoexcept determines if insert with copy is noexcept
///
template <class T, bool IsMoveNoexcept, bool IsCopyNoexcept>
struct noexcept_test_container {
  // container value type
  using value_type = T;

  // iterator type
  using iterator = int*;

  // NOLINTBEGIN(hicpp-move-const-arg)

  // insert invoked with rvalue reference overload
  constexpr auto insert(iterator it, value_type&&) noexcept(IsMoveNoexcept) -> iterator { return std::move(it); }
  // insert invoked with lvalue reference overload
  constexpr auto insert(iterator it, value_type const&) noexcept(IsCopyNoexcept) -> iterator { return std::move(it); }

  // NOLINTEND(hicpp-move-const-arg)
};

using container_types = ::testing::Types<
    bad_container,
    mock_container,
    proxy_container,
    noexcept_test_container<int, true, true>,
    bad_container const,
    mock_container const,
    proxy_container const,
    noexcept_test_container<int, true, true> const>;

template <class T>
struct InsertIterator : testing::Test {};

TYPED_TEST_SUITE(InsertIterator, container_types, );

/// @test Check that @c insert_iterator publicly derives from @c iterator.
///
TYPED_TEST(InsertIterator, PubliclyDerivesFromIterator) {
  using testing::is_unambiguously_publicly_derived_from_v;

  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<
              std::insert_iterator<TypeParam>,
              std::iterator<std::output_iterator_tag, void, void, void, void>>);
}

/// @test Check that @c insert_iterator defines iterator member type aliases
///
TYPED_TEST(InsertIterator, DefinesIteratorMemberTypeAliases) {
  ASSERT_TRUE(std::is_same_v<std::output_iterator_tag, typename std::insert_iterator<TypeParam>::iterator_category>);
  ASSERT_TRUE(std::is_same_v<void, typename std::insert_iterator<TypeParam>::value_type>);
  ASSERT_TRUE(std::is_same_v<void, typename std::insert_iterator<TypeParam>::difference_type>);
  ASSERT_TRUE(std::is_same_v<void, typename std::insert_iterator<TypeParam>::reference>);
  ASSERT_TRUE(std::is_same_v<void, typename std::insert_iterator<TypeParam>::pointer>);
}

/// @test Check that @c insert_iterator defines container_type member type alias
///
TYPED_TEST(InsertIterator, DefinesContainerTypeTypeAliases) {
  ASSERT_TRUE(std::is_same_v<TypeParam, typename std::insert_iterator<TypeParam>::container_type>);
}

/// @test Check that @c insert_operator models @c OutputIterator.
///
TYPED_TEST(InsertIterator, ModelsOutputIterator) {
  using arene::base::is_output_iterator_v;

  ASSERT_TRUE(is_output_iterator_v<std::insert_iterator<TypeParam>>);
}

/// @test Given non-const container types, check that an insert iterator is
/// only constructible with a non-const lvalue reference
///
TYPED_TEST(InsertIterator, ConstructibleWithOnlyNonConstLvalueReference) {
  using container_type = std::remove_const_t<TypeParam>;
  using insert_iterator_type = std::insert_iterator<container_type>;

  ASSERT_TRUE(std::is_constructible_v<insert_iterator_type, container_type&, typename container_type::iterator>);

  ASSERT_FALSE(std::is_constructible_v<insert_iterator_type, container_type const&, typename container_type::iterator>);
  ASSERT_FALSE(std::is_constructible_v<insert_iterator_type, container_type&&, typename container_type::iterator>);
  ASSERT_FALSE(std::
                   is_constructible_v<insert_iterator_type, container_type const&&, typename container_type::iterator>);
}

template <class T>
struct InsertIteratorAssign : testing::Test {};

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

TYPED_TEST_SUITE(InsertIteratorAssign, assign_ref_qual_types, );

/// @test Check that both lvalue and rvalue ref qualified assignment invokes
/// insert on the underlying container with an rvalue reference
///
CONSTEXPR_TYPED_TEST(InsertIteratorAssign, InvokesInsertWithRvalueReference) {
  auto container = mock_container{};
  auto pointee = int{};

  auto it = std::insert_iterator<decltype(container)>{container, &pointee};

  ASSERT_EQ(mock_container::state::unpushed, container.latest);

  using as_lvalue_or_rvalue = TypeParam;
  as_lvalue_or_rvalue{}(it) = int{};

  ASSERT_EQ(mock_container::state::moved, container.latest);
}

/// @test Check that both lvalue and rvalue ref qualified assignment invokes
/// insert on the underlying container with an lvalue reference
///
CONSTEXPR_TYPED_TEST(InsertIteratorAssign, InvokesInsertWithLvalueReference) {
  auto container = mock_container{};
  auto pointee = int{};

  auto it = std::insert_iterator<decltype(container)>{container, &pointee};

  ASSERT_EQ(mock_container::state::unpushed, container.latest);

  using as_lvalue_or_rvalue = TypeParam;
  auto value = int{};
  as_lvalue_or_rvalue{}(it) = value;

  ASSERT_EQ(mock_container::state::copied, container.latest);
}

/// @test Check that both lvalue and rvalue ref qualified assignment invokes
/// insert on a const proxy container with an rvalue reference
///
CONSTEXPR_TYPED_TEST(InsertIteratorAssign, InvokesInsertWithRvalueReferenceAndProxyContainer) {
  mock_container container{};
  proxy_container const proxy{container};
  auto pointee = int{};

  auto it = std::insert_iterator<decltype(proxy)>{proxy, &pointee};

  ASSERT_EQ(mock_container::state::unpushed, container.latest);

  using as_lvalue_or_rvalue = TypeParam;
  as_lvalue_or_rvalue{}(it) = int{};

  ASSERT_EQ(mock_container::state::moved, container.latest);
}

/// @test Check that both lvalue and rvalue ref qualified assignment invokes
/// insert on a const proxy container with an lvalue reference
///
CONSTEXPR_TYPED_TEST(InsertIteratorAssign, InvokesInsertWithLvalueReferenceAndProxyContainer) {
  mock_container container{};
  proxy_container const proxy{container};
  auto pointee = int{};

  auto it = std::insert_iterator<decltype(proxy)>{proxy, &pointee};

  ASSERT_EQ(mock_container::state::unpushed, container.latest);

  using as_lvalue_or_rvalue = TypeParam;
  auto value = int{};
  as_lvalue_or_rvalue{}(it) = value;

  ASSERT_EQ(mock_container::state::copied, container.latest);
}

/// @brief obtain copies of the protected members of an @c std::insert_iterator
///
template <class I>
constexpr auto protected_members(I const& iter) {
  struct derived_iterator : I {
    ARENE_IGNORE_START();
    ARENE_IGNORE_ARMCLANG("-Wunused-local-typedef", "These type aliases are used below");
    using container_type = typename I::container_type;
    using base_iterator_type = typename container_type::iterator;
    ARENE_IGNORE_END();

    constexpr explicit derived_iterator(I const& base)
        : I{base} {}

    constexpr auto into_members() && {
      struct members {
        container_type* container;
        base_iterator_type iter;
      };

      return members{this->container, this->iter};
    }
  };

  return derived_iterator{iter}.into_members();
}

/// @brief function object that returns a member named container
///
struct container_member_accessible_fn {
  template <class I>
  constexpr auto operator()(I it) -> decltype(it.container);
};

/// @brief function object that returns a member named iter
///
struct iter_member_accessible_fn {
  template <class I>
  constexpr auto operator()(I it) -> decltype(it.iter);
};

/// @test Check that the insert_iterator defines a protected member named @c container
///
CONSTEXPR_TEST(InsertIterator, HasProtectedContainerMember) {
  auto container = mock_container{};

  using insert_iterator_type = std::insert_iterator<decltype(container)>;
  auto const it = insert_iterator_type{container, {}};

  ASSERT_FALSE(::testing::simple_is_invocable_v<container_member_accessible_fn, insert_iterator_type>);

  ASSERT_EQ(std::addressof(container), protected_members(it).container);
}

/// @test Check that the insert_iterator defines a protected member named @c iter
///
CONSTEXPR_TEST(InsertIterator, HasProtectedIterMember) {
  auto container = mock_container{};
  auto pointee = typename mock_container::value_type{};

  using insert_iterator_type = std::insert_iterator<decltype(container)>;
  auto const it = insert_iterator_type{container, &pointee};

  ASSERT_FALSE(::testing::simple_is_invocable_v<iter_member_accessible_fn, insert_iterator_type>);

  ASSERT_EQ(&pointee, protected_members(it).iter);
}

/// @test Check that both lvalue and rvalue ref qualified assignment will assign and
/// increment the stored iterator
///
CONSTEXPR_TYPED_TEST(InsertIteratorAssign, InvokesPreincrementOnStoredIterator) {
  struct mock_container_with_trace_iterator {
    struct value_type {};

    struct iterator {  // NOLINT(hicpp-special-member-functions)
      int assign_count{};
      int increment_count{};

      iterator() = default;
      iterator(iterator const&) = default;

      constexpr auto operator=(iterator) & -> iterator& {
        ++assign_count;
        return *this;
      }

      constexpr auto operator++() & -> iterator& {
        ++increment_count;
        return *this;
      }
    };

    constexpr auto insert(iterator it, value_type) -> iterator { return std::move(it); }
  };

  auto container = mock_container_with_trace_iterator{};

  auto it = std::insert_iterator<decltype(container)>{container, {}};

  {
    auto const base_iterator = protected_members(it).iter;
    ASSERT_EQ(0, base_iterator.assign_count);
    ASSERT_EQ(0, base_iterator.increment_count);
  }

  using as_lvalue_or_rvalue = TypeParam;

  for (auto i = int{1}; i != 4; ++i) {
    as_lvalue_or_rvalue{}(it) = typename mock_container_with_trace_iterator::value_type{};

    auto const base_iterator = protected_members(it).iter;
    ASSERT_EQ(i, base_iterator.assign_count);
    ASSERT_EQ(i, base_iterator.increment_count);
  }
}

using noexcept_container_types = ::testing::Types<
    noexcept_test_container<int, true, true>,
    noexcept_test_container<int, true, false>,
    noexcept_test_container<int, false, false>,
    noexcept_test_container<user_defined_type, true, true>,
    noexcept_test_container<user_defined_type, true, false>,
    noexcept_test_container<user_defined_type, false, false>>;

template <class T>
struct InsertIteratorNoexcept : testing::Test {};

TYPED_TEST_SUITE(InsertIteratorNoexcept, noexcept_container_types, );

/// @test Check that assignment is noexcept if the underlying call to insert is noexcept
///
TYPED_TEST(InsertIteratorNoexcept, AssignmentIsConditionallyNoexcept) {
  using container_type = TypeParam;
  using insert_iterator_type = std::insert_iterator<container_type>;
  using value_type = typename TypeParam::value_type;

  ASSERT_EQ(
      noexcept(std::declval<container_type&>().insert(  //
          std::declval<typename container_type::iterator>(),
          std::declval<value_type&&>()
      )),
      (std::is_nothrow_assignable_v<insert_iterator_type, value_type&&>)
  );
  ASSERT_EQ(
      noexcept(std::declval<container_type&>().insert(  //
          std::declval<typename container_type::iterator>(),
          std::declval<value_type const&>()
      )),
      (std::is_nothrow_assignable_v<insert_iterator_type, value_type const&>)
  );
}

/// @brief helper container used to test noexcept-ness of insert_iterator's assignment operator
/// @tparam IsAssignNoexcept determines if the container iterator's assignment operator is noexcept
/// @tparam IsIncrementNoexcept determines if the container iterator's increment operator is noexcept
///
template <bool IsAssignNoexcept, bool IsIncrementNoexcept>
struct noexcept_test_iterator_container {
  struct value_type {};

  struct iterator {  // NOLINT(hicpp-special-member-functions)
    iterator(iterator const&) noexcept;
    auto operator=(iterator const&) noexcept(IsAssignNoexcept) -> iterator&;
    auto operator++() const noexcept(IsIncrementNoexcept) -> iterator&;
  };

  auto insert(iterator, value_type) noexcept -> iterator;
};

/// @test Check that assignment is *not* noexcept if the underlying iterator is
/// *not* nothrow move assignable or *not* nothrow incrementable.
///
TEST(InsertIteratorNoexcept, CanThrowIfUnderlyingIteratorIsNotNothrowMoveAssignable) {
  auto const is_nothrow_assignable = [](auto container) {
    using container_type = decltype(container);
    using insert_iterator_type = std::insert_iterator<container_type>;
    using value_type = typename container_type::value_type;

    return std::is_nothrow_assignable_v<insert_iterator_type, value_type>;
  };

  ASSERT_TRUE(is_nothrow_assignable(noexcept_test_iterator_container<true, true>{}));
  ASSERT_FALSE(is_nothrow_assignable(noexcept_test_iterator_container<false, true>{}));
  ASSERT_FALSE(is_nothrow_assignable(noexcept_test_iterator_container<true, false>{}));
  ASSERT_FALSE(is_nothrow_assignable(noexcept_test_iterator_container<false, false>{}));
}

/// @test Check that @c inserter creates a @c insert_iterator
///
TEST(Inserter, CreatesInserter) {
  using container_type = mock_container;

  ASSERT_TRUE(std::is_same_v<
              std::insert_iterator<container_type>,
              decltype(std::inserter(std::declval<container_type&>(), std::declval<typename container_type::iterator>())
              )>);
  ASSERT_TRUE(std::is_same_v<
              std::insert_iterator<container_type const>,
              decltype(std::inserter(
                  std::declval<container_type const&>(),
                  std::declval<typename container_type::iterator>()
              ))>);
}

/// @test Check that @c inserter is not invocable with a non-const rvalue reference
///
TEST(Inserter, NotInvocableWithNonConstRvalueReference) {
  using container_type = mock_container;

  auto const inserter = FUNCTION_LIFT(std::inserter);

  ASSERT_FALSE(::testing::simple_is_invocable_v<  //
               decltype(inserter),
               container_type&&,
               typename container_type::iterator>);
}

/// @brief a type that only satisfies the syntactic requirements of a container
/// @tparam IteratorThrows determines if the iterator move constructor throws
///
template <bool IteratorThrows>
struct bad_container_with_throwing_iterator {
  // container value type
  struct value_type {};

  // iterator type
  struct iterator {                                    // NOLINT(hicpp-special-member-functions)
    iterator(iterator&&) noexcept(!IteratorThrows) {}  // NOLINT(hicpp-noexcept-move)
  };
};

/// @test Check that @c inserter is noexcept iff the move constructor of container_type::iterator is noexcept
///
TEST(Inserter, InsertIsNoexceptIfContainerIteratorMoveConstructorIsNoexcept) {
  auto const insert_is_noexcept = [](auto container) {
    using container_type = decltype(container);
    return noexcept(std::inserter(container, std::declval<typename container_type::iterator>()));
  };

  ASSERT_TRUE(insert_is_noexcept(bad_container_with_throwing_iterator<false>{}));
  ASSERT_FALSE(insert_is_noexcept(bad_container_with_throwing_iterator<true>{}));
}

/// @brief helper function to test equality of a @c insert_iterator
///
template <class Container>
constexpr auto equal(std::insert_iterator<Container> lhs, std::insert_iterator<Container> rhs) -> bool {
  auto lhs_members = protected_members(lhs);
  auto rhs_members = protected_members(rhs);

  return lhs_members.container == rhs_members.container && lhs_members.iter == rhs_members.iter;
}

/// @test Check that the return value of @c inserter is equivalent to
/// constructing a @c insert_iterator from the same container.
///
CONSTEXPR_TEST(Inserter, ReturnedIteratorEquivalentToInsertIterator) {
  auto container = mock_container{};
  auto pointee = mock_container::value_type{};

  auto it1 = std::insert_iterator<decltype(container)>{container, &pointee};
  auto it2 = std::inserter(container, &pointee);

  ASSERT_TRUE(equal(it1, it2));
}

/// @test Check that @c std is an associated namespace of @c std::insert_iterator
///
CONSTEXPR_TEST(InsertIterator, AssociatedNamespaceIsStd) {
  // std is an associated namespace due to base class std::iterator

  auto container = mock_container{};
  auto it = std::insert_iterator<mock_container>{container, {}};

  ARENE_IGNORE_START();
  ARENE_IGNORE_CLANG("-Wunqualified-std-cast-call", "explicit test of associated namespace via ADL");
  static_cast<void>(move(it));  // NOLINT(hicpp-move-const-arg)
  ARENE_IGNORE_END();
}

/// @test Check that @c inserter can be used with an rvalue reference to a
/// const container
///
TEST(BackInserter, InvocableWithConstRvalueReferenceContainer) {
  auto const container = mock_container{};
  auto pointee = mock_container::value_type{};

  auto it1 = std::insert_iterator<decltype(container)>{container, &pointee};
  auto it2 = std::inserter(std::move(container), &pointee);  // NOLINT(hicpp-move-const-arg)

  ASSERT_TRUE(equal(it1, it2));
}

}  // namespace

// NOLINTEND(readability-identifier-length)
