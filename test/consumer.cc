#include <consumer.hh>
#include <gtest/gtest.h>
#include <unistd.h>
#include "test_consumer.hh"

namespace klog {
namespace  {

TEST(sink_test, post_connects_to_set_sink)
{
  test::fixture fx;

  auto before = std::chrono::system_clock::now();
  implementation::post(loglevel::info, "tag", "message");
  auto after = std::chrono::system_clock::now();

  EXPECT_EQ(getpid(), fx.last_event.process);
  EXPECT_EQ(std::this_thread::get_id(), fx.last_event.thread);
  EXPECT_EQ(loglevel::info, fx.last_event.severity);
  EXPECT_EQ("tag", fx.last_event.tag);
  EXPECT_EQ("message", fx.last_event.msg);

  EXPECT_GE(fx.last_event.time, before);
  EXPECT_LE(fx.last_event.time, after);
}

}
}
