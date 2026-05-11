// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/thread_safety_annotations.hpp"

#include <gtest/gtest.h>

namespace {

// Unfortunately, we can't really do any better than validating the annotations are defined and can be applied as
// expected to the appropriate identifier types, as we lack the ability to test "will it compile." So we can test stuff
// that should be valid to do to ensure we haven't gotten something horribly **wrong**, but we can't test we've gotten
// it _correct_.

// Note: you _must_ use EXPECT_XXX macros if you're going to unlock after an assertion, because ASSERT_XXX will return
// on failure, and TSA cannot handle conditional locking/unlocking.

/// @test The `ARENE_TSA_CAPABILITY` marker can be applied to classes
TEST(ThreadSafetyAnnotations, CapabilityCanBeAppliedToTypes) {
  class ARENE_TSA_CAPABILITY("a_capability") a_capability {};
}

/// @test The `ARENE_TSA_SCOPED_CAPABILITY` marker can be applied to classes
TEST(ThreadSafetyAnnotations, ScopedCapabilityCanBeAppliedToTypes) {
  class ARENE_TSA_SCOPED_CAPABILITY a_scoped_capability {};
}

// We need an actual capability for the rest or they will fail under clang. It doesn't need to do anything though.
class ARENE_TSA_CAPABILITY("mock_mutex") mock_mutex {
 public:
  void lock() ARENE_TSA_ACQUIRE() {}

  void reader_lock() ARENE_TSA_ACQUIRE_SHARED() {}

  void unlock() ARENE_TSA_RELEASE() {}

  void reader_unlock() ARENE_TSA_RELEASE_SHARED() {}

  void generic_unlock() ARENE_TSA_RELEASE_GENERIC() {}

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static) mock implementation
  ARENE_TSA_TRY_ACQUIRE(true) auto try_lock() -> bool { return true; }

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static) mock implementation
  ARENE_TSA_TRY_ACQUIRE_SHARED(true) auto reader_try_lock() -> bool { return true; }

  void assert_held() ARENE_TSA_ASSERT_CAPABILITY(this) {}

  void assert_reader_held() ARENE_TSA_ASSERT_SHARED_CAPABILITY(this) {}
};

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables) explicitly testing behavior of annotation on globals.
mock_mutex global_mtx;
int a_global ARENE_TSA_GUARDED_BY(global_mtx) = 0;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables) explicitly testing behavior of annotation on globals.

/// @test `ARENE_TSA_GUARDED_BY` can be applied to a data member to indicate that it is protected by a mutex data member
TEST(ThreadSafetyAnnotations, GuardedByCanBeAppliedToGlobalVariablesAndMembers) {
  struct data {
    mock_mutex mtx;
    int data ARENE_TSA_GUARDED_BY(mtx);
  };

  data test{};
  test.mtx.lock();
  test.data = 1;
  test.mtx.unlock();

  global_mtx.lock();
  a_global = 1;
  global_mtx.unlock();
}

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables) explicitly testing behavior of annotation on globals.
int* a_global_ptr_to_a_global ARENE_TSA_PT_GUARDED_BY(global_mtx) = &a_global;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables) explicitly testing behavior of annotation on globals.

/// @test `ARENE_TSA_PT_GUARDED_BY` can be applied to a data member to indicate that it is a pointer to a data member
/// protected by a mutex data member
TEST(ThreadSafetyAnnotations, PtGuardedByCanBeAppliedToPointerGlobalVariablesAndPointerMembers) {
  struct data {
    mock_mutex mtx;
    int data = 0;
    int* ptr_to_data ARENE_TSA_PT_GUARDED_BY(mtx) = &data;
  };

  data test{};
  test.ptr_to_data = nullptr;     // this is fine, PT_GUARDED_BY only protects the pointed to thing
  test.ptr_to_data = &test.data;  // this is fine, PT_GUARDED_BY only protects the pointed to thing
  test.mtx.lock();
  *test.ptr_to_data = 1;
  test.mtx.unlock();

  a_global_ptr_to_a_global = nullptr;    // this is fine, PT_GUARDED_BY only protects the pointed to thing
  a_global_ptr_to_a_global = &a_global;  // this is fine, PT_GUARDED_BY only protects the pointed to thing
  global_mtx.lock();
  *a_global_ptr_to_a_global = 1;
  global_mtx.unlock();
}

