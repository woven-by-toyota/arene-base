<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page functional functional: Facilities For Functional Programming

<!-- markdownlint-enable MD041 -->

Arene Base provides facilities to make it easier to write code that uses
invocable types.

The public header is

```{.cpp}
#include "arene/base/functional.hpp"
```

The Bazel target is

```text
//:functional
```

## Introduction {#introduction}

There are many ways of writing expressions that are
[**invocable**](https://en.cppreference.com/w/cpp/utility/functional):
functions, lambdas, class types with `operator()`, function pointers, member
function pointers, etc. Dealing with all of these when accepting something that
should be invoked within generic code can be complex, and type-erased
implementations using
[`std::function`](https://en.cppreference.com/w/cpp/utility/functional/function).
can allocate dynamic memory. The `functional` sub-package provides facilities to
make dealing with such cases easier, especially in combination with the
`arene::base::inline_function` class template, and `arene::base::is_invocable`
from the [type_traits subpackage](\ref type_traits).

`arene::base::invoke` provides a way to invoke _any_ invocable with a specified
set of arguments, seamlessly handling functions, lambdas and member function
pointers. It is a back-port of
[`std::invoke`](https://en.cppreference.com/w/cpp/utility/functional/invoke).

`arene::base::function_ref` provides a type-erased class template that
references an invocable, for use as a function argument. This is a back-port of
[`std::function_ref`](https://en.cppreference.com/w/cpp/utility/functional/function_ref).

`arene::base::function_traits` provides a way to get traits for any given
function type. This includes member function pointers.

`arene::base::bind_front` provides a way to bind arguments to the front of an
existing invocable for argument
[currying](https://en.wikipedia.org/wiki/Currying) without needing hand-written
lambdas. This is a backport of
[`std::bind_front`](https://en.cppreference.com/w/cpp/utility/functional/bind_front.html)

`arene::base::bind_back` provides a way to bind arguments to the back of an
existing invocable for argument
[currying](https://en.wikipedia.org/wiki/Currying) without needing hand-written
lambdas. This is a backport of
[`std::bind_back`](https://en.cppreference.com/w/cpp/utility/functional/bind_back.html)

`arene::base::bind_overloads` provides a way to easily combine function objects
(generally lambdas) together into a single invocable. It's generally used for
creating visitors for variants.

`arene::base::not_fn` provides a way to negate the return of existing invocable
for functional composition without needing hand-written lambdas. This is a
backport of
[`std::not_fn`](https://en.cppreference.com/w/cpp/utility/functional/not_fn.html)

## Using Invocables {#using-invocables}

Invocables are used in multiple contexts:

- For specifying the ordering or hashing operation for associative containers,
- As arguments to algorithms, specifying the operation or comparison to be made
  on each element,
- As callbacks, to be stored and invoked later, under particular conditions,
- As arguments to functions, to be invoked immediately with arguments derived
  from other data (e.g. member data from the class, if the function is a member
  function),
- etc.

The common property in all these cases is that the invocable is being used to
customize the operation of the code to which it is being passed.

There are fundamentally two ways to do this:

1. Specify the invocable with a template type parameter, or
2. Use a type-erased wrapper such as `std::function`,
   `arene::base::function_ref`, or `arene::base::inline_function`.

Which is better depends on the context.

## Specifying An Invocable With A Template Type Parameter For Class Templates {#specifying-an-invocable-with-a-template-type-parameter-for-class-templates}

The common pattern for class templates that need an invocable is to specify the
_type_ of the invocable with a template parameter, and then have the constructor
accept a concrete instance to be used, possibly using a default-constructed
instance by default.

```{.cpp}
template<typename T, typename Comparator>
class my_container {
  Comparator compare_;
public:
  my_container(Comparator comparator = {}): compare_(std::move(comparator)) {}
};

my_container<int, std::less<>> cont1;
my_container<int, custom_comparator> cont2(custom_comparator{arg1,arg2});
```

Inside the class template implementation, the invocable will then be invoked
with some specific set of arguments:

```{.cpp}
template<typename T, typename Comparator>
void my_container::some_member_func(){
  // ...
  if(compare_(val1, val2)) {
    // ...
  } else {
    // ...
  }
  // ...
}
```

### Downsides {#downsides}

This works well, but has downsides. Firstly, if you pass an invocable that
accepts the wrong set of parameters, or returns the wrong type, then the errors
refer to the member function that actually invoked it, rather than the point of
use of the class template. This can lead to messy error messages with multiple
layers of template instantiations, hiding the source of the error.

Secondly, it limits the category of invocables to those that can be invoked
directly with the function call syntax (thus ruling out member function
pointers, for example).

Thirdly, it prevents the use of lambdas, since the type of each lambda is
unique, even if they are token-by-token identical.

### Detecting Issues With static_assert and arene::base::is_invocable_v {#detecting-issues-with-static_assert-and-arene-base-is_invocable_v}

The first problem can usually be solved by adding an appropriate `static_assert`
to the class template body, using `arene::base::is_invocable_v` or
`arene::base::is_invocable_r_v` as the condition. This will trigger errors
immediately upon instantiation of the class template, rather than waiting for
the appropriate member function to be invoked, and provides an opportunity for a
clear error message.

```{.cpp}
template<typename T, typename Comparator>
class my_container {
  static_assert(arene::base::is_invocable_r_v<bool, Comparator, const T&, const T&>,
                "The Comparator must be invocable with two objects of type const T, returning a boolean");
};
```

If we specify an incompatible comparator, then we get an error message from the
`static_assert`:

```{.cpp}
my_container<int, std::hash<std::string>> cont; // wrong comparator
```

The compiler now generates a concise error referencing the `static_assert` and
the container instantiation:

```{.txt}
my_project/header.hpp:123:17: error: static assertion failed due to requirement 'arene::base::is_invocable_r_v<bool, std::hash<std::string>, const int &, const int &>': The Comparator must be invocable with two objects of type const T, returning a boolean
  616 |   static_assert(arene::base::is_invocable_r_v<bool, Comparator, const T&, const T&>,
      |                 ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
my_project/file1.cpp:620:43: note: in instantiation of template class 'my_container<int, std::hash<std::string>>' requested here
  620 | my_container<int, std::hash<std::string>> cont; // wrong comparator
      |                                           ^
```

### Handling Any Invocable With arene::base::invoke {#handling-any-invocable-with-arene-base-invoke}

The second problem is solved by using `arene::base::invoke` rather than calling
the invocable directly:

```{.cpp}
  if(arene::base::invoke(compare_, val1, val2)) {
```

This would then allow `Comparator` to be a member function pointer:
`bool MyData::*(const MyData&) const`

```{.cpp}
class MyData {
  int i_;

 public:
  MyData(int i) : i_(i) {}

  bool Compare(const MyData& other) const { return i_ < other.i_; }
};

my_container<MyData, bool (MyData::*)(const MyData&) const> cont(&MyData::Compare);
```

### Handling Lambdas with std::function and arene::base::inline_function {#handling-lambdas-with-std-function-and-arene-base-inline_function}

The solution to the third problem (using lambdas) is to use a type-erased
invocable such as `std::function` or `arene::base::inline_function` as the
template parameter. This will allow passing any invocable that can be stored in
the type-erased wrapper, at the cost of a pointer indirection for every call,
and possibly dynamic memory allocation (with `std::function`).

```{.cpp}
my_container<MyData, arene::base::inline_function<bool (const MyData&, const MyData&)>> cont(
  [](auto& lhs,auto& rhs) { return lhs.Compare(rhs);});
```

<!-- markdownlint-disable MD013 -->

## Specifying An Invocable With A Template Type Parameter For Function Templates {#specifying-an-invocable-with-a-template-type-parameter-for-function-templates}

<!-- markdownlint-enable MD013 -->

Function templates that need to accept invocables, such as `std::for_each`,
commonly have a template parameter that specifies the type of the invocable, and
then a function parameter for the actual value:

```{.cpp}
template<typename Iterator, typename Func>
void for_each(Iterator begin, Iterator end, Func&& func){
  for(;begin != end; ++begin) {
    func(*begin);
  }
}
```

This works better than the class template case, because the template parameter
is usually deduced rather than being explicitly specified. This means that it
works seamlessly with lambdas, but the other problems are still present.

The same solutions work for function templates too: use `static_cast` to get a
sensible error message if the invocable can't be invoked with the desired
arguments, and use `arene::base::invoke` to allow usage with other invocables.

### Better Error Messages With Constraints {#better-error-messages-with-constraints}

Function templates also have an additional mechanism for improving the error
messages: we can use [constraints](\ref constraints) to remove the function from
the overload set if the invocable doesn't meet the constraints.

```{.cpp}
template<typename Iterator, typename Func,
  arene::base::constraints<
    std::enable_if_t<arene::base::is_invocable_v<Func,typename std::iterator_traits<Iterator>::value_type>>> = nullptr>
void ForEach(Iterator begin, Iterator end, Func&& func){
  for(;begin != end; ++begin) {
    arene::base::invoke(func, *begin);
  }
}
```

If there is not a viable overload of the function for a given call, then the
compiler gives a friendly error message relating to the constraint:

```{.cpp}
void foo(){
  std::vector<std::string> v;
  ForEach(v.begin(),v.end(),[](int){}); // lambda takes an int instead of a string
}
```

Here, the error message clearly identifies that the `is_invocable_v` requirement
was not satisfied:

```{.txt}
my_project/file1.cpp:225:3: error: no matching function for call to 'ForEach'
  625 |   ForEach(v.begin(), v.end(), [](int) {});
      |   ^~~~~~~
my_project/header.hpp:47:6: note: candidate template ignored: requirement 'arene::base::is_invocable_v<(lambda at my_project/file1.cpp:225:31), std::string>' was not satisfied [with Iterator = iterator, Func = (lambda at my_project/file1.cpp:225:31)]
  617 | void ForEach(Iterator begin, Iterator end, Func func) {
      |      ^
```

## References To Invocables In Function Arguments {#references-to-invocables-in-function-arguments}

Where it is desirable for a function to accept any invocable which can be
invoked with a specific set of arguments, but it is not desirable to make the
function a template, `arene::base::function_ref` can be used as the function
argument. This avoids copying the invocable, which would be required
`std::function` or `arene::base::inline_function` is used. This is a back-port
of
[`std::function_ref`](https://en.cppreference.com/w/cpp/utility/functional/function_ref).
This allows the function to be declared in a header, and defined in a .cpp file.

```{.cpp}
double foo(arene::base::function_ref<double(int,int)> f) {
  return f(42, 99) * 2.0;
}

double bar(int,int);

int main(){
  foo(&bar);
  foo([](int,int){return 1.2;]);
}
```

\warning Note: `arene::base::function_ref` binds a _reference_ to the supplied
invocable, so should always have a lifetime that is shorter than the referenced
invocable. Using it _exclusively_ for function arguments ensures this
requirement is upheld.

\warning Passing `arene::base::function_ref` objects to a type-erasing invocable
like `std::function` or `arene::base::inline_function` will store a copy **of
the `arene::base::function_ref` object**. This will still reference the original
invocable, so is not recommended.

## Storing Invocables For Later Invocation {#storing-invocables-for-later-invocation}

If you want to store an invocable for later use, but cannot make the type of the
invocable a template parameter, then you need a type-erased invocable wrapper.
The C++ Standard Library provides
[`std::function`](https://en.cppreference.com/w/cpp/utility/functional/function)
and
[`std::move_only_function`](https://en.cppreference.com/w/cpp/utility/functional/move_only_function)
for this purpose, but these use dynamic memory allocation, and
`std::move_only_function` is only available from C++23. The `functional`
subpackage fills this need with `arene::base::inline_function`, which is an
[inline container](\ref inline_container), so stores the invocable internally.

Here, `my_class` stores a callback internally that is invoked later as part of
the `do_stuff` member function:

```{.cpp}
// my_class.hpp
class my_class{
public:
  class some_data{};

  template<typename Func,
           arene::base::constraints<std::enable_if_t<arene::base::is_invocable_v<Func,some_data>>> = nullptr>
  my_class(Func&& func): callback_(std::forward<Func>(func)){}

  void do_stuff();
private:
  arene::base::inline_function<void(some_data)> callback_;
};
```

```{.cpp}
// my_class.cpp

namespace{
  some_data make_data();
}

void my_class::do_stuff(){
  callback_(make_data());
}
```

`my_class::do_stuff` can be defined out of line, in the .cpp file, since
`my_class` is not a template, and thus can isolate the `make_data` function from
the user of `my_class`.

## Getting Traits From Function Types {#getting-traits-from-function-types}

It can be useful to get the traits associated with a given function type. Using
`arene::base::function_traits` templated on a function type will give a class
with the following member types:

- `return_type`: The type returned by the function
- `argument_types`: An `arene::base::type_list` of the types expected as
  arguments

Additionally, these types can be accessed directly
with`arene::base::function_return_t` and `arene::base::function_arguments_t`,
respectively.

Traits are accessed from a member pointer via
`arene::base::member_pointer_traits`. This includes both function and
non-function member pointers. These traits are:

- `class_type`: The class the pointee is a member of
- `pointee_type`: The type being pointed to

`arene::base::member_function_pointer_traits` is a combination of both above
trait types for member function pointers. The equivalent return and argument
types for member function pointers are `arene::base::member_function_return_t`
and `arene::base::member_function_arguments_t`. Additionally, it also contains
the member type `implicit_argument_type` which is `class_type` qualified to
match the qualifiers for `pointee_type`. When invoking a member function pointer
this will implicitly be the first argument.

`function_traits` also contains `static constexpr` enum members specfying the
qualifiers for the underlying function. These enums are
`arene::base::cv_qualifier` and `arene::base::reference_qualifer`. For function
types these are both always `unqualified`. These are available as
`std::integral_constant` for member function pointers as
`arene::base::member_function_cv` and `arene::base::mamber_function_reference`
or directly as enums as `arene::base::member_function_cv_v` and
`arene::base::member_function_reference_v`.
