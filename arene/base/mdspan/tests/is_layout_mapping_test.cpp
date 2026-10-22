// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/is_layout_mapping.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/attributes/maybe_unused.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/tests/test_layout_types.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace {

using test::basic_layout_mapping;
using test::basic_mapping_policy;

/// @test The basic layout mapping is a valid layout mapping
TEST(IsLayoutMapping, BasicLayoutMappingIsALayoutMapping) {
  ASSERT_TRUE(arene::base::is_layout_mapping_v<basic_layout_mapping<arene::base::extents<std::size_t>>>);
  ASSERT_TRUE(arene::base::is_layout_mapping_v<basic_layout_mapping<arene::base::extents<std::int32_t>>>);
  ASSERT_TRUE((arene::base::is_layout_mapping_v<basic_layout_mapping<arene::base::extents<std::int32_t, 1, 2, 3, 4>>>));
}

template <typename Extents>
class not_comparable : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<not_comparable>;

  auto operator==(not_comparable const&) const -> bool = delete;
};

/// @test Something that fulfils all the requirements except being equality comparable is not a layout mapping
TEST(IsLayoutMapping, NotEqualityComparableMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<not_comparable<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class not_copy_constructible : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<not_copy_constructible>;

  not_copy_constructible() = default;
  not_copy_constructible(not_copy_constructible const&) = delete;
};

/// @test Something that fulfils all the requirements except being copyable is not a layout mapping
TEST(IsLayoutMapping, NotCopyableMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<not_copy_constructible<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class not_copy_assignable : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<not_copy_assignable>;

  auto operator=(not_copy_assignable const&) -> not_copy_assignable& = delete;
};

/// @test Something that fulfils all the requirements except being copyable is not a layout mapping
TEST(IsLayoutMapping, NotCopyAssignableMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<not_copy_assignable<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class not_nothrow_move_assignable : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<not_nothrow_move_assignable>;

  auto operator=(not_nothrow_move_assignable&&) noexcept(false) -> not_nothrow_move_assignable&;
};

/// @test Something that fulfils all the requirements except being nothrow-move-assignable is not a layout mapping
TEST(IsLayoutMapping, NotNothrowMoveAssignableMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<not_nothrow_move_assignable<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class not_nothrow_move_constructible : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<not_nothrow_move_constructible>;

  not_nothrow_move_constructible() = default;
  not_nothrow_move_constructible(not_nothrow_move_constructible&&) noexcept(false);
};

/// @test Something that fulfils all the requirements except being nothrow-move-constructible is not a layout mapping
TEST(IsLayoutMapping, NotNothrowMoveConstructibleMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<not_nothrow_move_constructible<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class not_nothrow_swappable : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<not_nothrow_swappable>;
};

template <typename Extents>
ARENE_MAYBE_UNUSED void swap(not_nothrow_swappable<Extents>&, not_nothrow_swappable<Extents>&) noexcept(false);

/// @test Something that fulfils all the requirements except being nothrow-swappable is not a layout mapping
TEST(IsLayoutMapping, NotNothrowSwappableMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<not_nothrow_swappable<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class no_extents : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<no_extents>;
  auto extents() const -> Extents const& = delete;
};

/// @test Something that fulfils all the requirements except having an @c extents function is not a layout mapping
TEST(IsLayoutMapping, NoExtentsMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<no_extents<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class wrong_extents : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<wrong_extents>;
  auto extents() const -> void;
};

/// @test Something that fulfils all the requirements except that @c extents returns the wrong type is not a layout
/// mapping
TEST(IsLayoutMapping, ExtentsWithWrongReturnMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<wrong_extents<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class no_required_span_size : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<no_required_span_size>;
  auto required_span_size() const -> std::size_t = delete;
};

/// @test Something that fulfils all the requirements except not having a @c required_span_size member function is not a
/// layout mapping
TEST(IsLayoutMapping, NoRequiredSpanSizeMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<no_required_span_size<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class wrong_required_span_size : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<wrong_required_span_size>;
  auto required_span_size() const -> void;
};

