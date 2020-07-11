#include <ostream>
//using namespace std;

// Encapsulate a double so we can easily count operations of interest
// Note: Doing this well is complicated by the mix of types in numerical
// statements (e.g. int*double) made possible by modern compilers 
class Double {
  public:
    static void *operator new(std::size_t sz) { Double::nbytes += sz; return ::operator new(sz); };
    static void *operator new[](std::size_t sz) { Double::nbytes += sz; return ::operator new(sz); };
    static char const *counts_string() { static char counts[256];
        sprintf(counts, "Adds:%d, Mults:%d, Divs:%d, Bytes:%d", Double::nadds, Double::nmults, Double::ndivs, (int) Double::nbytes);
        return counts; };
    inline Double() : x(0) {};
    inline Double(double _x) : x(_x) {};
    inline Double(int _x) : x((double) _x) {};
    inline Double &operator=(const Double& rhs) { this->x = rhs.x; return *this; };
    inline operator double() const { return x; };
  private:
    static int nadds;
    static int nmults;
    static int ndivs;
    static std::size_t nbytes;
    double x;

    // Various operators on Double w/mix of int
    friend Double operator+(const Double& lhs, const Double& rhs) {  Double::nadds++; return lhs.x + rhs.x; };
    friend Double operator+(const int& lhs, const Double& rhs) {  Double::nadds++; return lhs + rhs.x; };
    friend Double operator+(const Double& lhs, const int& rhs) {  Double::nadds++; return lhs.x + rhs; };
    friend Double operator+(const double& lhs, const Double& rhs) {  Double::nadds++; return lhs + rhs.x; };
    friend Double operator+(const Double& lhs, const double& rhs) {  Double::nadds++; return lhs.x + rhs; };
    friend Double operator+=(Double& lhs, const Double& rhs) {  Double::nadds++; return lhs.x += rhs.x; };
    friend Double operator-(const Double& lhs, const Double& rhs) {  Double::nadds++; return lhs.x - rhs.x; };
    friend Double operator-(const int& lhs, const Double& rhs) {  Double::nadds++; return lhs - rhs.x; };
    friend Double operator-(const Double& lhs, const int& rhs) {  Double::nadds++; return lhs.x - rhs; };
    friend Double operator-(const double& lhs, const Double& rhs) {  Double::nadds++; return lhs - rhs.x; };
    friend Double operator-(const Double& lhs, const double& rhs) {  Double::nadds++; return lhs.x - rhs; };
    friend Double operator-(const Double& rhs) {  Double::nadds++; return -rhs.x; };
    friend Double operator-=(Double& lhs, const Double& rhs) {  Double::nadds++; return lhs.x -= rhs.x; };
    friend Double operator*(const Double& lhs, const Double& rhs) { Double::nmults++; return lhs.x * rhs.x; };
    friend Double operator*(const int& lhs, const Double& rhs) { Double::nmults++; return lhs * rhs.x; };
    friend Double operator*(const Double& lhs, const int& rhs) { Double::nmults++; return lhs.x * rhs; };
    friend Double operator*(const double& lhs, const Double& rhs) { Double::nmults++; return lhs * rhs.x; };
    friend Double operator*(const Double& lhs, const double& rhs) { Double::nmults++; return lhs.x * rhs; };
    friend Double operator*=(Double& lhs, const Double& rhs) { Double::nmults++; return lhs.x *= rhs.x; };
    friend Double operator/(const Double& lhs, const Double& rhs) { Double::ndivs++; return lhs.x / rhs.x; };
    friend Double operator/(const int& lhs, const Double& rhs) { Double::ndivs++; return lhs / rhs.x; };
    friend Double operator/(const Double& lhs, const int& rhs) { Double::ndivs++; return lhs.x / rhs; };
    friend Double operator/(const double& lhs, const Double& rhs) { Double::ndivs++; return lhs / rhs.x; };
    friend Double operator/(const Double& lhs, const double& rhs) { Double::ndivs++; return lhs.x / rhs; };
    friend Double operator/=(Double& lhs, const Double& rhs) { Double::ndivs++; return lhs.x /= rhs.x; };
    friend bool operator< (const Double& lhs, const Double& rhs){ return lhs.x < rhs.x; }
    friend bool operator< (const int& lhs, const Double& rhs){ return lhs < rhs.x; }
    friend bool operator< (const Double& lhs, const int& rhs){ return lhs.x < rhs; }
    friend bool operator< (const double& lhs, const Double& rhs){ return lhs < rhs.x; }
    friend bool operator< (const Double& lhs, const double& rhs){ return lhs.x < rhs; }
    friend bool operator> (const Double& lhs, const Double& rhs){ return rhs < lhs; }
    friend bool operator> (const int& lhs, const Double& rhs){ return rhs < lhs; }
    friend bool operator> (const Double& lhs, const int& rhs){ return rhs < lhs; }
    friend bool operator> (const double& lhs, const Double& rhs){ return rhs < lhs; }
    friend bool operator> (const Double& lhs, const double& rhs){ return rhs < lhs; }
    friend bool operator<=(const Double& lhs, const Double& rhs){ return !(lhs > rhs); }
    friend bool operator<=(const int& lhs, const Double& rhs){ return !(lhs > rhs); }
    friend bool operator<=(const Double& lhs, const int& rhs){ return !(lhs > rhs); }
    friend bool operator<=(const double& lhs, const Double& rhs){ return !(lhs > rhs); }
    friend bool operator<=(const Double& lhs, const double& rhs){ return !(lhs > rhs); }
    friend bool operator>=(const Double& lhs, const Double& rhs){ return !(lhs < rhs); }
    friend bool operator>=(const int& lhs, const Double& rhs){ return !(lhs < rhs); }
    friend bool operator>=(const Double& lhs, const int& rhs){ return !(lhs < rhs); }
    friend bool operator>=(const double& lhs, const Double& rhs){ return !(lhs < rhs); }
    friend bool operator>=(const Double& lhs, const double& rhs){ return !(lhs < rhs); }
    friend bool operator==(const Double& lhs, const Double& rhs){ return lhs.x == rhs.x; }
    friend bool operator==(const int& lhs, const Double& rhs){ return lhs == rhs.x; }
    friend bool operator==(const Double& lhs, const int& rhs){ return lhs.x == rhs; }
    friend bool operator==(const double& lhs, const Double& rhs){ return lhs == rhs.x; }
    friend bool operator==(const Double& lhs, const double& rhs){ return lhs.x == rhs; }
    friend bool operator!=(const Double& lhs, const Double& rhs){ return !(lhs == rhs); }
    friend bool operator!=(const int& lhs, const Double& rhs){ return !(lhs == rhs); }
    friend bool operator!=(const Double& lhs, const int& rhs){ return !(lhs == rhs); }
    friend bool operator!=(const double& lhs, const Double& rhs){ return !(lhs == rhs); }
    friend bool operator!=(const Double& lhs, const double& rhs){ return !(lhs == rhs); }
    friend std::ostream& operator<<(std::ostream& os, const Double& rhs)  { os << rhs.x; return os; }
};

#define TSTART -1
#define TFINAL -2
#define RESIDUAL -3
#define ERROR -4
