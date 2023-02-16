#include <exception>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class S_AES {  //����������Կ16λ������16λ������2�ֵ�AES�ӽ��ܡ�ģx^4+x+1
public:
    S_AES(const string &key) {
        setKey(key);
    }
    void setKey(const string &key) {
        if (int(key.size()) != keySize || !isBinary(key))
            throw std::invalid_argument("key is not valid");
        roundKeys[0][0] = key.substr(0, 8);  //��8λ
        roundKeys[0][1] = key.substr(8, 8);  //��8λ
        keyExpansion();
    }
    string encryptToBinary(const string &plainBinary) const {//ֻ����16λ������
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
    int convertBinaryToDecimal(const string &binary) const {  //��������string�Ķ����ƽ���ת��ʮ��������
        int decimal = 0;
        for (int i = 0, size = binary.size(); i < size; i++)  // binary[0]�������λ
            decimal += (binary[i] == '1' ? (1 << (size - i - 1)) : 0);
        return decimal;
    };
    bool isBinary(const string &txt) const {//�ж��Ƿ��Ƕ�����(��������0���͡�1��)
        for (const auto &c : txt)
            if (c != '0' && c != '1')
                return false;
        return true;
    };
    string Xor(const string &a, const string &b) const {  //������string����ַ�xor,���Ǳ����ǵȳ���
        string result;
        for (int i = 0, size = a.size(); i < size; ++i)
            result += (a[i] != b[i] ? "1" : "0");
        return result;
    };
    char Xor(const char &a, const char &b) const {  //������char����xor
        return (a != b ? '1' : '0');
    };
    string g(const string &w, const string &rc) const {
        string low = w.substr(0, 4), high = w.substr(4, 4);//�ֳɸߵ�4λ,���ҽ���������4λ
        int sub_row = convertBinaryToDecimal(high.substr(0, 2)), sub_col = convertBinaryToDecimal(high.substr(2, 2));
        high = SBox[sub_row][sub_col];
        sub_row = convertBinaryToDecimal(low.substr(0, 2)), sub_col = convertBinaryToDecimal(low.substr(2, 2));
        low = SBox[sub_row][sub_col];
        return Xor(high + low, rc);
    }
    vector<string> stringToArray(const string &str) const {//��16λ��stringÿ��λ�ֳ�һ��string
        vector<string> array;
        for (int i = 0; i < int(str.size()); i += 4)
            array.push_back(str.substr(i, 4));
        return array;
    }
    void substitute(vector<string> &array, const string SBox[4][4]) const {
        int r, c;
        for (auto &i : array) {
            r = convertBinaryToDecimal(i.substr(0, 2));//ǰ��λ��S�е��е��±�
            c = convertBinaryToDecimal(i.substr(2, 2));//����λ��S�е��е��±�
            i = SBox[r][c];
        }
    }
    void shiftRows(vector<string> &array) const {
        swap(array[1], array[3]);  //ѭ�����Ƶڶ���һ��
    }
    void invShiftRows(vector<string> &array) const {
        swap(array[1], array[3]);  //��ת֮ǰ��ѭ�����Ƶڶ���һ��
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
        vector<string> array = stringToArray(text);  //vector��ǰ�����ǵ�һ�У��������ǵڶ���
        substitute(array, SBox);//Ӧ��S��
        shiftRows(array);//ѭ������
        if (round != 2)//����������һ��
            mixColumns(array);//�����
        return Xor(array[0] + array[1] + array[2] + array[3], key);//��������Կ
    }
    string DecryptRound(const string &text, const string &key, const int &round) const {
        vector<string> array = stringToArray(text);
        invShiftRows(array);//��ת֮ǰ��S��
        substitute(array, invSBox);//��ת֮ǰ��ѭ������
        string tmp = Xor(array[0] + array[1] + array[2] + array[3], key);//��������Կ
        array[0] = tmp.substr(0, 4), array[1] = tmp.substr(4, 4), array[2] = tmp.substr(8, 4), array[3] = tmp.substr(12, 4);
        if (round != 2)//����������һ��
            invMixColumns(array);//��ת֮ǰ�Ļ����
        return array[0] + array[1] + array[2] + array[3];
    }
    void keyExpansion() {  //��Կ��չ
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
    string roundKeys[3][2];                           //����Կ,��һά���������ڶ�ά����Կ������������
    const string rcon[2] = {"10000000", "00110000"};  //�ֳ���,��������Կ������,rocn[0]�ǵ�һ�ֵ��ֳ���
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