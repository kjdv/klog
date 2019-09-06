#include "sink.hh"

namespace klog {
namespace implementation {

std::unique_ptr<consumer> g_sink = std::make_unique<noop_consumer>();

consumer_override_guard::consumer_override_guard(std::unique_ptr<consumer> consumer)
  : d_restore(std::move(implementation::g_sink))
{
  g_sink = std::move(consumer);
}

consumer_override_guard::~consumer_override_guard()
{
  g_sink = std::move(d_restore);
}


}
}
