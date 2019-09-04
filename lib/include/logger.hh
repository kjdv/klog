#pragma once

#include <fmt/format.h>
#include <thread>
#include <string>
#include <chrono>

namespace klog {

enum class loglevel
{
  debug,
  info,
  warning,
  error,
};

struct event
{
  using timestamp_t = std::chrono::time_point<std::chrono::system_clock>;

  int process;
  std::thread::id thread;
  timestamp_t time;
  loglevel severity;
  std::string tag;
  std::string msg;
};

namespace implementation {
void post(loglevel severity, std::string_view tag, std::string msg);
}


}
