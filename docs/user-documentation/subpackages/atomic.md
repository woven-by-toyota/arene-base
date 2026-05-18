<!--
Copyright 2026, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page atomic atomic: Atomic operations in concurrent systems

<!-- markdownlint-enable MD041 -->

Arene Base provides facilities for atomic operations across threads that do not
depend on the C++ standard library.

The public header is

```{.cpp}
#include "arene/base/atomic.hpp"
```

The Bazel target is

```text
//:atomic
```

## Introduction {#introduction}

The `atomic` subpackage provides atomic operations and types. It is intended as
an alternative to the C++ standard libarary `<atomic>` header, for environments
where the standard library implementation is not available.

## Types {#types}

### atomic_counter {#atomic_counter}

The `atomic_counter` class provides a thread-safe counter implementation using
compiler intrinsics rather than standard library facilities.

```cpp
#include "arene/base/atomic.hpp"

arene::base::atomic_counter counter;
```

#### API Reference {#api-reference}

##### Constructors {#constructors}

```cpp
constexpr atomic_counter() noexcept;
constexpr explicit atomic_counter(uint64_t initial_value) noexcept;
```

##### Operations {#operations}

```cpp
// Pre-increment
auto operator++() noexcept -> uint64_t;

// Post-increment
auto operator++(int) noexcept -> uint64_t;

// Pre-decrement
auto operator--() noexcept -> uint64_t;

// Post-decrement
auto operator--(int) noexcept -> uint64_t;

// Addition assignment
auto operator+=(uint64_t val) noexcept -> uint64_t;

// Subtraction assignment
auto operator-=(uint64_t val) noexcept -> uint64_t;
```

##### Value Access {#value-access}

```cpp
// Explicit load
auto load() const noexcept -> uint64_t;

// Implicit conversion
operator uint64_t() const noexcept;
```

#### Thread Safety {#thread-safety}

All operations on `atomic_counter` are atomic and thread-safe. The
implementation uses compiler intrinsics with sequential consistency memory
ordering to ensure proper behavior in multi-threaded environments.

#### Examples {#examples}

##### Basic Usage {#basic-usage}

\snippet docs/examples/atomic_counter_examples.cpp basic_usage

##### Thread-Safe Counting {#thread-safe-counting}

\snippet docs/examples/atomic_counter_examples.cpp concurrent_updates

##### Using Counter for Event Tracking {#using-counter-for-event-tracking}

\snippet docs/examples/atomic_counter_examples.cpp counter_as_event_tracker
