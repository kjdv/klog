#include <consumer.hh>

namespace klog {
namespace test {

class fixture
{
public:
  explicit fixture();

  event last_event;

private:
  consumer_override_guard d_override;
};

}
}
