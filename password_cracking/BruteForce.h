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
    string force_brute_openmp(string target, string current_password);
private:
    const int MAX_ITERATIONS = 1000000; // Limite du nombre d'itérations pour éviter les boucles infinies
    void search_combinations(const string& target, string& current_password, const string& alphabet, int alphabet_size, int length, bool& password_found, string& found_password, int thread_id, int total_threads);
};