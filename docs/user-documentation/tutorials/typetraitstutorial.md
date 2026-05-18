<!--
Copyright 2026, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page typetraitstutorial Tutorial for the type_traits subpackage

<!-- markdownlint-enable MD041 -->

The \link type_traits type_traits \endlink sub-package provides a series of
class and variable templates that describe properties of their template
parameters, akin to the type traits provided by the C++ Standard Library. Some
of these are back-ported from later versions of the C++ Standard, while others
are provided as extensions.

These are typically used in conjunction with \link constraints constraints
\endlink and `std::enable_if_t` for overload selection or template
specialization, or `static_assert`s for verifying that requirements are met.

## Build setup {#build-setup}

To use the type traits sub-package, add it to the dependencies in your
`BUILD.bazel` file:

\snippet docs/examples/BUILD.bazel type_traits_build_example

Then, you can include the header file in your source code:

\snippet docs/examples/type_traits_tutorial.cpp type_traits_include

## Categories of Type Traits {#categories-of-type-traits}

There are two categories of type traits. The most common is one that gives you a
**value**, usually a `bool`, to indicate whether a type has a particular
property (e.g. is the type an integral type?), or the numerical value of a
property (such as the array bounds for an array type). The second category gives
you another **type**, based on the template parameters, such as the supplied
type with `const` added or removed, or the result type of calling a function
with the specified parameters.

### _Value_ type traits {#value-type-traits}

These are either class templates with a `static` data member of the appropriate
type called `value` that has the relevant value (e.g.
`std::is_integral<T>::value`), or variable templates that are the actual value
(e.g. `arene::base::is_less_than_comparable_v<T>`). Typically, variable template
type traits have a `_v` suffix. This helps distinguish them from the class
template ones, and allows both forms to be provided for some type traits (e.g.
`arene::base::is_nothrow_invocable<T>::value` and
`arene::base::is_nothrow_invocable_v<T>`).

The class template form usually derives from an instantiation of
`std::integral_constant`. This is particularly useful when the trait is boolean,
as the type trait class is then derived from `std::true_type` or
`std::false_type` depending on whether the value is `true` or `false`.

### _Type_ type traits {#type-type-traits}

These are either class templates with a type alias member called `type` that has
the relevant type (e.g. `std::remove_const<T>::type`), or an alias template that
is the actual resulting type (e.g. `std::remove_reference_t<T>`). Typically,
alias template type traits have a `_t` suffix. Again, this helps distinguish
them from the class template ones, and allows both forms to be provided for some
type traits (e.g. `std::make_signed<T>::type` and `std::make_signed_t<T>`).

## Usage of Type Traits {#usage-of-type-traits}

The most common use of type traits to _constrain_ function templates or template
specializations, or to generate errors if conditions aren't met using
`static_assert`.

### Checking Constraints {#checking-constraints}

Suppose you have a function template that requires that the template parameter
is a type which can be compared for equality. There are 3 ways you can check
this property:

1. Use equality comparison in your function, and let the compiler generate an
   error if the user supplies a type that doesn't support comparison.

   \snippet docs/examples/type_traits_tutorial.cpp unchecked_function

2. Use a `static_assert` to check if the type is equality-comparable, and report
   an error if it is not.

   \snippet docs/examples/type_traits_tutorial.cpp static_assert_function

   Here, the `static_assert` uses the `arene::base::is_equality_comparable_v`
   type trait to check the property of the template parameter `T`.

3. _Constrain_ the function template so that it is ignored if the type is not
   equality-comparable.

   \snippet docs/examples/type_traits_tutorial.cpp constrained_function

   Here, the `arene::base::constraints<>` template parameter introduces a list
   of constraints on the function. In this case, there is only one: the function
   is only enabled if the `arene::base::is_equality_comparable_v` type trait
   evaluates to `true` for the template parameter `T`.

The first choice is the "default" if you don't think about things, but leads to
the worst error messages, particularly if the use of `==` is not directly in the
function body, but in some other template needed for the implementation.

```text
docs/examples/type_traits_tutorial.cpp:20:16: error: invalid operands to binary expression ('type_traits_tutorial::x' and 'type_traits_tutorial::x')
   20 |   if (some_arg == some_other_arg) {
      |       ~~~~~~~~ ^  ~~~~~~~~~~~~~~
docs/examples/type_traits_tutorial.cpp:27:17: note: in instantiation of function template specialization 'type_traits_tutorial::unchecked::func<type_traits_tutorial::x>' requested here
   27 | void wibble() { func(x1, x2); }
      |                 ^
```

