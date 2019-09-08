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
  thread_local auto id = std::this_thread::get_id();
  event::threadid_t tid = reinterpret_cast<event::threadid_t>(&id);

  std::vector<char> buf(tag.size() + msg.size() + g_ctx.size());
  std::copy(tag.begin(), tag.end(), buf.begin());
  std::copy(msg.begin(), msg.end(), buf.begin() + tag.size());
  std::copy(g_ctx.begin(), g_ctx.end(), buf.begin() + tag.size() + msg.size());

  assert(g_sink);
  g_sink->consume(event{pid,
                        tid,
                        std::chrono::system_clock::now(),
                        severity,
                        std::string(tag),
                        std::string(msg),
                        g_ctx});
}

}
}
