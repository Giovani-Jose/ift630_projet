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
    string force_brute_openmp(string target, string current_password, int index, int iter);
    int temps_exec;
private:
    const int MAX_ITERATIONS = 1000000; // Limite du nombre d'itérations pour éviter les boucles infinies
};