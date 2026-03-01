#pragma once

#include <memory>
#include <utility>

template <class F>
auto makeScopeGuard(F&& f) {
  struct Deleter {
    F func;
    void operator()(void*) noexcept { func(); }
  };

  return std::unique_ptr<void, Deleter>(reinterpret_cast<void*>(1),
                                        Deleter{std::forward<F>(f)});
}