/// @test Something that fulfils all the requirements except that @c required_span_size returns the wrong type is not a
/// layout mapping
TEST(IsLayoutMapping, RequiredSpanSizeWithWrongReturnMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<wrong_required_span_size<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class no_is_unique : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<no_is_unique>;
  auto is_unique() const -> bool = delete;
};

/// @test Something that fulfils all the requirements except there is no @c is_unique member function is not a layout
/// mapping
TEST(IsLayoutMapping, NoIsUniqueMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<no_is_unique<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class wrong_is_unique : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<wrong_is_unique>;
  auto is_unique() const -> int;
};

/// @test Something that fulfils all the requirements except that @c is_unique returns the wrong type is not a layout
/// mapping
TEST(IsLayoutMapping, IsUniqueWithWrongReturnTypeMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<wrong_is_unique<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class no_is_exhaustive : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<no_is_exhaustive>;
  auto is_exhaustive() const -> bool = delete;
};

/// @test Something that fulfils all the requirements except there is no @c is_exhaustive member function is not a
/// layout mapping
TEST(IsLayoutMapping, NoIsExhaustiveMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<no_is_exhaustive<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class wrong_is_exhaustive : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<wrong_is_exhaustive>;
  auto is_exhaustive() const -> int;
};

/// @test Something that fulfils all the requirements except that @c is_exhaustive returns the wrong type is not a
/// layout mapping
TEST(IsLayoutMapping, IsExhaustiveWithWrongReturnTypeMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<wrong_is_exhaustive<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class no_is_strided : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<no_is_strided>;
  auto is_strided() const -> bool = delete;
};

/// @test Something that fulfils all the requirements except there is no @c is_strided member function is not a layout
/// mapping
TEST(IsLayoutMapping, NoIsStridedMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<no_is_strided<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class wrong_is_strided : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<wrong_is_strided>;
  auto is_strided() const -> int;
};

/// @test Something that fulfils all the requirements except that @c is_strided returns the wrong type is not a layout
/// mapping
TEST(IsLayoutMapping, IsStridedWithWrongReturnMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<wrong_is_strided<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class no_is_always_unique : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<no_is_always_unique>;
  auto is_always_unique() const -> bool = delete;
};

/// @test Something that fulfils all the requirements except there is no @c is_always_unique static member function is
/// not a layout mapping
TEST(IsLayoutMapping, NoIsAlwaysUniqueMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<no_is_always_unique<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class wrong_is_always_unique : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<wrong_is_always_unique>;
  static constexpr auto is_always_unique() -> int { return {}; }
};

/// @test Something that fulfils all the requirements except that @c is_always_unique returns the wrong type is not a
/// layout mapping
TEST(IsLayoutMapping, IsAlwaysUniqueWithWrongReturnTypeMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<wrong_is_always_unique<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class non_constexpr_is_always_unique : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<non_constexpr_is_always_unique>;
  static auto is_always_unique() -> bool { return true; }
};

/// @test Something that fulfils all the requirements except that @c is_always_unique is not constexpr is not a layout
/// mapping
TEST(IsLayoutMapping, NonConstexprIsAlwaysUniqueMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<non_constexpr_is_always_unique<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class no_is_always_exhaustive : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<no_is_always_exhaustive>;
  auto is_always_exhaustive() const -> bool = delete;
};

/// @test Something that fulfils all the requirements except there is no @c is_always_exhaustive static member function
/// is not a layout mapping
TEST(IsLayoutMapping, NoIsAlwaysExhaustiveMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<no_is_always_exhaustive<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class wrong_is_always_exhaustive : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<wrong_is_always_exhaustive>;
  static constexpr auto is_always_exhaustive() -> int { return {}; }
};

/// @test Something that fulfils all the requirements except that @c is_always_exhaustive returns the wrong type is not
/// a layout mapping
TEST(IsLayoutMapping, IsAlwaysExhaustiveWithWrongReturnTypeMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<wrong_is_always_exhaustive<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class non_constexpr_is_always_exhaustive : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<non_constexpr_is_always_exhaustive>;
  static auto is_always_exhaustive() -> bool { return true; }
};

