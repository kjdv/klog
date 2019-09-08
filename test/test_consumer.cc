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

  void consume(event ev) override
  {
    *d_last_event = std::move(ev);
  }

private:
  event *d_last_event;
};

}

fixture::fixture()
  : last_event(int{}, event::threadid_t{}, event::timestamp_t{}, loglevel{}, "", "", "")
  , d_override(std::make_unique<fake_consumer>(&last_event))
{}


}
}
