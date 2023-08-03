#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;

class BruteForce {
public:
    BruteForce();
    string generate_all_alphanumeric(int length, string encodedPass, string alphabet);
    int temps_exec;


private:
    //string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    string mot = "";

    atomic<bool> passFoundDic = { false };
    atomic<bool> passFoundAllComb = { false };

    void generate_alphanumeric(int length, string encodedPass, string current, string alphabet, bool& passFoundAllComb);
};