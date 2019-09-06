#include <consumer.hh>
#include <gtest/gtest.h>
#include <unistd.h>
#include <thread>
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
  EXPECT_EQ(loglevel::info, fx.last_event.severity);
  EXPECT_EQ("tag", fx.last_event.tag);
  EXPECT_EQ("message", fx.last_event.msg);

  EXPECT_GE(fx.last_event.timestamp, before);
  EXPECT_LE(fx.last_event.timestamp, after);
}

TEST(ostream_consumer, consumes)
{
  std::stringstream stream;
  ostream_consumer consumer(stream);

  event ev{
    1,
    2,
    event::timestamp_t{} + std::chrono::microseconds(3),
    loglevel::info,
    "tag",
    "message"
  };
  consumer.consume(ev);

  EXPECT_EQ("1970-01-01T00:00:00.000003 1:2 INFO [tag] message\n", stream.str()); // todo: fix microsecond precisision
}

TEST(ostream_consumer, loglevel_is_respected)
{
  std::stringstream stream;
  implementation::consumer_override_guard g(std::make_unique<ostream_consumer>(stream, loglevel::warning, "{severity} {tag}"));

  implementation::post(loglevel::info, "ignored", "");
  implementation::post(loglevel::warning, "printed", "");

  std::string s, t;
  stream >> s >> t;
  EXPECT_EQ(s, "WARNING");
  EXPECT_EQ(t, "printed");
}

TEST(ostream_consumer, produces_process_and_thread_id)
{
  std::stringstream stream;
  implementation::consumer_override_guard g(std::make_unique<ostream_consumer>(stream, loglevel::all, "{process} {thread}"));

  implementation::post(loglevel::info, "", "");

  int p;
  std::string t;
  stream >> p >> t;
  EXPECT_EQ(getpid(), p);
}

}
}
