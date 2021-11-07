#include <iostream>
#include <sstream>
#include <math.h>
#include <bitset>
#include <time.h>
using namespace std;

#define Ch(x,y,z) ((x&y)^(~x&z))
#define Ma(x,y,z) ((x&y)^(x&z)^(y&z))

// #define Cror(x,n) ((x>>n))|(x<<(32-n))  //使用函数才可以
unsigned int Cror(unsigned int x,int n){
    return ((x>>n))|(x<<(32-n));
}

#define Sigma0(x) (Cror(x,2)^Cror(x,13)^Cror(x,22))
#define Sigma1(x) (Cror(x,6)^Cror(x,11)^Cror(x,25))
#define sigma0(x) (Cror(x,7)^Cror(x,18)^(x>>3))
#define sigma1(x) (Cror(x,17)^Cror(x,19)^(x>>10))

string Int_to_String(int m){
    stringstream ss;
    ss << m;
    string s;
    ss >> s;
    return s;
}

string String_to_Binary(string str){
    stringstream ss;
    for(int i = 0;i < str.length();i++){
        ss << bitset<8>(str[i]);
    }
    string s;
    ss >> s;
    return s;
}

string Unsigned_int_to_Binary_32(int x){
    stringstream ss;
    ss << bitset<32>(x);
    string s;
    ss >> s;
    return s;
}

string Unsigned_int_to_Binary_64(int x){
    stringstream ss;
    ss << bitset<64>(x);
    string s;
    ss >> s;
    return s;
}

int Char_to_Int(char ch){
    stringstream ss;
    ss << ch;
    int x;
    ss >> x;
    return x;
}

unsigned int String_to_Unsigned_int(string str){
    unsigned int sum = 0;
    for(int i = 0;i < 32;i++){
        sum += pow(2,31-i) * Char_to_Int(str[i]);
    }
    return sum;
}

bool is_prime(int x){
    int cnt = 0;
    for(int i = 1;i <= sqrt(x);i++){
        if(x % i == 0){
            cnt++;
        }
    }
    if(cnt == 1){
        return true;
    }
    else{
        return false;
    }
}

void prime_64(int *x){
    int cnt = 0;
    int num = 2;
    while(cnt < 64){
        if(is_prime(num)){
            x[cnt++] = num;
        }
        num++;
    }
}

unsigned int sqrt_get_32(int x,int root){
    double y;
    if(root == 2){
        y = sqrt(x) - floor(sqrt(x));
    }
    if(root == 3){
        y = pow(x,1.0/3) - floor(pow(x,1.0/3));
    }
    int cnt = 0;
    unsigned int sum = 0;
    for(int i = 0;i < 8;i++){
        while(y >= pow(16,-i-1)){
            y -= pow(16,-i-1);
            cnt++;
        }
        sum += pow(16,7-i) * cnt;
        cnt = 0;
    }
    return sum;
}

string info_extend(string message){
    string message_binary = String_to_Binary(message);
    string message_extended = message_binary.append(1,'1');
    int bits_remain = message_binary.length() % 512;
    int bit0_fill = bits_remain>448?960-bits_remain:448-bits_remain;
    message_extended.append(bit0_fill,'0');
    message_extended.append(Unsigned_int_to_Binary_64(message.length()*sizeof(char)*8));
    return message_extended;
}

string HASH(string message_extended,unsigned int *h,unsigned int *k){
    int num_of_chunk = message_extended.length() / 512;
    string chunk[num_of_chunk];
    for(int i = 0;i < num_of_chunk;i++){
        chunk[i] = message_extended.substr(512*i,512);
        unsigned int w[64];
        int j;
        for(j = 0;j < 16;j++){
            w[j] = String_to_Unsigned_int(chunk[i].substr(32*j,32));
        }
        unsigned int s0,s1;
        for(j = 16;j < 64;j++){
            s0 = sigma0(w[j-15]);
            s1 = sigma1(w[j-2]);
            w[j] = w[j-16] + s0 + w[j-7] + s1;
        }
        unsigned int A = h[0];
        unsigned int B = h[1];
        unsigned int C = h[2];
        unsigned int D = h[3];
        unsigned int E = h[4];
        unsigned int F = h[5];
        unsigned int G = h[6];
        unsigned int H = h[7];
        for(j = 0;j < 64;j++){
            s0 = Sigma0(A);
            unsigned int maj = Ma(A,B,C);
            unsigned int t2 = s0 + maj;
            s1 = Sigma1(E);
            unsigned int ch = Ch(E,F,G);
            unsigned int t1 = H + s1 + ch + k[j] + w[j];
            H = G;
            G = F;
            F = E;
            E = D + t1;
            D = C;
            C = B;
            B = A;
            A = t1 + t2;
        }
        h[0] = h[0] + A;
        h[1] = h[1] + B;
        h[2] = h[2] + C;
        h[3] = h[3] + D;
        h[4] = h[4] + E;
        h[5] = h[5] + F;
        h[6] = h[6] + G;
        h[7] = h[7] + H;
    }
    string hash = Unsigned_int_to_Binary_32(h[0]);
    for(int i = 1;i < 8;i++){
        hash.append(Unsigned_int_to_Binary_32(h[i]));
    }
    return hash;
}

int main(){
    unsigned int h[8],k[64];
    int x[65] = {0};
    prime_64(x);
    for(int i = 0;i < 8;i++){
        h[i] = sqrt_get_32(x[i],2);
    }
    for(int i = 0;i < 64;i++){
        k[i] = sqrt_get_32(x[i],3);
    }
    int mes = -1;
    string hash;
    clock_t start_time = clock();
    do{
        mes++;
        cout << mes << endl;
        string message = Int_to_String(mes);
        string message_extended = info_extend(message);
        hash = HASH(message_extended,h,k);
        cout << "hash:" << hash << endl;
    }
    while(hash.substr(0,30) != "000000000000000000000000000000");  //找出hash值前30位为0的输入
    cout << mes << endl;
    clock_t end_time = clock();
    cout << "The run time is: " <<(double)(end_time - start_time) / CLOCKS_PER_SEC << "s" << endl;
}
