#include <logger.hh>
#include <unistd.h>
#include <thread>
#include "sink.hh"

namespace klog {
namespace implementation {

void post(klog::loglevel severity, std::string_view tag, std::string_view msg)
{
  static int pid = getpid();
  thread_local auto id = std::this_thread::get_id();
  event::threadid_t tid = reinterpret_cast<event::threadid_t>(&id);

  assert(g_sink);
  g_sink->consume(event{pid,
                        tid,
                        std::chrono::system_clock::now(),
                        severity,
                        tag,
                        msg});
}

}
}
