#include <functional>  //require c++11
#include <iostream>
#include <string>
#include <vector>

#include "time.h"
using namespace std;

int inv(const int& num, const int& mod = 26) {  //求 num 关于 mod 26 的逆元
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

string encryption(const string& str, const int& a, const int& b) {
    string ret = str;
    for (auto& c : ret)
        if (isupper(c)) {
            c = (((c - 'A') * a + b) % 26 + 26) % 26 + 'A';
        } else if (islower(c)) {
            c = (((c - 'a') * a + b) % 26 + 26) % 26 + 'a';
        }
    return ret;
}

string decrypt(const string& str, const int& a, const int& b) {
    int inva = inv(a);
    string ret = str;
    for (auto& c : ret)
        if (isupper(c)) {
            c = (((c - 'A' - b) * inva) % 26 + 26) % 26 + 'A';
        } else if (islower(c)) {
            c = (((c - 'a' - b) * inva) % 26 + 26) % 26 + 'a';
        }
    return ret;
}

string get_input(const string& inputPath) {  //从文件中读取输入
    freopen(inputPath.c_str(), "r", stdin);
    string t, input;
    while (getline(cin, t))
        input += t + '\n';
    input.pop_back();
    freopen("CON", "r", stdin);
    return input;
}

void decipher_1(const string& inputPath, string outputPath) {  //采用暴力破解
    vector<int> a = {1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25};
    outputPath.erase(outputPath.size() - 4);  //移除.txt后缀
    int cnt = 1;
    string input = get_input(inputPath);
    for (const auto& ta : a)
        for (int b = 1; b <= 26; ++b) {
            string outPath = outputPath + "_" + to_string(cnt++) + ".txt";
            freopen(outPath.c_str(), "w", stdout);
            cout << "a: " << ta << " b: " << b << "\n";
            cout << "original text: \n\n"
                 << decrypt(input, ta, b);
            freopen("CON", "w", stdout);
        }
}

vector<vector<char>> count_alphabet(const string& txt) {  //统计字母(不区分大小写)出现的频率
    auto sort = [](int* a, const int& length, int* b) {   // b来保存a数组对应的下标以及排序后的下标
        for (int j = 0; j < length; j++)
            for (int i = 0; i < length - 1 - j; i++)
                if (a[i] < a[i + 1])  //大的排前面
                    swap(a[i], a[i + 1]), swap(b[i], b[i + 1]);
    };
    vector<vector<char>> ret;  // vector前面存数量最多的字母，vector用于应对多个字母数量相同的情况
    int m[26], index[26];
    for (int i = 0; i < 26; ++i)
        m[i] = 0, index[i] = i;
    for (auto& c : txt)
        if (isupper(c))
            ++m[c - 'A'];
        else if (islower(c))
            ++m[c - 'a'];
    sort(m, 26, index);
    ret.push_back({char('a' + index[0])});  //统一采用小写字母
    for (int i = 1; i < 26; ++i)
        (m[i] == m[i - 1]) ? ret.back().push_back(char('a' + index[i])) : ret.push_back({char('a' + index[i])});
    return ret;
}

void decipher_2(const string& inputPath, string outputPath) {  //采用词频统计方式破解
    vector<int> a = {1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25};
    vector<int> theta = {19, 0, 14, 8, 13, 18, 7, 17};  // t,a,o,i,n,s,h,r
    vector<pair<int, int>> abPair;                      // a,b
    outputPath.erase(outputPath.size() - 4);            //移除.txt后缀
    string input = get_input(inputPath);
    vector<vector<char>> result = count_alphabet(input);
    int my_id_1 = result[0][0] - 'a', my_id_2 = result[1][0] - 'a';
    for (int b = 1; b <= 26; ++b)
        for (auto& alpha : a)
            for (auto& t : theta)
                if ((t * alpha + b - my_id_2) % 26 == 0 && (t * alpha + b - my_id_2) > 0)
                    if ((4 * alpha + b - my_id_1) % 26 == 0 && (4 * alpha + b - my_id_1) > 0)
                        abPair.emplace_back(alpha, b);
    cout << "most possible key are those:\n\n";
    for (auto& t : abPair) {
        cout << "a: " << t.first << " b: " << t.second << endl;
        cout << "sample decryption:\n\n";
        cout << decrypt(input.substr(0, input.size() / 10), t.first, t.second) << "\n\n";
        string outPath = outputPath + "_a_" + to_string(t.first) + "_b_" + to_string(t.second) + ".txt";
        freopen(outPath.c_str(), "w", stdout);
        cout << "a: " << t.first << " b: " << t.second << "\n";
        cout << "original text: \n\n"
             << decrypt(input, t.first, t.second);
        freopen("CON", "w", stdout);
    }
}

int main() {
    string inputPath = "密文.txt", outputPath = "原文.txt";  // a: 7 b: 19
    clock_t start, finish;
    start = clock();//开始计时

    // decipher_1(inputPath, outputPath);//暴力破解
    decipher_2(inputPath, outputPath);//词频分析破解

    finish = clock();//结束计时
    printf("finished in %f seconds\n", double(finish - start) / CLOCKS_PER_SEC);
    return 0;
}