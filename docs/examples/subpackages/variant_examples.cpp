///
/// @example examples/subpackages/variant.cpp
/// @brief Examples for the variant subpackage documentation
///

//! [include_header]
#include "arene/base/variant.hpp"
//! [include_header]
#include <iostream>
#include <utility>

#include "arene/base/functional.hpp"
#include "arene/base/string_view.hpp"

namespace {

//! [declaring_variant]
using my_variant = arene::base::variant<int, float, arene::base::string_view>;
//! [declaring_variant]

enum class access_by {
  index,
  type,
};

//! [procedural_variant_usage]
/// @brief Prints the value of the variant using the given access method.
void print_variant_value(my_variant const& var, access_by access) {
  // The variant can be checked for valueless state.
  if (var.valueless_by_exception()) {
    std::cout << "The variant is valueless by exception.\n";
    return;
  }

  // The index of the active alternative can be queried.
  std::cout << "The active alternative is at index: " << var.index() << '\n';

  // The value can be accessed by index.
  if (access == access_by::index) {
    switch (var.index()) {
      case 0:
        std::cout << "The value is an int: " << arene::base::get<0>(var) << '\n';
        break;
      case 1:
        std::cout << "The value is a float: " << arene::base::get<1>(var) << '\n';
        break;
      case 2:
        std::cout << "The value is a string_view: " << arene::base::get<2>(var) << '\n';
        break;
      default:
        std::cout << "Unknown type in variant.\n";
        break;
    }
    return;
  }
  // The value can also be accessed by type.
  if (access == access_by::type) {
    if (arene::base::holds_alternative<int>(var)) {
      std::cout << "The value is an int: " << arene::base::get<int>(var) << '\n';
    } else if (arene::base::holds_alternative<float>(var)) {
      std::cout << "The value is a float: " << arene::base::get<float>(var) << '\n';
    } else if (arene::base::holds_alternative<arene::base::string_view>(var)) {
      std::cout << "The value is a string_view: " << arene::base::get<arene::base::string_view>(var) << '\n';
    } else {
      std::cout << "Unknown type in variant.\n";
    }
  }
}

void use_procedurally() {
  // A variant can be constructed with any of the alternatives.
  my_variant var{42};

  // The value can be printed using the function above.
  print_variant_value(var, access_by::index);
  // prints "The value is an int: 42"

  // The value can be changed to another alternative.
  var = 3.14F;
  print_variant_value(var, access_by::type);
  // prints "The value is a float: 3.14"

  // The value can also be changed to a string_view.
  var = "Hello, Variant!";
  print_variant_value(var, access_by::index);
  // prints "The value is a string_view: Hello, Variant!"
}
//! [procedural_variant_usage]

//! [visit_variant_usage]
/// @brief A visitor object that prints the value of the variant. Note that it must handle all alternatives of the
/// variant.
struct print_visitor {
  auto operator()(int value) const -> int {
    std::cout << "The value is an int: " << value << '\n';
    return 0;
  }

  auto operator()(float value) const -> int {
    std::cout << "The value is a float: " << value << '\n';
    return 1;
  }

  auto operator()(arene::base::string_view value) const -> int {
    std::cout << "The value is a string_view: " << value << '\n';
    return 2;
  }
};

void use_visit() {
  my_variant var{42};
  // prints "The value is an int: 42\n0\n"
  std::cout << arene::base::visit(print_visitor{}, var) << '\n';
  var = 3.14F;
  // prints "The value is a float: 3.14\n1\n"
  std::cout << arene::base::visit(print_visitor{}, var) << '\n';
  var = "Hello, Variant!";
  // prints "The value is a string_view: Hello, Variant!\n2\n"
  std::cout << arene::base::visit(print_visitor{}, var) << '\n';
}
//! [visit_variant_usage]

//! [binding_an_overload_set]
auto print(int value) -> int {
  std::cout << "The value is an int: " << value << '\n';
  return 0;
}
auto print(float value) -> int {
  std::cout << "The value is a float: " << value << '\n';
  return 1;
}
auto print(arene::base::string_view value) -> int {
  std::cout << "The value is a string_view: " << value << '\n';
  return 2;
}

auto print(my_variant const& var) -> int {
  // The visitor can be constructed from a set of overloaded functions.
  return arene::base::visit([](auto const& value) { return print(value); }, var);
}
//! [binding_an_overload_set]

//! [bind_overloads_usage]
void use_bind_overloads() {
  my_variant var{"Hello, Variant!"};
  // prints "The value is a string_view: Hello, Variant!\n2\n"
  std::cout << arene::base::visit(
                   arene::base::bind_overloads(
                       [](int value) {
                         std::cout << "The value is an int: " << value << '\n';
                         return 0;
                       },
                       [](float value) {
                         std::cout << "The value is a float: " << value << '\n';
                         return 1;
                       },
                       [](arene::base::string_view value) {
                         std::cout << "The value is a string_view: " << value << '\n';
                         return 2;
                       }
                   ),
                   var
               )
            << '\n';
}
//! [bind_overloads_usage]

}  // namespace

int main() {
  std::cout << "Using procedural variant access:\n";
  use_procedurally();
  std::cout << "Using visit-based variant access:\n";
  use_visit();
  std::cout << "Using bound overloads for variant access:\n";
  print(my_variant{42});
  std::cout << "Using bound overloads for variant access:\n";
  use_bind_overloads();
  return 0;
}
