<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page tuple tuple: Facilities For Manipulating std::tuple

<!-- markdownlint-enable MD041 -->

The `tuple` sub-package provides a facility for working with `std::tuple`.

The public header is

```{.cpp}
#include "arene/base/tuple.hpp"
```

The Bazel target is

```text
//:tuple
```

## Working With Tuples {#working-with-tuples}

The `tuple` package provides facilities for applying
[invocables](https://en.cppreference.com/w/cpp/utility/functional) to the
elements of a `std::tuple`.

Firstly, `arene::base::apply` invokes the supplied invocable once, with the
elements of the tuple as arguments to the invocable:

```{.cpp}
void foo() {
  std::tuple<int, double, std::string> values(42,3.14,"hello");

  arene::base::apply([](int intvar, double dblvar, const std::string& strvar) {
    std::cout<<"i= "<<intvar<<", d="<<dblvar<<", s="<<strvar<<"\n";
  }, values);
}
```

Here, the `values` tuple is split into separate arguments and passed to the
lambda, which then prints them out.

`arene::base::for_each` instead invokes the invocable for each element in turn:

```{.cpp}
class do_print{
public:
  void operator()(int val) const{
    std::cout<<"int ="<<val<<"\n";
  }
  void operator()(double val) const{
    std::cout<<"double ="<<val<<"\n";
  }
  void operator()(const std::string& val) const{
    std::cout<<"string ="<<val<<"\n";
  }
};

void bar() {
  std::tuple<int, double, std::string> values(42,3.14,"hello");

  arene::base::for_each(values, do_print{});
}
```

In this case, the `values` tuple is again split, but now into separate function
calls, so the `do_print` invocable is invoked 3 times: once with the `int`
element, once with the `double` element, and finally once with the `std::string`
element.

Finally, `arene::base::for_each_index` does the same splitting as
`arene::base::for_each`, but additionally passes the index of the element to the
operation as the first argument.

```{.cpp}
void baz() {
  std::tuple<std::string, std::string, std::string> values("hello","world","goodbye");

  arene::base::for_each_index([](std::size_t index, const std::string& str) {
    std::cout<<"val "<<index<<" = "<<str<<"\n";
  });
}
```

Here, the lambda is again called 3 times: first as `lambda(0, "hello")`, then as
`lambda(1, "world")`, and finally as `lambda(2, "goodbye")`.
