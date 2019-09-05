#include <consumer.hh>

namespace klog {
namespace test {

class fixture
{
public:
  explicit fixture();

  event last_event;

private:
  temp_override_consumer d_override;
};

}
}
