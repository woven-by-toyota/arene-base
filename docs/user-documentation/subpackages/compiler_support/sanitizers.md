<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 MD013-->
<!-- prettier-ignore -->
\page compiler_support_sanitizers Facilities For Working With Sanitizers

<!-- markdownlint-enable MD041 -->

Modern compilers come with a number of "sanitizers" built into them which can
perform runtime analysis of code for various kinds of memory safety violations
which are difficult to identify statically. The most commonly used of these
sanitizers are:

- [AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer)
- [UndefinedBehaviorSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)
- [ThreadSanitizer](https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual)

While these sanitizers generally "just work," there are certain situations in
which having additional, explicit user annotation of intent can assist the
sanitizers in having more reliable detection of errors. \arene_base provides
portable interfaces to support these annotations in a coding-standards-compliant
manner.

## AddressSanitizer Annotations {#addresssanitizer-annotations}

AddressSanitizer (asan) can reliably detect invalid memory access for any memory
explicitly allocated by standard library components or through explicit heap
allocation facilities like regular `new`. However, when users write their own
memory allocation facilities leveraging
[placement-new](https://en.cppreference.com/w/cpp/language/new), it is unable to
inherently track the "validity" of the objects, as the user program always
"owns" the memory and the proper control of object lifetime is program defined.
Placement-new allocation is often used in usecases such as arena allocators and
inline containers, which in turn are often used in safety-critical code to avoid
heap allocation after program initialization.

To avoid losing asan's protection, an interface is defined to allow programs to
manually annotate regions of memory as "poisoned" and "unpoisoned." A poisoned
region of memory will trigger an asan violation for any access other than to
unpoison it. This interface is exposed in `compiler_support` via the following
header:

```cpp
#include "arene/base/compiler_support/asan_annotations.hpp"
```

There are four primary APIs:

|                     Method                      |                                   Description                                   |
| :---------------------------------------------: | :-----------------------------------------------------------------------------: |
|  `ARENE_ASAN_POISON_MEMORY_REGION(addr, size)`  |  Marks the region starting at `addr`, and extending `size` bytes, as poisoned.  |
|   `ARENE_ASAN_POISON_MEMORY_SPAN(span_of_t)`    |  Marks the region represented by the input `arene::base::span<T>` as poisoned.  |
| `ARENE_ASAN_UNPOISON_MEMORY_REGION(addr, size)` | Marks the region starting at `addr`, and extending `size` bytes, as unpoisoned. |
|  `ARENE_ASAN_UNPOISON_MEMORY_SPAN(span_of_t)`   | Marks the region represented by the input `arene::base::span<T>` as unpoisoned. |

Users should prefer the `arene::base::span<T>` based API when possible, as it
handles computing the appropriate memory region automatically.

\note These annotations are safe to use in "production" code. When asan is not
enabled, they turn into noops, and thus there is no effect to the resulting
binary.
