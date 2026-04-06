// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/semantic_version/semantic_version.hpp"

#include <gtest/gtest.h>

#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/testing/gtest.hpp"

// These are undefined by semantic_version.hpp, but it seems like they are
// redefined by some header included by gtest.h
#undef major
#undef minor

namespace {

using ::arene::base::compute_difference;
using ::arene::base::is_major_change;
using ::arene::base::is_minor_change;
using ::arene::base::is_patch_change;
using ::arene::base::sem_ver_diff;
using ::arene::base::semantic_version;
using ::arene::base::strong_ordering;

constexpr semantic_version ver000{};
constexpr semantic_version ver100 = semantic_version{1};
constexpr semantic_version ver010{0, 1};
constexpr semantic_version ver001{0, 0, 1};
constexpr semantic_version ver110{1, 1};
constexpr semantic_version ver011{0, 1, 1};
constexpr semantic_version ver101{1, 0, 1};
constexpr semantic_version ver111{1, 1, 1};
constexpr semantic_version ver200{2, 0, 0};

/// @test A default-constructed `semantic_version` is 0.0.0
TEST(SemanticVersion, DefaultCtorProducesAllZeros) {
  STATIC_ASSERT_EQ(ver000.major(), 0);
  STATIC_ASSERT_EQ(ver000.minor(), 0);
  STATIC_ASSERT_EQ(ver000.patch(), 0);
  STATIC_ASSERT_EQ(semantic_version{}, semantic_version(0, 0, 0));
}

/// @test A `semantic_version` constructed with a specific version has the appropriate major/minor/patch values
TEST(SemanticVersion, VersionConsumingCtorStoresVersions) {
  STATIC_ASSERT_EQ(ver100.major(), 1);
  STATIC_ASSERT_EQ(ver100.minor(), 0);
  STATIC_ASSERT_EQ(ver100.patch(), 0);

  STATIC_ASSERT_EQ(ver010.major(), 0);
  STATIC_ASSERT_EQ(ver010.minor(), 1);
  STATIC_ASSERT_EQ(ver010.patch(), 0);

  STATIC_ASSERT_EQ(ver001.major(), 0);
  STATIC_ASSERT_EQ(ver001.minor(), 0);
  STATIC_ASSERT_EQ(ver001.patch(), 1);

  STATIC_ASSERT_EQ(ver110.major(), 1);
  STATIC_ASSERT_EQ(ver110.minor(), 1);
  STATIC_ASSERT_EQ(ver110.patch(), 0);

  STATIC_ASSERT_EQ(ver011.major(), 0);
  STATIC_ASSERT_EQ(ver011.minor(), 1);
  STATIC_ASSERT_EQ(ver011.patch(), 1);

  STATIC_ASSERT_EQ(ver101.major(), 1);
  STATIC_ASSERT_EQ(ver101.minor(), 0);
  STATIC_ASSERT_EQ(ver101.patch(), 1);

  STATIC_ASSERT_EQ(ver111.major(), 1);
  STATIC_ASSERT_EQ(ver111.minor(), 1);
  STATIC_ASSERT_EQ(ver111.patch(), 1);

  STATIC_ASSERT_EQ(ver200.major(), 2);
  STATIC_ASSERT_EQ(ver200.minor(), 0);
  STATIC_ASSERT_EQ(ver200.patch(), 0);
}

/// @test A default-constructed `semantic_version` compares equal to one constructed with an explicit version of zero
TEST(SemanticVersion, ConstructorsIdenticalSemantics) {
  constexpr semantic_version ver000_other = semantic_version{0};

  STATIC_ASSERT_EQ(ver000, ver000);
  STATIC_ASSERT_LE(ver000, ver000);
  STATIC_ASSERT_GE(ver000, ver000);

  STATIC_ASSERT_FALSE(ver000 != ver000);
  STATIC_ASSERT_FALSE(ver000 > ver000);
  STATIC_ASSERT_FALSE(ver000 < ver000);

  STATIC_ASSERT_EQ(ver000, ver000_other);
  STATIC_ASSERT_LE(ver000, ver000_other);
  STATIC_ASSERT_GE(ver000, ver000_other);
}

/// @test `semantic_version` comparisons give lexicographic ordering on major/minor/patch
TEST(SemanticVersion, ThreeWayCompareIsLexicographicalSortByMajorMinorPatch) {
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver000, ver000), strong_ordering::equal);
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver001, ver001), strong_ordering::equal);
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver010, ver010), strong_ordering::equal);
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver100, ver100), strong_ordering::equal);
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver110, ver110), strong_ordering::equal);
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver111, ver111), strong_ordering::equal);

  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver000, ver001), strong_ordering::less);
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver000, ver010), strong_ordering::less);
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver000, ver100), strong_ordering::less);
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver000, ver110), strong_ordering::less);
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver000, ver111), strong_ordering::less);
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver010, ver111), strong_ordering::less);
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver100, ver111), strong_ordering::less);

  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver001, ver000), strong_ordering::greater);
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver010, ver000), strong_ordering::greater);
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver100, ver000), strong_ordering::greater);
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver110, ver000), strong_ordering::greater);
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver111, ver000), strong_ordering::greater);
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver111, ver001), strong_ordering::greater);
  STATIC_ASSERT_EQ(semantic_version::three_way_compare(ver111, ver010), strong_ordering::greater);
}

