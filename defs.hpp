#ifndef _GLOBAL_DEFINATION_HEADER
#define _GLOBAL_DEFINATION_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <initializer_list>
#include <utility>
#include <type_traits>
#include <memory>

using std::initializer_list;
using std::is_same_v, std::decay_t;
using std::move, std::forward;

inline void print(const char *str) { printf("%s", str); }
inline void print(char *str) { printf("%s", str); }
inline void print(char ch) { putchar(ch); }

inline void print(int num) { printf("%d", num); }
inline void print(unsigned int num) { printf("%u", num); }
inline void print(long num) { printf("%ld", num); }
inline void print(unsigned long num) { printf("%lu", num); }
inline void print(long long num) { printf("%lld", num); }
inline void print(unsigned long long num) { printf("%llu", num); }

inline void print(float num) { printf("%f", num); }
inline void print(double num) { printf("%lf", num); }
inline void print(long double num) { printf("%llf", num); }

inline void print(void *ptr) { printf("%p", ptr); }

inline void print(void) {}

template <typename Any, typename... Args>
inline void print(Any &&element, Args &&...rest)
{
    print(element);
    print(forward<Args>(rest)...);
}

#endif
