// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <type_traits>

#include "arene/base/constraints.hpp"
//! [type_traits_include]
#include "arene/base/type_traits.hpp"
//! [type_traits_include]

namespace type_traits_tutorial {
struct x {};
x x1{};
x x2{};

void do_something();
void do_something_else();

namespace unchecked {
//! [unchecked_function]
template <typename T>
void func(T some_arg, T some_other_arg) {
  if (some_arg == some_other_arg) {
    do_something();
  } else {
    do_something_else();
  }
}
//! [unchecked_function]

// Uncomment the following line to see the error message:
// void wibble() { func(x1, x2); }
}  // namespace unchecked

namespace assertion {
//! [static_assert_function]
template <typename T>
void func(T some_arg, T some_other_arg) {
  static_assert(
      arene::base::is_equality_comparable_v<T>,
      "T must be equality-comparable; please check that you are calling the right function"
  );
  if (some_arg == some_other_arg) {
    do_something();
  } else {
    do_something_else();
  }
}
//! [static_assert_function]

// Uncomment the following line to see the error message:
// void wibble() { func(x1, x2); }
}  // namespace assertion

namespace constrained {
//! [constrained_function]
template <typename T, arene::base::constraints<std::enable_if_t<arene::base::is_equality_comparable_v<T>>> = nullptr>
void func(T some_arg, T some_other_arg) {
  if (some_arg == some_other_arg) {
    do_something();
  } else {
    do_something_else();
  }
}
//! [constrained_function]

// Uncomment the following line to see the error message:
// void wibble() { func(x1, x2); }
}  // namespace constrained

namespace modified {
template <typename T>
class my_data {
  typename T::field_type data_;

 public:
  //! [adjusted_type]
  auto get_element_field(std::size_t index) const ->
      typename std::add_lvalue_reference<typename std::add_const<typename T::field_type>::type>::type;
  //! [adjusted_type]
};
}  // namespace modified

namespace remove_cvref {
//! [remove_cvref]
template <typename T>
class my_data {
  arene::base::remove_cvref_t<T> stored_copy_;

 public:
  // ...
};
//! [remove_cvref]
}  // namespace remove_cvref

namespace reverse {

template <typename Iterator>
void forward_iterator_reverse(Iterator, Iterator);

template <typename Iterator>
void bidirectional_iterator_reverse(Iterator, Iterator);

template <typename Iterator>
void random_access_iterator_reverse(Iterator, Iterator);

//! [reverse_overloads]
template <
    typename Iterator,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_forward_iterator_v<Iterator>>,
        std::enable_if_t<not arene::base::is_bidirectional_iterator_v<Iterator>>> = nullptr>
void reverse(Iterator begin, Iterator end) {
  forward_iterator_reverse(begin, end);
}

template <
    typename Iterator,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_bidirectional_iterator_v<Iterator>>,
        std::enable_if_t<not arene::base::is_random_access_iterator_v<Iterator>>> = nullptr>
void reverse(Iterator begin, Iterator end) {
  bidirectional_iterator_reverse(begin, end);
}

template <
    typename Iterator,
    arene::base::constraints<std::enable_if_t<arene::base::is_random_access_iterator_v<Iterator>>> = nullptr>
void reverse(Iterator begin, Iterator end) {
  random_access_iterator_reverse(begin, end);
}
//! [reverse_overloads]

}  // namespace reverse

namespace class_templates {

//! [primary_template]
template <typename T, typename = arene::base::constraints<>>
class some_class;
//! [primary_template]

//! [template_specializations]
template <typename T>
class some_class<
    T,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_less_than_comparable_v<T>>,
        std::enable_if_t<arene::base::is_equality_comparable_v<T>>>> {
  // Implementation for types that are less than and equality comparable
};

template <typename T>
class some_class<
    T,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_less_than_comparable_v<T>>,
        std::enable_if_t<not arene::base::is_equality_comparable_v<T>>>> {
  // Implementation for types that are just less-than comparable
};
//! [template_specializations]

}  // namespace class_templates

}  // namespace type_traits_tutorial
