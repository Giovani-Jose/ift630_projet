#include <iostream>
#include <string>
#include <omp.h>
#include "BruteForce.h"

using namespace std;



BruteForce::BruteForce() {
}


string BruteForce::force_brute_openmp(string target, string current_password, int index, int iteration) {
    const int num_threads = omp_get_max_threads();
    static const string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    string found_password = "";

#pragma omp parallel for num_threads(num_threads) shared(found_password)
    for (int i = 0; i < alphabet.size(); ++i)
    {
        if (!found_password.empty() || iteration >= MAX_ITERATIONS) // Si le mot de passe a déjà été trouvé par un autre thread, sortir prématurément
            continue;

        string next_password = current_password;
        next_password[index] = alphabet[i];

        if (index == target.length() - 1)
        {
            if (next_password == target) // Mot de passe trouvé
            {
#pragma omp critical // S'assurer qu'un seul thread peut mettre à jour la variable found_password
                {
                    if (found_password.empty())
                    {
                        found_password = next_password;
                    }
                }
            }
        }
        else
        {
            found_password = force_brute_openmp(target, next_password, index + 1, iteration + 1);
        }
    }

    return found_password;
}



