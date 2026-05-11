<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page compiler_support_preprocessor Facilities For Working With The Preprocessor

<!-- markdownlint-enable MD041 -->

Working with the Preprocessor is complex and easy to get wrong, particularly
while maintaining compliance with various safety critical coding standards.
\arene_base provides a number of facilities to facilitate working with the
preprocessor in a robust, consistent manner. They serve as the basis for many of
the macro-based implementations of facilities in the `compiler_support`
subpackage.

The public header for these facilities is:

```cpp
#include "arene/base/compiler_support/preprocessor.hpp"
```

## Define-Based Configuration Support {#define-based-configuration-support}

In order to facilitate build-system independent library configuration, a number
of facilities are provided to standardize a mechanism for testing `#define`
based configuration parameters. The basic idea is that a given configuration
define can be in one of 4 states:

- on
- on by default
- off
- off by default

By using the appropriate configuration macros and queries described below,
conditional logic can be enabled or disabled based on these define
configurations. The intended pattern of usage is as follows:

- The name used to _query_ the setting, or to set the setting from a command
  line or build system, is `FOO_SETTING`.
- The name used to _define_ the setting in code, either its default value or its
  state based on querying other properties, is `FOO_SETTING_I_`.

For example, the following is how the configuration stating that the current
compiler should be treated as GCC, which is provided via the query name
`ARENE_COMPILER_GCC` is defined:

```cpp
#if defined(ARENE_COMPILER_GCC)
  #if (ARENE_COMPILER_GCC != 0)
    #define ARENE_COMPILER_GCC_I_ ARENE_ON
  #else
    #define ARENE_COMPILER_GCC_I_ ARENE_OFF
  #endif
#elif ARENE_IS_OFF(ARENE_COMPILER_CLANG) && (defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__))
  #define ARENE_COMPILER_GCC_I_ ARENE_ON_BY_DEFAULT
#else
  #define ARENE_COMPILER_GCC_I_ ARENE_OFF_BY_DEFAULT
#endif
```

The logic tree of this operation is as follows:

1. If `ARENE_COMPILER_GCC` is already defined (ex: it was defined via compiler
   commands):
   1. If the existing definition is "truthy," define `ARENE_COMPILER_GCC_I_` as
      `ARENE_ON`.
   2. Otherwise, define `ARENE_COMPILER_GCC_I_` as `ARENE_OFF`.
2. Otherwise, we fall back to detection based configuration:
   1. If `ARENE_COMPILER_CLANG` is "off", and we detect intrinsics that suggest
      we're executing under `gcc`, define `ARENE_COMPILER_GCC_I_` as
      `ARENE_ON_BY_DEFAULT`.
   2. Otherwise, define `ARENE_COMPILER_GCC_I_` as `ARENE_OFF_BY_DEFAULT`

See below for further explanation on
[how to test](#testing-for-configuration-settings) if a setting is on/off, and
the definition of the various [states](#configuration-setting-states) a setting
can be in.

### Testing For Configuration Settings {#testing-for-configuration-settings}

The following table defines the queries for testing if a define-based
configuration setting is "on" or "off":

|                  Query                   |                                                           Description                                                           |
| :--------------------------------------: | :-----------------------------------------------------------------------------------------------------------------------------: |
|         `ARENE_IS_ON(OP_SYMBOL)`         |           Will be `true` if `OP_SYMBOL`, with `_I_` appended to it, is defined as `ARENE_ON` or `ARENE_ON_BY_DEFAULT`           |
|        `ARENE_IS_OFF(OP_SYMBOL)`         |          Will be `true` if `OP_SYMBOL`, with `_I_` appended to it, is defined as `ARENE_OFF` or `ARENE_OFF_BY_DEFAULT`          |
|   `ARENE_IS_ON_BY_DEFAULT(OP_SYMBOL)`    |                  Will be `true` if `OP_SYMBOL`, with `_I_` appended to it, is defined as `ARENE_ON_BY_DEFAULT`                  |
|   `ARENE_IS_OFF_BY_DEFAULT(OP_SYMBOL)`   |                 Will be `true` if `OP_SYMBOL`, with `_I_` appended to it, is defined as `ARENE_OFF_BY_DEFAULT`                  |
|       `ARENE_IS_ON_RAW(OP_SYMBOL)`       |      Same meaning as `ARENE_IS_ON`, but without appending `_I_` to `OP_SYMBOL`. Should generally not be used in user code       |
|      `ARENE_IS_OFF_RAW(OP_SYMBOL)`       |      Same meaning as `ARENE_IS_ON`, but without appending `_I_` to `OP_SYMBOL`. Should generally not be used in user code       |
| `ARENE_IS_ON_BY_DEFAULT_RAW(OP_SYMBOL)`  | Same meaning as `ARENE_IS_ON_BY_DEFAULT`, but without appending `_I_` to `OP_SYMBOL`. Should generally not be used in user code |
| `ARENE_IS_OFF_BY_DEFAULT_RAW(OP_SYMBOL)` | Same meaning as `ARENE_IS_ON_BY_DEFAULT`, but without appending `_I_` to `OP_SYMBOL`. Should generally not be used in user code |

\warning These macros will only work correctly with the supporting
[configuration setting states](#configuration-setting-states). Attempting to
pass a symbol which evaluates to something other than one of those states is
undefined behavior. The `ARENE_GUARANTEE_INTERNAL_DEFINITION` macro can be used
to [test](#ensuring-a-configuration-symbol-is-properly-specified) that a
configuration setting is validly specified.

### Configuration Setting States {#configuration-setting-states}

The following table defines the states a configuration define can be in:

|       Sate Macro       |                     Description                      |
| :--------------------: | :--------------------------------------------------: |
|       `ARENE_ON`       | The setting should be considered explicitly enabled  |
|      `ARENE_OFF`       | The setting should be considered explicitly disabled |
| `ARENE_ON_BY_DEFAULT`  | The setting should be considered implicitly enabled  |
| `ARENE_OFF_BY_DEFAULT` | The setting should be considered implicitly disabled |

### Ensuring A Configuration Symbol Is Properly Specified {#ensuring-a-configuration-symbol-is-properly-specified}

The `ARENE_GUARANTEE_INTERNAL_DEFINITION` macro can be used to write unit tests
that ensure a configuration parameter is validly defined:

```cpp
#include "arene/base/testing/gtest.hpp"
#include <gtest/gtest.h>

TEST(AreneCompilerGcc, IsProperlyDefined) {
    STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_COMPILER_GCC));
}
```

## Stringizing A Macro Argument's Value {#stringizing-a-macro-arguments-value}

`ARENE_STRINGIZE(x)` provides the necessary indirection to result in the
stringification of the _value_ of a macro argument, rather than its name, when
the argument is itself a macro:

```cpp
#define STRINGIZE_TEST_CAT(a, b) a##b

ARENE_STRINGIZE(STRINGIZE_TEST_CAT(foo, bar)) // will be "foobar"
```