/// @test Something that fulfils all the requirements except that @c is_always_exhaustive is not constexpr is not a
/// layout mapping
TEST(IsLayoutMapping, NonConstexprIsAlwaysExhaustiveMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<non_constexpr_is_always_exhaustive<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class no_is_always_strided : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<no_is_always_strided>;
  auto is_always_strided() const -> bool = delete;
};

/// @test Something that fulfils all the requirements except there is no @c is_always_strided static member function is
/// not a layout mapping
TEST(IsLayoutMapping, NoIsAlwaysStridedMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<no_is_always_strided<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class wrong_is_always_strided : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<wrong_is_always_strided>;
  static constexpr auto is_always_strided() -> int { return {}; }
};

/// @test Something that fulfils all the requirements except that @c is_always_strided returns the wrong type is not a
/// layout mapping
TEST(IsLayoutMapping, IsAlwaysStridedWithWrongReturnTypeMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<wrong_is_always_strided<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class non_constexpr_is_always_strided : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<non_constexpr_is_always_strided>;
  static auto is_always_strided() -> bool { return true; }
};

/// @test Something that fulfils all the requirements except that @c is_always_strided is not constexpr is not a layout
/// mapping
TEST(IsLayoutMapping, NonConstexprIsAlwaysStridedMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<non_constexpr_is_always_strided<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class wrong_extents_type : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<wrong_extents_type>;
  using extents_type = int;
  auto extents() const -> extents_type const&;
};

/// @test Something that fulfils all the requirements except that @c extents_type is not an instantiation of @c extents
/// is not a layout mapping
TEST(IsLayoutMapping, WrongExtentsTypeMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<wrong_extents_type<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class wrong_index_type : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<wrong_index_type>;
  using index_type = int;
  auto required_span_size() const -> index_type;
};

/// @test Something that fulfils all the requirements except that @c index_type does not match that from @c extents_type
/// is not a layout mapping
TEST(IsLayoutMapping, WrongIndexTypeMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<wrong_index_type<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class wrong_rank_type : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<wrong_rank_type>;
  using rank_type = int;
};

/// @test Something that fulfils all the requirements except that @c rank_type does not match that from @c extents_type
/// is not a layout mapping
TEST(IsLayoutMapping, WrongRankTypeMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<wrong_rank_type<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class wrong_call_return_type : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<wrong_call_return_type>;
  auto operator()() -> bool;
};

/// @test Something that fulfils all the requirements except that @c operator() does not return the right type is not a
/// layout mapping
TEST(IsLayoutMapping, WrongCallOperatorReturnTypeMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<wrong_call_return_type<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class wrong_call_args_type : public basic_layout_mapping<Extents> {
 public:
  using layout_type = basic_mapping_policy<wrong_call_args_type>;
  auto operator()(int, int, int) -> std::size_t;
};

/// @test Something that fulfils all the requirements except that @c operator() does not take the right arguments is not
/// a layout mapping
TEST(IsLayoutMapping, WrongCallOperatorParamsMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<wrong_call_args_type<arene::base::extents<std::size_t>>>);
}

template <typename Extents>
class wrong_layout_type : public basic_layout_mapping<Extents> {
 public:
  using layout_type = int;
};

/// @test Something that fulfils all the requirements except that @c layout_type does not meet the requirements is not a
/// layout mapping
TEST(IsLayoutMapping, WrongLayoutTypeMeansNotALayoutMapping) {
  ASSERT_FALSE(arene::base::is_layout_mapping_v<wrong_layout_type<arene::base::extents<std::size_t>>>);
}

/// @test Check that our basic layout type is indeed a valid policy
TEST(IsLayoutMappingPolicy, BasicLayoutLayoutTypeIsAValidPolicy) {
  ASSERT_TRUE(arene::base::is_layout_mapping_policy_v<
              basic_layout_mapping<arene::base::extents<std::size_t>>::layout_type>);
}

/// @test Check integers are not a valid layout policy
TEST(IsLayoutMappingPolicy, IntIsNotAPolicy) { ASSERT_FALSE(arene::base::is_layout_mapping_policy_v<int>); }

