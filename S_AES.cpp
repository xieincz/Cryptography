#include <exception>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class S_AES {  //仅仅进行密钥16位、明文16位、轮数2轮的AES加解密。模x^4+x+1
public:
    S_AES(const string &key) {
        setKey(key);
    }
    void setKey(const string &key) {
        if (int(key.size()) != keySize || !isBinary(key))
            throw std::invalid_argument("key is not valid");
        roundKeys[0][0] = key.substr(0, 8);  //高8位
        roundKeys[0][1] = key.substr(8, 8);  //低8位
        keyExpansion();
    }
    string encryptToBinary(const string &plainBinary) const {//只接受16位二进制
        if (int(plainBinary.size()) != blockSize || !isBinary(plainBinary))
            throw std::invalid_argument("plainBinary is not valid");
        string cipherBinary = Xor(plainBinary, roundKeys[0][0] + roundKeys[0][1]);        // ROUND 0
        cipherBinary = EncryptRound(cipherBinary, roundKeys[1][0] + roundKeys[1][1], 1);  // ROUND 1
        return EncryptRound(cipherBinary, roundKeys[2][0] + roundKeys[2][1], 2);          // ROUND 2
    }
    string decryptToBinary(const string &cipherBinary) const {
        if (int(cipherBinary.size()) != blockSize || !isBinary(cipherBinary))
            throw std::invalid_argument("cipherBinary is not valid");
        string plainBinary = Xor(cipherBinary, roundKeys[2][0] + roundKeys[2][1]);      // ROUND 0
        plainBinary = DecryptRound(plainBinary, roundKeys[1][0] + roundKeys[1][1], 1);  // ROUND 1
        return DecryptRound(plainBinary, roundKeys[0][0] + roundKeys[0][1], 2);         // ROUND 2
    }

