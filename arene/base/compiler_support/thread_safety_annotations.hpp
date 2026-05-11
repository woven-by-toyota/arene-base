// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file thread_safety_annotations.hpp
/// @brief Provides a stable interface to clang's Thread Safety Annotations
///

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_THREAD_SAFETY_ANNOTATIONS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_THREAD_SAFETY_ANNOTATIONS_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

// NOLINTBEGIN(cppcoreguidelines-macro-usage) These are wrappers around existing macros that must be macros.

#if ARENE_IS_ON(ARENE_HAS_THREAD_SAFETY_ANALYSIS)
#include "arene/base/compiler_support/detail/thread_safety_annotations_enabled.hpp"
#else
#include "arene/base/compiler_support/detail/thread_safety_annotations_disabled.hpp"
#endif
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A16_0_1-d "Conditional defines permitted by A16-0-1 Permit #2"

///
/// @brief Annotates a type as a "capability," which is a type that provides synchronization protection.
/// @see https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#capability-string
/// @param x The name of the type; used in generating error messages.
///
#define ARENE_TSA_CAPABILITY(x) ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(capability(x))

///
/// @brief Annotates a type as a "scoped capability," which is a type that provides RAII synchronization protection.
/// @see https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#scoped-capability
///
#define ARENE_TSA_SCOPED_CAPABILITY ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(scoped_lockable)

///
/// @brief Annotates a member as being guarded by a given capability.
/// @see https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#guarded-by-c-and-pt-guarded-by-c
/// @param x The name of the member which provides the capability that guards the annotated member
///
#define ARENE_TSA_GUARDED_BY(x) ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(guarded_by(x))

///
/// @brief Annotates a member that is a (smart)pointer's _pointed to content_ as being guarded by a given capability.
/// @see https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#guarded-by-c-and-pt-guarded-by-c
/// @param x The name of the member which provides the capability that guards the annotated pointer-member
/// @note This does _not_ mark the pointer value itself as requiring synchronization. Only dereferencing it.
///
#define ARENE_TSA_PT_GUARDED_BY(x) ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(pt_guarded_by(x))

///
/// @brief Annotates a function or method as needing the calling thread to hold **exclusive** access to the named
///        capabilities _before entering_ and _after exit_.
/// @see https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#requires-requires-shared
/// @param ... A comma-separated list of the names of members/variables which provide the needed capabilities.
///
#define ARENE_TSA_REQUIRES(...) ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(requires_capability(__VA_ARGS__))

///
/// @brief Annotates a function or method as needing the calling thread to hold **shared** access to the named
///        capabilities _before entering_ and _after exit_.
/// @see https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#requires-requires-shared
/// @param ... A comma-separated list of the names of members/variables which provide the needed capabilities.
///
#define ARENE_TSA_REQUIRES_SHARED(...) \
  ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(requires_shared_capability(__VA_ARGS__))

///
/// @brief Annotates a function or method as acquiring **exclusive** access to the named capabilities _before exit_,
///        **without releasing them**  on exit.
/// @see
/// https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#acquire-acquire-shared-release-release-shared-release-generic
/// @param ... A comma-separated list of the names of members/variables which provide the capabilities to be acquired.
///            If omitted, assumed to be @c this .
///
#define ARENE_TSA_ACQUIRE(...) ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(acquire_capability(__VA_ARGS__))

///
/// @brief Annotates a function or method as acquiring **shared** access to the named capabilities _before exit_,
///        **without releasing them** on exit.
/// @see
/// https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#acquire-acquire-shared-release-release-shared-release-generic
/// @param ... A comma-separated list of the names of members/variables which provide the capabilities to be acquired.
///            If omitted, assumed to be @c this .
///
#define ARENE_TSA_ACQUIRE_SHARED(...) \
  ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(acquire_shared_capability(__VA_ARGS__))

///
/// @brief Annotates a function or method as releasing access to the named capabilities _before exit_. The calling
///        thread must hold **exclusive** access to the named capabilities _before entering_.
/// @see
/// https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#acquire-acquire-shared-release-release-shared-release-generic
/// @param ... A comma-separated list of the names of members/variables which provide the capabilities to be released.
///            If omitted, assumed to be @c this .
///
#define ARENE_TSA_RELEASE(...) ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(release_capability(__VA_ARGS__))

///
/// @brief Annotates a function or method as releasing access to the named capabilities _before exit_. The calling
///        thread must hold **shared** access to the named capabilities _before entering_.
/// @see
/// https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#acquire-acquire-shared-release-release-shared-release-generic
/// @param ... A comma-separated list of the names of members/variables which provide the capabilities to be released.
///            If omitted, assumed to be @c this .
///
#define ARENE_TSA_RELEASE_SHARED(...) \
  ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(release_shared_capability(__VA_ARGS__))

