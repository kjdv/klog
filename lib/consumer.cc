#include <consumer.hh>
#include <cassert>
#include <sstream>
#include <iomanip>

namespace klog {
namespace {

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

void validate_format(std::string_view f) // throws if f can't format events
{
  std::ostringstream stream;
  format_event(stream, f, event{});
}

}

namespace implementation {

std::unique_ptr<consumer> g_sink = std::make_unique<noop_consumer>();

}

void set_consumer(std::unique_ptr<consumer> c)
{
  assert(c);
  implementation::g_sink = std::move(c);
}

void set_default_consumer()
{
  implementation::g_sink = std::make_unique<noop_consumer>();
}

void noop_consumer::consume(const event &ev)
{}

ostream_consumer::ostream_consumer(std::ostream &out, std::string_view fmt)
  : d_out(out)
  , d_fmt(fmt)
{
  validate_format(d_fmt);
}

void ostream_consumer::consume(const event &ev)
{
  format_event(d_out, d_fmt, ev);
  d_out << std::endl;
}


}
