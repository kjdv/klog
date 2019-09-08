#include <logger.hh>
#include <consumer.hh>
#include <gtest/gtest.h>
#include "test_consumer.hh"

namespace klog {
namespace  {

static_assert(loglevel::debug >= loglevel::all);
static_assert(loglevel::info > loglevel::debug);
static_assert(loglevel::warning > loglevel::info);
static_assert(loglevel::error > loglevel::warning);
static_assert(loglevel::none > loglevel::error);


TEST(logger_test, all_enabled)
{
  test::fixture fx;

  logger<loglevel::all> l("test");
  l.debug("the number {}", 42);

  EXPECT_EQ("the number 42", fx.last_event.message());
}

TEST(logger_test, debug_enabled)
{
  test::fixture fx;

  logger<loglevel::debug> l("test");
  l.debug("the number {}", 42);

  EXPECT_EQ("the number 42", fx.last_event.message());
}

TEST(logger_test, debug_disabled)
{
  test::fixture fx;

  logger<loglevel::info> l("test");
  l.debug("the number {}", 42);

  EXPECT_EQ("", fx.last_event.message());
}

TEST(logger_test, info_enabled)
{
  test::fixture fx;

  logger<loglevel::info> l("test");
  l.info("the number {}", 42);

  EXPECT_EQ("the number 42", fx.last_event.message());
}

TEST(logger_test, info_disabled)
{
  test::fixture fx;

  logger<loglevel::warning> l("test");
  l.info("the number {}", 42);

  EXPECT_EQ("", fx.last_event.message());
}

TEST(logger_test, warning_enabled)
{
  test::fixture fx;

  logger<loglevel::warning> l("test");
  l.warning("the number {}", 42);

  EXPECT_EQ("the number 42", fx.last_event.message());
}

TEST(logger_test, warning_disabled)
{
  test::fixture fx;

  logger<loglevel::error> l("test");
  l.warning("the number {}", 42);

  EXPECT_EQ("", fx.last_event.message());
}

TEST(logger_test, error_enabled)
{
  test::fixture fx;

  logger<loglevel::error> l("test");
  l.error("the number {}", 42);

  EXPECT_EQ("the number 42", fx.last_event.message());
}

TEST(logger_test, error_disabled)
{
  test::fixture fx;

  logger<loglevel::none> l("test");
  l.error("the number {}", 42);

  EXPECT_EQ("", fx.last_event.message());
}

}
}
