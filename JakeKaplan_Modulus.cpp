#include <cstdlib>
#include "../../../../DS/Mod/Mod.h"

using std::endl;
using std::pair;

long Mod::modulus = 17; //define default modulus

pair<long, long> xgcd(long a, long b) {
    if(!b) return {1, 0};   //repeat function until new b (previous a%b) is 0
    else {
        ldiv_t q = ldiv(a, b);  //store a/b and a%b in q
        pair<long, long> v = xgcd(b, q.rem);    //run recursively
        return {v.second, v.first - v.second*q.quot};
    }
}

Mod::Mod(long t) {
    if(t >= modulus || t < 0) x = t%modulus;    //prevent unnecessary computation
    else x = t;
    if(x < 0) x += modulus; //ensure positivity
}
Mod::Mod(const Mod& m): x(m.x) {}
Mod& Mod::operator=(const Mod& m) {
    x = m.x;
    return *this;
}
Mod& Mod::operator+=(const Mod& m) {
    x += (m.x - modulus);   //prevent overflow
    if(x < 0) x += modulus; //ensure positivity
    return *this;
}
Mod& Mod::operator-=(const Mod& m) {
    x -= m.x;
    if(x < 0) x += modulus; //ensure positivity
    return *this;
}
Mod& Mod::operator*=(const Mod& m) {
    Mod a = x;
    Mod b = 0;
    long i = m.x;
    while(i) {  //break multipication down into additions of x and multipication by 2
        if(i & 1) { //when odd:
            b += a;
            --i;
        } else {    //when even:
            a += a;
            i >>= 1;
        }
    }
    x = b.x;
    return *this;
}
Mod& Mod::operator/=(const Mod& m) {
    *this *= inv(m.x);  //a/b = a*(1/b)
    return *this;
}
Mod Mod::operator-() const {
    if(x) return modulus - x;
    else return 0;
}
Mod Mod::pwr(long e) const {
    Mod a = x;
    Mod b = 1;
    if(e < 0) { //a^(-b) = (1/a)^b
        a = inv(x);
        e = -e;
    }
    while(e) {  //break exponentiation down into multipication by x and squaring
        if(e & 1) { //when odd:
            b *= a;
            --e;
        } else {    //when even:
            a *= a;
            e >>= 1;
        }
    }
    return b;
}
long Mod::val() const {
    return x;
}
void Mod::set_modulus(long m) {
    if(m < 2) exit(-1); //returns error if modulus is set too small
    modulus = m;
}
Mod Mod::inv(long r0) {
    if(!r0) exit(-1);   //returns error if number is divided by 0
    return xgcd(modulus, Mod(r0).x).second; //modular inverse of x = coefficient of x in xgcd of modulus and x
}
Mod operator+(const Mod& a, const Mod& b) {
    Mod x = a;
    return x += b;
}
Mod operator+(long t, const Mod& m) {
    Mod x = t;
    return x += m;
}
Mod operator-(const Mod& a, const Mod& b) {
    Mod x = a;
    return x -= b;
}
Mod operator-(long t, const Mod& m) {
    Mod x = t;
    return x -= m;
}
Mod operator*(const Mod& a, const Mod& b){
    Mod x = a;
    return x *= b;
}
Mod operator*(long t, const Mod& m) {
    Mod x = t;
    return x *= m;
}
Mod operator/(const Mod& a, const Mod& b) {
    Mod x = a;
    return x /= b;
}
Mod operator/(long t, const Mod& m) {
    Mod x = t;
    return x /= m;
}
bool operator==(const Mod& a, const Mod& b) {
    return a.val() == b.val();
}
bool operator==(long t, const Mod& m) {
    Mod x = t;
    return x == m;
}
bool operator!=(const Mod& a, const Mod& b) {
    return !(a == b);
}
bool operator!=(long t, const Mod& m){
    return !(t == m);
}
istream& operator>>(istream& is, Mod& m) {
    long i;
    is >> i;    //cin >> type long
    m = i;  //convert to mod
    return is;
}
ostream& operator<<(ostream& os, const Mod& m) {
    return os << m.val() << "(mod " << Mod::get_modulus() << ')';   //cout << type mod --> a(mod m)
}

/*
int main() {
    int i = 0;  //error counter
    Mod::set_modulus(224285003);
    long m = Mod::get_modulus();
    long b = 976571257;    //number to find in mod m
    long d = 121523; //number to divide by
    long e = 12423; //power to raise mod to
    Mod a = b;
    Mod c = 12125;  //number to multiply mod by (takes user input)
    cout << "Please enter a number: ";
    cin >> c;
    cout << b << " = " << a << ", ";
    cout << b << "/" << d << " = " << a/d << ", ";
    cout << b << "^" << e << " = " << a.pwr(e) << ", ";
    cout << b << "*" << c << " = " << a*c << endl;
    if(a == a.pwr(m)) cout << "Mod == Mod works" << endl;
    else i++;
    if(b == a.pwr(m)) cout << "long == Mod works" << endl;
    else i++;
    if((a + a) != a.pwr(m)) cout << "Mod != Mod works" << endl;
    else i++;
    if(b != (3 * a.pwr(m))) cout << "long != Mod works" << endl;
    else i++;
    if(!i) cout << "All clear! (so far)" << endl;
    else cout << "Error in " << i << " cases." << endl;
    return 0;
}
*/
