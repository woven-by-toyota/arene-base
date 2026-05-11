// Copyright 2024, Toyota Motor Corporation

#include <cassert>

#include "arene/base/utility.hpp"

namespace utility_examples {
void alignment_example() {
  //! [bit_mask_usage_example]
  assert(arene::base::bit_mask<std::uint32_t>(5) == 0b0000'0000'0001'1111u);
  //! [bit_mask_usage_example]
}

template <typename T>
void swap_examples(T a, T b) {
  //! [swap_old_usage_example]
  using std::swap;
  swap(a, b);
  //! [swap_old_usage_example]

  //! [swap_new_usage_example]
  arene::base::swap(a, b);
  //! [swap_new_usage_example]
}

}  // namespace utility_examples
