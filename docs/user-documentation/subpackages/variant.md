<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page variant variant: A Discriminated Union Type

<!-- markdownlint-enable MD041 -->

`arene::base::variant` is a backport of
[`std::variant`](https://en.cppreference.com/w/cpp/container/variant), with some
modifications to be in line with the general error handling policies of
`arene_base`.

The public header is

\snippet docs/examples/subpackages/variant_examples.cpp include_header

The Bazel target is

```text
//:variant
```

# A Discriminated Union Type {#subpackages-variant-a-discriminated-union-type}

`arene::base::variant` allows you to store an object which is one of a specified
list of types. This is similar to a `union`, but with stronger type safety: the
type of the stored object can always be queried, and attempting to access the
stored object via the wrong type is a runtime error rather than undefined
behaviour; variants are therefore also known as _discriminated unions_. Beyond
type safety, `variant` allows for types with non-trivial constructors,
destructors and assignment operators to be stored directly. This is in contrast
to a plain `union` which would require the manual invocation of constructors,
assignment operators, and destructors to properly manage object lifetimes.

## Declaring a Variant {#subpackages-variant-declaring-a-variant}

`variant`s are declared with a list of types that represent the possible active
alternatives. Note that the types do not need to be unique. However, it is
unspecified which alternative of a duplicate type is interacted with when using
type-based access APIs, and the visitor pattern will not distinguish between
them.

\snippet docs/examples/subpackages/variant_examples.cpp declaring_variant

## Interacting with a Variant {#subpackages-variant-interacting-with-a-variant}

There are two primary ways to interact with a `variant`: procedurally, and with
the visitor pattern.

### Procedural Access {#subpackages-variant-procedural-access}

With procedural access, the active alternative is queried, and the appropriate
accessor method is used to fetch the active alternative. The active alternative
can be queried by index or type, and similarly can be accessed by index or type.
It is invalid to access an alternative which is not currently active. Doing so
will throw `arene::base::bad_variant_access`.

A simple example of procedural access for printing the active alternative can be
seen below:

\snippet docs/examples/subpackages/variant_examples.cpp procedural_variant_usage

### Visitor Pattern {#subpackages-variant-visitor-pattern}

The visitor pattern allows for a more structured way to interact with the
`variant`. A visitor is a callable object that can be invoked with all of the
possible alternatives of the `variant`. The visitor can be a function, a lambda,
or any invocable object that can accept the types of the alternatives in the
`variant`. The visitor is invoked with the active alternative, and can perform
operations based on the type of the alternative. The return types of the
alternative handlers in the visitor can be any type, including `void`, and this
will be the return type of `visit`. The cref qualification of both the `visitor`
and `variant` are maintained.

@note If the return type for `visit` is not explicitly specified, then the
return type from every alternative's handler must be _exactly_ the same type.
Alternatively, by invoking `visit<RetT>(visitor, variant)`, the requirement is
relaxed to require only that the return type from every alternative's handler is
implicitly convertible to `RetT`.

The printing example above can be rewritten using the visitor pattern as
follows:

\snippet docs/examples/subpackages/variant_examples.cpp visit_variant_usage

The visitor pattern is exactly analogous to overload resolution which has been
deferred to runtime. In fact, a free function overload set can be exposed for
runtime overload resolution across a constrained type set like so:

\snippet docs/examples/subpackages/variant_examples.cpp binding_an_overload_set

This pattern is useful for building up a DSL around a constrained type set as a
form of dynamic polymorphism with different tradeoffs. Typical inheritance with
virtual functions makes adding new _type-dependent behavior_ for existing
operations easy: you simply implement a new type which inherits from the base
and overload the methods. However adding new _operations_ is difficult, because
it involves altering all existing derived classes. In contrast, using a variant
and the visitor pattern makes adding new _operations_ easy: you simply implement
a new visitor. However adding new _types_ to existing operations is difficult,
as it involves altering all existing visitors. Which is more appropriate for a
given application will depend on system design.

### bind_overloads {#subpackages-variant-bind_overloads}

The `arene::base::bind_overloads` utility function, provided by the
`//:functional` subpackage, can be used to simplify the creation of visitors for
use with `arene::base::variant`, allowing the easy use of inline lambdas to
create visitors, like so:

\snippet docs/examples/subpackages/variant_examples.cpp bind_overloads_usage

### Casting Variants {#subpackages-variant-casting-variants}

The `arene::base::variant_cast` function can be used to cast between variants of
compatible _type spaces_. This is useful when you need to extend or rearrange
the alternatives in a variant.

The cast function supports both copy semantics (for const lvalue references) and
move semantics (for rvalue references). The function will preserve the active
alternative and its value, forwarding it to the corresponding type in the target
variant.

A simple example of casting a variant can be seen below:

\snippet docs/examples/subpackages/variant_examples.cpp variant_cast_usage

#### Requirements compatible _type spaces_ in variant_cast {#subpackages-variant-variant-cast-requirements}

For `variant_cast<variant<To...>>(variant<From...>)` to be invocable, the _type
spaces_ of the source and target variants must be compatible. For two type
spaces, `Ts...` and `Us...`, to be compatible, the following conditions must be
met:

> For every type in `From...` there must be a type in `To...` which satisfies
> `std::is_constructible<To, From>` where `From`'s cref qualification matches
> `FromVariant`'s.
>
> For every type in `From...` there must be a unique, unambiguous _best_ type in
> `To...` to construct from it. This _best_ type is determined using standard
> overload resolution rules. If the _best_ type is ambiguous, then the cast is
> ill-formed.

If these conditions are not met, the function will not be invocable, and the
code will fail to compile. This ensures type safety at compile time.

For example
`variant_cast<variant<std::int32_t, std::int64_t>>(variant<std::int16_t>(1))`
will fail to compile because both `std::int32_t` and `std::int64_t` are
constructible from `std::int16_t`, and neither is an exact match. Whereas
`variant_cast<variant<std::int32_t, std::int64_t>>(variant<std::int32_t>(1))`
will compile and always select the `std::int32_t` alternative in the target
variant because it is an exact match, and
`variant_cast<variant<std::int32_t, not_an_int>>(variant<std::int16_t>(1))` will
always select the `std::int32_t` alternative because there is no ambiguity.

Keep in mind that a compatible _type space_ may be _smaller_ in terms of number
of types than the source type space, so long as the requirements of compatible
_type spaces_ are met. For example it is possible to cast from
`variant<std::string, std::string_view, double>` to
`variant<std::string, double>`, as `std::string` is an unambiguous match for
`std::string_view` to cast to.

A couple of additional notes:

- The order of the types in the target variant does not need to match the order
  of the types in the source variant. The active alternative will be preserved
  based on type, not position.
- If there are duplicate types in the target variant, it is unspecified which
  alternative (index-wise) of the duplicate alternatives in the target variant
  will be used. It is recommended to avoid duplicate types in variants when
  using `variant_cast` if index-based differentiation is important.
- While the failure to match _any_ type will result in a constraints failure, if
  there are multiple types in the target variant that are constructible from a
  type in the source variant and none of them are exact type matches, then you
  will instead get an _ambiguous overload resolution_ error at compile time.
