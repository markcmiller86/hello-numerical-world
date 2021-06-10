#include <ostream>

#include "Half.H"

#ifndef FPTYPE
#define FPTYPE 2
#endif

// 0=half, 1=float, 2=double, 3=quad
#if   FPTYPE == 0
typedef half_float::half Number;
template <typename T> Number castNum(T x) {
    return half_float::half_cast<half_float::half>(x);
}
#elif FPTYPE == 1
typedef float Number;
template <typename T> Number castNum(T x) { return (Number) x; }
#elif FPTYPE == 2
typedef double Number;
template <typename T> Number castNum(T x) { return (Number) x; }
#elif FPTYPE == 3
typedef long double Number;
template <typename T> Number castNum(T x) { return (Number) x; }
#else
#error UNRECOGNIZED FPTYPE
#endif

