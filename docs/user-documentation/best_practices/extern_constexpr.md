<!-- markdownlint-disable MD041 MD013 -->
<!-- prettier-ignore -->
\page extern_constexpr Defining compile-time constants for use in multiple translation units

<!-- markdownlint-enable MD041 MD013 -->

It is often useful to declare variables `constexpr`, so that they are
compile-time constants, with the ensuing benefits:

- they can be used in contexts that require compile-time constants, such as
  array bounds, or template parameters
- the calculation of the value does not require runtime resources, and the
  result can be put in a "constant" section of the final executable

By default, `constexpr` variables have _internal linkage_, the same as `static`
variables at file scope, which means that if a `constexpr` variable is declared
in a header file, then it is actually a different variable in each translation
unit that includes that header file.

```{.cpp}
int x1 = 42; // external linkage
static int x2 = 42; // internal linkage
constexpr int x3 = 42; // internal linkage
```

Mostly, this does not cause an issue. However, if the variable is used in an
`inline` function, or a function template, or member function of a class
template that is defined in a header file, then each of the uses of the `inline`
function or template in different translation units now refer to _different
instances_ of the `constexpr` variable. This is thus a _violation of the
[One Definition Rule (ODR)](https://en.cppreference.com/w/cpp/language/definition#One_Definition_Rule)_,
and **Undefined Behaviour**.

## C++17 inline variables {#cpp17-inline-variables}

C++17 introduces the concept of `inline` variables, which work like `inline`
functions in that if they are duplicated in multiple translation units, then the
duplicates are coalesced, and only one instance of the variable is created. The
uses of such a variable by `inline` functions or templates in multiple
translation units is thus OK, as the same instance of the variable is referenced
in each case.

Given the following declarations:

```{.cpp}
// C++17 code
inline constexpr std::size_t the_answer = 42;

// C++17 code
template<typename T>
inline constexpr std::size_t inline_size_of_t = sizeof(T);
```

`the_answer` is now _the same variable_ in all translation units, as is
`inline_size_of_t<int>` or `inline_size_of_t<std::string>`.

The use of `inline` variables thus avoids the ODR violation and avoids undefined
behaviour.

## C++14 solution for variable templates {#cpp14-solution-for-variable-templates}

C++14 does not allow the `inline` keyword to be applied to variables. However,
the same effect can be achieved for _variable templates_ by using the `extern`
specifier. In this case, the compiler treats the variable template the same as a
non-`constexpr` variable template, and ensures there is only one global instance
for each set of template parameters.

\note The `extern` keyword may not be required for recent versions of some
compilers, as the C++ standard has been clarified to say that all variable
templates are supposed to be implicitly `extern`, but it is always correct to
specify it explicitly, and avoids portability problems.

In this example, there is only one `extern_size_of_t<int>` instance in the
program, even if used in multiple translation units.

```{.cpp}
// C++14 code, also works in C++17
template<typename T>
extern constexpr std::size_t extern_size_of_t = sizeof(T);
```

This does not work with variables that are not templates though: using `extern`
in such a case will lead to duplicate definition errors, since `constexpr`
variables must be initialized at the point of declaration, and duplicate
definitions of `extern` variables is always an error.

```{.cpp}
// Error if used in multiple translation units
extern constexpr std::size_t the_answer = 42;
```

## C++14 solution for non-template variables {#cpp14-solution-for-non-template-variables}

Having found a solution for variable templates, we can extend that to
non-template variables. The first step is to define a variable template that has
exactly one instantiation, by using a _Non-Type Template Parameter_ of type
`std::nullptr_t`, which has exactly one possible value: `nullptr`.

```{.cpp}
// C++14 code, also works in C++17
template<std::nullptr_t = nullptr>
extern constexpr std::size_t the_answer_impl = 42;
```

The default parameter means that we can use `the_answer_impl<>` to refer to the
one and only instantiation of that variable template in all translation units.

The trailing `<>` still provides a suboptimal experience though, so we need to
eliminate that. We can do this by providing a _reference_ to the variable
template, with the desired name.

```{.cpp}
// C++14 code, also works in C++17
static constexpr std::size_t& the_answer = the_answer_impl<>;
```

This should be declared `static` to explicitly mark it with internal linkage, to
ensure and highlight that there is actually a distinct reference in each
translation unit. This is explicitly what we were trying to avoid in the initial
case, but this is now what we want: without it, we will get duplicate definition
errors. The key difference here is that this is a _reference_, and each use of
this reference _refers to the same instance of `the_answer_impl<>`_. This means
that all uses of `the_answer` in all translation units _refer to_ the same
variable, so cannot cause ODR violations.

This is used in multiple places in Arene Base. e.g. `arene::base::swap` is
defined using this pattern:

<!-- prettier-ignore-start -->
\dontinclude arene/base/utility/swap.hpp
\skipline namespace swap_detail
\skipline class swap_fn_impl
\skip *this
\skipline };
\skip std::nullptr_t = nullptr
\until ;
\skipline }
\skipline auto const&
<!-- prettier-ignore-end -->
