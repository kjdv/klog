#include <consumer.hh>
#include <gtest/gtest.h>
#include <unistd.h>
#include "test_consumer.hh"

namespace klog {
namespace  {

template <typename T>
std::string to_string(const T &v)
{
  std::ostringstream stream;
  stream << v;
  return stream.str();
}

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

TEST(ostream_consumer, consumes)
{
  std::stringstream stream;
  consumer_override_guard g(std::make_unique<ostream_consumer>(stream));

  implementation::post(loglevel::debug, "tag", "message");

  std::istringstream istream(stream.str());
  std::string item = "scramble";

  // timestamp
  stream >> item;
  // non-determnistic, skip check

  // level
  stream >> item;
  EXPECT_EQ("DEBUG", item);

  // tag
  stream >> item;
  EXPECT_EQ("[tag]", item);

  // message
  stream >> item;
  EXPECT_EQ("message", item);
}

TEST(ostream_consumer, loglevel_is_respected)
{
  std::stringstream stream;
  consumer_override_guard g(std::make_unique<ostream_consumer>(stream, loglevel::warning, "{severity} {tag}"));

  implementation::post(loglevel::info, "ignored", "");
  implementation::post(loglevel::warning, "printed", "");

  std::string s, t;
  stream >> s >> t;
  EXPECT_EQ(s, "WARNING");
  EXPECT_EQ(t, "printed");
}

}
}
