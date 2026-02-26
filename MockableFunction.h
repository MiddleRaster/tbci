#pragma once

#define MOCKABLE_FUNCTION(NAME) template <typename... Args> static inline decltype(auto) NAME(Args&&... args) { return ::NAME(std::forward<Args>(args)...); }

template<auto Fn>
struct Callable
{
    template<typename... Args> decltype(auto) operator()(Args&&... args) const { return Fn(std::forward<Args>(args)...); }
};
