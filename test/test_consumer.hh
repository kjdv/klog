#include <consumer.hh>
#include <sink.hh>

namespace klog {
namespace test {

class fixture
{
public:
  explicit fixture();

  implementation::event_value last_event;

private:
  implementation::consumer_override_guard d_override;
};

}
}
