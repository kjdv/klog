#pragma once

#include "logger.hh"
#include <functional>

namespace klog {

using sink_t = std::function<void(event ev)>;


// note: not thread safe. the idea is that you call this once at the start of main()
void set_sink(sink_t s);

void set_default_sink();

}
