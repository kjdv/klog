#include <context.hh>
#include <gtest/gtest.h>
#include "test_consumer.hh"

namespace klog {

TEST(context, propagated)
{
  test::fixture fx;
  logger<> l("test");

  context_guard g1("foo");
  {
    context_guard g2("bar", "baz");
    l.info("inside");

    EXPECT_EQ("foo bar=baz", fx.last_event.context());
  }

  l.info("outsize");

  EXPECT_EQ("foo", fx.last_event.context());
}

}
