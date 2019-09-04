#include <logger.hh>
#include <sink.hh>
#include <unistd.h>

namespace klog {
namespace implementation {

extern sink_t g_sink; // defined in sink.cc

void post(klog::loglevel severity, std::string_view tag, std::string msg)
{
  static int pid = getpid();

  g_sink(event{pid,
               std::this_thread::get_id(),
               std::chrono::system_clock::now(),
               severity, tag,
               std::move(msg)});
}

}

}

