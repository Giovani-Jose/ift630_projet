#include <iostream>
#include <string>
#include <omp.h>
#include "BruteForce.h"

using namespace std;



BruteForce::BruteForce() {
}

void BruteForce::search_combinations(const string& target, string& current_password, const string& alphabet, int alphabet_size, int length, bool& password_found, string& found_password, int thread_id, int total_threads) {
    if (password_found) {
        return; // Si un mot de passe a d�j� �t� trouv�, sortir pr�matur�ment de la recherche
    }

    // Calculer l'indice de d�but et de fin du sous-espace de recherche pour ce thread
    int start_idx = thread_id;
    int end_idx = alphabet_size;
    int num_iterations = 1;
    for (int i = 1; i < length; ++i) {
        start_idx *= alphabet_size;
        end_idx *= alphabet_size;
        num_iterations *= alphabet_size;
    }

    // Afficher les informations de l'espace de recherche allou� � ce thread
#pragma omp critical
    {
        cout << "Thread " << thread_id << " searching space [" << start_idx << ", " << end_idx - 1 << "] for password length " << length << endl;
    }

    // Recherche dans le sous-espace allou� � ce thread
    for (int iteration = 0; iteration < num_iterations; ++iteration) {
        if (!password_found && !found_password.empty()) {
            // Si un autre thread a d�j� trouv� le mot de passe, arr�ter la recherche
            break;
        }

        // G�n�rer le mot de passe potentiel en utilisant l'indice d'it�ration pour les caract�res appropri�s
        string next_password = current_password;
        int remaining_iteration = iteration + start_idx; // Ajouter l'indice de d�but pour que chaque thread commence � une position diff�rente
        for (int j = length - 1; j >= 0; --j) {
            int char_idx = remaining_iteration % alphabet_size;
            remaining_iteration /= alphabet_size;
            next_password[j] = alphabet[char_idx];
        }

        // Afficher le mot de passe test� par ce thread
#pragma omp critical
        {
            cout << "Thread " << thread_id << ": Trying password: " << next_password << endl;
        }

        // V�rifier si le mot de passe potentiel est �gal � la cible
        if (next_password == target) {
            // Mettre � jour 'found_password' en utilisant une section critique
#pragma omp critical
            {
                if (!password_found) {
                    found_password = next_password;
                    password_found = true; // Signaler aux autres threads que le mot de passe a �t� trouv�
                }
            }
        }
    }

    // Appel r�cursif pour la recherche des longueurs de mot de passe suivantes jusqu'� la longueur cible
    if (length < target.length()) {
        for (int next_length = length + 1; next_length <= target.length(); ++next_length) {
            for (int i = thread_id; i < alphabet_size; i += total_threads) {
                current_password += alphabet[i];
                search_combinations(target, current_password, alphabet, alphabet_size, next_length, password_found, found_password, thread_id, total_threads);
                current_password.pop_back(); // Retirer le dernier caract�re pour passer � la combinaison suivante
            }
        }
    }
}



string BruteForce::force_brute_openmp(string target, string current_password) {
    const int num_threads = omp_get_max_threads();
    static const string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int alphabet_size = alphabet.size();

    string found_password = "";
    bool password_found = false;

    // Boucle pour g�n�rer les combinaisons de mots de passe de longueur 1 jusqu'� la longueur cible
    for (int length = 1; length <= target.length(); ++length) {
        cout << "Generating combinations for password length " << length << endl;

        // Calculer la taille du sous-espace de recherche pour chaque thread en fonction de la longueur actuelle du mot de passe
        int chunk_size = (alphabet_size + num_threads - 1) / num_threads;

#pragma omp parallel num_threads(num_threads)
        {
            int thread_id = omp_get_thread_num();

            // Calculer l'indice de d�but et de fin du sous-espace de recherche pour ce thread
            int start_idx = thread_id * chunk_size;
            int end_idx = min(start_idx + chunk_size, alphabet_size);

            cout << "Thread " << thread_id << " searching space [" << start_idx << ", " << end_idx - 1 << "] for password length " << length << endl;

            // Recherche dans le sous-espace allou� � ce thread
            for (int i = start_idx; i < end_idx; ++i) {
                if (!password_found && !found_password.empty()) {
                    continue; // Si un autre thread a d�j� trouv� le mot de passe, arr�ter la recherche
                }

                // G�n�rer le mot de passe potentiel en rempla�ant le dernier caract�re par un caract�re de l'alphabet
                string next_password = current_password;
                next_password[next_password.length() - 1] = alphabet[i];

                // V�rifier si le mot de passe potentiel est �gal � la cible
                if (next_password == target) {
                    // Mettre � jour 'found_password' en utilisant une section critique
#pragma omp critical
                    {
                        if (!password_found) {
                            found_password = next_password;
                            password_found = true; // Signaler aux autres threads que le mot de passe a �t� trouv�
                        }
                    }
                }
            }
        }

        // Mettre � jour le mot de passe actuel pour la prochaine it�ration
        if (length < target.length()) {
            current_password.push_back(alphabet[0]);
        }
    }

    // Retourner le mot de passe trouv� (vide si aucun mot de passe n'a �t� trouv�)
    return found_password;
}













