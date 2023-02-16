#include <iostream>
#include <string>
using namespace std;

class DES {
public:
    DES(string key) {
        setKey(key);
    }
    void setKey(string key) {                                                        //生成16个key存到keys数组
        auto circularLeftShift = [](string key_chunk, const int& times) -> string {  // Lambda表达式,循环左移times次
            for (int i = 0; i < times; i++) {
                key_chunk += key_chunk.front();  //将开头一个放到末尾
                key_chunk.erase(0, 1);           //删除开头一个
            }
            return key_chunk;
        };
        if (!isBinary(key))
            key = convertTextToBinary(key);
        string perm_key;
        for (int i = 0; i < 56; i++)
            perm_key += key[pc1[i] - 1];
        string left = perm_key.substr(0, 28), right = perm_key.substr(28, 28);
        for (int i = 0; i < 16; i++) {
            if (i == 0 || i == 1 || i == 8 || i == 15) {  //第1,2, 9, 16轮只需循环左移一次
                left = circularLeftShift(left, 1);
                right = circularLeftShift(right, 1);
            } else {  //其他轮需要循环左移2次
                left = circularLeftShift(left, 2);
                right = circularLeftShift(right, 2);
            }
            string combined_key = left + right, round_key;
            for (int i = 0; i < 48; i++)
                round_key += combined_key[pc2[i] - 1];
            keys[i] = round_key;
        }
    }
    string encrypt(string plaintext) const {
        if (!isBinary(plaintext)) {
            for (int i = 0, num = 8 - plaintext.size() % 8; i < num; i++)
                plaintext += num;                        //按照PKCS#5规范进行填充
            plaintext = convertTextToBinary(plaintext);  //转成二进制
        } else if (plaintext.size() % 64 != 0) {
            cout << "Error ! Check the parameter";
            exit(1);
        }
        string ciphertext;
        for (int i = 0, num = plaintext.size() / 64; i < num; i++)
            ciphertext += encrypt64(plaintext.substr(i * 64, 64), keys);
        return ciphertext;
    }
    string decryptToBinary(const string& cipherBinary) const {
        string newKeys[16], plaintext;
        for (int i = 0; i < 16; i++)
            newKeys[i] = keys[15 - i];
        for (int i = 0, num = cipherBinary.size() / 64; i < num; i++)
            plaintext += encrypt64(cipherBinary.substr(i * 64, 64), newKeys);
        int padNum = convertBinaryToDecimal(plaintext.substr(plaintext.size() - 8));
        if (padNum <= 8)
            plaintext.erase(plaintext.size() - 8 * padNum);  //将按照PKCS#5规范填充的字符移除
        return plaintext;
    }
    string decryptToText(const string& cipherBinary) const {
        return convertBinaryToText(decryptToBinary(cipherBinary));
    }

private:
    string convertBinaryToText(const string& binary) const {
        string txt;
        for (int i = 0, num = binary.size() / 8; i < num; i++)
            txt += convertBinaryToDecimal(binary.substr(i * 8, 8));
        return txt;
    }
    string encrypt64(const string& binary64, const string keys[16]) const {
        string perm;
        for (int i = 0; i < 64; i++)
            perm += binary64[ip[i] - 1];  //应用初始置换表
        string left = perm.substr(0, 32), right = perm.substr(32, 32);
        for (int i = 0; i < 16; i++) {
            string right_expanded, res;
            for (int i = 0; i < 48; i++)  //拓展右半部分的原文
                right_expanded += right[expansion_table[i] - 1];
            string xored = Xor(keys[i], right_expanded);  //密钥和右半部分xor
            for (int i = 0; i < 8; i++) {                 //将xor结果分成8份，每份6位经过S盒变成每份4位
                string row1 = xored.substr(i * 6, 1) + xored.substr(i * 6 + 5, 1);
                int row = convertBinaryToDecimal(row1);
                string col1 = xored.substr(i * 6 + 1, 1) + xored.substr(i * 6 + 2, 1) + xored.substr(i * 6 + 3, 1) + xored.substr(i * 6 + 4, 1);
                int col = convertBinaryToDecimal(col1);
                int val = s[i][row][col];
                string val4 = convertDecimalToBinary(val);  //最高位在左边
                res += val4.substr(val4.size() - 4);        //只要最低的4位
            }
            string perm2;
            for (int i = 0; i < 32; i++)  //应用另一个置换表
                perm2 += res[permutation_tab[i] - 1];
            xored = Xor(perm2, left);  //置换后和左边的进行xor
            left = xored;
            if (i < 15) {
                left = right;
                right = xored;
            }
        }
        string combined_text = left + right, ciphertext;
        for (int i = 0; i < 64; i++)  //应用逆转的初始置换表IP-1
            ciphertext += combined_text[inverse_permutation[i] - 1];
        return ciphertext;
    };

    string convertDecimalToBinary(const int& decimal) const {  //将十进制数字进制转成二进制储存在string
        string binary;                                         // binary[0]就是最高位
        for (int i = sizeof(decimal) * 8 - 1; i >= 0; --i)     //从最高位开始
            binary += ((decimal & (1 << i)) ? "1" : "0");
        return binary;
    };

