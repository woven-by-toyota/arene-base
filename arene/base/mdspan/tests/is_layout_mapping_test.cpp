// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/is_layout_mapping.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace {

template <template <typename> class LayoutMapping>
class basic_mapping_policy {
 public:
  template <typename E>
  using mapping = LayoutMapping<E>;
};

template <typename Extents>
class basic_layout_mapping {
 public:
  using rank_type = std::size_t;
  using extents_type = Extents;
  using index_type = typename Extents::index_type;

  using layout_type = basic_mapping_policy<basic_layout_mapping>;

  auto operator==(basic_layout_mapping const&) const -> bool;

  auto extents() const -> extents_type const&;
  template <typename... Indices>
  auto operator()(Indices...) const -> index_type;

  auto required_span_size() const -> index_type;
  auto is_unique() const -> bool;
  auto is_exhaustive() const -> bool;
  auto is_strided() const -> bool;

  auto stride(rank_type rank) const -> index_type;

  static constexpr auto is_always_unique() -> bool { return false; }
  static constexpr auto is_always_exhaustive() -> bool { return false; }
  static constexpr auto is_always_strided() -> bool { return false; }
};

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

class bad_mapping {
 public:
  template <typename T>
  struct mapping {};
};

/// @test A class with a mapping template that does not yield a layout mapping is not a policy
TEST(IsLayoutMappingPolicy, WrongMappingIsNotAPolicy) {
  ASSERT_FALSE(arene::base::is_layout_mapping_policy_v<bad_mapping>);
}

}  // namespace
