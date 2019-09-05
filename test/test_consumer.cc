#include "test_consumer.hh"

namespace klog {
namespace test {

namespace {

class fake_consumer : public consumer
{
public:
  fake_consumer(event *ev)
    : d_last_event(ev)
  {
    assert(d_last_event);
  }

  void consume(const event &ev) override
  {
    *d_last_event = ev;
  }

private:
  event *d_last_event;
};

}

fixture::fixture()
{
  set_consumer<fake_consumer>(&last_event);
}

fixture::~fixture()
{
  set_default_consumer();
}

}
}
