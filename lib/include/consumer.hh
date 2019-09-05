#pragma once

#include "logger.hh"
#include <functional>
#include <ostream>
#include <chrono>
#include <thread>
#include <memory>
#include <type_traits>

namespace klog {

struct event
{
  using timestamp_t = std::chrono::time_point<std::chrono::system_clock>;

  int process{};
  std::thread::id thread{};
  timestamp_t time{};
  loglevel severity{};
  std::string_view tag{};
  std::string msg{};
};

class consumer
{
public:
  virtual ~consumer() = default;

  virtual void consume(const event &ev) = 0;
};

void set_consumer(std::unique_ptr<consumer> c);
void set_default_consumer();
template <typename T, typename... Args>
void set_consumer(Args&&... args)
{
  static_assert(std::is_base_of_v<consumer, T>);
  set_consumer(std::make_unique<T>(std::forward<Args>(args)...));
}

class noop_consumer : public consumer
{
public:
  void consume(const event &ev) override;
};

class ostream_consumer : public consumer
{
public:
  explicit ostream_consumer(std::ostream &out,
                   std::string_view fmt = "{time} {process}:{thread} {level} [{tag}] {msg}");

  void consume(const event &ev) override;
private:
  std::ostream &d_out;
  std::string d_fmt;
};

// useful for testing, to use a temp override for the global consumer
class temp_override_consumer
{
public:
  explicit temp_override_consumer(std::unique_ptr<consumer> consumer);

  ~temp_override_consumer();

private:
  std::unique_ptr<consumer> d_restore;
};

}
