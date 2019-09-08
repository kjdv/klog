#include <logger.hh>
#include <unistd.h>
#include <thread>
#include "sink.hh"
#include <context.hh>

namespace klog {

namespace implementation {

void post(klog::loglevel severity, std::string_view tag, std::string_view msg)
{
  static int pid = getpid();
  static int thread_counter = 0;
  thread_local event::threadid_t tid = thread_counter++;

  assert(g_sink);
  g_sink->consume(event{pid,
                        tid,
                        std::chrono::system_clock::now(),
                        severity,
                        tag,
                        msg,
                        g_ctx});
}

}
}
