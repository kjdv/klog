#include <benchmark/benchmark.h>
#include <logger.hh>
#include <consumer.hh>
#include <fstream>
#include <sink.hh>

namespace klog {
namespace {

void bm_enabled_log(benchmark::State& state) {
  logger<loglevel::info> log("bm");

  for (auto _ : state)
    log.info("benchmark this");
}

BENCHMARK(bm_enabled_log);

void bm_disabled_log(benchmark::State& state) {
  logger<loglevel::info> log("bm");

  // this should take 0 time, no overhead from disabled logs
  for (auto _ : state)
  {
    log.debug("benchmark this");
    log.debug("with {} formatting of numbers: {} {}", "some", 42, 3.14);
  }
}

BENCHMARK(bm_disabled_log);

void bm_ostream_consumer(benchmark::State& state) {
  std::ofstream f("/dev/null");
  ostream_consumer consumer(f);

  event ev{
    1,
    2,
    std::chrono::system_clock::now(),
    loglevel::info,
    "tag",
    "message",
  };

  for (auto _ : state)
    consumer.consume(ev);

}

BENCHMARK(bm_ostream_consumer);

void bm_threaded_consumer(benchmark::State& state) {
  auto delegate = std::make_unique<noop_consumer>();
  threaded_consumer consumer(std::move(delegate));

  event ev{
      1,
      2,
      std::chrono::system_clock::now(),
      loglevel::info,
      "tag",
      "message",
  };

  for (auto _ : state)
    consumer.consume(ev);
}

BENCHMARK(bm_threaded_consumer);


}
}

BENCHMARK_MAIN();
