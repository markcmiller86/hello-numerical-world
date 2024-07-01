#include <ostream>

#include "Half.H"

#ifndef FPTYPE
#define FPTYPE 2
#endif

// 0=half, 1=float, 2=double, 3=quad
#if   FPTYPE == 0
typedef half_float::half fpnumber;
#elif FPTYPE == 1
typedef float fpnumber;
#elif FPTYPE == 2
typedef double fpnumber;
#elif FPTYPE == 3
typedef long double fpnumber;
#else
#error UNRECOGNIZED FPTYPE
#endif

#define Number double

#define TSTART -1
#define TFINAL -2
#define RESIDUAL -3
#define ERROR -4
