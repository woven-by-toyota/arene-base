// Copyright 2024, Toyota Motor Corporation

#include "arene/base/type_traits.hpp"

namespace type_traits_example {
//! [always_false_usage_example]
/// @brief Class that must be specialized; the primary template is not intended to be usable
/// @tparam T The template parameter
template <typename T>
struct must_be_specialized {
  static_assert(
      arene::base::always_false_v<T>,
      "No specialization for the input type found. Perhaps you forgot to include a header?"
  );
};
//! [always_false_usage_example]

//! [type_identity_usage_example]
/// @brief A function that takes two parameters of the same type
/// @tparam T The type of the arguments
template <typename T>
void foo(T, T);

/// @brief A function that takes two parameters of the same type, but only uses the first for type deduction
/// @tparam T The type of the arguments
template <typename T>
void bar(T, arene::base::type_identity_t<T>);

void example() {
  // foo(1, 2u);  // error: Ambiguous what T should deduce to.
  foo(1u, 2u);  // OK, T is deduced to unsigned
  bar(1, 2u);   // Ok. T is deduced to type of first argument only (i.e. int)
}
//! [type_identity_usage_example]

}  // namespace type_traits_example
