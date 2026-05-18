<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 MD013-->
<!-- prettier-ignore -->
\page compiler_support_platform_queries Facilities For Querying Platform/Compiler Properties

<!-- markdownlint-enable MD041 -->

When writing certain types of low level functionality, information about
platform specific properties such as the availability of a particular builtin
intrinsic is needed in order to produce optimal code. Support for these queries
is provided via the following header:

```cpp
#include "arene/base/compiler_support/platform_queries.hpp"

```

## Querying For The Current Compiler {#querying-for-the-current-compiler}

When writing portable code, occasionally conditional action is needed depending
on the compiler being used to compile the code. The following queries can be
used to test if the current compiler is a given compiler:

|         Query          |                             Description                              |
| :--------------------: | :------------------------------------------------------------------: |
|  `ARENE_COMPILER_GCC`  |  Tests if the current compiler is GCC, or should be treated as GCC   |
| `ARENE_COMPILER_CLANG` | Test if the current compiler is Clang, or should be treated as Clang |

Example usage:

```cpp
#if ARENE_IS_ON(ARENE_COMPILER_GCC)
// do something GCC specific
#elif ARENE_IS_ON(ARENE_COMPILER_CLANG)
// do something Clang specific
#else
// default fallback
#endif
```

## Querying For The Standard Library Implementation In Use {#querying-for-the-standard-library-implementation-in-use}

When writing portable code, occasionally conditional action is needed depending
on the standard library implementation being used. The following queries can be
used to test if the current standard library implementation is a given standard
library:

|           Query            |                                        Description                                         |
| :------------------------: | :----------------------------------------------------------------------------------------: |
|   `ARENE_STDLIB_LIBCXX`    |           Tests if the current stdlib is libc++, or should be treated as libc++            |
|  `ARENE_STDLIB_LIBSTDCXX`  |         Test if the current stdlib is libstdc++, or should be treated as libstdc++         |
| `ARENE_STDLIB_LIBARENECXX` | Test if the current stdlib is the arene_base internal stdlib, or should be treated as such |

Example usage:

```cpp
#if ARENE_IS_ON(ARENE_STDLIB_LIBCXX)
// do something libc++ specific
#elif ARENE_IS_ON(ARENE_STDLIB_LIBSTDCXX)
// do something libstdc++ specific
#else
// default fallback
#endif
```

## Querying For A Header's Existence {#querying-for-a-headers-existence}

When writing platform specific code, it is sometimes needed to test if a given
system header is available. The `ARENE_HAS_INCLUDE` macro can be used for this
purpose. For example, the following snippet would select between a platform
provided and custom implementation header depending on if the platform header
`<some_platform_header.h>` is provided:

```cpp
#if ARENE_HAS_INCLUDE(<some_platform_header.h>)
#include <some_platform_header.h>
#else
#include "our/custom/implementation.h"
#endif
```

