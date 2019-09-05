#include <cassert>
#include <consumer.hh>
#include <iomanip>
#include <sstream>

namespace klog {
namespace {

using lock_t = std::unique_lock<std::mutex>;

constexpr const char* severity(loglevel l)
{
  switch(l)
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
std::string to_string(const T& v)
{
  std::ostringstream stream;
  stream << v;
  return stream.str();
}

template <>
std::string to_string<event::timestamp_t>(const event::timestamp_t& v)
{
  std::time_t t  = std::chrono::system_clock::to_time_t(v);
  std::tm     tm = *std::gmtime(&t);

  auto us = std::chrono::duration_cast<std::chrono::microseconds>(v.time_since_epoch()).count();

  return to_string(std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")) + "." + std::to_string(us % 1000000);
}

void format_event(std::ostream& out, std::string_view f, const event& ev)
{
  fmt::memory_buffer buf;
  fmt::format_to(buf, f, fmt::arg("time", to_string(ev.time)), fmt::arg("process", ev.process), fmt::arg("thread", to_string(ev.thread)), fmt::arg("severity", severity(ev.severity)), fmt::arg("tag", ev.tag), fmt::arg("msg", ev.msg));
  out << std::string_view(buf.data(), buf.size());
}

void validate_format(std::string_view f) // throws if f can't format events
{
  std::ostringstream stream;
  format_event(stream, f, event{});
}

} // namespace

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

void noop_consumer::consume(const event& ev)
{
}

ostream_consumer::ostream_consumer(std::ostream& out, loglevel min_level, std::string_view fmt)
  : d_out(out)
  , d_minlevel(min_level)
  , d_fmt(fmt)
{
  validate_format(d_fmt);
}

void ostream_consumer::consume(const event& ev)
{
  if(ev.severity >= d_minlevel)
  {
    lock_t l(d_mut);
    format_event(d_out, d_fmt, ev);
    d_out << std::endl;
  }
}

consumer_override_guard::consumer_override_guard(std::unique_ptr<consumer> consumer)
  : d_restore(std::move(implementation::g_sink))
{
  implementation::g_sink = std::move(consumer);
}

consumer_override_guard::~consumer_override_guard()
{
  implementation::g_sink = std::move(d_restore);
}

} // namespace klog
