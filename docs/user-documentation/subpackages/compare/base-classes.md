<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page operator-inheritance Defining Operators via Inheritance

<!-- markdownlint-enable MD041 -->

If you are writing a class which is intended to be a _value_ type, then you will
probably want to provide comparisons. All that is needed to form a total
ordering (as shown by it being the only requirement for `std::sort`) is that you
can provide an `operator<`, but checking other conditions using only `operator<`
can lead to convoluted code and mistakes, and questions like _Is `!(a < b)`
really what is meant here?_ It is much clearer if you can write `a >= b`
instead.

To reduce the effort of writing all the different comparison operators, Arene
Base provides a selection of class templates which can be used as base classes,
and which provide the full set of comparison operators given one or two basic
comparisons.

For the best experience, and ease of migration to the
[C++20 three-way-comparison operator](https://eel.is/c++draft/expr.spaceship)
`<=>` in future, user defined types should provide `three_way_compare` and then
the remaining ordering operations can be provided
["from just the three-way-comparison function"](#full-ordering-from-three-way-compare),
or
["from three-way-comparison and equality"](#full-ordering-from-three-way-compare-and-equality).

Alternatively, if your type already has `operator<` or an equivalent operation,
or such an operation is the easiest to write, then the remaining operators can
be provided based on
["just less-than"](#full-ordering-when-all-you-have-is-less-than), or
["less-than and equality"](#full-ordering-from-less-than-and-equals).

## Full ordering from three-way-compare {#full-ordering-from-three-way-compare}

If you've written a nice `three_way_compare` function for your class, it would
be great to have the operators generated for you like C++20 does from the
spaceship operator. `arene::base::generic_ordering_from_three_way_compare` does
just that: it generates all the comparison operators from the class'
`three_way_compare` member function.

It is a class template with only one template parameter: the type of the derived
class. The class template will then provide all the comparison operators, for
every type of `other` where `Derived::three_way_compare(derived, other)` is
well-defined. If the `Derived` class also provides
`Derived::fast_inequality_check(derived, other)`, then the equality and
inequality operators will use that to avoid calling `three_way_compare` for
instances that are definitely not equal.

A string-like type could thus be defined as:

\snippet docs/examples/ordering_examples.cpp string_like_generic_example

This will provide all operators for comparing an instance of `string_like` with
another instance of `string_like`, or an instance of `arene::base::string_view`
or `arene::base::null_terminated_string_view`, in either order. Equality
comparisons against `string_like` or `arene::base::string_view` will check the
length first to avoid character-by-character checking of long strings of unequal
lengths, but comparisons against `arene::base::null_terminated_string_view` do
not do that, as the length is not available without traversing the string
anyway.

## Full ordering when all you have is "less than" {#full-ordering-when-all-you-have-is-less-than}

If the only operation you want to write is `operator<`, to support STL
algorithms like `std::sort`, then
`arene::base::full_ordering_operators_from_less_than` is your friend. It is a
class template with two template parameters. The first is the type of the
derived class, and the second is the type of objects you would like to compare
it to, which defaults to the type of the derived class. The class template will
then provide the remaining 5 comparison operators, on the assumption that
`derived < other` and `other < derived` are well-defined.

For example:

```{.cpp}
class foo: arene::base::full_ordering_operators_from_less_than<foo> {
private:
  int i;

public:
  ARENE_NODISCARD friend bool operator<(const foo& lhs, const foo& rhs) noexcept {
    return lhs.i < rhs.i;
  }
};
```

The developer of class `foo` only needs to provide `operator<`, and all 6
comparisons are now provided due to the base class.

The second template parameter can be used if you also want your class to be
comparable with instances of another type. For example, it might make sense for
a string-like type to be comparable with `arene::base::string_view`:

```{.cpp}
class string_like:
  arene::base::full_ordering_operators_from_less_than<string_like>,
  arene::base::full_ordering_operators_from_less_than<string_like, arene::base::string_view>
{
public:
  ARENE_NODISCARD friend bool operator<(const string_like& lhs, const string_like& rhs) noexcept {
    /* whatever */
  }
  ARENE_NODISCARD friend bool operator<(const string_like& lhs, arene::base::string_view rhs) noexcept {
    /* whatever */
  }
  ARENE_NODISCARD friend bool operator<(arene::base::string_view lhs, const string_like& rhs) noexcept {
    /* whatever */
  }
};
```

Note the need for 2 additional overloads of `operator<`: as well as the initial
overload with the `string_like` type on both sides, we have a new overload with
`string_like` on the left-hand-side, and `arene::base::string_view` on the
right-hand-side, and a second new overload with `string_like` on the
right-hand-side, and `arene::base::string_view` on the left-hand-side.

The additional base class allows the comparisons with `arene::base::string_view`
to work as expected, both ways round. Between them, the two base classes now
provide 15 of the required 18 operators.

## Full ordering from "less than" and "equals" {#full-ordering-from-less-than-and-equals}

In some cases, checking for equality with `operator<` is slow. The poster child
for this is string comparisons. `"hello"` and `"hello world"` are clearly
distinct, but if all you have is `operator<`, then you have to process all of
`"hello"` before you know which is less than the other. If the common prefix is
long, this can be incredibly slow. If all you have is `operator<`, then equality
is _even slower_ to check: you have to do the comparison _twice_, since equality
is `!(a<b) && !(b<a)`. On the other hand, if you know the string length, then
you can see immediately that they are not equal because they have different
lengths. You might therefore want to write `operator==` yourself, as well as
`operator<`, in order to provide the fast equality comparison.

If this is your scenario,
`arene::base::full_ordering_operators_from_less_than_and_equals` is your friend.
It is a class template with two template parameters. The first is the type of
the derived class, and the second is the type of objects you would like to
compare it to, which defaults to the type of the derived class. The class
template will then provide the remaining 4 comparison operators, on the
assumption that `derived < other`, `other < derived`, `derived == other` and
`other == derived` are well-defined.

A string-like type might thus inherit from this base class twice: once for
self-comparisons, and once for comparisons against `arene::base::string_view`:

```{.cpp}
class string_like:
  arene::base::full_ordering_operators_from_less_than_and_equals<string_like>,
  arene::base::full_ordering_operators_from_less_than_and_equals<string_like, arene::base::string_view>
{
public:
  ARENE_NODISCARD friend bool operator<(const string_like& lhs, const string_like& rhs) noexcept {
    /* whatever */
  }
  ARENE_NODISCARD friend bool operator<(const string_like& lhs, arene::base::string_view rhs) noexcept {
    /* whatever */
  }
  ARENE_NODISCARD friend bool operator<(arene::base::string_view lhs, const string_like& rhs) noexcept {
    /* whatever */
  }
  ARENE_NODISCARD friend bool operator==(const string_like& lhs, const string_like& rhs) noexcept {
    if(lhs.length() != rhs.length()) return false;
    return character_by_character_equality(lhs, rhs);
  }
  ARENE_NODISCARD friend bool operator==(const string_like& lhs, arene::base::string_view rhs) noexcept {
    if(lhs.length() != rhs.length()) return false;
    return character_by_character_equality(lhs, rhs);
  }
  ARENE_NODISCARD friend bool operator==(arene::base::string_view lhs, const string_like& rhs) noexcept {
    if(lhs.length() != rhs.length()) return false;
    return character_by_character_equality(lhs, rhs);
  }
};
```
