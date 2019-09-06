#include <consumer.hh>

namespace klog {
namespace implementation {

extern std::unique_ptr<consumer> g_sink;

// useful for testing, to use a temp override for the global consumer
class consumer_override_guard
{
public:
  explicit consumer_override_guard(std::unique_ptr<consumer> consumer);

  ~consumer_override_guard();

private:
  std::unique_ptr<consumer> d_restore;
};

}
}
