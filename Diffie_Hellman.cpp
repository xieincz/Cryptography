#include <stdlib.h>  //rand
#include <time.h>    //for srand

#include <functional>  //require c++11
#include <iostream>
using namespace std;

class Diffie_Hellman {
public:
    Diffie_Hellman(const int& a, const int& q) {
        setAQ(a, q);
        srand((unsigned)time(NULL));  // init random seed
    }
    // for easy use
    void setAQ(const int& a, const int& q) {
        this->a = a, this->q = q;
    }
    int getPrivateKey() const {
        return getPrivateKey(q);
    }
    int getPublicKey(const int& privateKey) const {
        return getPublicKey(a, q, privateKey);
    }
    int getSharedKey(const int& publicKey, const int& privateKey) const {
        return getSharedKey(publicKey, privateKey, q);
    }
    int crackPrivateKey(const int& publicKey) const {
        return crackPrivateKey(a, q, publicKey);
    }
    int crackSharedKey(const int& publicKeyReciver,const int& publicKeySender) const {
        return crackSharedKey(publicKeyReciver,publicKeySender,a, q);
    }
    // implementation
    int getPrivateKey(const int& q) const {
        return (rand() % (q - 1)) + 1;  // 1~q-1
    }
    int getPublicKey(const int& a, const int& q, const int& privateKey) const {
        return fastpow(a, privateKey, q);
    }
    int getSharedKey(const int& publicKey, const int& privateKey, const int& q) const {
        return fastpow(publicKey, privateKey, q);
    }
    int crackPrivateKey(const int& publicKey, const int& a, const int& q) const {
        for (int xa = 1; xa < q; ++xa)
            if (fastpow(a, xa, q) == publicKey)
                return xa;
        return -1;  // just to make the compiler happy, this line will never be executed
    }
    int crackSharedKey(const int& publicKeyReciver,const int& publicKeySender, const int& a, const int& q) const {
        int xb = crackPrivateKey(publicKeyReciver, a, q);
        return fastpow(publicKeySender, xb, q);
    }

private:
    int fastpow(int disu, int zhisu, const int& mod) const {
        int ans = 1;
        while (zhisu) {
            if (zhisu & 1)  //ÆæÊý
                ans = ans * disu % mod;
            disu = disu * disu % mod;
            zhisu >>= 1;
        }
        return ans;
    }
    int a, q;
};

int main() {
    int q = 71, a = 7;
    Diffie_Hellman diffie_hellman(a, q);
    int xa = 5, ya = diffie_hellman.getPublicKey(xa);
    cout << "xa=" << xa << ",ya=" << ya << endl;  // xa=5,ya=51
    int xb = 12, yb = diffie_hellman.getPublicKey(xb);
    cout << "xb=" << xb << ",yb=" << yb << endl;  // xb=12,yb=4
    int sharedKey = diffie_hellman.getSharedKey(yb, xa);
    cout << "sharedKey=" << sharedKey << endl;  // sharedKey=30
    
    q = 11, a = 2;
    diffie_hellman.setAQ(a, q);
    ya = 9, xa = diffie_hellman.crackPrivateKey(ya);
    cout << "ya=" << ya << ",crack private key xa=" << xa << endl;  // ya=9,crack private key xa=1
    yb = 3, sharedKey = diffie_hellman.crackSharedKey(yb, ya);
    cout << "yb=" << yb << ",crack shared key sharedKey=" << sharedKey << endl;  // yb=3,crack shared key sharedKey=3
}