/// @test The comparison operators for `semantic_version` yield results consistent with the lexicographic ordering
TEST(SemanticVersion, ComparisonOperatorsAreDefined) {
  STATIC_ASSERT_EQ(ver111, ver111);
  STATIC_ASSERT_NE(ver000, ver111);
  STATIC_ASSERT_LT(ver010, ver100);
  STATIC_ASSERT_LE(ver010, ver010);
  STATIC_ASSERT_LE(ver010, ver100);
  STATIC_ASSERT_GT(ver111, ver001);
  STATIC_ASSERT_GE(ver111, ver111);
  STATIC_ASSERT_GE(ver111, ver101);
}

/// @test `is_unstable` returns `true` if the major version is zero, `false` otherwise
TEST(SemanticVersion, IsUnstableIsTrueIffMajorIsZero) {
  STATIC_ASSERT_TRUE(ver000.is_unstable());
  STATIC_ASSERT_TRUE(ver010.is_unstable());
  STATIC_ASSERT_TRUE(ver001.is_unstable());
  STATIC_ASSERT_TRUE(ver011.is_unstable());
  STATIC_ASSERT_FALSE(ver111.is_unstable());
  STATIC_ASSERT_FALSE(ver100.is_unstable());
  STATIC_ASSERT_FALSE(ver101.is_unstable());
  STATIC_ASSERT_FALSE(ver110.is_unstable());
  STATIC_ASSERT_FALSE(ver200.is_unstable());
}

/// @test `compute_difference` returns `none` when given identical versions
TEST(ComputeDifference, ReturnsNoneForEqualVersions) {
  STATIC_ASSERT_EQ(compute_difference(ver000, ver000), sem_ver_diff::none);
  STATIC_ASSERT_EQ(compute_difference(ver001, ver001), sem_ver_diff::none);
  STATIC_ASSERT_EQ(compute_difference(ver010, ver010), sem_ver_diff::none);
  STATIC_ASSERT_EQ(compute_difference(ver100, ver100), sem_ver_diff::none);
  STATIC_ASSERT_EQ(compute_difference(ver101, ver101), sem_ver_diff::none);
  STATIC_ASSERT_EQ(compute_difference(ver111, ver111), sem_ver_diff::none);
}

/// @test `compute_difference` returns `major` when the versions differ in the major number but not the minor or patch
/// numbers
TEST(ComputeDifference, ReturnsMajorForPureMajorDelta) {
  STATIC_ASSERT_EQ(compute_difference(ver000, ver100), sem_ver_diff::major);
  STATIC_ASSERT_EQ(compute_difference(ver100, ver000), sem_ver_diff::major);
}

