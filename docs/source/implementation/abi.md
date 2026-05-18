<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

# Application Binary Interface (ABI) {#application-binary-interface-abi}

ABI is an important consideration in deployed software. It is the agreement
between you and an end-user who may not have the ability to rebuild your library
code in conjunction with their application or program that certain qualities
that are not exactly specified by the C++ Standard are neverthless strongly tied
to a given set of behaviors, register conventions, calling conventions, object
sizes, and memory layouts.

At the moment, Arene makes no promises about ABI and is currently in a phase
where we are pushing for the smallest object sizes, smallest code size, and
fastest runtime. In pursuit of this goal, the library contains mostly templated
code that can be shared freely between implementations. There are some
mitigations built into the code so far, and as more are added and more tools
developed they will be documented there.

## Inline ABI Namespaces {#inline-abi-namespaces}

To alleviate some of the pain, there is an internal implementation ABI namespace
that is used in headers within the normal arene namespace definition. It looks
as follows:

```cpp
#include <arene/version.hpp>

namespace arene {
ARENE_BASE_INLINE_ABI_NAMESPACE_OPEN_I_

ARENE_BASE_INLINE_ABI_NAMESPACE_CLOSE_I_
} // namespace arene
```

The `ARENE_BASE_INLINE_ABI_NAMESPACE_OPEN_I_` macro and the paired `CLOSE_I_`
macro just define an inline namespace value. These help mark each version of
defined classes and structures that will allow us to migrate code in the future
to new ABI versions, should we need to do a breaking change well into the
lifecycle of Arene.

## Inline Namespaces: a Caveat {#inline-namespaces-a-caveat}

Note that this does not cover the case where a structure outside of this inline
namespace uses a structure inside of this namespace:

```cpp
#include <arene/span.hpp>

namespace elsewhere {
  struct my_cool_thing {
    arene::base::span<int> data;
  };
}
```

The name in a binary of `elsewhere::my_cool_thing` will not contain any of the
ABI namespace information from `arene::base::span<int>`, even if the official
binary name of `arene::base::span<int>` will be derived from
`arene::base::v0::span<int>` (for example).
