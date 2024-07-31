#ifndef FPTYPE
#define FPTYPE 2
#endif

// 0=half, 1=float, 2=double, 3=quad
#if   FPTYPE == 0
typedef _Float16 fpnumber;
#define FPFMT "%- .4g"
#define FPCAST double
#elif FPTYPE == 1
typedef float fpnumber;
#define FPFMT "%- .7g"
#define FPCAST double
#elif FPTYPE == 2
typedef double fpnumber;
#define FPFMT "%- .16g"
#define FPCAST double
#elif FPTYPE == 3
typedef long double fpnumber;
#define FPFMT ((sizeof(fpnumber)==10)?"%- .19Lg":"%- .34Lg")
#define FPCAST long double
#else
#error UNRECOGNIZED FPTYPE
#endif

#define Number fpnumber

#define TSTART -1
#define TFINAL -2
#define RESIDUAL -3
#define ERROR -4