\note `ARENE_HAS_INCLUDE` relies on the
[`__has_include`](https://en.cppreference.com/w/cpp/preprocessor/include)
compiler intrinsic, which is supported in `gcc >= 5.0`, and in all `clang`
versions. For other compilers, generally if the compiler could support C++17,
then it should support `__has_include`. If `__has_include` is not supported,
this macro always returns `0`

## Querying For Compiler Builtins, Features and Sanitizers {#querying-for-compiler-builtins-features-and-sanitizers}

There are a number of facilities provided for testing for compiler builtins,
including predefined queries for specific builtins. They are intended to be used
with `ARENE_IS_ON` and friends.

### `ARENE_HAS_BUILTIN` {#arene_has_builtin}

This macro queries if a particular compiler builtin is supported. It should be
used when building wrapper macros for additional queries, to avoid boilerplate.
For example, this is how the query for `__builtin_fabs` is implemented:

```cpp
#ifndef ARENE_HAS_BUILTIN_FABS_I_
#if ARENE_HAS_BUILTIN(__builtin_fabs)
#define ARENE_HAS_BUILTIN_FABS_I_ ARENE_ON_BY_DEFAULT
#elif ARENE_IS_ON(ARENE_COMPILER_GCC) && __GNUC__ >= 8
// The __builtin_fabs() exists in GCC (at least) 8 and later,
// but the __has_builtin query was not added until GCC 10 and so we
// may have returned a false negative when probing for the builtin above.
#define ARENE_HAS_BUILTIN_FABS_I_ ARENE_ON_BY_DEFAULT
#else
#define ARENE_HAS_BUILTIN_FABS_I_ ARENE_OFF_BY_DEFAULT
#endif
```

And it would be used like so:

```cpp
#if ARENE_IS_ON(ARENE_HAS_BUILTIN_FABS)
// do something with the knowledge that __builtin_fabs exists
#endif
```

\note As suggested in the example, this macro depends on the
`__has_builtin(...)` compiler intrinsic, which is supported in `gcc >= 10.0` and
in all `clang` versions. This intrinsic is not standardized, so for other
compilers you must consult the appropriate compiler documentation. If the
intrinsic is not supported, this macro always returns `0`.

### `ARENE_HAS_FEATURE` {#arene_has_feature}

This macro queries if a particular compiler "feature" is enabled. This is a
clang-specific facility, though newer versions of GCC also support it for
compatibility with clang, and generally should only be used when writing macros
for additional platform feature queries.

### Predefined Queries For Compiler Builtins/Features {#predefined-queries-for-compiler-builtinsfeatures}

The following queries for builtins are supported.

|                   Query                   |                                                      Description                                                       |
| :---------------------------------------: | :--------------------------------------------------------------------------------------------------------------------: |
|         `ARENE_HAS_BUILTIN_FABS`          |                                        Tests if `__builtin_fabs` is supported.                                         |
|         `ARENE_HAS_BUILTIN_FABSF`         |                                        Tests if `__builtin_fabsf` is supported.                                        |
| `ARENE_HAS_BUILTIN_IS_CONSTANT_EVALUATED` |                                Tests if `__builtin_is_constant_evaluated` is supported.                                |
|    `ARENE_HAS_THREAD_SAFETY_ANALYSIS`     | Tests if the current compiler supports [Thread Safety Analysis](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html) |

### Predefined Queries for Standard Library Facility Support {#predefined-queries-for-standard-library-facility-support}

The following queries for standard library features are supported.

|                  Query                  |                      Description                       |
| :-------------------------------------: | :----------------------------------------------------: |
|      `ARENE_STD_INLINE_VARIABLES`       |    Tests if `__cpp_inline_variables` is supported.     |
|     `ARENE_STD_LIBRARY_STRING_VIEW`     |     Tests if `__cpp_lib_string_view` is supported.     |
|  `ARENE_STD_LIBRARY_STARTS_ENDS_WITH`   |  Tests if `__cpp_lib_starts_ends_with` is supported.   |
|   `ARENE_STD_LIBRARY_STRING_CONTAINS`   |   Tests if `__cpp_lib_string_contains` is supported.   |
| `ARENE_STD_LIBRARY_UNCAUGHT_EXCEPTIONS` | Tests if `__cpp_lib_uncaught_exceptions` is supported. |
|   `ARENE_STD_NOEXCEPT_FUNCTION_TYPE`    | Tests if `__cpp_noexcept_function_type` is supported.  |

### Predefined Queries for Active Sanitizers {#predefined-queries-for-active-sanitizers}

The following queries for determining if one of the
[sanitizers](https://github.com/google/sanitizers) are active are supported:

|        Query         |                                                          Description                                                           |
| :------------------: | :----------------------------------------------------------------------------------------------------------------------------: |
| `ARENE_ASAN_ENABLED` | Tests if [AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer) is enabled in the current compilation. |

### Predefined Queries For Coverage Tooling Detection {#predefined-queries-for-coverage-tooling-detection}

The following queries for detecting the presense of coverage tooling are
supported:

|              Query              |                                               Description                                               |
| :-----------------------------: | :-----------------------------------------------------------------------------------------------------: |
|         `ARENE_IS_GCOV`         |                      Tests if the current compiler is building with gcov enabled.                       |
|       `ARENE_IS_LLVMCOV`        |                    Tests if the current compiler is building with LLVCMCov enabled.                     |
| `ARENE_IS_COVERAGE_COMPILATION` | Tests if the current compiler is building with one of the coverage tools (gcov or LLVMCov etc) enabled. |
