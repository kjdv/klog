#pragma once

#include <string>
#include <string_view>
#include <fmt/format.h>

namespace klog {

class context_guard
{
public:
  explicit context_guard(std::string_view ctx);
  template <typename T>
  explicit context_guard(std::string_view key, T&& value)
    : context_guard(fmt::format("{}={}", key, value))
  {}

  context_guard(context_guard const &) = delete;
  context_guard(context_guard&&) = delete;

  context_guard &operator=(context_guard const &) = delete;
  context_guard &operator=(context_guard&&) = delete;

  ~context_guard();

private:
  size_t d_idx;
};

}
