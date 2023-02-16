#include <string.h>  //for memcpy

#include <iostream>
#include <string>
using namespace std;
typedef unsigned int uint;
typedef unsigned char uchar;
class MD5 {
public:
    MD5(){};
    string encode(const string &input) {
        count[0] = 0, count[1] = 0;
        state[0] = 0x67452301;  // A
        state[1] = 0xEFCDAB89;  // B
        state[2] = 0x98BADCFE;  // C
        state[3] = 0x10325476;  // D
        update((uchar *)input.c_str(), input.length());
        uchar digest[16], bits[8], PADDING[64] = {0x80} /*第0个为0x80即10000000，后面剩下的默认为0*/;
        uint index = (count[0] >> 3) & 0x3F, padlen = (index < 56) ? (56 - index) : (120 - index);
        encode(bits, count, 8);
        update(PADDING, padlen);
        update(bits, 8);
        encode(digest, state, 16);
        string result;
        for (int i = 0; i < 16; i++) {
            char temp[3] = {0};
            sprintf(temp, "%02x", digest[i]);
            result += temp;
        }
        return result;
    }

private:
    static void encode(uchar *output, uint *input, uint len) {
        for (uint i = 0, j = 0; j < len; i++, j += 4) {
            output[j] = input[i] & 0xFF;
            output[j + 1] = (input[i] >> 8) & 0xFF;
            output[j + 2] = (input[i] >> 16) & 0xFF;
            output[j + 3] = (input[i] >> 24) & 0xFF;
        }
    }
    void process(uchar m[64]) {
        uint a = state[0], b = state[1], c = state[2], d = state[3], x[16];  // step2
        for (int i = 0, j = 0; i < 16; ++i, j += 4)                          // step1
            x[i] = (m[j]) | (m[j + 1] << 8) | (m[j + 2] << 16) | (m[j + 3] << 24);
        // step3
        round1_fuction(a, b, c, d, x[0], 7, 0xd76aa478);
        round1_fuction(d, a, b, c, x[1], 12, 0xe8c7b756);
        round1_fuction(c, d, a, b, x[2], 17, 0x242070db);
        round1_fuction(b, c, d, a, x[3], 22, 0xc1bdceee);
        round1_fuction(a, b, c, d, x[4], 7, 0xf57c0faf);
        round1_fuction(d, a, b, c, x[5], 12, 0x4787c62a);
        round1_fuction(c, d, a, b, x[6], 17, 0xa8304613);
        round1_fuction(b, c, d, a, x[7], 22, 0xfd469501);
        round1_fuction(a, b, c, d, x[8], 7, 0x698098d8);
        round1_fuction(d, a, b, c, x[9], 12, 0x8b44f7af);
        round1_fuction(c, d, a, b, x[10], 17, 0xffff5bb1);
        round1_fuction(b, c, d, a, x[11], 22, 0x895cd7be);
        round1_fuction(a, b, c, d, x[12], 7, 0x6b901122);
        round1_fuction(d, a, b, c, x[13], 12, 0xfd987193);
        round1_fuction(c, d, a, b, x[14], 17, 0xa679438e);
        round1_fuction(b, c, d, a, x[15], 22, 0x49b40821);
        // step4
        round2_fuction(a, b, c, d, x[1], 5, 0xf61e2562);
        round2_fuction(d, a, b, c, x[6], 9, 0xc040b340);
        round2_fuction(c, d, a, b, x[11], 14, 0x265e5a51);
        round2_fuction(b, c, d, a, x[0], 20, 0xe9b6c7aa);
        round2_fuction(a, b, c, d, x[5], 5, 0xd62f105d);
        round2_fuction(d, a, b, c, x[10], 9, 0x02441453);
        round2_fuction(c, d, a, b, x[15], 14, 0xd8a1e681);
        round2_fuction(b, c, d, a, x[4], 20, 0xe7d3fbc8);
        round2_fuction(a, b, c, d, x[9], 5, 0x21e1cde6);
        round2_fuction(d, a, b, c, x[14], 9, 0xc33707d6);
        round2_fuction(c, d, a, b, x[3], 14, 0xf4d50d87);
        round2_fuction(b, c, d, a, x[8], 20, 0x455a14ed);
        round2_fuction(a, b, c, d, x[13], 5, 0xa9e3e905);
        round2_fuction(d, a, b, c, x[2], 9, 0xfcefa3f8);
        round2_fuction(c, d, a, b, x[7], 14, 0x676f02d9);
        round2_fuction(b, c, d, a, x[12], 20, 0x8d2a4c8a);
        // step5
        round3_fuction(a, b, c, d, x[5], 4, 0xfffa3942);
        round3_fuction(d, a, b, c, x[8], 11, 0x8771f681);
        round3_fuction(c, d, a, b, x[11], 16, 0x6d9d6122);
        round3_fuction(b, c, d, a, x[14], 23, 0xfde5380c);
        round3_fuction(a, b, c, d, x[1], 4, 0xa4beea44);
        round3_fuction(d, a, b, c, x[4], 11, 0x4bdecfa9);
        round3_fuction(c, d, a, b, x[7], 16, 0xf6bb4b60);
        round3_fuction(b, c, d, a, x[10], 23, 0xbebfbc70);
        round3_fuction(a, b, c, d, x[13], 4, 0x289b7ec6);
        round3_fuction(d, a, b, c, x[0], 11, 0xeaa127fa);
        round3_fuction(c, d, a, b, x[3], 16, 0xd4ef3085);
        round3_fuction(b, c, d, a, x[6], 23, 0x04881d05);
        round3_fuction(a, b, c, d, x[9], 4, 0xd9d4d039);
        round3_fuction(d, a, b, c, x[12], 11, 0xe6db99e5);
        round3_fuction(c, d, a, b, x[15], 16, 0x1fa27cf8);
        round3_fuction(b, c, d, a, x[2], 23, 0xc4ac5665);
        // step6
        round4_fuction(a, b, c, d, x[0], 6, 0xf4292244);
        round4_fuction(d, a, b, c, x[7], 10, 0x432aff97);
        round4_fuction(c, d, a, b, x[14], 15, 0xab9423a7);
        round4_fuction(b, c, d, a, x[5], 21, 0xfc93a039);
        round4_fuction(a, b, c, d, x[12], 6, 0x655b59c3);
        round4_fuction(d, a, b, c, x[3], 10, 0x8f0ccc92);
        round4_fuction(c, d, a, b, x[10], 15, 0xffeff47d);
        round4_fuction(b, c, d, a, x[1], 21, 0x85845dd1);
        round4_fuction(a, b, c, d, x[8], 6, 0x6fa87e4f);
        round4_fuction(d, a, b, c, x[15], 10, 0xfe2ce6e0);
        round4_fuction(c, d, a, b, x[6], 15, 0xa3014314);
        round4_fuction(b, c, d, a, x[13], 21, 0x4e0811a1);
        round4_fuction(a, b, c, d, x[4], 6, 0xf7537e82);
        round4_fuction(d, a, b, c, x[11], 10, 0xbd3af235);
        round4_fuction(c, d, a, b, x[2], 15, 0x2ad7d2bb);
        round4_fuction(b, c, d, a, x[9], 21, 0xeb86d391);
        state[0] += a, state[1] += b, state[2] += c, state[3] += d;  // step7，注: unsigned int 的加法相当于模2^32的加法
    }
    void update(uchar *input, uint input_length) {
        uint index = count[0] / 8 % 64, part_length = 64 - index, i = 0;
        if ((count[0] += (input_length << 3)) < (input_length << 3))
            count[1]++;
        count[1] += (input_length >> 29);
        if (input_length >= part_length) {
            memcpy(&buffer[index], input, part_length);
            process(buffer);
            for (i = part_length; i + 63 < input_length; i += 64)
                process(&input[i]);
            index = 0;
        }
        memcpy(&buffer[index], &input[i], input_length - i);
    }
    static uint F(uint x, uint y, uint z) {
        return ((x & y) | (~x & z));
    }
    static uint G(uint x, uint y, uint z) {
        return ((x & z) | (y & ~z));
    }
    static uint H(uint x, uint y, uint z) {
        return (x ^ y ^ z);
    }
    static uint I(uint x, uint y, uint z) {
        return (y ^ (x | ~z));
    }
    static uint rotate_left(uint x, uint n) {  // 将x循环左移n位
        return ((x << n) | (x >> (32 - n)));
    }
    static void round1_fuction(uint &a, uint b, uint c, uint d, uint i, uint s, uint t) {
        a += F(b, c, d) + i + t;
        a = rotate_left(a, s);
        a += b;
    }
    static void round2_fuction(uint &a, uint b, uint c, uint d, uint i, uint s, uint t) {
        a += G(b, c, d) + i + t;
        a = rotate_left(a, s);
        a += b;
    }
    static void round3_fuction(uint &a, uint b, uint c, uint d, uint i, uint s, uint t) {
        a += H(b, c, d) + i + t;
        a = rotate_left(a, s);
        a += b;
    }
    static void round4_fuction(uint &a, uint b, uint c, uint d, uint i, uint s, uint t) {
        a += I(b, c, d) + i + t;
        a = rotate_left(a, s);
        a += b;
    }
    uint count[2];
    uint state[4];  // A,B,C,D
    uchar buffer[64];
};

int main() {
    MD5 md5;
    string str = "HelloWorld";        // 68e109f0f40ca72a15e05cc22786f8e6
    cout << md5.encode(str) << endl;  // 68e109f0f40ca72a15e05cc22786f8e6
    return 0;
}