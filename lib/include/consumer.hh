#pragma once

#include "logger.hh"
#include <chrono>
#include <functional>
#include <memory>
#include <ostream>
#include <type_traits>
#include <mutex>
#include <cstdint>
#include <kthread/threadpool.hh>
#include <vector>
#include <utility>

namespace klog {

struct event
{
  using timestamp_t = std::chrono::time_point<std::chrono::system_clock>;
  using threadid_t = uintptr_t;

  int              process{};
  threadid_t       thread{};
  timestamp_t      timestamp{};
  loglevel         severity{};
  std::string_view tag{};
  std::string_view msg{};
};

class consumer
{
public:
  virtual ~consumer() = default;

  virtual void consume(const event& ev) = 0;
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
  void consume(const event& ev) override;
};

class ostream_consumer : public consumer
{
public:
  explicit ostream_consumer(
      std::ostream&    out,
      loglevel         min_level = loglevel::all,
      std::string_view fmt       = "{time} {process}:{thread} {severity} [{tag}] {msg}\n");

  void consume(const event& ev) override;

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

  void consume(const event &ev) override;

private:
  std::unique_ptr<consumer> d_delegate;
  kthread::threadpool d_pool;
};

} // namespace klog
