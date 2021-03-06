#pragma once

#include "logger.hh"
#include <chrono>
#include <functional>
#include <memory>
#include <ostream>
#include <type_traits>
#include <mutex>
#include <kthread/processor.hh>
#include <thread>

namespace klog {

class event
{
public:
  using timestamp_t = std::chrono::time_point<std::chrono::system_clock>;
  using threadid_t = int;

  event(threadid_t thread, timestamp_t ts, loglevel severity,
        std::string_view tag, std::string_view msg, std::string_view ctx) noexcept;

  threadid_t thread() const;

  timestamp_t timestamp() const;

  loglevel severity() const;

  std::string_view tag() const;

  std::string_view message() const;

  std::string_view context() const;

private:
  threadid_t       d_thread{};
  timestamp_t      d_timestamp{};
  loglevel         d_severity{};

  std::string      d_buffer{};
  size_t d_msg_offset{};
  size_t d_ctx_offset{};
};

class consumer
{
public:
  virtual ~consumer() = default;

  virtual void consume(event ev) = 0;
};

void set_consumer(std::unique_ptr<consumer> c);
void set_default_consumer();
template <typename T, typename... Args>
void set_consumer(Args&&... args)
{
  static_assert(std::is_base_of_v<consumer, T>);
  set_consumer(std::make_unique<T>(std::forward<Args>(args)...));
}

// shorthand for set_consumer(make_unique<ostream_consumer>(std::cout))
void set_stdout_consumer();
// shorthand for set_consumer(make_unique<ostream_consumer>(std::cerr))
void set_stderr_consumer();
// shorthand for set_consumer(make_unique<ostream_consumer>(std::clog))
void set_stdlog_consumer();


class noop_consumer : public consumer
{
public:
  void consume(event ev) override;
};

class ostream_consumer : public consumer
{
public:
  explicit ostream_consumer(
      std::ostream&    out,
      loglevel         min_level = loglevel::all,
      std::string_view fmt       = "{time} #{thread} {severity} [{tag}] [{context}] {msg}\n");

  void consume(event ev) override;

private:
  std::ostream& d_out;
  loglevel      d_minlevel;
  std::string   d_fmt;
  std::mutex    d_mut;
};

// does the actual logging on another thread
class threaded_consumer : public consumer
{
public:
  explicit threaded_consumer(std::unique_ptr<consumer> delegate);

  void consume(event ev) override;

private:
  struct helper
  {
    std::unique_ptr<consumer> delegate;
    void operator()(event ev);
  };

  kthread::processor<event, helper> d_processor;
  kthread::sender<event> d_tx;
};

} // namespace klog
