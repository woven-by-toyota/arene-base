// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file type_traits_test.cpp
/// @brief Provides unit tests to validate export of content in type_traits.hpp
///

#include "arene/base/type_traits.hpp"

#include <gtest/gtest.h>

namespace {

/// @test `always_false_v` is declared
TEST(TypeTraits, AlwaysFalseV) { using arene::base::always_false_v; }

/// @test `all_are_same_v` is declared
TEST(TypeTraits, AllAreSameV) { using arene::base::all_are_same_v; }

/// @test `all_of_v` is declared
TEST(TypeTraits, AllOfV) { using arene::base::all_of_v; }
/// @test `any_of_v` is declared
TEST(TypeTraits, AnyOfV) { using arene::base::any_of_v; }
/// @test `none_of_v` is declared
TEST(TypeTraits, NoneOfV) { using arene::base::none_of_v; }

/// @test `decays_to_v` is declared
TEST(TypeTraits, DecaysTo) { using arene::base::decays_to_v; }

/// @test `is_less_than_comparable_v` is declared
TEST(TypeTraits, IsLessThanComparableV) { using arene::base::is_less_than_comparable_v; }
/// @test `is_less_than_or_equal_comparable_v` is declared
TEST(TypeTraits, IsLessThanOrEqualComparableV) { using arene::base::is_less_than_or_equal_comparable_v; }
/// @test `is_greater_than_comparable_v` is declared
TEST(TypeTraits, IsGreaterThanComparableV) { using arene::base::is_greater_than_comparable_v; }
/// @test `is_greater_than_or_equal_comparable_v` is declared
TEST(TypeTraits, IsGreaterThanOrEqualComparableV) { using arene::base::is_greater_than_or_equal_comparable_v; }
/// @test `is_equality_comparable_v` is declared
TEST(TypeTraits, IsEqualityComparableV) { using arene::base::is_equality_comparable_v; }
/// @test `is_inequality_comparable_v` is declared
TEST(TypeTraits, IsInequalityComparableV) { using arene::base::is_inequality_comparable_v; }

/// @test `denotes_range_v` is declared
TEST(TypeTraits, IsIterableRangeV) { using arene::base::denotes_range_v; }
/// @test `denotes_nothrow_iterable_range_v` is declared
TEST(TypeTraits, IsNothrowIterableRangeV) { using arene::base::denotes_nothrow_iterable_range_v; }

/// @test `index_of` is declared
TEST(TypeTraits, IndexOf) { using arene::base::index_of; }
/// @test `index_of_v` is declared
TEST(TypeTraits, IndexOfV) { using arene::base::index_of_v; }

/// @test `is_array_convertible` is declared
TEST(TypeTraits, IsArrayConvertible) { using arene::base::is_array_convertible; }
/// @test `is_array_convertible_v` is declared
TEST(TypeTraits, IsArrayConvertibleV) { using arene::base::is_array_convertible_v; }

/// @test `is_copyable_v` is declared
TEST(TypeTraits, IsCopyableV) { using arene::base::is_copyable_v; }

/// @test `is_one_of` is declared
TEST(TypeTraits, IsOneOf) { using arene::base::is_one_of; }
/// @test `is_one_of_v` is declared
TEST(TypeTraits, IsOneOfV) { using arene::base::is_one_of_v; }

/// @test `is_output_iterator_v` is declared
TEST(TypeTraits, IsOutputIteratorV) { using arene::base::is_output_iterator_v; }
/// @test `is_input_iterator_v` is declared
TEST(TypeTraits, IsInputIteratorV) { using arene::base::is_input_iterator_v; }
/// @test `is_forward_iterator_v` is declared
TEST(TypeTraits, IsForwardIteratorV) { using arene::base::is_forward_iterator_v; }
/// @test `is_bidirectional_iterator_v` is declared
TEST(TypeTraits, IsBidirectionalIteratorV) { using arene::base::is_bidirectional_iterator_v; }
/// @test `is_random_access_iterator_v` is declared
TEST(TypeTraits, IsRandomAccessIteratorV) { using arene::base::is_random_access_iterator_v; }
/// @test `is_iterator_v` is declared
TEST(TypeTraits, IsIteratorV) { using arene::base::is_iterator_v; }

/// @test `remove_cv_t` is declared
TEST(TypeTraits, RemoveCvT) { using arene::base::remove_cv_t; }
/// @test `remove_reference_t` is declared
TEST(TypeTraits, RemoveReferenceT) { using arene::base::remove_reference_t; }
/// @test `remove_cvref` is declared
TEST(TypeTraits, RemoveCvRef) { using arene::base::remove_cvref; }
/// @test `remove_cvref_t` is declared
TEST(TypeTraits, RemoveCvrefT) { using arene::base::remove_cvref_t; }

/// @test `is_swappable_v` is declared
TEST(TypeTraits, IsSwappableV) { using arene::base::is_swappable_v; }
/// @test `is_nothrow_swappable_v` is declared
TEST(TypeTraits, IsNothrowSwappable) { using arene::base::is_nothrow_swappable_v; }

/// @test `type_identity` is declared
TEST(TypeTraits, TypeIdentity) { using arene::base::type_identity; }
/// @test `type_identity_t` is declared
TEST(TypeTraits, TypeIdentityT) { using arene::base::type_identity_t; }

/// @test `is_invocable` is declared
TEST(TypeTraits, IsInvocable) { using arene::base::is_invocable; }
/// @test `is_invocable_v` is declared
TEST(TypeTraits, IsInvocableV) { using arene::base::is_invocable_v; }
/// @test `is_invocable_r` is declared
TEST(TypeTraits, IsInvocableR) { using arene::base::is_invocable_r; }
/// @test `is_invocable_r_v` is declared
TEST(TypeTraits, IsInvocableRV) { using arene::base::is_invocable_r_v; }
/// @test `is_nothrow_invocable` is declared
TEST(TypeTraits, IsNothrowInvocable) { using arene::base::is_nothrow_invocable; }
/// @test `is_nothrow_invocable_v` is declared
TEST(TypeTraits, IsNothrowInvocableV) { using arene::base::is_nothrow_invocable_v; }
/// @test `is_nothrow_invocable_r` is declared
TEST(TypeTraits, IsNothrowInvocableR) { using arene::base::is_nothrow_invocable_r; }
/// @test `is_nothrow_invocable_r_v` is declared
TEST(TypeTraits, IsNothrowInvocableRV) { using arene::base::is_nothrow_invocable_r_v; }

/// @test `is_reference_wrapper_v` is declared
TEST(TypeTraits, IsReferenceWrapperV) { using arene::base::is_reference_wrapper_v; }

/// @test `member_pointer_class_type` is declared
TEST(TypeTraits, MemberPointerClassType) { using arene::base::member_pointer_class_type; }

/// @test `void_t` is declared
TEST(TypeTraits, VoidT) { using arene::base::void_t; }

/// @test `has_overloaded_address_operator` and `has_overloaded_address_operator_v` are declared
TEST(TypeTraits, HasOverloadedAddressOperator) {
  using arene::base::has_overloaded_address_operator;
  using arene::base::has_overloaded_address_operator_v;
}

/// @test `is_transparent_comparator` is declared
TEST(TypeTraits, IsTransparentComparator) { using arene::base::is_transparent_comparator; }
/// @test `is_transparent_comparator_v` is declared
TEST(TypeTraits, IsTransparentComparatorV) { using arene::base::is_transparent_comparator_v; }
/// @test `is_transparent_comparator_for` is declared
TEST(TypeTraits, IsTransparentComparatorFor) { using arene::base::is_transparent_comparator_for; }
/// @test `is_transparent_comparator_for_v` is declared
TEST(TypeTraits, IsTransparentComparatorForV) { using arene::base::is_transparent_comparator_for_v; }

/// @test `conjunction` is declared
TEST(TypeTraits, Conjunction) { using arene::base::conjunction; }
/// @test `conjunction_v` is declared
TEST(TypeTraits, ConjunctionV) { using arene::base::conjunction_v; }
/// @test `disjunction` is declared
TEST(TypeTraits, Disjunction) { using arene::base::disjunction; }
/// @test `disjunction_v` is declared
TEST(TypeTraits, DisjunctionV) { using arene::base::disjunction_v; }
/// @test `negation` is declared
TEST(TypeTraits, Negation) { using arene::base::negation; }
/// @test `negation_v` is declared
TEST(TypeTraits, NegationV) { using arene::base::negation_v; }

/// @test `is_implicity_constructible` is declared
TEST(TypeTraits, IsImplicitlyConstructible) { using arene::base::is_implicitly_constructible; }
/// @test `is_implicity_constructible_v` is declared
TEST(TypeTraits, IsImplicitlyConstructibleV) { using arene::base::is_implicitly_constructible_v; }

/// @test `is_only_explicitly_constructible` is declared
TEST(TypeTraits, IsOnlyExplicitlyConstructible) { using arene::base::is_only_explicitly_constructible; }
/// @test `is_only_explicitly_constructible_v` is declared
TEST(TypeTraits, IsOnlyExplicitlyConstructibleV) { using arene::base::is_only_explicitly_constructible_v; }

/// @test `is_tuple_like_v` is declared
TEST(TypeTraits, IsTupleLikeV) { using arene::base::is_tuple_like_v; }
/// @test `is_pair_like_v` is declared
TEST(TypeTraits, IsPairLikeV) { using arene::base::is_pair_like_v; }

/// @test `is_integral_constant_like` is declared
TEST(TypeTraits, IsIntegralConstantLike) { using arene::base::is_integral_constant_like; }
/// @test `is_integral_constant_like_v` is declared
TEST(TypeTraits, IsIntegralConstantLikeV) { using arene::base::is_integral_constant_like_v; }

}  // namespace
