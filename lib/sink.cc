#include <sink.hh>
#include <cassert>
#include <sstream>
#include <iomanip>

namespace klog {
namespace {

void null_sink(event ev)
{}

constexpr const char *severity(loglevel l)
{
  switch (l)
  {
  case loglevel::debug:
    return "DEBUG";
  case loglevel::info:
    return "INFO";
  case loglevel::warning:
    return "WARNING";
  case loglevel::error:
    return "ERRROR";
  case loglevel::none:
    return "NONE"; // ?
  }
}

template <typename T>
std::string to_string(const T &v)
{
  std::ostringstream stream;
  stream << v;
  return stream.str();
}

template <>
std::string to_string<event::timestamp_t>(const event::timestamp_t &v)
{
  std::time_t t = std::chrono::system_clock::to_time_t(v);
  std::tm tm = *std::gmtime(&t);

  auto us = std::chrono::duration_cast<std::chrono::microseconds>(v.time_since_epoch()).count();

  return to_string(std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")) + "." + std::to_string(us % 1000000);
}

void format_event(std::ostream &out, std::string_view f, const event &ev)
{
  out << fmt::format(f,
                     fmt::arg("time", to_string(ev.time)),
                     fmt::arg("process", ev.process),
                     fmt::arg("thread", to_string(ev.thread)),
                     fmt::arg("level", severity(ev.severity)),
                     fmt::arg("tag", ev.tag),
                     fmt::arg("msg", ev.msg));
}


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

void set_ostream_sink(std::ostream &out)
{
  set_ostream_sink("{time} {process}:{thread} {level} [{tag}] {msg}\n", out);
}

void set_ostream_sink(std::string_view f, std::ostream &out)
{
  // this will validate the format string, throw an exception if it cant format events
  // just a courtesy to the caller, has no other effect
  std::ostringstream stream;
  format_event(stream, f, event{});

  implementation::g_sink = [&out, f](event ev) { format_event(out, f, ev); };
}


}
