#include <iostream>
#include <stdexcept>
#include <string>
using namespace std;

class GF_2_4 {  //ʹ��x^4+x+1��Ϊ��GF(2^4)�Ĳ���Լ����ʽ����������ԪΪ2
public:
    GF_2_4(const int& polynomial) {  //��ֵ��ʾ�Ķ���ʽ
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
        if (this->polynomial == 0 || other.polynomial == 0)  //����ʽΪ0��ʱ��ֱ����
            return GF_2_4(0);
        return GF_2_4(g[(n[this->polynomial] + n[other.polynomial]) % 15] % m);  // a*b=g^n*g^m=g^(n+m)
    }
    GF_2_4 operator/(const GF_2_4& other) const {
        if (this->polynomial == 0)  //����ʽΪ0��ʱ��ֱ����
            return GF_2_4(0);
        else if (other.polynomial == 0)  //����Ϊ0��ʱ���׳��쳣
            throw std::invalid_argument("parameter must not be 0");
        return GF_2_4(g[(n[this->polynomial] + 15 - n[other.polynomial]) % 15] % m);  //���Գ�������Ԫ, a/b=g^n/g^m=g^n*g(15-m)=g^(n+15-m)
    }
    string to_string() const {
        string s = ((polynomial & (1 << 3)) ? string("x^3+") : string("")) + ((polynomial & (1 << 2)) ? "x^2+" : "") + ((polynomial & (1 << 1)) ? "x+" : "");
        if (s.size() == 0 && (polynomial & (1 << 0)) == 0)
            return string("0");
        else if ((polynomial & (1 << 0)) == 1)
            return s + "1";
        else
            return s.substr(0, s.size() - 1);  //ȥ�����һ��+
    }

private:
    //������ֵ��ʾ�Ķ���ʽ��ȡ����Ԫ��ָ�����±��ʾ����ʽ����ֵ��ʾ������ʽ0�޷�������Ԫ���ɣ�����Ϊ-1��ռλ��
    int n[16] = {-1, 0, 1, 4, 2, 8, 5, 10, 3, 14, 9, 7, 6, 13, 11, 12};
    //��������Ԫ��ָ����ȡ��ֵ��ʾ�Ķ���ʽ���±��ʾ����Ԫ��ָ����
    int g[15] = {1, 2, 4, 8, 3, 6, 12, 11, 5, 10, 7, 14, 15, 13, 9};
    int polynomial;   //��ֵ��ʾ�Ķ���ʽ
    int m = 0b10011;  //����Լ����ʽ x^4+x+1 ����ֵ��ʾ
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