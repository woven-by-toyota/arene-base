<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page type_manipulation type_manipulation: Template Metaprogramming Facilities

<!-- markdownlint-enable MD041 -->

The `type_manipulation` subpackage provides low-level utilities that are useful
for writing C++ templates, where the behaviour or properties of a class or
function depend on the properties of the template parameters.

There is no singular public header for this subpackage, as each facility is
bespoke to a given usecase and there is unlikely to be overlap between usage.

The Bazel target is

```text
//:type_manipulation
```

## Introduction {#introduction}

The `type_manipulation` subpackage provides facilities that mostly produce a new
type based on one or more existing types and properties. These vary from an
alias template that applies the `const` and `volatile` qualifiers from the
source type to the target type (`arene::base::give_qualifiers_to`), to a dummy
type that cannot ever be constructed (`arene::base::non_constructible_dummy`),
to a class template for passing a specified number of identical template
arguments to another class template (`arene::base::repeat_type`), amongst
others.

## A Non-constructible Dummy Type {#a-non-constructible-dummy-type}

`arene::base::non_constructible_dummy` can never be constructed: all its
constructors, and its destructor are defined as deleted. This means that there
can never be any instances of this type, and function overloads that accept it
as a function parameter can never be called.

Though there is little benefit to declaring a function that unconditionally
takes `arene::base::non_constructible_dummy` as an argument, there can be
benefit to _conditionally_ declaring a function to take one. For example, if a
move constructor should be declared _deleted_ under certain circumstances, then
this can be achieved with `arene::base::non_constructible_dummy`:

```{.cpp}
template<typename T>
class foo {
  static constexpr bool move_constructor_should_be_deleted = some_property_of<T>;

public:
  foo(std::conditional_t<move_constructor_should_be_deleted, foo, arene::base::non_constructible_dummy>&& ) =delete;
  // rest of foo
};
```

If `some_property_of<T>` is `true`, then the `std::conditional_t` evaluates to
`foo`, so the declaration is `foo(foo&&) = delete;`, and the move constructor is
deleted. If `some_property_of<T>` is `false` then the declaration is instead
`foo(arene::base::non_constructible_dummy&&) = delete;`, which deletes a
constructor overload that can never be called. This leaves the actual move
constructor unaffected.

Header:

```{.cpp}
#include <arene/base/type_manipulation/non_constructible_dummy.hpp>
```

## Types For Copying Properties {#types-for-copying-properties}

When writing C++ templates, it is relatively common to want a type that has the
same properties as another type, and the `type_manipulation` subpackage provides
two facilities to help with that: `arene::base::give_qualifiers_to` and
`arene::base::implicit_constructor_base`.

`arene::base::give_qualifiers_to` is a type alias that copies the `const` and
`volatile` qualifiers from the source to the target. Thus
`arene::base::give_qualifiers_to<const int, std::string>` is an alias for
`const std::string`, and
`arene::base::give_qualifiers_to<volatile double, std::uint32_t>` is an alias
for `volatile std::uint32_t`. This can be used to propagate `const` to something
referenced via a pointer:

```{.cpp}
template<typename T>
arene::base::give_qualifiers_to<T, typename T::data_type>& get_data(T& t) {
  return *t.get_data_ptr();
}

some_class x;
const some_class y;
auto& xd= get_data(x);
auto& yd= get_data(y);
```

Here, `T` is deduced, so includes the `const` or `volatile` qualification of the
supplied argument, which is then propagated to the return type of `get_data`, so
`xd` is just `some_class::data_type&`, whereas `yd` is
`const some_class::data_type&`.

Header:

```{.cpp}
#include <arene/base/type_manipulation/give_qualifiers_to.hpp>
```

The second facility for copying properties is
`arene::base::implicit_constructor_base`, which is a class template that takes a
single type argument, and has the same set of built-in constructors as the
supplied type, declared as _defaulted_ or _deleted_ as appropriate. Thus
`arene::base::implicit_constructor_base<int>` has all the constructors
defaulted, whereas
`arene::base::implicit_constructor_base<std::uinque_ptr<int>>` has a defaulted
default constructor and defaulted move constructor, and the other constructors
are deleted. This is useful for a base class of something like
`arene::base::optional`, that holds a value of type `T`, where the availability
of the constructors should match that of `T`.

Note: the use of this class as a base class merely affects whether a given
constructor is _defaulted_ or _deleted_ in the derived class, either when
implicitly provided, or when defined using `= default`. If you need non-default
behaviour for the constructors, then you will still need to write them
explicitly, and the normal rules for when constructors are implicitly declared
still apply.

