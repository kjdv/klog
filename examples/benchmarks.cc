#include <benchmark/benchmark.h>
#include <logger.hh>
#include <consumer.hh>
#include <fstream>

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

  for (auto _ : state)
    log.debug("benchmark this");
}

BENCHMARK(bm_disabled_log);

void bm_ostream_log(benchmark::State& state) {
  std::ofstream f("/dev/null");
  consumer_override_guard g(std::make_unique<ostream_consumer>(f));
  logger<loglevel::info> log("bm");

  for (auto _ : state)
    log.info("benchmark this");
}

BENCHMARK(bm_ostream_log);


}
}

BENCHMARK_MAIN();
