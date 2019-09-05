#pragma once

#include <fmt/format.h>
#include <string>

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
void logger<min_level>::print(std::string_view f, Args&&... args)
{
  if constexpr (min_level <= severity)
    implementation::post(severity, d_tag, fmt::format(f, std::forward<Args>(args)...));
}


}
