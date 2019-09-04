#include <logger.hh>
#include <sink.hh>
#include <gtest/gtest.h>
#include <unistd.h>

namespace klog {
namespace  {

static_assert(loglevel::debug >= loglevel::all);
static_assert(loglevel::info > loglevel::debug);
static_assert(loglevel::warning > loglevel::info);
static_assert(loglevel::error > loglevel::warning);
static_assert(loglevel::none > loglevel::error);

using namespace implementation;

struct fake_sink
{
  event last_event;

  void operator()(event ev)
  {
    last_event = std::move(ev);
  }
};

class logger_test : public testing::Test
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

TEST_F(logger_test, all_enabled)
{
  logger<loglevel::all> l("test");
  l.debug("the number {}", 42);

  EXPECT_EQ("the number 42", fs.last_event.msg);
}

TEST_F(logger_test, debug_enabled)
{
  logger<loglevel::debug> l("test");
  l.debug("the number {}", 42);

  EXPECT_EQ("the number 42", fs.last_event.msg);
}

TEST_F(logger_test, debug_disabled)
{
  fs.last_event.msg = "scramble";
  logger<loglevel::info> l("test");
  l.debug("the number {}", 42);

  EXPECT_EQ("scramble", fs.last_event.msg);
}

TEST_F(logger_test, info_enabled)
{
  logger<loglevel::info> l("test");
  l.info("the number {}", 42);

  EXPECT_EQ("the number 42", fs.last_event.msg);
}

TEST_F(logger_test, info_disabled)
{
  fs.last_event.msg = "scramble";
  logger<loglevel::warning> l("test");
  l.info("the number {}", 42);

  EXPECT_EQ("scramble", fs.last_event.msg);
}

TEST_F(logger_test, warning_enabled)
{
  logger<loglevel::warning> l("test");
  l.warning("the number {}", 42);

  EXPECT_EQ("the number 42", fs.last_event.msg);
}

TEST_F(logger_test, warning_disabled)
{
  fs.last_event.msg = "scramble";
  logger<loglevel::error> l("test");
  l.warning("the number {}", 42);

  EXPECT_EQ("scramble", fs.last_event.msg);
}

TEST_F(logger_test, error_enabled)
{
  logger<loglevel::error> l("test");
  l.error("the number {}", 42);

  EXPECT_EQ("the number 42", fs.last_event.msg);
}

TEST_F(logger_test, error_disabled)
{
  fs.last_event.msg = "scramble";
  logger<loglevel::none> l("test");
  l.error("the number {}", 42);

  EXPECT_EQ("scramble", fs.last_event.msg);
}


}
}
