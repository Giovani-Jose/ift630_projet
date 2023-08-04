#include <iostream>
#include <string>
#include <omp.h>
#include "BruteForce.h"

using namespace std;



BruteForce::BruteForce() {
}

void BruteForce::search_combinations(const string& target, string& current_password, const string& alphabet, int alphabet_size, int length, bool& password_found, string& found_password, int thread_id, int total_threads) {
    if (password_found) {
        return; // Si un mot de passe a déjà été trouvé, sortir prématurément de la recherche
    }

    // Calculer l'indice de début et de fin du sous-espace de recherche pour ce thread
    int start_idx = thread_id;
    int end_idx = alphabet_size;
    int num_iterations = 1;
    for (int i = 1; i < length; ++i) {
        start_idx *= alphabet_size;
        end_idx *= alphabet_size;
        num_iterations *= alphabet_size;
    }

    // Afficher les informations de l'espace de recherche alloué à ce thread
#pragma omp critical
    {
        cout << "Thread " << thread_id << " searching space [" << start_idx << ", " << end_idx - 1 << "] for password length " << length << endl;
    }

    // Recherche dans le sous-espace alloué à ce thread
    for (int iteration = 0; iteration < num_iterations; ++iteration) {
        if (!password_found && !found_password.empty()) {
            // Si un autre thread a déjà trouvé le mot de passe, arrêter la recherche
            break;
        }

        // Générer le mot de passe potentiel en utilisant l'indice d'itération pour les caractères appropriés
        string next_password = current_password;
        int remaining_iteration = iteration + start_idx; // Ajouter l'indice de début pour que chaque thread commence à une position différente
        for (int j = length - 1; j >= 0; --j) {
            int char_idx = remaining_iteration % alphabet_size;
            remaining_iteration /= alphabet_size;
            next_password[j] = alphabet[char_idx];
        }

        // Afficher le mot de passe testé par ce thread
#pragma omp critical
        {
            cout << "Thread " << thread_id << ": Trying password: " << next_password << endl;
        }

        // Vérifier si le mot de passe potentiel est égal à la cible
        if (next_password == target) {
            // Mettre à jour 'found_password' en utilisant une section critique
#pragma omp critical
            {
                if (!password_found) {
                    found_password = next_password;
                    password_found = true; // Signaler aux autres threads que le mot de passe a été trouvé
                }
            }
        }
    }

    // Appel récursif pour la recherche des longueurs de mot de passe suivantes jusqu'à la longueur cible
    if (length < target.length()) {
        for (int next_length = length + 1; next_length <= target.length(); ++next_length) {
            for (int i = thread_id; i < alphabet_size; i += total_threads) {
                current_password += alphabet[i];
                search_combinations(target, current_password, alphabet, alphabet_size, next_length, password_found, found_password, thread_id, total_threads);
                current_password.pop_back(); // Retirer le dernier caractère pour passer à la combinaison suivante
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

    // Boucle pour générer les combinaisons de mots de passe de longueur 1 jusqu'à la longueur cible
    for (int length = 1; length <= target.length(); ++length) {
        cout << "Generating combinations for password length " << length << endl;

        // Calculer la taille du sous-espace de recherche pour chaque thread en fonction de la longueur actuelle du mot de passe
        int chunk_size = (alphabet_size + num_threads - 1) / num_threads;

#pragma omp parallel num_threads(num_threads)
        {
            int thread_id = omp_get_thread_num();

            // Calculer l'indice de début et de fin du sous-espace de recherche pour ce thread
            int start_idx = thread_id * chunk_size;
            int end_idx = min(start_idx + chunk_size, alphabet_size);

            cout << "Thread " << thread_id << " searching space [" << start_idx << ", " << end_idx - 1 << "] for password length " << length << endl;

            // Recherche dans le sous-espace alloué à ce thread
            for (int i = start_idx; i < end_idx; ++i) {
                if (!password_found && !found_password.empty()) {
                    continue; // Si un autre thread a déjà trouvé le mot de passe, arrêter la recherche
                }

                // Générer le mot de passe potentiel en remplaçant le dernier caractère par un caractère de l'alphabet
                string next_password = current_password;
                next_password[next_password.length() - 1] = alphabet[i];

                // Vérifier si le mot de passe potentiel est égal à la cible
                if (next_password == target) {
                    // Mettre à jour 'found_password' en utilisant une section critique
#pragma omp critical
                    {
                        if (!password_found) {
                            found_password = next_password;
                            password_found = true; // Signaler aux autres threads que le mot de passe a été trouvé
                        }
                    }
                }
            }
        }

        // Mettre à jour le mot de passe actuel pour la prochaine itération
        if (length < target.length()) {
            current_password.push_back(alphabet[0]);
        }
    }

    // Retourner le mot de passe trouvé (vide si aucun mot de passe n'a été trouvé)
    return found_password;
}













