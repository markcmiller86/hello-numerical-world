#include <ostream>
//using namespace std;

typedef double fpnumber;

// Encapsulate a double so we can easily count operations of interest
// Note: Doing this well is complicated by the mix of types in numerical
// statements (e.g. int*double) made possible by modern compilers 
class Number {
  public:
    static void *operator new(std::size_t sz) { Number::nbytes += sz; return ::operator new(sz); };
    static void *operator new[](std::size_t sz) { Number::nbytes += sz; return ::operator new(sz); };
    static char const *counts_string() { static char counts[256];
        sprintf(counts, "Adds:%d, Mults:%d, Divs:%d, Bytes:%d", Number::nadds, Number::nmults, Number::ndivs, (int) Number::nbytes);
        return counts; };
    inline Number() : x(0) {};
    inline Number(fpnumber _x) : x(_x) {};
    inline Number(int _x) : x((fpnumber) _x) {};
    inline Number &operator=(const Number& rhs) { this->x = rhs.x; return *this; };
    inline operator fpnumber() const { return x; };
  private:
    static int nadds;
    static int nmults;
    static int ndivs;
    static std::size_t nbytes;
    fpnumber x;

    // Various operators on Number w/mix of int
    friend Number operator+(const Number& lhs, const Number& rhs) {  Number::nadds++; return lhs.x + rhs.x; };
    friend Number operator+(const int& lhs, const Number& rhs) {  Number::nadds++; return lhs + rhs.x; };
    friend Number operator+(const Number& lhs, const int& rhs) {  Number::nadds++; return lhs.x + rhs; };
    friend Number operator+(const fpnumber& lhs, const Number& rhs) {  Number::nadds++; return lhs + rhs.x; };
    friend Number operator+(const Number& lhs, const fpnumber& rhs) {  Number::nadds++; return lhs.x + rhs; };
    friend Number operator+=(Number& lhs, const Number& rhs) {  Number::nadds++; return lhs.x += rhs.x; };
    friend Number operator-(const Number& lhs, const Number& rhs) {  Number::nadds++; return lhs.x - rhs.x; };
    friend Number operator-(const int& lhs, const Number& rhs) {  Number::nadds++; return lhs - rhs.x; };
    friend Number operator-(const Number& lhs, const int& rhs) {  Number::nadds++; return lhs.x - rhs; };
    friend Number operator-(const fpnumber& lhs, const Number& rhs) {  Number::nadds++; return lhs - rhs.x; };
    friend Number operator-(const Number& lhs, const fpnumber& rhs) {  Number::nadds++; return lhs.x - rhs; };
    friend Number operator-(const Number& rhs) {  Number::nadds++; return -rhs.x; };
    friend Number operator-=(Number& lhs, const Number& rhs) {  Number::nadds++; return lhs.x -= rhs.x; };
    friend Number operator*(const Number& lhs, const Number& rhs) { Number::nmults++; return lhs.x * rhs.x; };
    friend Number operator*(const int& lhs, const Number& rhs) { Number::nmults++; return lhs * rhs.x; };
    friend Number operator*(const Number& lhs, const int& rhs) { Number::nmults++; return lhs.x * rhs; };
    friend Number operator*(const fpnumber& lhs, const Number& rhs) { Number::nmults++; return lhs * rhs.x; };
    friend Number operator*(const Number& lhs, const fpnumber& rhs) { Number::nmults++; return lhs.x * rhs; };
    friend Number operator*=(Number& lhs, const Number& rhs) { Number::nmults++; return lhs.x *= rhs.x; };
    friend Number operator/(const Number& lhs, const Number& rhs) { Number::ndivs++; return lhs.x / rhs.x; };
    friend Number operator/(const int& lhs, const Number& rhs) { Number::ndivs++; return lhs / rhs.x; };
    friend Number operator/(const Number& lhs, const int& rhs) { Number::ndivs++; return lhs.x / rhs; };
    friend Number operator/(const fpnumber& lhs, const Number& rhs) { Number::ndivs++; return lhs / rhs.x; };
    friend Number operator/(const Number& lhs, const fpnumber& rhs) { Number::ndivs++; return lhs.x / rhs; };
    friend Number operator/=(Number& lhs, const Number& rhs) { Number::ndivs++; return lhs.x /= rhs.x; };
    friend bool operator< (const Number& lhs, const Number& rhs){ return lhs.x < rhs.x; }
    friend bool operator< (const int& lhs, const Number& rhs){ return lhs < rhs.x; }
    friend bool operator< (const Number& lhs, const int& rhs){ return lhs.x < rhs; }
    friend bool operator< (const fpnumber& lhs, const Number& rhs){ return lhs < rhs.x; }
    friend bool operator< (const Number& lhs, const fpnumber& rhs){ return lhs.x < rhs; }
    friend bool operator> (const Number& lhs, const Number& rhs){ return rhs < lhs; }
    friend bool operator> (const int& lhs, const Number& rhs){ return rhs < lhs; }
    friend bool operator> (const Number& lhs, const int& rhs){ return rhs < lhs; }
    friend bool operator> (const fpnumber& lhs, const Number& rhs){ return rhs < lhs; }
    friend bool operator> (const Number& lhs, const fpnumber& rhs){ return rhs < lhs; }
    friend bool operator<=(const Number& lhs, const Number& rhs){ return !(lhs > rhs); }
    friend bool operator<=(const int& lhs, const Number& rhs){ return !(lhs > rhs); }
    friend bool operator<=(const Number& lhs, const int& rhs){ return !(lhs > rhs); }
    friend bool operator<=(const fpnumber& lhs, const Number& rhs){ return !(lhs > rhs); }
    friend bool operator<=(const Number& lhs, const fpnumber& rhs){ return !(lhs > rhs); }
    friend bool operator>=(const Number& lhs, const Number& rhs){ return !(lhs < rhs); }
    friend bool operator>=(const int& lhs, const Number& rhs){ return !(lhs < rhs); }
    friend bool operator>=(const Number& lhs, const int& rhs){ return !(lhs < rhs); }
    friend bool operator>=(const fpnumber& lhs, const Number& rhs){ return !(lhs < rhs); }
    friend bool operator>=(const Number& lhs, const fpnumber& rhs){ return !(lhs < rhs); }
    friend bool operator==(const Number& lhs, const Number& rhs){ return lhs.x == rhs.x; }
    friend bool operator==(const int& lhs, const Number& rhs){ return lhs == rhs.x; }
    friend bool operator==(const Number& lhs, const int& rhs){ return lhs.x == rhs; }
    friend bool operator==(const fpnumber& lhs, const Number& rhs){ return lhs == rhs.x; }
    friend bool operator==(const Number& lhs, const fpnumber& rhs){ return lhs.x == rhs; }
    friend bool operator!=(const Number& lhs, const Number& rhs){ return !(lhs == rhs); }
    friend bool operator!=(const int& lhs, const Number& rhs){ return !(lhs == rhs); }
    friend bool operator!=(const Number& lhs, const int& rhs){ return !(lhs == rhs); }
    friend bool operator!=(const fpnumber& lhs, const Number& rhs){ return !(lhs == rhs); }
    friend bool operator!=(const Number& lhs, const fpnumber& rhs){ return !(lhs == rhs); }
    friend std::ostream& operator<<(std::ostream& os, const Number& rhs)  { os << rhs.x; return os; }
};

#define TSTART -1
#define TFINAL -2
#define RESIDUAL -3
#define ERROR -4
