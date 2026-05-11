<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page three-way-compare Three-way Comparison Support

<!-- markdownlint-enable MD041 -->

A "Three-way comparison" between two objects yields a result indicating which of
the 3 relative orderings apply: are the values equal, the first less than the
second, or the first greater than the second. This is the fundamental operation
that the
[C++20 three-way-comparison operator](https://eel.is/c++draft/expr.spaceship)
(also known as the spaceship operator) performs, as does the C `strcmp`
function.

`arene::base::three_way_compare` is a function object type that performs this
three-way comparison for any types that provide ordering comparisons. Instances
of `arene::base::three_way_compare` can be called with two arguments, and the
result is a `arene::base::strong_ordering` value indicating the result of the
comparison. For built-in types, and types without a custom three-way comparison,
the result is determined either using `operator==` and `operator<` if both are
available, or just `operator<` if that is all that is available. If the
operators are not available, and the first argument type does not provide a
custom three-way comparison, then the comparison is ill-formed.

```{.cpp}
auto r1 = arene::base::three_way_compare(1, 2);                                            // OK, compares integers
auto r2 = arene::base::three_way_compare(1, "hello");                                      // error, cannot compare number to string
auto r3 = arene::base::three_way_compare(std::complex<double>{}, std::complex<double>{});  // error, cannot compare complex numbers for ordering
auto r4 = arene::base::three_way_compare(std::string{"hello"}, std::string{"goodbye"});    // OK, compares strings with operators
auto r5 = arene::base::three_way_compare(std::string{"hello"},
                                     arene::base::string_view{"goodbye"});  // OK, compares strings with operators
auto r6 = arene::base::three_way_compare(arene::base::string_view{"hello"},
                                     arene::base::string_view{"goodbye"});  // OK, compares strings with member function
```

## Custom three-way comparison for class types {#custom-three-way-comparison-for-class-types}

You can specify a three-way comparison operation for your type by writing a
`static` member function called `three_way_compare` that accepts the two values
to be compared and returns an instance of `arene::base::strong_ordering`:

```{.cpp}
struct X {
  ARENE_NODISCARD static arene::base::strong_ordering three_way_compare(const X& lhs, const X& rhs) noexcept;
};
```

This member function will then be used by the `arene::base::three_way_compare`
function object in preference to any comparison operators: if the first
parameter is a class type that has a `static` member function named
`three_way_compare` that accepts the values to be compared and returns
`arene::base::strong_ordering`, then that member function is used for the
comparison.

Note that the second argument does not have to be the same as your class type,
so this can be used to define three-way comparison with other types.

The `arene::base::three_way_compare` function object might be used for the
implementation, if the result is just the result of comparing data members.

```{.cpp}
struct X{
  int val;

  /// Compare based on the value of the member val
  ARENE_NODISCARD static arene::base::strong_ordering three_way_compare(const X& lhs, const X& rhs) noexcept {
    return arene::base::three_way_compare(lhs.val, rhs.val);
  }

  /// Compare against a plain integer
  ARENE_NODISCARD static arene::base::strong_ordering three_way_compare(const X& lhs, int rhs) noexcept {
    return arene::base::three_way_compare(lhs.val, rhs);
  }
};

auto r7 = arene::base::three_way_compare(X{42}, X{99});                   // OK, compares objects with member function
auto r8 = arene::base::three_way_compare(X{42}, 99);                      // OK, compares object and int with member function
```

### Three-way comparison for multiple data members {#three-way-comparison-for-multiple-data-members}

If you have a class with multiple data members, and you wish to write a
`three_way_compare` function that compares all the data members, you can use
`std::tie` to do this:

```{.cpp}
struct Y {
  int val1;
  arene::base::inline_string<10> val2;
  unsigned val3;

  /// Compare based on the value of the members val1, val2 and val3
  ARENE_NODISCARD static arene::base::strong_ordering three_way_compare(const Y& lhs, const Y& rhs) noexcept {
    return arene::base::three_way_compare(
      std::tie(lhs.val1, lhs.val2, lhs.val3),
      std::tie(rhs.val1, lhs.val2, lhs.val3));
  }
};
```

This creates `std::tuple`s that hold references to the data members, and
compares those. This is a lexicographical ordering based on the order specified
to `std::tie`, so if `lhs.val1` is less than `rhs.val1` then the result is
`arene::base::strong_ordering::less` even if `lhs.val2` is greater than
`rhs.val2`.

Currently this uses the `operator<` and `operator==` for the individual
elements, rather than any `three_way_compare` operations defined for the data
member types, but future versions will use `three_way_compare` for the data
members if available.
