<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page constraints constraints: Facilities For Constraining Templates (SFINAE)

<!-- markdownlint-enable MD041 -->

Facilities for constraining function templates and partially specializing class
templates and variable templates.

The public header is

```{.cpp}
#include "arene/base/constraints.hpp"
```

The Bazel target is

```text
//:constraints
```

## Constraining function templates {#constraining-function-templates}

When writing function templates, it can be useful to be able to impose
constraints on the template parameters. For example, maybe a function should
only work with built-in types, or only iterators, or only if a template
parameter can be implicitly converted to a specific type.

Prior to C++20, it is common to use `std::enable_if` and `std::enable_if_t` on
the function return type to facilitate writing these constraints. However, that
can be cumbersome to write, especially when there are multiple constraints to
check, and it makes it harder to see what the actual return type of the function
is, since it is wrapped in the `std::enable_if`.

The `arene::base::constraints` class template provides a solution to that. By
adding an additional template parameter to your function template which is an
instance of `arene::base::constraints`, you can specify the constraints in the
template declaration, rather than as part of the function return type.
`arene::base::constraints` is a variadic alias template, which takes zero or
more type arguments, and results in an alias of `std::nullptr_t` if the
arguments are all valid. By passing an instance of `std::enable_if_t` as an
argument for the `arene::base::constraints` template, you ensure that the
function template does not participate in overload resolution if the condition
specified in the `std::enable_if_t` parameter is false.

Because the `arene::base::constraints` yields an alias to `std::nullptr_t` if
the constraints are satisfied, the template parameter must be given a default
value of `nullptr`.

A minimal "constrained" template with no constraints is thus:

```{.cpp}
template<typename T, arene::base::constraints<> = nullptr>
void foo(T) {}
```

\note The space between the closing `>` of the `constraints` and the `=` is
important. Without the space, then they are parsed as a single `>=` token, and
unpleasant compiler error messages will follow.

### Constraining with a single constraint {#constraining-with-a-single-constraint}

Obviously, there's not much point specifying an empty list of constraints. The
following function can be used to add two integers, but will not be considered
if the template argument `T` is not an integer:

```{.cpp}
template<typename T, arene::base::constraints<std::enable_if_t<std::is_integral<T>::value>> = nullptr>
T add(T a,T b) {
  return a + b;
}

int ok = add(4, 5); // OK, 4 and 5 are of type int
double error = add(1.2, 3.4); // error, 1.2 and 3.4 are of type double
```

The error message makes it clear which constraint was violated:

