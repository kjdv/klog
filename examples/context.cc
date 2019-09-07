#include <logger.hh>
#include <consumer.hh>
#include <context.hh>
#include <kthread/threadpool.hh>

using namespace klog;

void bar(int j)
{
  logger<> log("bar");

  context_guard g("j", j);

  log.info("insided bar");
  std::this_thread::yield();
}

void foo(int i) {
  logger<> log("foo");

  context_guard g("i", i);

  log.info("inside foo, before bar");
  std::this_thread::yield();

  bar(i + 100);

  log.info("back inside foo, after bar");
  std::this_thread::yield();
}

int main()
{
  set_stdout_consumer();

  kthread::threadpool pool;

  for (int i = 0; i < 20; ++i)
  {
    pool.post([i] { foo(i); });
    std::this_thread::yield();
  }

  return 0;
}
