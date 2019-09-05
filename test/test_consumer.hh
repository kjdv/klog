#include <consumer.hh>

namespace klog {
namespace test {

class fixture
{
public:
  explicit fixture();
  ~fixture();

  event last_event;
};

}
}