<!-- markdownlint-disable MD046 -->

    test.cpp:27:16: error: no matching function for call to 'add'
    double error = add(1.2, 3.4);  // error, 1.2 and 3.4 are of type double
                   ^~~
    test.cpp:22:3: note: candidate template ignored: requirement 'std::is_integral<double>::value' was not satisfied [with T = double]
    T add(T a, T b) {

<!-- markdownlint-enable MD046 -->

### Constraining with multiple constraints {#constraining-with-multiple-constraints}

Multiple constraints can be added by specifying multiple `std::enable_if_t`
parameters. In the following example, `convert` can only be called if the
`Target` type can be constructed from the `Source` type, and the `Target` is
move-constructible:

```{.cpp}
template<typename Target, typename Source,
  arene::base::constraints<
    std::enable_if_t<std::is_constructible<Target, Source>::value>,
    std::enable_if_t<std::is_move_constructible<Target>::value>
  > = nullptr>
Target convert(Source value) {
  return static_cast<Target>(value);
}

auto ok = convert<std::string>("hello");
auto error = convert<std::string>(std::vector<int>{1, 2, 3});

struct non_movable{
  non_movable(int){}
  non_movable(non_movable&&) = delete;
};

auto error2 = convert<non_movable>(42);
```

The construction of `ok` is fine: a string literal decays to make `Source` a
`const char*`, which can be used to construct the `Target` of `std::string`. On
the other hand, the construction of `error` is invalid because you cannot
construct a `std::string` from a `std::vector<int>`, and the construction of
`error2` is invalid because `non_movable` isn't move-constructible.

Again, the error message from the compiler indicates why:

<!-- markdownlint-disable MD046 -->

    test.cpp:129:14: error: no matching function for call to 'convert'
    auto error = convert<std::string>(std::vector<int>{1, 2, 3});
                 ^~~~~~~~~~~~~~~~~~~~
    test.cpp:124:8: note: candidate template ignored: requirement 'std::is_constructible<std::basic_string<char, std::char_traits<char>, std::allocator<char>>, std::vector<int, std::allocator<int>>>::value' was not satisfied [with Target = std::basic_string<char>, Source = std::vector<int>]
    Target convert(Source value) {
           ^
    src/lib/arene/constraints/tests/constraints_test.cpp:136:15: error: no matching function for call to 'convert'
    auto error2 = convert<non_movable>(42);
                  ^~~~~~~~~~~~~~~~~~~~
    src/lib/arene/constraints/tests/constraints_test.cpp:124:8: note: candidate template ignored: requirement 'std::is_move_constructible<non_movable>::value' was not satisfied [with Target = non_movable, Source = int]
    Target convert(Source value) {
           ^

<!-- markdownlint-enable MD046 -->

For the first call it is rejected because of the `is_constructible` constraint
not being satisfied; for the second it was rejected due to the
`is_move_constructible` constraint not being satisfied.

### "Type expression" constraints {#type-expression-constraints}

Though the normal use case of `arene::base::constraints` will be for each
constraint to be specified with `std::enable_if_t` and a boolean expression, it
is also possible to have "type expression" constraints, that are valid if an
expression yields a type, and not otherwise. Such "type expressions" are
commonly expressed using `decltype`, or nested-name-specifiers.

For example:

```{.cpp}
template<typename T, arene::base::constraints<T::element_type> = nullptr>
void foo(T);

template<typename T, arene::base::constraints<decltype(std::declval<T&>().do_foo())> = nullptr>
void bar(T);
```

Here, `foo` is only callable if the type parameter is a class type with an
`element_type` member type, whereas `bar` is only callable if the type parameter
is a class type with a non-`const` `do_foo` member function that can be called
without any arguments.

"Type expressions" using `decltype` often use `std::declval` to provide objects
of the appropriate type without knowing how to construct them. `std::declval` is
a function from the `<utility>` header which is declared but not defined, and
has a return value of the specified type.

### Using constraints for overloads {#using-constraints-for-overloads}

An important aspect of using `arene::base::constraints` in this way is that
constrained function overloads are ignored if the constraints are not met,
rather than being a hard error. This is often called _Substitution Failure Is
Not An Error_ (SFINAE). The error message above all specified "no matching
function".

This means that you can use constraints to provide multiple overloaded versions
of the same function, with the same signature, but different constraints.

For example, C++11 provides `std::to_string` as a set of individual overloads
for each fundamental type. However, we could provide it instead as a couple of
constrained template overloads:

```{.cpp}
template<typename T,
  arene::base::constraints<
    std::enable_if_t<std::is_integral<T>::value>,
    std::enable_if_t<std::is_signed<T>::value>> = nullptr>
std::string to_string(T value) {
  char buffer[22];
  sprintf(buffer, "%lld", static_cast<long long>(value));
  return buffer;
}

template<typename T,
  arene::base::constraints<
    std::enable_if_t<std::is_integral<T>::value>,
    std::enable_if_t<std::is_unsigned<T>::value>> = nullptr>
std::string to_string(T value) {
  char buffer[22];
  sprintf(buffer, "%llu", static_cast<unsigned long long>(value));
  return buffer;
}

template<typename T,
arene::base::constraints<std::enable_if_t<std::is_floating_point<T>::value>> = nullptr>
std::string to_string(T value){
  char buffer[400];
  sprintf(buffer, "%Lf", static_cast<long double>(value));
  return buffer;
}
```

The first is only considered if `T` is a signed integral type, the second for an
unsigned integral type, and the third for floating point types.

## Constraining class templates {#constraining-class-templates}

If a template should not be instantiated at all with template parameters that
don't meet certain requirements, those requirements should be checked with
`static_assert` rather than constraints. That will lead to better error
messages, and is altogether clearer.

```{.cpp}
template<typename T>
class Foo {
  static_assert(std::is_integral<T>::value, "Foo can only be used for integral types");
};
```

However, you _can_ use constraints to help the compiler choose a partial
specialization. That way you can define multiple partial specializations for a
class template to handle the cases where the supplied template parameters
provide different properties. For example, you might have a general
implementation of a class template that works for all types, but there is a more
efficient implementation that works for integers. You could create explicit
specializations for each of the integer types, but that's a lot of
almost-identical specializations to create, with a lot of code duplication, and
one of the benefits of templates is avoiding code duplication.

This is where `arene::base::constraints` can help. Whereas for function
templates we added an extra non-type template parameter using
`arene::base::constraints` which was defaulted to `nullptr`, for class
templates, we add an extra type template parameter, which is defaulted to
`arene::base::constraints<>` --- `arene::base::constraints` instantiated with no
constraints at all. There are other types that you could use here (since this
parameter is only used for partial specialization), but this makes it clear that
this is the constraints parameter.

```{.cpp}
template<typename T, typename = arene::base::constraints<>>
class Foo {
  // general implementation
};
```

Then for the partial specialization you specify the constraints that must be
satisfied:

```{.cpp}
template<typename T>
class Foo<T, arene::base::constraints<std::enable_if_t<std::is_integral<T>::value>>> {
  // implementation for integers
};
```

Now `Foo<int>` and `Foo<unsigned long long>` will use the specialization, but
`Foo<std::string>` will use the primary template.

## Constraining variable templates {#constraining-variable-templates}

Variable templates are similar to class templates: they can't be overloaded, but
they can be partially specialized. Therefore the same partial specialization
mechanism can be used for variable templates as for class templates.

```{.cpp}
template<typename T, typename = arene::base::constraints<>>
variable_type some_variable = /* general implementation */;

template<typename T>
variable_type some_variable<
  T,
  arene::base::constraints<std::enable_if_t</*condition*/>>> = /* specific implementation */;
```
