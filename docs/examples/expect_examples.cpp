// Copyright 2024, Toyota Motor Corporation

#include "arene/base/compiler_support/expect.hpp"
#include "arene/base/optional.hpp"

namespace expect_examples {

struct config_data {};

void do_default_foo_processing() {}
void do_custom_foo_processing(config_data const&) {}

//! [expect_usage_example]
void foo(arene::base::optional<config_data> optional_configuration = {}) {
  if (ARENE_EXPECT(!optional_configuration.has_value())) {
    do_default_foo_processing();
  } else {
    do_custom_foo_processing(*optional_configuration);
  }
}
//! [expect_usage_example]
}  // namespace expect_examples
