#include <logger.hh>
#include <consumer.hh>
#include <unistd.h>

namespace klog {
namespace implementation {

extern std::unique_ptr<consumer> g_sink; // defined in consumer.cc

void post(klog::loglevel severity, std::string_view tag, std::string msg)
{
  static int pid = getpid();

  assert(g_sink);
  g_sink->consume(event{pid,
               std::this_thread::get_id(),
               std::chrono::system_clock::now(),
               severity,
               tag,
               std::move(msg)});
}

}

}

