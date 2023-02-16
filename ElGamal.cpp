#include <stdlib.h>  //rand
#include <time.h>    //for srand

#include <functional>  //require c++11
#include <iostream>
// note: this is code for c++11, if you want to run this code, you should add -std=c++11 to your compiler
using namespace std;

class ElGamal {
public:
    ElGamal(int q, int a) {
        setQA(q, a);
    }
    // for easy use
    void setQA(int q, int a) {
        this->q = q, this->a = a;
    }
    int getPrivateKey() const {
        return getPrivateKey(q);
    }
    int getPublicKey(const int& privateKey) const {
        return getPublicKey(a, q, privateKey);
    }
    int getSenderPublicKey(const int& reciverPublicKey, const int& senderPrivateKey) const {
        return fastpow(reciverPublicKey, senderPrivateKey, q);
    }
    pair<int, int> getEncryptedMessage(const int& senderPrivateKey, const int& reciverPublicKey, const int& message) const {
        return getEncryptedMessage(a, q, senderPrivateKey, reciverPublicKey, message);
    }
    int getBK(const int& c1, const int& reciverPrivateKey) const {  // for decrypt
        return fastpow(c1, reciverPrivateKey, q);
    }
    int getDecryptedMessage(const int& bk, const int& c2) const {  // for decrypt
        return (c2 * inv(bk, q)) % q;
    }
    int crackSenderPrivateKey(const int& c1) const {
        for (int k = 1; k <= q - 1; ++k)
            if (fastpow(a, k, q) == c1)
                return k;
        return -1;  // just to make the compiler happy, this line will never be executed
    }
    // implementation
    int getPrivateKey(const int& q) const {
        return (rand() % (q - 2)) + 2;  // 2~q-2
    }
    int getPublicKey(const int& a, const int& q, const int& privateKey) const {
        return fastpow(a, privateKey, q);
    }
    pair<int, int> getEncryptedMessage(const int& a, const int& q, const int& senderPrivateKey, const int& reciverPublicKey, const int& message) const {
        int senderPublicKey = getSenderPublicKey(reciverPublicKey, senderPrivateKey);
        return {fastpow(a, senderPrivateKey, q), (message * senderPublicKey) % q};
    }

private:
    int inv(const int& num, const int& mod) const {  //求 num 关于模 mod 的逆元
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
    int fastpow(int disu, int zhisu, const int& mod) const {
        int ans = 1;
        while (zhisu) {
            if (zhisu & 1)  //奇数
                ans = ans * disu % mod;
            disu = disu * disu % mod;
            zhisu >>= 1;
        }
        return ans;
    }
    int q, a;
};

int main() {
    ElGamal elgamal(71, 7);
    //(a) sender is A, reciver is B
    int yb = 3 /*reciver PublicKey*/, k = 2 /*sender PrivateKey*/, m = 30;
    pair<int, int> c = elgamal.getEncryptedMessage(k, yb, m);
    cout << "c1 = " << c.first << ", c2 = " << c.second << endl;  // c1 = 49, c2 = 57

    //(b) sender is A, reciver is B
    int c1 = 59;
    k = elgamal.crackSenderPrivateKey(c1);
    c = elgamal.getEncryptedMessage(k, yb, m);                    // reciver PublicKey is yb=3
    cout << "c1 = " << c.first << ", c2 = " << c.second << endl;  // c1 = 59, c2 = 29
}