<!-- markdownlint-disable MD041 -->
<!-- markdownlint-disable MD013 -->
<!-- prettier-ignore -->
\page compiler_support_cpp14_inline_variable Facilities For Creating Inline Variables in C++14

<!-- markdownlint-enable MD041 -->
<!-- markdownlint-enable MD013 -->

C++17 added support for declaring `inline` variables, which allow variables to
be defined in header files without violating the One Definition Rule (ODR). To
allow similar functionality in C++14, \arene_base provides a facility,
`ARENE_CPP14_INLINE_VARIABLE(type,name)`, for creating "inline variables" in
C++14. This primarily intended to ease the creation of Niebloid-like function
objects. The public header is:

```cpp
#include "arene/base/compiler_support/cpp14_inline.hpp"
```

An example of defining a function object using the macro can be seen below:

\snippet docs/examples/compiler_support_examples.cpp cpp14_inline_variable_fo

\note The type used with the macro must be `constexpr` default constructible.