/// @test `ARENE_TSA_REQUIRES` can be applied to a member function to indicate that it must be called with a given mutex
/// locked.
TEST(ThreadSafetyAnnotations, RequiresCanBeAppliedToMethods) {
  class needs_synchronization {
   public:
    mock_mutex mtx;

    ARENE_TSA_REQUIRES(mtx, global_mtx) void data(int val) { data_ = val; }
    ARENE_TSA_REQUIRES(mtx) auto data() const -> int { return data_; }

   private:
    int data_ ARENE_TSA_GUARDED_BY(mtx) = 0;
  };

  needs_synchronization test;
  global_mtx.lock();
  test.mtx.lock();
  constexpr int expected = 1;
  test.data(expected);
  EXPECT_EQ(test.data(), expected);
  test.mtx.generic_unlock();
  global_mtx.generic_unlock();
}

/// @test `ARENE_TSA_REQUIRES_SHARED` can be applied to a member function to indicate that it must be invoked with the
/// specified mutexes locked or locked shared.
TEST(ThreadSafetyAnnotations, RequiresSharedCanBeAppliedToMethods) {
  class needs_synchronization {
   public:
    mock_mutex mtx;

    ARENE_TSA_REQUIRES(mtx) void data(int val) { data_ = val; }
    ARENE_TSA_REQUIRES_SHARED(mtx, global_mtx) auto data() const -> int { return data_; }

   private:
    int data_ ARENE_TSA_GUARDED_BY(mtx) = 0;
  };

  needs_synchronization test;
  global_mtx.reader_lock();
  test.mtx.lock();
  constexpr int expected = 1;
  test.data(expected);
  EXPECT_EQ(test.data(), expected);
  test.mtx.generic_unlock();
  global_mtx.generic_unlock();
}

// And we need an _actual_ (but simple) lock guard to validate EXCLUDES
class ARENE_TSA_SCOPED_CAPABILITY mock_lock_guard {
 public:
  explicit mock_lock_guard(mock_mutex* mtx) ARENE_TSA_ACQUIRE(mtx)
      : mtx_(mtx) {
    mtx_->lock();
  }
  ~mock_lock_guard() ARENE_TSA_RELEASE() {
    if (mtx_ != nullptr) {
      mtx_->unlock();
    }
  }

  mock_lock_guard(mock_lock_guard const&) = delete;
  mock_lock_guard(mock_lock_guard&&) noexcept = delete;
  auto operator=(mock_lock_guard const&) -> mock_lock_guard& = delete;
  auto operator=(mock_lock_guard&&) noexcept -> mock_lock_guard& = delete;

 private:
  mock_mutex* mtx_;
};

/// @test `ARENE_TSA_EXCLUDES` can be applied to a member function to indicate that the specified mutex must *not* be
/// locked when the member function is invoked.
TEST(ThreadSafetyAnnotations, ExcludesCanBeAppliedToMethods) {
  class needs_synchronization {
   public:
    mutable mock_mutex mtx;

    ARENE_TSA_EXCLUDES(mtx, global_mtx) void data(int val) {
      mock_lock_guard const lgrd{&mtx};
      mock_lock_guard const ggrd{&global_mtx};
      data_ = val;
    }

    ARENE_TSA_EXCLUDES(mtx) auto data() const -> int {
      mock_lock_guard const lgrd{&mtx};
      return data_;
    }

   private:
    int data_ ARENE_TSA_GUARDED_BY(mtx) = 0;
  };

  needs_synchronization test;
  constexpr int expected = 1;
  test.data(expected);
  EXPECT_EQ(test.data(), expected);
}

}  // namespace