private:
    int convertBinaryToDecimal(const string &binary) const {  //将储存在string的二进制进制转成十进制数字
        int decimal = 0;
        for (int i = 0, size = binary.size(); i < size; i++)  // binary[0]就是最高位
            decimal += (binary[i] == '1' ? (1 << (size - i - 1)) : 0);
        return decimal;
    };
    bool isBinary(const string &txt) const {//判断是否是二进制(仅包含‘0’和‘1’)
        for (const auto &c : txt)
            if (c != '0' && c != '1')
                return false;
        return true;
    };
    string Xor(const string &a, const string &b) const {  //对两个string逐个字符xor,他们必须是等长的
        string result;
        for (int i = 0, size = a.size(); i < size; ++i)
            result += (a[i] != b[i] ? "1" : "0");
        return result;
    };
    char Xor(const char &a, const char &b) const {  //对两个char进行xor
        return (a != b ? '1' : '0');
    };
    string g(const string &w, const string &rc) const {
        string low = w.substr(0, 4), high = w.substr(4, 4);//分成高低4位,并且交换这两个4位
        int sub_row = convertBinaryToDecimal(high.substr(0, 2)), sub_col = convertBinaryToDecimal(high.substr(2, 2));
        high = SBox[sub_row][sub_col];
        sub_row = convertBinaryToDecimal(low.substr(0, 2)), sub_col = convertBinaryToDecimal(low.substr(2, 2));
        low = SBox[sub_row][sub_col];
        return Xor(high + low, rc);
    }
    vector<string> stringToArray(const string &str) const {//将16位的string每四位分成一个string
        vector<string> array;
        for (int i = 0; i < int(str.size()); i += 4)
            array.push_back(str.substr(i, 4));
        return array;
    }
    void substitute(vector<string> &array, const string SBox[4][4]) const {
        int r, c;
        for (auto &i : array) {
            r = convertBinaryToDecimal(i.substr(0, 2));//前两位是S盒的行的下标
            c = convertBinaryToDecimal(i.substr(2, 2));//后两位是S盒的列的下标
            i = SBox[r][c];
        }
    }
    void shiftRows(vector<string> &array) const {
        swap(array[1], array[3]);  //循环左移第二行一次
    }
    void invShiftRows(vector<string> &array) const {
        swap(array[1], array[3]);  //逆转之前的循环左移第二行一次
    }
    void mixColumns(vector<string> &array) const {
        for (int i = 0; i < 4; i += 2) {
            const string w = array[i] + array[i + 1];
            array[i] = string("") + Xor(w[0], w[6]) + Xor(Xor(w[1], w[4]), w[7]) + Xor(Xor(w[2], w[4]), w[5]) + Xor(w[3], w[5]);//b0^b6,b1^b4^b7,b2^b4^b5,b3^b5
            array[i + 1] = string("") + Xor(w[2], w[4]) + Xor(Xor(w[0], w[3]), w[5]) + Xor(Xor(w[0], w[1]), w[6]) + Xor(w[1], w[7]);//b2^b4,b0^b3^b5,b0^b1^b6,b1^b7
        }
    }
    void invMixColumns(vector<string> &array) const {
        for (int i = 0; i < 4; i += 2) {
            const string w = array[i] + array[i + 1];
            array[i] = string("") + Xor(w[3], w[5]) + Xor(w[0], w[6]) + Xor(Xor(w[1], w[4]), w[7]) + Xor(Xor(w[2], w[3]), w[4]);//b3^b5,b0^b6,b1^b4^b7,b2^b3^b4
            array[i + 1] = string("") + Xor(w[1], w[7]) + Xor(w[2], w[4]) + Xor(Xor(w[0], w[3]), w[5]) + Xor(Xor(w[0], w[6]), w[7]);//b1^b7,b2^b4,b0^b3^b5,b0^b6^b7
        }
    }
    string EncryptRound(const string &text, const string &key, const int &round) const {
        vector<string> array = stringToArray(text);  //vector的前两个是第一列，后两个是第二列
        substitute(array, SBox);//应用S盒
        shiftRows(array);//循环左移
        if (round != 2)//如果不是最后一轮
            mixColumns(array);//混合列
        return Xor(array[0] + array[1] + array[2] + array[3], key);//加上轮密钥
    }
    string DecryptRound(const string &text, const string &key, const int &round) const {
        vector<string> array = stringToArray(text);
        invShiftRows(array);//逆转之前的S盒
        substitute(array, invSBox);//逆转之前的循环左移
        string tmp = Xor(array[0] + array[1] + array[2] + array[3], key);//加上轮密钥
        array[0] = tmp.substr(0, 4), array[1] = tmp.substr(4, 4), array[2] = tmp.substr(8, 4), array[3] = tmp.substr(12, 4);
        if (round != 2)//如果不是最后一轮
            invMixColumns(array);//逆转之前的混合列
        return array[0] + array[1] + array[2] + array[3];
    }
    void keyExpansion() {  //密钥扩展
        string tmp = g(roundKeys[0][1], rcon[0]);
        roundKeys[1][0] = Xor(tmp, roundKeys[0][0]);
        roundKeys[1][1] = Xor(roundKeys[1][0], roundKeys[0][1]);
        tmp = g(roundKeys[1][1], rcon[1]);
        roundKeys[2][0] = Xor(tmp, roundKeys[1][0]);
        roundKeys[2][1] = Xor(roundKeys[2][0], roundKeys[1][1]);
    }
    // S-Box
    const string SBox[4][4] = {{"1001", "0100", "1010", "1011"},
                               {"1101", "0001", "1000", "0101"},
                               {"0110", "0010", "0000", "0011"},
                               {"1100", "1110", "1111", "0111"}};
    const string invSBox[4][4] = {{"1010", "0101", "1001", "1011"},
                                  {"0001", "0111", "1000", "1111"},
                                  {"0110", "0000", "0010", "0011"},
                                  {"1100", "0100", "1101", "1110"}};
    const int keySize = 16, blockSize = 16;
    string roundKeys[3][2];                           //轮密钥,第一维是轮数，第二维是密钥的左右两部分
    const string rcon[2] = {"10000000", "00110000"};  //轮常数,用于轮密钥的生成,rocn[0]是第一轮的轮常数
};

int main() {
    string key = "1010011100111011";
    string text = "0110111101101011";
    string answer = "0000011100111000";
    S_AES aes(key);
    string cipher = aes.encryptToBinary(text);
    cout << "Cipher Text: " << cipher << endl;
    if (cipher == answer) {
        cout << "Encryption Success!" << endl;
    } else {
        cout << "Encryption Failed!" << endl;
    }
    if (aes.decryptToBinary(cipher) == text) {
        cout << "Decryption Success!" << endl;
    } else {
        cout << "Decryption Failed!" << endl;
    }
    return 0;
}