Header:

```{.cpp}
#include <arene/base/type_manipulation/implicit_constructor_base.hpp>
```

## The Smallest Integer Type That Can Hold A Value Or Range Of Values {#the-smallest-integer-type-that-can-hold-a-value-or-range-of-values}

If you are trying to save space, and you wish to store an integer value with a
range determined from template parameters, then it can be helpful to identify
the smallest integer type that can store the desired range. To this end, the
`type_manipulation` subpackage provides two type aliases:
`arene::base::smallest_unsigned_integer_for` and
`arene::base::smallest_signed_integer_for`.

`arene::base::smallest_unsigned_integer_for` takes a single template parameter,
which is a `std::uint64_t`, and is an alias for the smallest unsigned integer
type that can hold the given value.

Examples:

- `arene::base::smallest_unsigned_integer_for<25>` => `std::uint8_t`
- `arene::base::smallest_unsigned_integer_for<66000>` => `std::uint32_t`.

On the other hand, `arene::base::smallest_signed_integer_for` takes _two_
template parameters of type `std::int64_t`, which are the minimum and maximum
values to be stored. This allows for the case where the negative magnitude of
the range may be larger than the positive magnitude. This is a type alias for
the smallest _signed_ integer type that can hold the minimum and maximum values.

Examples:

- `arene::base::smallest_signed_integer_for<-100,100>` => `std::int8_t`,
- `arene::base::smallest_signed_integer_for<-1000,-100>` => `std::int16_t`
- `arene::base::smallest_signed_integer_for<-1000,100000>` => `std::int32_t`
- `arene::base::smallest_signed_integer_for<100,1000>` => `std::int16_t`

Header:

```{.cpp}
#include <arene/base/type_manipulation/smallest_integer_for.hpp>
```

## Empty Base Optimization {#empty-base-optimization}

The Empty Base Optimization (EBO) is a space saving technique based on the
feature that a base class with no members can have the same address as a class
derived from it, and thus occupy no additional space, whereas a data member
always has to occupy at least one byte. Taking advantage of this is tricky in
templates, since not all types can be used as base classes.
`arene::base::ebo_holder` takes care of this for you: if you derive a class from
`arene::base::ebo_holder<Tag, T>` then it will take advantage of EBO if `T` is a
class type that can be derived from, and use a data member otherwise. In either
case, the API for accessing the value is the same: `this->get_value(Tag{})`.
`Tag` is purely used to differentiate different base classes so that you can use
multiple instances of `arene::base::ebo_holder` in the same class, and is
typically an empty `struct` named after the purpose of the base class.

For example, a "compressed pair" type designed to use minimal space might be
implemented as:

```{.cpp}
struct first_field_tag{};
struct second_field_tag{};

template<typename T1, typename T2>
class compressed_pair:
  arene::base::ebo_holder<first_field_tag, T1>,
  arene::base::ebo_holder<second_field_tag, T2> {
public:
  T1& first() {
    return this->get_value(first_field_tag{});
  }
  T2& second() {
    return this->get_value(second_field_tag{});
  }

  // other members
};
```

The use of `first_field_tag` and `second_field_tag` means that the correct
fields are accessed even if `T1` and `T2` are the same.

Using this example type, the following holds:

```{.cpp}
struct empty_class{};
struct other_empty_class{};

class derived : compressed_pair<empty_class, other_empty_class>
{
  std::int32_t data;
};

assert(sizeof(std::pair<empty_class, empty_class>) == 2);
assert(sizeof(compressed_pair<empty_class, empty_class>) == 2);
assert(sizeof(std::pair<empty_class, other_empty_class>) == 2);
assert(sizeof(compressed_pair<empty_class, other_empty_class>) == 1);
assert(sizeof(std::pair<std::int32_t, empty_class>) == 8);
assert(sizeof(compressed_pair<std::int32_t, empty_class>) == 4);
assert(sizeof(std::pair<empty_class, std::int32_t>) == 8);
assert(sizeof(compressed_pair<empty_class, std::int32_t>) == 4);
assert(sizeof(std::pair<std::int32_t, std::uint32_t>) == 8);
assert(sizeof(compressed_pair<std::int32_t, std::uint32_t>) == 8);
assert(sizeof(derived) == 4);
```

The use of `arene::base::ebo_holder` means that in every case where the types
are different, if one of them is empty then it doesn't contribute to the size.
Also, even though `compressed_pair<empty_class, other_empty_class>` has a size
of 1 since every object has a size of at least 1, `sizeof(derived)` is still 4:
the EBO is applied to both fields of the pair.

Header:

```{.cpp}
#include <arene/base/type_manipulation/ebo_holder.hpp>
```

## Static If {#static-if}

`arene::base::static_if` is a variation on `std::conditional`, designed to
minimize template instantiations for the non-selected cases. If this isn't a
problem, `std::conditional` is likely simpler and clearer.

This allows selection of class templates based on a condition, even if the
non-chosen template cannot be instantiated with the specified template
parameters.

```{.cpp}
template <class T>
using queue_type
  = typename static_if<std::is_trivially_copyable_v<T>> // The condition
    ::template then_apply_else_apply<
      lockfree_queue,    // A lockfree queue template
      queue_with_locks,  // A queue-with-locks template
      T                  // The parameters to pass to the chosen template
    >;
```

Here, `lockfree_queue<T>` is not instantiated if `T` is not trivially-copyable.
This might be important if `lockfree_queue` contains a `static_assert` to ensure
that it is only instantiated with trivially-copyable types.

Header:

```{.cpp}
#include <arene/base/type_manipulation/static_if.hpp>
```

## Ignoring Values and Forcing Evaluation Order {#ignoring-values-and-forcing-evaluation-order}

The Woven Coding Standards require that every object and function return value
is used. In some cases, this is not desired, so we need an explicit way to
"ignore" the values.

`std::ignore` can be used to ignore a single value:

```{.cpp}
std::ignore = whatever;
```

but this doesn't readily scale when you have multiple expressions to ignore, or
where the expressions are the result of a variadic expansion.

Also, in variadic templates, it is common to want to apply an operation to every
element of a parameter pack. With C++17 fold expressions, this can easily be
done using the comma operator: `(foo<Ts>(), ...)` will expand `foo<T>()` for
each `T` in the variadic parameter pack `Ts`. In C++14 this is not so easy.

Using a comma fold expression also has an additional property that is frequently
useful: the expressions are evaluated in order, so the expansion for the first
variadic element is evaluated before the expansion for the second, and so forth.

If all the elements in the variadic expansion have the same non-`void` type,
then an ignored initializer list can be used, since that preserves the in-order
guarantee:

```{.cpp}
std::ignore = std::initializer_list<TheReturnType>{foo<Ts>()...};
```

However, this requires that `TheReturnType` is known, or readily obtainable.

`arene::base::consume_values` provides a mechanism for addressing these issues:
it takes an `std::initializer_list` of a special type that can be constructed
with anything at all, and then discards the value.

Our fold expression can thus be written as
`arene::base::consume_values({foo<Ts>(), ...})` instead. Note the braces `{}`
around the pack expansion: this is necessary for the `std::initializer_list`. We
no longer need to specify the type of the return value, and it does not have to
be the same for each element.

This also works for non-variadic expansions as a substitute for using
`std::ignore` for each expression in turn:

```{.cpp}
arene::base::consume_values({foo(), bar(), baz()});
```

Again, these can all return different types, and `foo()` is evaluated before
`bar()`, which is evaluated before `baz()`.

Note: this doesn't work if the element of a variadic expansion such as
`foo<T>()` returns `void`. In that case you will need to wrap the call in
something that has a return value; `arene::base::monostate` is ideal for that
return type. You can use a lambda as part of the expansion:

```{.cpp}
template<typename ... Ts>
void bar(){
  arene::base::consume_values({[]{
      foo<Ts>();
      return arene::base::monostate{};}(), ...});
};
```

This does of course work with value packs as well as type packs:

```{.cpp}
template<typename ... Ts>
void baz(Ts&& ... values){
  arene::base::consume_values({foo(values), ...});
};
```

As already mentioned, the use of `std::initializer_list` guarantees a strict
evaluation order, so `foo(value0)` is called before `foo(value1)`, which is in
turn called before `foo(value2)`, and so on.

Header:

```{.cpp}
#include <arene/base/type_manipulation/consume_values.hpp>
```

## Passing Repeated Types To Templates {#passing-repeated-types-to-templates}

If you need to pass multiple copies of the same template argument to a class
template, then `arene::base::repeat_type` is your friend. For example,
`arene::base::repeat_type<5, std::tuple, bool>::value` is an alias for
`std::tuple<bool, bool, bool, bool, bool>`.

You can also pass an additional first argument to the template with
`arene::base::repeat_type_ex`. Thus
`arene::base::repeat_type_ex<5, std::tuple, int, bool>::value` is an alias for
`std::tuple<int, bool, bool, bool, bool, bool>`.

Header:

```{.cpp}
#include <arene/base/type_manipulation/repeat_type.hpp>
```
