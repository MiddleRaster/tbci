#pragma once

#define MOCKABLE_FUNCTION(NAME) template <typename... Args> static inline decltype(auto) NAME(Args&&... args) { return ::NAME(std::forward<Args>(args)...); }