The second choice gives you a nice error message of your choosing instead of (or
in addition to) the default compiler error message.

```text
docs/examples/type_traits_tutorial.cpp:34:7: error: static assertion failed due to requirement 'arene::base::is_equality_comparable_v<type_traits_tutorial::x, type_traits_tutorial::x, std::nullptr_t>': T must be equality-comparable; please check that you are calling the right function
   34 |       arene::base::is_equality_comparable_v<T>,
      |       ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
docs/examples/type_traits_tutorial.cpp:44:17: note: in instantiation of function template specialization 'type_traits_tutorial::assertion::func<type_traits_tutorial::x>' requested here
   44 | void wibble() { func(x1, x2); }
      |                 ^
```

The third choice gives you an error message that tells you that the function
could not be called because a requirement was not met:

```text
docs/examples/type_traits_tutorial.cpp:56:17: error: no matching function for call to 'func'
   56 | void wibble() { func(x{}, x{}); }
      |                 ^~~~
docs/examples/type_traits_tutorial.cpp:47:6: note: candidate template ignored: requirement 'arene::base::is_equality_comparable_v<type_traits_tutorial::x, type_traits_tutorial::x, std::nullptr_t>' was not satisfied [with T = x]
   47 | void func(T some_arg, T some_other_arg) {
   |      ^
```

However, the difference is more than just the error messages. This is hinted at
by the text of the error message for the constrained function: "no matching
function for call". Whereas in the other two cases, the function is available to
call, and the error comes from the function body, in this case, the function is
not available to call. This allows you to add overloads of the same function
with different constraints, and the compiler will pick the one that is valid for
the supplied parameters. It also allows you to _check_ if the function can be
called with the specified parameters.

This is actually what the `is_equality_comparable_v` type trait does internally:
it checks to see if `a == b` is a valid expression for the type `T`. See \link
constraints the constraints package documentation \endlink for an example of
that sort of check.

For most cases, using `arene::base::constraints` to specify the constraints is
the best choice, as it allows different overloads of the same function to be
specified with different constraints.

#### Selecting Overloads With Type Traits {#selecting-overloads-with-type-traits}

If the implementation of a function depends on the properties of one of the
template parameters, type traits can be used with constraints to select between
multiple overloads of the same function that use different implementations.

For example, reversing a sequence can be done for any range specified with
**forward** iterators, but the algorithm is better with **bidirectional**
iterators, and best with **random access** iterators.

By specifying constraints on the functions, we can write 3 different overloads
that use the different algorithms, and overload resolution will choose the
overload that matches the constraints:

\snippet docs/examples/type_traits_tutorial.cpp reverse_overloads

Note: since multiple constraints could be true at once, (e.g. a random access
iterator is also a bidirectional iterator and a forward iterator), we use
_negative_ constraints as well, to prevent ambiguities.

Calling `reverse(begin,end)` then chooses the overload depending on the iterator
category. If you pass plain input or output iterators, then you get an error as
none of the overloads are viable.

#### Class Template Partial Specialization With Type Traits {#class-template-partial-specialization-with-type-traits}

\link constraints Constraints \endlink don't just work with function templates:
they can be used for partially specializing class templates too.

The primary template has an additional unnamed type template parameter,
defaulted to `arene::base::constraints<>` . This is used as the fallback for
when none of the constrained specializations apply. It can be left undefined, or
a default implementation can be provided, as appropriate.

\snippet docs/examples/type_traits_tutorial.cpp primary_template

The specializations then provide concrete `constraints` parameters based on the
properties of the other template parameters:

\snippet docs/examples/type_traits_tutorial.cpp template_specializations

Here, there are specializations for types that are both less-than comparable and
equality comparable, and for types that are just less-than comparable. Supplying
a type that is not ordered would fall back to the unimplemented primary template
and give an error.

### Adjusting Types {#adjusting-types}

Another major use of type traits is to adjust a provided type to yield another,
for use as a return type, or the type of a data member.

\snippet docs/examples/type_traits_tutorial.cpp adjusted_type

Here, the return type is based on the `field_type` from the template parameter
`T`, but instead of just returning that type directly, the type traits are used
to first make the type `const`, and then add a reference. Presumably, this is so
that the function can return a reference to an internal data member of type
`T::field_type`, but this is a `const` member function, so a `const` reference
is desired.

A similar common case for adjusting types is to **remove** `const`, `volatile`
and reference qualification from a type:

\snippet docs/examples/type_traits_tutorial.cpp remove_cvref

Here, the template accepts **any** type, but the "stored copy" has the
qualifications removed, so it actually _is_ a copy, and not a reference, and it
can be modified by the other functions.
