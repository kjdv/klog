#include <consumer.hh>

namespace klog {
namespace implementation {

extern std::unique_ptr<consumer> g_sink;
extern thread_local std::string g_ctx;

// useful for testing, to use a temp override for the global consumer
class consumer_override_guard
{
public:
  explicit consumer_override_guard(std::unique_ptr<consumer> consumer);

  ~consumer_override_guard();

private:
  std::unique_ptr<consumer> d_restore;
};

struct event_value
{
  using timestamp_t = event::timestamp_t;
  using threadid_t = event::threadid_t;

  int process{};
  threadid_t       thread{};
  timestamp_t      timestamp{};
  loglevel         severity{};
  std::string tag{};
  std::string msg{};
  std::string ctx{};

  static event_value from(const event &ev);

  event as() const;
};

}
}
