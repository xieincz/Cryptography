#include <stdlib.h>  //rand
#include <time.h>    //for srand

#include <functional>  //require c++11
#include <iostream>
using namespace std;
typedef long long ll;
class DSA {
public:
    DSA(const int& p, const int& q, const int& h) {
        setPQH(p, q, h);
    }
    // for easy use
    void setPQH(const int& p, const int& q, const int& h) {
        this->p = p, this->q = q, this->h = h;
    }
    int getG() const {
        return fastpow(h, (p - 1) / q, p);//g=h^(p-1)/q mod p，(p - 1)必定是q的倍数
    }
    int getPrivateKey() const {
        return getPrivateKey(q);
    }
    int getPublicKey(const int& privateKey) const {
        return getPublicKey(getG(), p, privateKey);
    }
    int getSecret() const {
        return getSecret(q);
    }
    // implementation
    int getPrivateKey(const int& q) const {
        return (rand() % (q - 1)) + 1;  // [1,q-1]
    }
    int getPublicKey(const int& g, const int& p, const int& privateKey) const {
        return fastpow(g, privateKey, p);
    }
    pair<int, int> getSignature(const int& privateKey, const int& message, const int& k) const {
        int r = fastpow(getG(), k, p) % q;
        int s = (inv(k, q) * ((message + privateKey * r) % q)) % q;  // 这里为了简化，用M代替了H(M)
        return {r, s};
    }
    bool verify(const int& publicKey, const int& message, const pair<int, int>& signature) const {
        int r = signature.first, s = signature.second;
        if (r < 1 || r > q - 1 || s < 1 || s > q - 1)  // r,s不在[1,q-1]内，返回false
            return false;
        int w = inv(s, q);
        int u1 = (message * w) % q;  // 这里为了简化，用M代替了H(M)
        int u2 = (r * w) % q;
        int v = ((fastpow(getG(), u1, p) * fastpow(publicKey, u2, p)) % p) % q;
        return v == r;
    }
    int getSecret(const int& q) const {
        return (rand() % (q - 1)) + 1;  // [1,q-1]
    }

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
    int fastpow(int disu, int zhisu, const int& mod) const {
        int ans = 1;
        while (zhisu) {
            if (zhisu & 1)  // 奇数
                ans = ans * disu % mod;
            disu = disu * disu % mod;
            zhisu >>= 1;
        }
        return ans;
    }
    int p, q, h;
};

int main() {
    int p = 67, q = 11, h = 3;
    DSA dsa(p, q, h);
    int g = dsa.getG();//g=59
    cout << "g=" << g << endl;
    int privateKey = 5, publicKey = dsa.getPublicKey(privateKey);//publicKey=62
    cout << "publicKey=" << publicKey << endl;
    int message = 4, k = 3;
    pair<int, int> signature = dsa.getSignature(privateKey, message, k);//signature=(2,1)
    cout << "signature=(" << signature.first << "," << signature.second << ")" << endl;
    cout << "verify=" << (dsa.verify(publicKey, message, signature) ? "true" : "false") << endl;//verify=true
}