/// @test Check empty classes are not a valid layout policy
TEST(IsLayoutMappingPolicy, EmptyClassIsNotAPolicy) {
  struct empty {};
  ASSERT_FALSE(arene::base::is_layout_mapping_policy_v<empty>);
}

class mapping_template_only {
 public:
  template <typename T>
  struct mapping {};
};

/// @test A class with a one-parameter @c mapping member template is structurally a policy, regardless of whether the
/// instantiated mapping is a valid layout mapping. Use @c is_layout_mapping_policy_for_extents_v for the deep check.
TEST(IsLayoutMappingPolicy, MappingMemberTemplateIsStructurallyAPolicy) {
  ASSERT_TRUE(arene::base::is_layout_mapping_policy_v<mapping_template_only>);
}

// Reproduces OSC-15066 / LWG 4582: a layout policy whose mapping body imposes a rank mandate via @c static_assert .
// Probing this with synthetic sample extents would fire the static_assert and break compilation. The structural trait
// must not instantiate @c mapping<E> for any @c E ; the per-extents trait must only instantiate for the caller's @c E .
template <typename Extents>
class rank_two_only_mapping : public basic_layout_mapping<Extents> {
  static_assert(Extents::rank() == 2U, "rank_two_only_mapping requires rank-2 extents");

 public:
  using layout_type = basic_mapping_policy<rank_two_only_mapping>;
};

using rank_two_only_layout = basic_mapping_policy<rank_two_only_mapping>;

/// @test A rank-mandated layout policy is still structurally a policy. Without this guarantee, the prior
/// implementation hit a hard @c static_assert during sample-extents probing (OSC-15066).
TEST(IsLayoutMappingPolicy, RankMandatedPolicyIsStructurallyAPolicy) {
  ASSERT_TRUE(arene::base::is_layout_mapping_policy_v<rank_two_only_layout>);
}

/// @test Check that a basic layout policy is a valid policy for the extents its mapping accepts
TEST(IsLayoutMappingPolicyForExtents, BasicLayoutPolicyAcceptsItsExtents) {
  using extents_type = arene::base::extents<std::size_t>;
  ASSERT_TRUE((arene::base::is_layout_mapping_policy_for_extents_v<
               basic_layout_mapping<extents_type>::layout_type,
               extents_type>));
}

/// @test Integers are not a policy for any extents
TEST(IsLayoutMappingPolicyForExtents, IntIsNotAPolicyForAnyExtents) {
  ASSERT_FALSE((arene::base::is_layout_mapping_policy_for_extents_v<int, arene::base::extents<std::size_t>>));
}

/// @test Empty classes are not a policy for any extents
TEST(IsLayoutMappingPolicyForExtents, EmptyClassIsNotAPolicyForAnyExtents) {
  struct empty {};
  ASSERT_FALSE((arene::base::is_layout_mapping_policy_for_extents_v<empty, arene::base::extents<std::size_t>>));
}

/// @test A structurally-shaped policy whose @c mapping<E> is not a valid layout mapping is rejected
TEST(IsLayoutMappingPolicyForExtents, MappingTemplateOnlyIsNotAPolicyForExtents) {
  ASSERT_FALSE(
      (arene::base::is_layout_mapping_policy_for_extents_v<mapping_template_only, arene::base::extents<std::size_t>>)
  );
}

/// @test A rank-mandated layout policy is a valid policy for extents that satisfy the mandate
TEST(IsLayoutMappingPolicyForExtents, RankMandatedPolicyAcceptsRankTwoExtents) {
  ASSERT_TRUE((arene::base::is_layout_mapping_policy_for_extents_v<
               rank_two_only_layout,
               arene::base::extents<std::size_t, 2, 2>>));
}

/// @test A type that is not a specialization of @c extents is not a valid second template argument
TEST(IsLayoutMappingPolicyForExtents, NonExtentsSecondArgumentIsRejected) {
  using policy = basic_layout_mapping<arene::base::extents<std::size_t>>::layout_type;
  ASSERT_FALSE((arene::base::is_layout_mapping_policy_for_extents_v<policy, int>));
}

}  // namespace
