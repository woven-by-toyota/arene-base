<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page mutex mutex: `<mutex>` backports and Thread Safety Analysis compatible wrappers.

<!-- markdownlint-enable MD041 -->

The `mutex` subpackage provides backports of the content of `<mutex>` from newer
C++ versions, as well as providing thin wrappers around `std::mutex`,
`std::timed_mutex`, and `std::lock_guard` suitable for use with <!-- break -->
[Thread Safety Analysis](\ref compiler_support_thread_safety_annotations).

The public header is

```{.cpp}
#include "arene/base/mutex.hpp"
```

The Bazel target is

```text
//:mutex
```

## Thread Safety Analysis Capability Wrappers {#thread-safety-analysis-capability-wrappers}

In order for [TSA](\ref compiler_support_thread_safety_annotations) to function
correctly, synchronization primitives need to be annotated as capabilities.
`libcxx` provides appropriately annotated capabilities directly in `<mutex>`,
but for other stdlib implementations they must be provided manually. To support
this, the following annotated types are provided:

- `arene::base::mutex`: annotated
  [`std::mutex`](https://en.cppreference.com/w/cpp/thread/mutex)
- `arene::base::timed_mutex`: annotated
  [`std::timed_mutex`](https://en.cppreference.com/w/cpp/thread/timed_mutex)
- `arene::base::lock_guard`: annotated
  [`std::lock_guard`](https://en.cppreference.com/w/cpp/thread/lock_guard)

Their usage is identical to their stdlib equivalents.

\note It is recommended to always use the TSA-compatible types from this
subpackage instead of their raw stdlib equivalents in order to maximize the
possibility of protection from TSA: the wrappers are zero-overhead abstractions,
so there is no runtime performance penalty. In addition, when compiling with a
compiler that does not support TSA or when using a stdlib which already supports
TSA such as `libcxx`, these types become simple aliases to their stdlib
counterparts to improve compile times.
