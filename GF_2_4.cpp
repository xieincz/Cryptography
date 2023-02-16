#include <iostream>
#include <stdexcept>
#include <string>
using namespace std;

class GF_2_4 {  //使用x^4+x+1作为域GF(2^4)的不可约多项式，并且生成元为2
public:
    GF_2_4(const int& polynomial) {  //数值表示的多项式
        if (polynomial < 0 || polynomial > 15)
            throw std::invalid_argument("parameter must be between 0 and 15");
        this->polynomial = polynomial;
    }
    GF_2_4 operator+(const GF_2_4& other) const {
        return GF_2_4((this->polynomial ^ other.polynomial) % m);
    }
    GF_2_4 operator-(const GF_2_4& other) const {
        return GF_2_4((this->polynomial ^ other.polynomial) % m);
    }
    GF_2_4 operator*(const GF_2_4& other) const {
        if (this->polynomial == 0 || other.polynomial == 0)  //多项式为0的时候直接算
            return GF_2_4(0);
        return GF_2_4(g[(n[this->polynomial] + n[other.polynomial]) % 15] % m);  // a*b=g^n*g^m=g^(n+m)
    }
    GF_2_4 operator/(const GF_2_4& other) const {
        if (this->polynomial == 0)  //多项式为0的时候直接算
            return GF_2_4(0);
        else if (other.polynomial == 0)  //除数为0的时候抛出异常
            throw std::invalid_argument("parameter must not be 0");
        return GF_2_4(g[(n[this->polynomial] + 15 - n[other.polynomial]) % 15] % m);  //乘以除数的逆元, a/b=g^n/g^m=g^n*g(15-m)=g^(n+15-m)
    }
    string to_string() const {
        string s = ((polynomial & (1 << 3)) ? string("x^3+") : string("")) + ((polynomial & (1 << 2)) ? "x^2+" : "") + ((polynomial & (1 << 1)) ? "x+" : "");
        if (s.size() == 0 && (polynomial & (1 << 0)) == 0)
            return string("0");
        else if ((polynomial & (1 << 0)) == 1)
            return s + "1";
        else
            return s.substr(0, s.size() - 1);  //去掉最后一个+
    }

private:
    //根据数值表示的多项式获取生成元的指数。下标表示多项式的数值表示。多项式0无法用生成元生成，故设为-1来占位。
    int n[16] = {-1, 0, 1, 4, 2, 8, 5, 10, 3, 14, 9, 7, 6, 13, 11, 12};
    //根据生成元的指数获取数值表示的多项式。下标表示生成元的指数。
    int g[15] = {1, 2, 4, 8, 3, 6, 12, 11, 5, 10, 7, 14, 15, 13, 9};
    int polynomial;   //数值表示的多项式
    int m = 0b10011;  //不可约多项式 x^4+x+1 的数值表示
};

int main() {
    GF_2_4 a(0b0001);  // 1
    GF_2_4 b(0b1100);  // x^3+x^2
    cout << "a = " << a.to_string() << endl;
    cout << "b = " << b.to_string() << endl;
    cout << "a + b = " << (a + b).to_string() << endl;  // x^3+x^2+1
    cout << "a - b = " << (a - b).to_string() << endl;  // x^3+x^2+1
    cout << "a * b = " << (a * b).to_string() << endl;  // x^3+x^2
    cout << "a / b = " << (a / b).to_string() << endl;  // x^3+x
    return 0;
}