    int convertBinaryToDecimal(const string& binary) const {  //将储存在string的二进制进制转成十进制数字
        int decimal = 0;
        for (int i = 0, size = binary.size(); i < size; i++)  // binary[0]就是最高位
            decimal += (binary[i] == '1' ? (1 << (size - i - 1)) : 0);
        return decimal;
    };
    string convertTextToBinary(const string& txt) const {
        string binary;  // binary[0]就是txt的左边第一个char(共8位)的最高位
        for (const auto& c : txt) {
            string b = convertDecimalToBinary(c);
            binary += b.substr(b.size() - 8);
        }
        return binary;
    };

    string Xor(const string& a, const string& b) const {  //对两个string逐个字符xor,他们必须是等长的
        string result;
        for (int i = 0, size = a.size(); i < size; ++i)
            result += (a[i] != b[i] ? "1" : "0");
        return result;
    };

    bool isBinary(const string& txt) const {
        for (const auto& c : txt)
            if (c != '0' && c != '1')
                return false;
        return true;
    };
    string keys[16];       //储存16轮的密钥
    const int pc1[56] = {  //密钥初始置换表PC_1
        57, 49, 41, 33, 25, 17, 9,
        1, 58, 50, 42, 34, 26, 18,
        10, 2, 59, 51, 43, 35, 27,
        19, 11, 3, 60, 52, 44, 36,
        63, 55, 47, 39, 31, 23, 15,
        7, 62, 54, 46, 38, 30, 22,
        14, 6, 61, 53, 45, 37, 29,
        21, 13, 5, 28, 20, 12, 4};
    const int pc2[48] = {  //密钥压缩置换表PC_1
        14, 17, 11, 24, 1, 5,
        3, 28, 15, 6, 21, 10,
        23, 19, 12, 4, 26, 8,
        16, 7, 27, 20, 13, 2,
        41, 52, 31, 37, 47, 55,
        30, 40, 51, 45, 33, 48,
        44, 49, 39, 56, 34, 53,
        46, 42, 50, 36, 29, 32};
    const int ip[64] = {  //初始数据置换表IP
        58, 50, 42, 34, 26, 18, 10, 2,
        60, 52, 44, 36, 28, 20, 12, 4,
        62, 54, 46, 38, 30, 22, 14, 6,
        64, 56, 48, 40, 32, 24, 16, 8,
        57, 49, 41, 33, 25, 17, 9, 1,
        59, 51, 43, 35, 27, 19, 11, 3,
        61, 53, 45, 37, 29, 21, 13, 5,
        63, 55, 47, 39, 31, 23, 15, 7};
    const int expansion_table[48] = {  // E拓展表
        32, 1, 2, 3, 4, 5,
        4, 5, 6, 7, 8, 9,
        8, 9, 10, 11, 12, 13,
        12, 13, 14, 15, 16, 17,
        16, 17, 18, 19, 20, 21,
        20, 21, 22, 23, 24, 25,
        24, 25, 26, 27, 28, 29,
        28, 29, 30, 31, 32, 1};
    const int s[8][4][16] =  // S盒
        {{{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
          {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
          {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
          {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},
         {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
          {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
          {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
          {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
         {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
          {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
          {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
          {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},
         {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
          {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
          {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
          {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},
         {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
          {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
          {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
          {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},
         {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
          {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
          {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
          {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},
         {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
          {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
          {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
          {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
         {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
          {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
          {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
          {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}};
    const int permutation_tab[32] = {  // p置换表
        16, 7, 20, 21,
        29, 12, 28, 17,
        1, 15, 23, 26,
        5, 18, 31, 10,
        2, 8, 24, 14,
        32, 27, 3, 9,
        19, 13, 30, 6,
        22, 11, 4, 25};
    const int inverse_permutation[64] = {  //最终置换表IP-1
        40, 8, 48, 16, 56, 24, 64, 32,
        39, 7, 47, 15, 55, 23, 63, 31,
        38, 6, 46, 14, 54, 22, 62, 30,
        37, 5, 45, 13, 53, 21, 61, 29,
        36, 4, 44, 12, 52, 20, 60, 28,
        35, 3, 43, 11, 51, 19, 59, 27,
        34, 2, 42, 10, 50, 18, 58, 26,
        33, 1, 41, 9, 49, 17, 57, 25};
};

int main() {
    // 64位密钥
    // string key = "0110000101100010011000110110010001000001010000100100001101000100";
    string key = "abcdABCD";  //也可以是"0110000101100010011000110110010001000001010000100100001101000100"
    //明文
    string txt0 = "Big Data Security";
    //另外两个64位明文
    //string txt1 = "0000000000000000000000000000000000000000000000000000000000000000";
    //string txt2 = "1000000000000000000000000000000000000000000000000000000000000000";
    DES des(key);
    cout << "Plain text: " << txt0 << endl;
    string ciphertext = des.encrypt(txt0);
    cout << "Ciphertext (Binary): " << ciphertext << endl;
    cout << "Decrypted text (Binary): " << des.decryptToBinary(ciphertext) << endl;
    cout << "Decrypted text (String): " << des.decryptToText(ciphertext) << endl;
    cout <<"最终解密结果字节数: "<< des.decryptToText(ciphertext).size() << endl;
    cout <<"加密前的明文字节数: "<< txt0.size() << endl;
    return 0;
}