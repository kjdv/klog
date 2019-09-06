#include <logger.hh>
#include <unistd.h>
#include "sink.hh"

namespace klog {
namespace implementation {

void post(klog::loglevel severity, std::string_view tag, std::string_view msg)
{
  static int pid = getpid();

  assert(g_sink);
  g_sink->consume(event{pid,
                        std::this_thread::get_id(),
                        std::chrono::system_clock::now(),
                        severity,
                        tag,
                        msg});
}

}
}
