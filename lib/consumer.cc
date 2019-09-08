#include <cassert>
#include <consumer.hh>
#include <iomanip>
#include "sink.hh"
#include <unistd.h>
#include <fmt/chrono.h>
#include <iostream>

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
  auto time = put_time(ev.timestamp());
  fmt::format_to(buf, f,
                 fmt::arg("time", std::string_view(time.data(), time.size())),
                 fmt::arg("process", ev.process()),
                 fmt::arg("thread", ev.thread()),
                 fmt::arg("severity", severity(ev.severity())),
                 fmt::arg("tag", ev.tag()),
                 fmt::arg("context", ev.context()),
                 fmt::arg("msg", ev.message()));

  return buf;
}

void validate_format(std::string_view f) // throws if f can't format events
{
  format_event(f, event(int{}, event::threadid_t{}, event::timestamp_t{}, loglevel::info, "", "", ""));
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

void noop_consumer::consume(event ev)
{
}

ostream_consumer::ostream_consumer(std::ostream& out, loglevel min_level, std::string_view fmt)
  : d_out(out)
  , d_minlevel(min_level)
  , d_fmt(fmt)
{
  validate_format(d_fmt);
}

void ostream_consumer::consume(event ev)
{
  if(ev.severity() >= d_minlevel)
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

void threaded_consumer::consume(event ev)
{
  auto do_log = [this, evm = std::move(ev)] () mutable {
    this->d_delegate->consume(std::move(evm));
  };

  d_pool.post(std::move(do_log));
}

void set_stdout_consumer()
{
  set_consumer(std::make_unique<ostream_consumer>(std::cout));
}

void set_stderr_consumer()
{
  set_consumer(std::make_unique<ostream_consumer>(std::cerr));
}

void set_stdlog_consumer()
{
  set_consumer(std::make_unique<ostream_consumer>(std::clog));
}

event::event(int process, event::threadid_t thread, event::timestamp_t ts, loglevel severity, std::string_view tag, std::string_view msg, std::string_view ctx) noexcept
  : d_process(process)
  , d_thread(thread)
  , d_timestamp(ts)
  , d_severity(severity)
  , d_msg_offset(tag.size())
  , d_ctx_offset(tag.size() + msg.size())
{
  // trick: by putting all strings in the same buffer we only need one allocation
  d_buffer.resize(tag.size() + msg.size() + ctx.size());

  std::copy(tag.begin(), tag.end(), d_buffer.begin());
  std::copy(msg.begin(), msg.end(), d_buffer.begin() + tag.size());
  std::copy(ctx.begin(), ctx.end(), d_buffer.begin() + tag.size() + msg.size());
}

int event::process() const
{
  return d_process;
}

event::threadid_t event::thread() const
{
  return d_thread;
}

event::timestamp_t event::timestamp() const
{
  return d_timestamp;
}

loglevel event::severity() const
{
  return d_severity;
}

std::string_view event::tag() const
{
  return std::string_view(d_buffer.data(), d_msg_offset);
}

std::string_view event::message() const
{
  return std::string_view(d_buffer.data() + d_msg_offset, d_ctx_offset - d_msg_offset);
}

std::string_view event::context() const
{
  return std::string_view(d_buffer.data() + d_ctx_offset, d_buffer.size() - d_ctx_offset);
}

} // namespace klog
