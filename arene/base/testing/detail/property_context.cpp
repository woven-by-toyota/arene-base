#include "arene/base/testing/detail/property_context.hpp"

#include "arene/base/optional/optional.hpp"
#include "arene/base/testing/property_error.hpp"

namespace {

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
arene::base::optional<arene::base::testing::property_error> global_property_context{};

}  // namespace

auto arene::base::testing::detail::property_context() noexcept
    -> arene::base::optional<arene::base::testing::property_error>& {
  return global_property_context;
}
