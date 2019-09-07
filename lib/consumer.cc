#include <cassert>
#include <consumer.hh>
#include <iomanip>
#include <sstream>
#include "sink.hh"
#include <unistd.h>
#include <fmt/chrono.h>

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

fmt::memory_buffer put_time(const event::timestamp_t& v)
{
  fmt::memory_buffer buf;

  std::time_t t  = std::chrono::system_clock::to_time_t(v);
  std::tm     tm = fmt::gmtime(t);

  auto us = std::chrono::duration_cast<std::chrono::microseconds>(v.time_since_epoch()).count() % 1000000;

  fmt::format_to(buf, "{:0>4d}-{:0>2d}-{:0>2d}T{:0>2d}:{:0>2d}:{:0>2d}.{:0>6d}",
                 tm.tm_year + 1900,
                 tm.tm_mon + 1,
                 tm.tm_mday,
                 tm.tm_hour,
                 tm.tm_min,
                 tm.tm_sec,
                 us);
  return buf;
}

fmt::memory_buffer format_event(std::string_view f, const event& ev)
{
  fmt::memory_buffer buf;
  auto time = put_time(ev.timestamp);
  fmt::format_to(buf, f,
                 fmt::arg("time", std::string_view(time.data(), time.size())),
                 fmt::arg("process", ev.process),
                 fmt::arg("thread", ev.thread),
                 fmt::arg("severity", severity(ev.severity)),
                 fmt::arg("tag", ev.tag),
                 fmt::arg("msg", ev.msg));

  return buf;
}

void validate_format(std::string_view f) // throws if f can't format events
{
  format_event(f, event{});
}

} // namespace

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
    auto buf = format_event(d_fmt, ev);
    d_out.write(buf.data(), buf.size());
    d_out.flush();
  }
}

threaded_consumer::threaded_consumer(std::unique_ptr<consumer> delegate)
  : d_delegate(std::move(delegate))
  , d_pool(1)
{
  assert(d_delegate);
}

void threaded_consumer::consume(const event &ev)
{
  // we need copies of these
  std::string tag(ev.tag);
  std::string msg(ev.msg);

  auto do_log = [this, tag = std::move(tag), msg = std::move(msg), process = ev.process, thread = ev.thread, timestamp = ev.timestamp, severity = ev.severity] {
    event ev{
      process,
      thread,
      timestamp,
      severity,
      tag,
      msg
    };

    this->d_delegate->consume(ev);
  };

  d_pool.post(do_log);
}

} // namespace klog
