#include <benchmark/benchmark.h>
#include <logger.hh>

namespace klog {
namespace {

void bm_enabled_log(benchmark::State& state) {
  logger<loglevel::info> log("bm");

  for (auto _ : state)
    log.info("benchmark this");
}

BENCHMARK(bm_enabled_log);

void bm_disabled_log(benchmark::State& state) {
  logger<loglevel::debug> log("bm");

  for (auto _ : state)
    log.debug("benchmark this");
}

BENCHMARK(bm_disabled_log);

}
}

BENCHMARK_MAIN();
