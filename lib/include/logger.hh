#pragma once

#include <fmt/format.h>
#include <thread>
#include <string>
#include <chrono>

namespace klog {

enum class loglevel
{
  all, // special value that denotes always enabled
  debug = all,
  info,
  warning,
  error,
  none, // special value that denotes never enabled
};

struct event
{
  using timestamp_t = std::chrono::time_point<std::chrono::system_clock>;

  int process{};
  std::thread::id thread{};
  timestamp_t time{};
  loglevel severity{};
  std::string_view tag{};
  std::string msg{};
};

template <loglevel min_level = loglevel::info>
class logger
{
public:
  explicit logger(std::string_view tag);

  template <typename... Args>
  void debug(std::string_view fmt, Args&&... args);

  template <typename... Args>
  void info(std::string_view fmt, Args&&... args);

  template <typename... Args>
  void warning(std::string_view fmt, Args&&... args);

  template <typename... Args>
  void error(std::string_view fmt, Args&&... args);

private:
  template <loglevel severity, typename... Args>
  void print(std::string_view fmt, Args&&... args);

  std::string_view d_tag;
};

namespace implementation {

void post(loglevel severity, std::string_view tag, std::string msg);

template <bool enabled>
struct formatter
{
  template<typename... Args>
  void format(loglevel severity, std::string_view tag, std::string_view fmt, Args&&... args) const
  {
    post(severity, tag, fmt::format(fmt, std::forward<Args>(args)...));
  }
};

template <>
struct formatter<false>
{
  template<typename... Args>
  void format(loglevel, std::string_view, std::string_view, Args&&...) const
  {}
};

}


template<loglevel min_level>
logger<min_level>::logger(std::string_view tag)
  : d_tag(tag)
{}

template <loglevel min_level>
template <typename... Args>
void logger<min_level>::debug(std::string_view fmt, Args&&... args)
{
  print<loglevel::debug>(fmt, std::forward<Args>(args)...);
}

template <loglevel min_level>
template <typename... Args>
void logger<min_level>::info(std::string_view fmt, Args&&... args)
{
  print<loglevel::info>(fmt, std::forward<Args>(args)...);
}


template <loglevel min_level>
template <typename... Args>
void logger<min_level>::warning(std::string_view fmt, Args&&... args)
{
  print<loglevel::warning>(fmt, std::forward<Args>(args)...);
}


template <loglevel min_level>
template <typename... Args>
void logger<min_level>::error(std::string_view fmt, Args&&... args)
{
  print<loglevel::error>(fmt, std::forward<Args>(args)...);
}


template <loglevel min_level>
template <loglevel severity, typename... Args>
void logger<min_level>::print(std::string_view fmt, Args&&... args)
{
  implementation::formatter<min_level <= severity> f;
  f.format(loglevel::debug, d_tag, fmt, std::forward<Args>(args)...);
}


}