///
/// @brief Annotates a function or method as releasing access to the named capabilities _before exit_. The calling
///        thread can hold either **exclusive** or **shared** access to the named capabilities _before entering_.
/// @see
/// https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#acquire-acquire-shared-release-release-shared-release-generic
/// @param ... A comma-separated list of the names of members/variables which provide the capabilities to be released.
///            If omitted, assumed to be @c this .
///
#define ARENE_TSA_RELEASE_GENERIC(...) \
  ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(release_generic_capability(__VA_ARGS__))

///
/// @brief Annotates a function or method as attempting to acquire **exclusive** access to the named capabilities
///        _before exit_, **without releasing them**  on exit, and returning a bool of success/failure.
/// @see https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#try-acquire-bool-try-acquire-shared-bool
/// @param ... A boolean value, followed by a comma-separated list of the names of members/variables which provide the
///            capabilities to be acquired. The boolean value represents which boolean value represents "success" for
///            the assertion. If the name of the capability(s) are omitted, the capability is assumed to be @c this .
/// @warning Because TSA does not support conditional locking analysis, the capability will assume to have been locked
///          after the first branch based on the return value from the annotated function. Therefore, callers of the
///          annotated function must have the "success" path as the first branch.
///
#define ARENE_TSA_TRY_ACQUIRE(...) ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(try_acquire_capability(__VA_ARGS__))

///
/// @brief Annotates a function or method as attempting to acquire **shared** access to the named capabilities
///        _before exit_, **without releasing them**  on exit, and returning a bool of success/failure.
/// @see https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#try-acquire-bool-try-acquire-shared-bool
/// @param ... A boolean value, followed by a comma-separated list of the names of members/variables which provide the
///            capabilities to be acquired. The boolean value represents which boolean value represents "success" for
///            the assertion. If the name of the capability(s) are omitted, the capability is assumed to be @c this .
/// @warning Because TSA does not support conditional locking analysis, the capability will assume to have been locked
///          after the first branch based on the return value from the annotated function. Therefore, callers of the
///          annotated function must have the "success" path as the first branch.
///
#define ARENE_TSA_TRY_ACQUIRE_SHARED(...) \
  ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(try_acquire_shared_capability(__VA_ARGS__))

///
/// @brief Annotates a function or method as requiring the calling thread must **not** hold access to the named
///        capabilities before entry.
/// @see https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#excludes
/// @param ... A comma-separated list of the names of members/variables which provide the needed capabilities.
///
#define ARENE_TSA_EXCLUDES(...) ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(locks_excluded(__VA_ARGS__))

///
/// @brief Annotates a function or method as asserting that the calling thread holds *exclusive access* to the named
///        capability, and would terminate the process if it is not held.
/// @see https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#assert-capability-and-assert-shared-capability
/// @param x The name of the member or variable that provides the needed capability
/// @post The analysis will assume the given capability is held after exit from the annotated function or method.
///
#define ARENE_TSA_ASSERT_CAPABILITY(x) ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(assert_capability(x))

///
/// @brief Annotates a function or method as asserting that the calling thread holds **shared access** to the named
///        capability, and would terminate the process if it is not held.
/// @see https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#assert-capability-and-assert-shared-capability
/// @param x The name of the member or variable that provides the needed capability
/// @post The analysis will assume the given capability is held after exit from the annotated function or method.
///
#define ARENE_TSA_ASSERT_SHARED_CAPABILITY(x) ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(assert_shared_capability(x))

///
/// @brief Annotates a function or method as returning a reference to the named capability.
/// @see https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#return-capability-c
/// @param x The name of the member or variable that provides the needed capability
///
#define ARENE_TSA_RETURN_CAPABILITY(x) ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(lock_returned(x))

///
/// @brief Annotates a function or method as explicitly excluded from thread safety analysis.
/// @see https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#no-thread-safety-analysis
/// @note This should only be done in situations where a function is too complex for TSA to work correctly, due to the
///       [known limitations](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#limitations) of TSA.
///
#define ARENE_TSA_NO_THREAD_SAFETY_ANALYSIS ARENE_THREAD_SAFETY_ANNOTATION_ATTRIBUTE_I_(no_thread_safety_analysis)

// parasoft-end-suppress AUTOSAR-A16_0_1-d
// NOLINTEND(cppcoreguidelines-macro-usage) These are wrappers around existing macros that must be macros.

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_THREAD_SAFETY_ANNOTATIONS_HPP_