/// @test `compute_difference` returns `minor` when the versions differ in the minor number but not the major or patch
/// numbers
TEST(ComputeDifference, ReturnsMinorForPureMinorDelta) {
  STATIC_ASSERT_EQ(compute_difference(ver000, ver010), sem_ver_diff::minor);
  STATIC_ASSERT_EQ(compute_difference(ver010, ver000), sem_ver_diff::minor);
  STATIC_ASSERT_EQ(compute_difference(ver100, ver110), sem_ver_diff::minor);
  STATIC_ASSERT_EQ(compute_difference(ver110, ver100), sem_ver_diff::minor);
}

/// @test `compute_difference` returns `patch` when the versions differ in the patch number, but not the major or minor
/// numbers
TEST(ComputeDifference, ReturnsPatchForPurePatchDelta) {
  STATIC_ASSERT_EQ(compute_difference(ver000, ver001), sem_ver_diff::patch);
  STATIC_ASSERT_EQ(compute_difference(ver001, ver000), sem_ver_diff::patch);
  STATIC_ASSERT_EQ(compute_difference(ver100, ver101), sem_ver_diff::patch);
  STATIC_ASSERT_EQ(compute_difference(ver110, ver111), sem_ver_diff::patch);
}

/// @test `compute_difference` returns `major_minor` when the versions differ in the major and minor numbers but not the
/// patch number
TEST(ComputeDifference, ReturnsMajorMinorForMajorPlusMinorDelta) {
  STATIC_ASSERT_EQ(compute_difference(ver000, ver110), sem_ver_diff::major_minor);
  STATIC_ASSERT_EQ(compute_difference(ver110, ver000), sem_ver_diff::major_minor);
}

/// @test `compute_difference` returns `major_patch` when the versions differ in the major and patch numbers but not the
/// minor number
TEST(ComputeDifference, ReturnsMajorPatchForMajorPlusPatchDelta) {
  STATIC_ASSERT_EQ(compute_difference(ver000, ver101), sem_ver_diff::major_patch);
  STATIC_ASSERT_EQ(compute_difference(ver101, ver000), sem_ver_diff::major_patch);
}

/// @test `compute_difference` returns `minor_patch` when the versions differ in the minor and patch numbers but not the
/// major number
TEST(ComputeDifference, ReturnsMinorPatchForMinorPlusPatchDelta) {
  STATIC_ASSERT_EQ(compute_difference(ver000, ver011), sem_ver_diff::minor_patch);
  STATIC_ASSERT_EQ(compute_difference(ver011, ver000), sem_ver_diff::minor_patch);
}

/// @test `compute_difference` returns `major_minor_patch` when the versions differ in the major, minor and patch number
TEST(ComputeDifference, ReturnsMajorMinorPatchForMajorPlusMinorPlusPatchDelta) {
  STATIC_ASSERT_EQ(compute_difference(ver000, ver111), sem_ver_diff::major_minor_patch);
  STATIC_ASSERT_EQ(compute_difference(ver111, ver000), sem_ver_diff::major_minor_patch);
}

/// @test `contains_major_change` returns `true` when the versions differ in the major number, `false` otherwise
TEST(ContainsMajorChange, ReturnsTrueForVersionsThatVaryByMajor) {
  STATIC_ASSERT_TRUE(contains_major_change(sem_ver_diff::major));
  STATIC_ASSERT_TRUE(contains_major_change(sem_ver_diff::major_minor));
  STATIC_ASSERT_TRUE(contains_major_change(sem_ver_diff::major_patch));
  STATIC_ASSERT_TRUE(contains_major_change(sem_ver_diff::major_minor_patch));
  STATIC_ASSERT_TRUE(contains_major_change(ver000, ver100));
  STATIC_ASSERT_TRUE(contains_major_change(ver000, ver110));
  STATIC_ASSERT_TRUE(contains_major_change(ver000, ver101));
  STATIC_ASSERT_TRUE(contains_major_change(ver000, ver111));

  STATIC_ASSERT_FALSE(contains_major_change(sem_ver_diff::minor));
  STATIC_ASSERT_FALSE(contains_major_change(sem_ver_diff::minor_patch));
  STATIC_ASSERT_FALSE(contains_major_change(ver000, ver000));
  STATIC_ASSERT_FALSE(contains_major_change(ver000, ver010));
  STATIC_ASSERT_FALSE(contains_major_change(ver000, ver001));
  STATIC_ASSERT_FALSE(contains_major_change(ver000, ver011));
}

