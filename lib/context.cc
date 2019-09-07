#include <context.hh>
#include <sink.hh>

namespace klog {
namespace implementation {

thread_local std::string g_ctx;

namespace {

size_t add_ctx(std::string_view c) noexcept
{
  size_t i = g_ctx.size();
  if (!g_ctx.empty())
    g_ctx += ' ';
  g_ctx += c;

  return i;
}

void pop_ctx(size_t idx) noexcept
{
  g_ctx.resize(idx);
}

}

}

using namespace implementation;

context_guard::context_guard(std::string_view c)
  : d_idx(add_ctx(c))
{}

context_guard::~context_guard()
{
  pop_ctx(d_idx);
}

}
