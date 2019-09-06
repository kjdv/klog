#include <logger.hh>
#include <consumer.hh>
#include <iostream>
#include <kthread/threadpool.hh>

int main()
{
  using namespace klog;

  set_consumer<ostream_consumer>(std::cout);

  logger<loglevel::debug> log("sample");

  kthread::threadpool pool;

  for (int i = 0; i < 20; ++i)
  {
    pool.post([&log, i=i] { log.info("message number {}", i); });
    std::this_thread::yield();
  }

  return 0;
}
