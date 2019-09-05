#pragma once

#include "logger.hh"
#include <functional>
#include <iostream>
#include <chrono>
#include <thread>

namespace klog {

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

using sink_t = std::function<void(event ev)>;


// note: not thread safe. the idea is that you call this once at the start of main()
void set_sink(sink_t s);

void set_ostream_sink(std::ostream &out = std::cout);

// set with fmt string, must use the named argument format. i.e.
// "{time} {process}:{thread} [{level}] {tag} {msg}"
void set_ostream_sink(std::string_view fmt, std::ostream &out = std::cout);

void set_default_sink();

}
