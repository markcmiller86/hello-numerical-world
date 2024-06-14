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

// Encapsulate a floating point number to easily track various metrics.
// Note: Doing this well is complicated by the mix of types in numerical
// statements (e.g. int*double) made explicit by modern compilers.
class Number {
  public:
    static void *operator new(std::size_t sz) { Number::nbytes += sz; return ::operator new(sz); };
    static void *operator new[](std::size_t sz) { Number::nbytes += sz; return ::operator new(sz); };
    static char const *counts_string() { static char counts[256];
        snprintf(counts, sizeof(counts), "Adds:%d, Mults:%d, Divs:%d, Bytes:%d", Number::nadds, Number::nmults, Number::ndivs, (int) Number::nbytes);
        return counts; };
    inline Number() : x(0) {};
    inline Number(fpnumber _x) : x(_x) {};
#if FPTYPE!=2
    inline Number(double _x) : x((fpnumber) _x) {};
#endif
    inline Number(int _x) : x((fpnumber) _x) {};

    inline Number &operator=(const Number& rhs) { this->x = rhs.x; return *this; };
    inline operator fpnumber() const { return x; };
    inline operator int() const { return static_cast<int>(x); };
#if FPTYPE!=2
    inline operator double() const { return static_cast<double>(x); };
#endif
  private:
    static int nadds;
    static int nmults;
    static int ndivs;
    static std::size_t nbytes;
    fpnumber x;

    #pragma omp threadprivate(nadds)
    #pragma omp threadprivate(nmults)
    #pragma omp threadprivate(ndivs)
    #pragma omp threadprivate(nbytes)

