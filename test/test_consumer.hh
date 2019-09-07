#include <consumer.hh>
#include <sink.hh>

namespace klog {
namespace test {

class fixture
{
public:
  explicit fixture();

  event last_event;

private:
  implementation::consumer_override_guard d_override;
};

}
}
