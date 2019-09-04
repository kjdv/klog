#include <gtest/gtest.h>
#include <internal_sample.hh>
#include <sample.hh>

namespace klog {
namespace {

TEST(sample, public_function) {
  EXPECT_EQ("hello from a public function", public_function());
}

TEST(sample, private_function) {
  EXPECT_EQ("hello from a private function", private_function());
}

} // namespace
} // namespace klog
