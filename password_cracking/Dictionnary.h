#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;

class Dictionnaire {
public:
    Dictionnaire() = default;
    bool distributeThreadsForSearching(string password);
    string locatedPassword();
    float temps_exec;

private:
    mutex mtx;
    bool shouldShowProgress=true;
    string foundPasswordFileName;
    void showProgress(int threadNum);
    string searchingPassword = "";
    atomic<bool> isPasswordFound = { false };
    void search(int numFile, string encodedPass, int threadNumber);
};