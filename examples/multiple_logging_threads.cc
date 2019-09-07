#include <logger.hh>
#include <consumer.hh>
#include <kthread/threadpool.hh>

int main()
{
  using namespace klog;

  set_stdout_consumer();

  logger<loglevel::debug> log("sample");

  kthread::threadpool pool;

  for (int i = 0; i < 20; ++i)
  {
    pool.post([&log, i=i] { log.info("message number {}", i); });
    std::this_thread::yield();
  }

  return 0;
}
