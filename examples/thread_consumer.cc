#include <logger.hh>
#include <consumer.hh>
#include <iostream>

int main()
{
  using namespace klog;

  auto delegate = std::make_unique<ostream_consumer>(std::clog);
  set_consumer<threaded_consumer>(std::move(delegate));

  logger<loglevel::debug> log("sample");

  log.debug("debug message {}", 1);
  log.info("info message {}", 2);
  log.warning("warning message {}", 3);
  log.error("error message {}", 4);

  return 0;
}
