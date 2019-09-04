#include <sink.hh>
#include <cassert>

namespace klog {
namespace {

void null_sink(event ev)
{}

constexpr const char *severity(loglevel l)
{
  switch (l)
  {
  case loglevel::debug:
    return "DEBUG";
  case loglevel::info:
    return "INFO";
  case loglevel::warning:
    return "WARNING";
  case loglevel::error:
    return "ERRROR";
  case loglevel::none:
    return "NONE"; // ?
  }
}

}

namespace implementation {

sink_t g_sink = null_sink;

}

void set_sink(sink_t s)
{
  assert(s);
  implementation::g_sink = std::move(s);
}

void set_default_sink()
{
  implementation::g_sink = null_sink;
}

void set_ostream_sink(std::ostream &out)
{
  set_ostream_sink("{time} {process}:{thread} [{level}] {tag} {msg}\n", out);
}

void set_ostream_sink(std::string_view f, std::ostream &out)
{
  implementation::g_sink = [&out, f](event ev) {
    out << fmt::format(f,
                       fmt::arg("time", 123),
                       fmt::arg("process", ev.process),
                       fmt::arg("thread", 456),
                       fmt::arg("level", severity(ev.severity)),
                       fmt::arg("tag", ev.tag),
                       fmt::arg("msg", ev.msg));
  };
}


}
