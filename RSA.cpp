#include <functional>  //require c++11
#include <iostream>
using namespace std;

class RSA {
public:
    RSA(const int& p, const int& q, const int& e);
    int getN() const;
    int getD() const;
    int encrypt(int m) const;
    int decrypt(int c) const;
    void set(const int& p, const int& q, const int& e);

private:
    int fastpow(int disu, int zhisu, const int& mod) const;
    int inv(const int& num, const int& mod) const;
    int e, n, d;
};

int RSA::fastpow(int disu, int zhisu, const int& mod) const {
    int ans = 1;
    while (zhisu) {
        if (zhisu & 1)  //奇数
            ans = ans * disu % mod;
        disu = disu * disu % mod;
        zhisu >>= 1;
    }
    return ans;
}

int RSA::inv(const int& num, const int& mod) const {  //求 num 关于模 mod 的逆元
    auto exgcd = [](const int& a, const int& b, int& g, int& x, int& y) {
        std::function<void(const int&, const int&, int&, int&, int&)> exgcd;
        exgcd = [&](const int& a, const int& b, int& g, int& x, int& y) {
            if (!b)
                g = a, x = 1, y = 0;
            else
                exgcd(b, a % b, g, y, x), y -= x * (a / b);
        };
        return exgcd(a, b, g, x, y);
    };  //可递归的匿名函数
    int g, x, y;
    exgcd(num, mod, g, x, y);
    return ((x % mod) + mod) % mod;
}

RSA::RSA(const int& p, const int& q, const int& e) {
    set(p, q, e);
}

int RSA::getN() const {
    return n;
}

int RSA::getD() const {
    return d;
}

int RSA::encrypt(int m) const {
    return fastpow(m, e, n);
}

int RSA::decrypt(int c) const {
    return fastpow(c, d, n);
}

void RSA::set(const int& p, const int& q, const int& e) {
    this->e = e;
    n = p * q;
    d = inv(e, (p - 1) * (q - 1));
}

void check(const RSA& rsa, const int& m, const int& c) {
    cout <<"c="<< c << endl;
    if (m == rsa.decrypt(c))
        cout << "decrypt success" << endl;
    else
        cout << "decrypt fail" << endl;
}

int main() {
    RSA rsa(3, 11, 7);
    int m = 5;
    int c = rsa.encrypt(m);//14
    check(rsa, m, c);
    rsa.set(5, 11, 3);
    m = 9;
    c=rsa.encrypt(m);//14
    check(rsa, m, c);
    rsa.set(7,11,17);
    m = 8;
    c=rsa.encrypt(m);//57
    check(rsa, m, c);
    rsa.set(11,13,11);
    m = 7;
    c=rsa.encrypt(m);//106
    check(rsa, m, c);
    rsa.set(17, 31, 7);
    m = 2;
    c=rsa.encrypt(m);//128
    check(rsa, m, c);
    return 0;
}