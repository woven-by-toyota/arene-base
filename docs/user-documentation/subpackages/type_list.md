<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page type_list type_list: Template Metaprogramming Facilities For Sequences of Types

<!-- markdownlint-enable MD041 -->

## Introduction {#introduction}

In C++ storing and manipulating lists of types requires a different library than
is used for lists of values.

Following principals of C++ design there is a 'Concept' for what a type-list
type must satisfy and a vocabulary type that satisfies the concept.

The TypeList Concept matches any C++ template type that has zero or more
template arguments that are types (Non-Type-Template-Parameters - NTTP are not
supported)

The `arene::base::type_list<Tn...>` type stores a list of types and satisfies
the TypeList Concept.

The algorithms that manipulate type-lists that satisfy the TypeList Concept
include:

    - `apply_all`
    - `apply_each`
    - `at`
    - `cartesian_product`
    - `concat`
    - `contains`
    - `empty`
    - `filter`
    - `flat_map`
    - `flatten`
    - `index_of`
    - `remove_duplicates`
    - `reverse`
    - `size`
    - `sort`

The type-list algorithms are contained in the `arene::base::type_lists`
namespace.

## Basic Usage {#basic-usage}

Given:

    struct S0 {};
    template<class... Tn>
    struct TL0 {};
    using Example0 = arene::base::type_list<int, double, std::vector<int>>;
    using Example1 = TL0<S0>;

Get the count of type elements in the type-list `Example0`:

    constexpr size_t size = arene::base::type_lists::size_v<Example0>;
    //size == 3

Get the index of the specified type element in the type-list `Example0`:

    constexpr size_t idx = arene::base::type_lists::index_of_v<Example0, std::vector<int>>;
    // idx == 2

Concat type-lists into a single type-list:

    // Order of arguments matters because the first argument will be used to create the result
        using all_t = arene::base::type_lists::concat_t<Example0, Example1>;
        // all_t == arene::base::type_list<int, double, std::vector<int>, S0>
    // And
        using all_t = arene::base::type_lists::concat_t<Example1, Example0>;
        // all_t == TL0<S0, int, double, std::vector<int>>

Convert from one type-list type to a different type-list type:

    using tl_t = arene::base::type_lists::apply_all<Example0, TL0>;
    // tl_t == TL0<int, double, std::vector<int>>

Flatten one level of nested lists

    using tl_t = arene::base::type_lists::flatten<TL0<Example0, Example1>>
    // tl_t == TL0<int, double, std::vector<int>, S0>

Compose two algorithms on one type-list type:

    using all_t = arene::base::type_lists::concat_t<Example0, Example0>;
    using unique_elements_t = arene::base::type_lists::remove_duplicates_t<all_t>;
    // all_t == arene::base::type_list<int, double, std::vector<int>, int, double, std::vector<int>>
    // unique_elements_t == arene::base::type_list<int, double, std::vector<int>>

## Advanced Usage {#advanced-usage}

Given:

    struct S0 {};
    template<class... Tn>
    struct TL0 {};
    using Example0 = arene::base::type_list<int, double, std::vector<int>>;
    using Example1 = TL0<S0>;
    template<class T>
    using F0 = std::conditional_t<
        sizeof(T) == sizeof(int),
        TL0<>,
        TL0<S0>>;

Apply Function to each type element in the type-list type, with or without
flattening:

    using tl_applied = arene::base::type_lists::apply_each<Example0, F0>;
    // tl_applied == arene::base::type_list<TL0<>, TL0<S0>, TL0<S0>>
    using tl_mapped = arene::base::type_lists::flat_map<Example0, F0>;
    // tl_mapped == arene::base::type_list<S0, S0>

Compose Apply And Remove Duplicates algorithms with the type-list type:

    using tl_applied = arene::base::type_lists::apply_each_t<Example0, F0>;
    using tl_applied_unique = arene::base::type_lists::remove_duplicates_t<tl_applied>;
    // tl_applied == arene::base::type_list<TL0<>, TL0<S0>, TL0<S0>>
    // tl_applied_unique == arene::base::type_list<TL0<>, TL0<S0>>

    using tl_mapped = arene::base::type_lists::apply_each_t<Example0, F0>;
    using tl_mapped_unique = arene::base::type_lists::remove_duplicates_t<tl_mapped>;
    // tl_mapped == arene::base::type_list<S0, S0>
    // tl_mapped_unique == arene::base::type_list<S0>