/// @test `is_major_change` returns `true` when the versions differ in the major number, `false` otherwise
TEST(IsMajorChange, ReturnsTrueForVersionsThatVaryByMajor) {
  STATIC_ASSERT_TRUE(is_major_change(sem_ver_diff::major));
  STATIC_ASSERT_TRUE(is_major_change(sem_ver_diff::major_minor));
  STATIC_ASSERT_TRUE(is_major_change(sem_ver_diff::major_patch));
  STATIC_ASSERT_TRUE(is_major_change(sem_ver_diff::major_minor_patch));
  STATIC_ASSERT_TRUE(is_major_change(ver000, ver100));
  STATIC_ASSERT_TRUE(is_major_change(ver000, ver110));
  STATIC_ASSERT_TRUE(is_major_change(ver000, ver101));
  STATIC_ASSERT_TRUE(is_major_change(ver000, ver111));

  STATIC_ASSERT_FALSE(is_major_change(sem_ver_diff::minor));
  STATIC_ASSERT_FALSE(is_major_change(sem_ver_diff::minor_patch));
  STATIC_ASSERT_FALSE(is_major_change(ver000, ver000));
  STATIC_ASSERT_FALSE(is_major_change(ver000, ver010));
  STATIC_ASSERT_FALSE(is_major_change(ver000, ver001));
  STATIC_ASSERT_FALSE(is_major_change(ver000, ver011));
}

/// @test `contains_minor_change` returns `true` when the versions differ in the minor number, `false` otherwise
TEST(ContainsMinorChange, ReturnsTrueForVersionsThatVaryByMinor) {
  STATIC_ASSERT_TRUE(contains_minor_change(sem_ver_diff::minor));
  STATIC_ASSERT_TRUE(contains_minor_change(sem_ver_diff::major_minor));
  STATIC_ASSERT_TRUE(contains_minor_change(sem_ver_diff::major_minor_patch));
  STATIC_ASSERT_TRUE(contains_minor_change(ver000, ver011));
  STATIC_ASSERT_TRUE(contains_minor_change(ver000, ver110));
  STATIC_ASSERT_TRUE(contains_minor_change(ver000, ver111));
  STATIC_ASSERT_TRUE(contains_minor_change(ver010, ver101));
  STATIC_ASSERT_TRUE(contains_minor_change(ver110, ver101));
  STATIC_ASSERT_TRUE(contains_minor_change(ver011, ver101));

  STATIC_ASSERT_FALSE(contains_minor_change(sem_ver_diff::major));
  STATIC_ASSERT_FALSE(contains_minor_change(sem_ver_diff::major_patch));
  STATIC_ASSERT_FALSE(contains_minor_change(ver000, ver000));
  STATIC_ASSERT_FALSE(contains_minor_change(ver000, ver100));
  STATIC_ASSERT_FALSE(contains_minor_change(ver000, ver101));
  STATIC_ASSERT_FALSE(contains_minor_change(ver100, ver000));
  STATIC_ASSERT_FALSE(contains_minor_change(ver101, ver000));
}

/// @test `is_minor_change` returns `true` when the versions differ in the minor number but not the major number,
/// `false` otherwise
TEST(IsMinorChange, ReturnsTrueOnlyForVersionsThatVaryByMinorAndNotMajor) {
  STATIC_ASSERT_TRUE(is_minor_change(sem_ver_diff::minor));
  STATIC_ASSERT_TRUE(is_minor_change(sem_ver_diff::minor_patch));
  STATIC_ASSERT_TRUE(is_minor_change(ver000, ver010));
  STATIC_ASSERT_TRUE(is_minor_change(ver000, ver011));

  STATIC_ASSERT_FALSE(is_minor_change(sem_ver_diff::major));
  STATIC_ASSERT_FALSE(is_minor_change(sem_ver_diff::patch));
  STATIC_ASSERT_FALSE(is_minor_change(sem_ver_diff::major_patch));
  STATIC_ASSERT_FALSE(is_minor_change(sem_ver_diff::major_patch));
  STATIC_ASSERT_FALSE(is_minor_change(ver000, ver000));
  STATIC_ASSERT_FALSE(is_minor_change(ver000, ver100));
  STATIC_ASSERT_FALSE(is_minor_change(ver000, ver001));
  STATIC_ASSERT_FALSE(is_minor_change(ver000, ver111));
}

