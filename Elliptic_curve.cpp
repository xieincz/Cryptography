#include <assert.h>
#include <stdlib.h>  //rand
#include <time.h>    //for srand

#include <functional>  //require c++11
#include <iostream>
// note: this is code for c++11, if you want to run this code, you should add -std=c++11 to your compiler
using namespace std;

class G {
public:
    G() {}
    G(const int& x, const int& y, const int& p, const int& a) {
        this->x = x, this->y = y, this->a = a, this->p = p;
    }
    G operator+(const G& g) const {
        assert(this->a == g.a);
        int t, xr, yr;
        if (this->y == 0)  // this 是O点
            return G(g.x, g.y, this->p, this->a);
        else if (g.y == 0)  // g 是O点
            return G(this->x, this->y, this->p, this->a);
        else if (this->x == g.x && this->y == -g.y)  // 互为负原的点
            return G(this->x, 0, this->p, this->a);
        else if (this->x == g.x && this->y == g.y)  // 同一点
            t=mod(3*this->x*this->x+this->a,this->p)*inv(2*this->y,this->p);
        else
            t=mod(g.y-this->y,this->p)*inv(mod(g.x-this->x,this->p),this->p);
        t = mod(t, this->p);
        xr = mod(t * t - this->x - g.x, this->p);
        yr = t * (this->x - xr) - this->y;
        yr = mod(yr, this->p);
        return G(xr, yr, this->p, this->a);
    }
    G operator-(const G& g) const {
        return *this + G(g.x, -g.y, this->p, g.a);
    }
    G operator*(const int& n) const {  // 重载右乘
        G g = *this;
        for (int i = 1; i < n; i++)
            g = g + *this;
        return g;
    }
    int x, y, p, a;

private:
    int inv(const int& num, const int& mod) const {  // 求 num 关于模 mod 的逆元
        auto exgcd = [](const int& a, const int& b, int& g, int& x, int& y) {
            std::function<void(const int&, const int&, int&, int&, int&)> exgcd;
            exgcd = [&](const int& a, const int& b, int& g, int& x, int& y) {
                if (!b)
                    g = a, x = 1, y = 0;
                else
                    exgcd(b, a % b, g, y, x), y -= x * (a / b);
            };
            return exgcd(a, b, g, x, y);
        };  // 可递归的匿名函数
        int g, x, y;
        exgcd(num, mod, g, x, y);
        return ((x % mod) + mod) % mod;
    }
    int mod(const int& num, const int& mod) const {
        return (num % mod + mod) % mod;// 保证返回值为正数
    }
};

G operator*(const int& n, const G& g) {  // 重载左乘
    return g * n;
}

class Elliptic_curve {
public:
    Elliptic_curve(const int& a, const int& b, const int& q, const G& g) {
        set(a, b, q, g);
    }
    void set(const int& a, const int& b, const int& q, const G& g) {
        this->a = a, this->b = b, this->q = q, this->g = g;
    }
    // for easy use
    int getPrivateKey() const {
        return getPrivateKey(q);
    }
    G getPublicKey(const int& privateKey) const {
        return getPublicKey(privateKey, g);
    }
    pair<G, G> getEncryptedMessage(const int& senderPrivateKey, const G& reciverPublicKey, const G& message) const {
        return getEncryptedMessage(senderPrivateKey, reciverPublicKey, message, g);
    }
    G getDecryptedMessage(const pair<G, G>& encryptedMessage, const int& reciverPrivateKey) const {
        return encryptedMessage.second-reciverPrivateKey*encryptedMessage.first; // C2-nb*C1
    }
    // implement
    int getPrivateKey(const int& q) const {
        return (rand() % (q - 1)) + 1;  // 1~q-1
    }
    G getPublicKey(const int& privateKey, const G& g) const {
        return privateKey * g;
    }
    pair<G,G> getEncryptedMessage(const int& senderPrivateKey,const G& reciverPublicKey,const G& message,const G& g)const {
        G c1 = senderPrivateKey * g;                           // k*G
        G c2 = message + senderPrivateKey * reciverPublicKey;  // Pm+k*Pb
        return {c1, c2};
    }

private:
    int a, b, q;
    G g;
};

int main() {
    int q = 11,a = 1, b = 6;
    G g(2, 7, q, a);
    Elliptic_curve ec(a, b, q, g);

    int nb = 7;
    G pb = ec.getPublicKey(nb);//pb={7,2}
    cout << "pb=" << pb.x << "," << pb.y << endl;

    int na = 3;
    // G pa=ec.getPublicKey(3);
    G pm(10, 9, q, a);
    pair<G, G> cm = ec.getEncryptedMessage(na, pb, pm);//cm={{8,3},{10,2}}
    cout << "cm1=" << cm.first.x << "," << cm.first.y << endl;
    cout << "cm2=" << cm.second.x << "," << cm.second.y << endl;

    G pm1 = ec.getDecryptedMessage(cm, nb);//pm1={10,9}
    cout << "pm=" << pm1.x << "," << pm1.y << endl;
 
    return 0;
}