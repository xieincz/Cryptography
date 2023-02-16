#include <iostream>
#include <string>
using namespace std;

string encryption(const string& str, const int& key) {
    int k = ((key % 26) + 26) % 26;  // ensure that key is in range [0,25]
    string ret = str;
    for (auto& c : ret)
        if (isupper(c)) {  // if c is in range ['A', 'Z']
            // first map char into [0,25] by minus 'A',
            // then add k, then mode 26,
            // then add 26 and mode 26 again,(ps: In order to ensure in [0,25])
            // then map [0,25] to ASCII characters by add 'A'
            c = ((c - 'A' + k) % 26 + 26) % 26 + 'A';
        } else if (islower(c)) {  // if c is in range ['a', 'z']
            c = ((c - 'a' + k) % 26 + 26) % 26 + 'a';
        }
    return ret;
}

string decrypt(const string& str, const int& key) {
    int k = ((key % 26) + 26) % 26;
    string ret = str;
    for (auto& c : ret)
        if (isupper(c)) {
            c = ((c - 'A' - k) % 26 + 26) % 26 + 'A';
        } else if (islower(c)) {
            c = ((c - 'a' - k) % 26 + 26) % 26 + 'a';
        }
    return ret;
}

void Decipher(const string& str) {
    for (int i = 0; i < 26; i++) {
        cout << "key: \n" << i << "\n";
        cout << "original text: \n" << decrypt(str, i) << "\n\n";
    }
}

int main() {
    // test encryption and decrypt functions
    string original = "123a bdc&A Zaz!";
    string encrypted = encryption(original, 25);
    cout << encrypted << endl;  // output: 123z acb&Z Yzy!
    string decrypted = decrypt(encrypted, 25);
    cout << (decrypted == original) << endl;  // output: 1

    cout << encryption("hello world !", 10) << endl;               // output: rovvy gybvn !
    cout << decrypt(encryption("hello world !", 10), 10) << endl;  // output: hello world !
    cout << endl;

    // Decipher ciphertext
    string ciphertext = "Izqzm piyzmznodhvoz tjpm kjrzm oj xcvibz tjpmnzga!";
    Decipher(ciphertext);
    /*output: 
    key:
    21
    original text:
    Never underestimate your power to change yourself!
    */
    return 0;
}