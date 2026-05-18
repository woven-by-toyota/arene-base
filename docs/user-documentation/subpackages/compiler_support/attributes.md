<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page compiler_support_attributes Facilities For Specifying C++ Attributes

<!-- markdownlint-enable MD041 -->

C++11 added the
[Attribute Specifier Sequence](https://en.cppreference.com/w/cpp/language/attributes),
a syntax for specifying uniformly named "attributes" with implementation defined
behavior attached to types, objects code, etc.

However, not every platform and compiler supports every attribute. In addition,
different customer's coding standards have different requirements with respect
to behavior in the presence of an unsupported attribute. Some allow unknown
attributes to be ignored, while others turn this into a compiler error.

To allow code to be written that leverages common attributes needed for accurate
static analysis (`[[noreturn]]`, `[[maybe_unused]]`, etc), while avoiding
unsupported attributes remaining in compiled code, \arene_base provides helper
facilities for working with attributes in the following header:

```cpp
#include "arene/base/compiler_support/attributes.hpp"
```

There are facilities for testing if attributes are supported by a platform, as
well as pre-built macros for injecting an attribute only if the
platform/compiler supports it for commonly used attributes.

## `ARENE_HAS_STD_ATTRIBUTE`: Testing For Attribute Existence {#arene_has_std_attribute-testing-for-attribute-existence}

The `ARENE_HAS_STD_ATTRIBUTE(...)` macro is a wrapper around the compiler
intrinsics `__has_cpp_attribute` or `__has_c_attribute`. It is used to test for
the existence of a given attribute with the current compiler, and is the basis
from which all the "stock" attribute wrappers are constructed. For example, the
wrapper for the
[`[[maybe_unused]]`](https://en.cppreference.com/w/cpp/language/attributes/maybe_unused)
attribute looks like this:

```cpp
#if ARENE_HAS_STD_ATTRIBUTE(gnu::unused)
#define ARENE_MAYBE_UNUSED [[gnu::unused]]
#elif ARENE_HAS_STD_ATTRIBUTE(maybe_unused) >= 201603L
#define ARENE_MAYBE_UNUSED [[maybe_unused]]
#else
#define ARENE_MAYBE_UNUSED
#endif
```

This construction tests first if the compiler supports `gnu::unused`, the
GCC-specific implementation of `[[maybe_unused]]` before `[[maybe_unused]]` was
standardized. If it is defined, this is used. Otherwise, it tests if the
standard `maybe_unused` attribute is defined, and if it is, uses it. Finally, if
neither are supported, the macro turns into a no-op.

## Supported Attributes {#supported-attributes}

The attributes which have stock wrapper macros are enumerated in the following
table, with links to their stdlib documentation for meaning:

|            Macro             |                                                      Attribute                                                       |
| :--------------------------: | :------------------------------------------------------------------------------------------------------------------: |
|      `ARENE_NODISCARD`       |                  [`[[nodiscard]]`](https://en.cppreference.com/w/cpp/language/attributes/nodiscard)                  |
| `ARENE_NODISCARD_WITH(...)`  |                              `[[nodiscard(..)]]`, provides a message for justification.                              |
|     `ARENE_MAYBE_UNUSED`     |               [`[[maybe_unused]]`](https://en.cppreference.com/w/cpp/language/attributes/maybe_unused)               |
|       `ARENE_NORETURN`       |                   [`[[noreturn]]`](https://en.cppreference.com/w/cpp/language/attributes/noreturn)                   |
|      `ARENE_DEPRECATED`      |                 [`[[deprecated]]`](https://en.cppreference.com/w/cpp/language/attributes/deprecated)                 |
| `ARENE_DEPRECATED_WITH(...)` |                             `[[deprecated(...)]]`, provides a message for justification                              |
|      `ARENE_MAY_ALIAS`       | [`__may_alias__`](https://gcc.gnu.org/onlinedocs/gcc/Common-Type-Attributes.html#index-may_005falias-type-attribute) |

And example usages:

```cpp
ARENE_NODISCARD bool do_foo(Args args...);

ARENE_NORETURN void always_throws(Args args...);

ARENE_DEPRECATED_WITH("Prefer new_api(Args args...)") old_api(Args args...);
```
