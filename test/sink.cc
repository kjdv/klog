#include <sink.hh>
#include <gtest/gtest.h>
#include <unistd.h>

namespace klog {
namespace  {

using namespace implementation;

struct fake_sink
{
  event last_event;

  void operator()(event ev)
  {
    last_event = std::move(ev);
  }
};

class sink_test : public testing::Test
{
public:
  fake_sink fs;

  void SetUp() override
  {
    set_sink([this](auto && ev) { fs(ev); });
  }

  void TearDown() override
  {
    set_default_sink();
  }
};

TEST_F(sink_test, post_connects_to_set_sink)
{
  auto before = std::chrono::system_clock::now();
  post(loglevel::info, "tag", "message");
  auto after = std::chrono::system_clock::now();

  EXPECT_EQ(getpid(), fs.last_event.process);
  EXPECT_EQ(std::this_thread::get_id(), fs.last_event.thread);
  EXPECT_EQ(loglevel::info, fs.last_event.severity);
  EXPECT_EQ("tag", fs.last_event.tag);
  EXPECT_EQ("message", fs.last_event.msg);

  EXPECT_GE(fs.last_event.time, before);
  EXPECT_LE(fs.last_event.time, after);
}

}
}
