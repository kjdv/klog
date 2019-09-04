#include <logger.hh>
#include <sink.hh>

int main()
{
  using namespace klog;

  set_ostream_sink();

  logger<loglevel::debug> log("sample");

  log.debug("debug message {}", 1);
  log.info("info message {}", 2);
  log.warning("warning message {}", 3);
  log.error("error message {}", 4);

  return 0;
}