/// @test `contains_patch_change` returns `true` when the versions differ in the patch number, `false` otherwise
TEST(ContainsPatchChange, ReturnsTrueForVersionsThatVaryByPatch) {
  STATIC_ASSERT_TRUE(contains_patch_change(sem_ver_diff::patch));
  STATIC_ASSERT_TRUE(contains_patch_change(sem_ver_diff::major_patch));
  STATIC_ASSERT_TRUE(contains_patch_change(sem_ver_diff::minor_patch));
  STATIC_ASSERT_TRUE(contains_patch_change(sem_ver_diff::major_minor_patch));
  STATIC_ASSERT_TRUE(contains_patch_change(ver000, ver001));
  STATIC_ASSERT_TRUE(contains_patch_change(ver000, ver011));
  STATIC_ASSERT_TRUE(contains_patch_change(ver000, ver101));
  STATIC_ASSERT_TRUE(contains_patch_change(ver000, ver111));
  STATIC_ASSERT_TRUE(contains_patch_change(ver111, ver000));
  STATIC_ASSERT_TRUE(contains_patch_change(ver111, ver010));
  STATIC_ASSERT_TRUE(contains_patch_change(ver111, ver100));

  STATIC_ASSERT_FALSE(contains_patch_change(sem_ver_diff::major));
  STATIC_ASSERT_FALSE(contains_patch_change(sem_ver_diff::major_minor));
  STATIC_ASSERT_FALSE(contains_patch_change(ver000, ver000));
  STATIC_ASSERT_FALSE(contains_patch_change(ver000, ver100));
  STATIC_ASSERT_FALSE(contains_patch_change(ver000, ver110));
  STATIC_ASSERT_FALSE(contains_patch_change(ver100, ver000));
  STATIC_ASSERT_FALSE(contains_patch_change(ver110, ver000));
  STATIC_ASSERT_FALSE(contains_patch_change(ver101, ver101));
}

/// @test `is_patch_change` returns `true` when the versions differ in the patch number, but not the major or minor
/// number, `false` otherwise
TEST(IsPatchChange, ReturnsTrueOnlyForVersionsThatVaryByPatchAndNotMajorOrMinor) {
  STATIC_ASSERT_TRUE(is_patch_change(sem_ver_diff::patch));
  STATIC_ASSERT_TRUE(is_patch_change(ver000, ver001));
  STATIC_ASSERT_TRUE(is_patch_change(ver100, ver101));
  STATIC_ASSERT_TRUE(is_patch_change(ver110, ver111));

  STATIC_ASSERT_FALSE(is_patch_change(sem_ver_diff::major));
  STATIC_ASSERT_FALSE(is_patch_change(sem_ver_diff::minor));
  STATIC_ASSERT_FALSE(is_patch_change(sem_ver_diff::major_patch));
  STATIC_ASSERT_FALSE(is_patch_change(sem_ver_diff::minor_patch));
  STATIC_ASSERT_FALSE(is_patch_change(sem_ver_diff::major_patch));
  STATIC_ASSERT_FALSE(is_patch_change(ver000, ver000));
  STATIC_ASSERT_FALSE(is_patch_change(ver000, ver010));
  STATIC_ASSERT_FALSE(is_patch_change(ver000, ver100));
  STATIC_ASSERT_FALSE(is_patch_change(ver000, ver101));
  STATIC_ASSERT_FALSE(is_patch_change(ver100, ver110));
  STATIC_ASSERT_FALSE(is_patch_change(ver101, ver111));
}

}  // namespace
