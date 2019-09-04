#include <sink.hh>
#include <cassert>

namespace klog {
namespace {

void null_sink(event ev)
{}

}

namespace implementation {

sink_t g_sink = null_sink;

}

void set_sink(sink_t s)
{
  assert(s);
  implementation::g_sink = std::move(s);
}

void set_default_sink()
{
  implementation::g_sink = null_sink;
}

}