    // Various operators on Number w/mix of int
    friend Number operator+(const Number& lhs, const Number& rhs) {  Number::nadds++; return lhs.x + rhs.x; };
    friend Number operator+(const int& lhs, const Number& rhs) {  Number::nadds++; return lhs + rhs.x; };
    friend Number operator+(const Number& lhs, const int& rhs) {  Number::nadds++; return lhs.x + rhs; };
    friend Number operator+(const double& lhs, const Number& rhs) {  Number::nadds++; return lhs + rhs.x; };
    friend Number operator+(const Number& lhs, const double& rhs) {  Number::nadds++; return lhs.x + rhs; };
    friend Number operator+=(Number& lhs, const Number& rhs) {  Number::nadds++; return lhs.x += rhs.x; };
    friend Number operator+=(Number& lhs, const double& rhs) {  Number::nadds++; return lhs.x += rhs; };
    friend Number operator+=(double& lhs, const Number& rhs) {  Number::nadds++; return lhs += rhs.x; };
    friend Number operator-(const Number& lhs, const Number& rhs) {  Number::nadds++; return lhs.x - rhs.x; };
    friend Number operator-(const int& lhs, const Number& rhs) {  Number::nadds++; return lhs - rhs.x; };
    friend Number operator-(const Number& lhs, const int& rhs) {  Number::nadds++; return lhs.x - rhs; };
    friend Number operator-(const double& lhs, const Number& rhs) {  Number::nadds++; return lhs - rhs.x; };
    friend Number operator-(const Number& lhs, const double& rhs) {  Number::nadds++; return lhs.x - rhs; };
    friend Number operator-(const Number& rhs) {  Number::nadds++; return -rhs.x; };
    friend Number operator-=(Number& lhs, const Number& rhs) {  Number::nadds++; return lhs.x -= rhs.x; };
    friend Number operator*(const Number& lhs, const Number& rhs) { Number::nmults++; return lhs.x * rhs.x; };
    friend Number operator*(const int& lhs, const Number& rhs) { Number::nmults++; return lhs * rhs.x; };
    friend Number operator*(const Number& lhs, const int& rhs) { Number::nmults++; return lhs.x * rhs; };
    friend Number operator*(const double& lhs, const Number& rhs) { Number::nmults++; return lhs * rhs.x; };
    friend Number operator*(const Number& lhs, const double& rhs) { Number::nmults++; return lhs.x * rhs; };
    friend Number operator*=(Number& lhs, const Number& rhs) { Number::nmults++; return lhs.x *= rhs.x; };
    friend Number operator/(const Number& lhs, const Number& rhs) { Number::ndivs++; return lhs.x / rhs.x; };
    friend Number operator/(const int& lhs, const Number& rhs) { Number::ndivs++; return lhs / rhs.x; };
    friend Number operator/(const Number& lhs, const int& rhs) { Number::ndivs++; return lhs.x / rhs; };
    friend Number operator/(const double& lhs, const Number& rhs) { Number::ndivs++; return lhs / rhs.x; };
    friend Number operator/(const Number& lhs, const double& rhs) { Number::ndivs++; return lhs.x / rhs; };
    friend Number operator/=(Number& lhs, const Number& rhs) { Number::ndivs++; return lhs.x /= rhs.x; };
    friend bool operator< (const Number& lhs, const Number& rhs){ return lhs.x < rhs.x; }
    friend bool operator< (const int& lhs, const Number& rhs){ return lhs < rhs.x; }
    friend bool operator< (const Number& lhs, const int& rhs){ return lhs.x < rhs; }
    friend bool operator< (const double& lhs, const Number& rhs){ return lhs < rhs.x; }
    friend bool operator< (const Number& lhs, const double& rhs){ return lhs.x < rhs; }
    friend bool operator> (const Number& lhs, const Number& rhs){ return rhs < lhs; }
    friend bool operator> (const int& lhs, const Number& rhs){ return rhs < lhs; }
    friend bool operator> (const Number& lhs, const int& rhs){ return rhs < lhs; }
    friend bool operator> (const double& lhs, const Number& rhs){ return rhs < lhs; }
    friend bool operator> (const Number& lhs, const double& rhs){ return rhs < lhs; }
    friend bool operator<=(const Number& lhs, const Number& rhs){ return !(lhs > rhs); }
    friend bool operator<=(const int& lhs, const Number& rhs){ return !(lhs > rhs); }
    friend bool operator<=(const Number& lhs, const int& rhs){ return !(lhs > rhs); }
    friend bool operator<=(const double& lhs, const Number& rhs){ return !(lhs > rhs); }
    friend bool operator<=(const Number& lhs, const double& rhs){ return !(lhs > rhs); }
    friend bool operator>=(const Number& lhs, const Number& rhs){ return !(lhs < rhs); }
    friend bool operator>=(const int& lhs, const Number& rhs){ return !(lhs < rhs); }
    friend bool operator>=(const Number& lhs, const int& rhs){ return !(lhs < rhs); }
    friend bool operator>=(const double& lhs, const Number& rhs){ return !(lhs < rhs); }
    friend bool operator>=(const Number& lhs, const double& rhs){ return !(lhs < rhs); }
    friend bool operator==(const Number& lhs, const Number& rhs){ return lhs.x == rhs.x; }
    friend bool operator==(const int& lhs, const Number& rhs){ return lhs == rhs.x; }
    friend bool operator==(const Number& lhs, const int& rhs){ return lhs.x == rhs; }
    friend bool operator==(const double& lhs, const Number& rhs){ return lhs == rhs.x; }
    friend bool operator==(const Number& lhs, const double& rhs){ return lhs.x == rhs; }
    friend bool operator!=(const Number& lhs, const Number& rhs){ return !(lhs == rhs); }
    friend bool operator!=(const int& lhs, const Number& rhs){ return !(lhs == rhs); }
    friend bool operator!=(const Number& lhs, const int& rhs){ return !(lhs == rhs); }
    friend bool operator!=(const double& lhs, const Number& rhs){ return !(lhs == rhs); }
    friend bool operator!=(const Number& lhs, const double& rhs){ return !(lhs == rhs); }
    friend std::ostream& operator<<(std::ostream& os, const Number& rhs)  { os << rhs.x; return os; }
};

#define TSTART -1
#define TFINAL -2
#define RESIDUAL -3
#define ERROR -4
