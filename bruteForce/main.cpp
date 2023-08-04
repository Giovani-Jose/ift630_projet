#include <iostream>
#include <string>
#include <vector>
#include <mpi.h>
#include <omp.h>

const std::string ALPHABET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const int LONGUEUR_MAX_CHAINE = 8;

bool motDePasseTrouve = false;
bool afficherCombinaisons = false;

// Fonction pour générer les combinaisons de manière itérative
void genererCombinaisons(std::vector<std::string>& combinaisons, int longueurMax) {
    std::string actuelle = "";
    std::vector<int> indices(longueurMax, 0);

    while (!motDePasseTrouve) {
        actuelle = "";
        for (int index : indices) {
            actuelle += ALPHABET[index];
        }

        // Afficher les combinaisons testées par chaque thread si demandé
        if (afficherCombinaisons) {
#pragma omp critical
            {
                std::cout << "Thread " << omp_get_thread_num() << " a généré : " << actuelle << std::endl;
            }
        }

        // Vérifier si la cible a été trouvée
        if (actuelle == combinaisons[0]) {
#pragma omp critical
            {
                motDePasseTrouve = true;
                std::cout << "Thread " << omp_get_thread_num() << " a trouvé le mot de passe : " << actuelle << std::endl;
            }
        }

        // Incrémenter les indices pour générer la prochaine combinaison
        int i = longueurMax - 1;
        while (i >= 0 && ++indices[i] >= ALPHABET.length()) {
            indices[i] = 0;
            i--;
        }

        if (i < 0) {
            break; // Toutes les combinaisons ont été générées
        }
    }
}

int main(int argc, char** argv) {
    // Initialisation de MPI
    MPI_Init(&argc, &argv);

    int rang, taille;
    // Récupérer le rang du processus courant et le nombre total de processus
    MPI_Comm_rank(MPI_COMM_WORLD, &rang);
    MPI_Comm_size(MPI_COMM_WORLD, &taille);

    // Vérifier que le programme est exécuté avec un seul processus
    if (taille > 1) {
        if (rang == 0) {
            std::cout << "Erreur : Ce programme doit être exécuté avec un seul processus." << std::endl;
        }
        // Finaliser MPI et quitter le programme
        MPI_Finalize();
        return 0;
    }

    std::string motDePasseCible;
    if (rang == 0) {
        // Demander à l'utilisateur d'entrer le mot de passe cible
        std::cout << "Entrez le mot de passe à trouver : ";
        std::cin >> motDePasseCible;
    }

    std::vector<std::string> combinaisons;
    combinaisons.push_back(motDePasseCible);

    int nombreDeThreads;
    // Récupérer le nombre de threads disponibles
#pragma omp parallel
    {
#pragma omp master
        {
            nombreDeThreads = omp_get_num_threads();
        }
    }

    if (rang == 0) {
        // Afficher le nombre de threads utilisés
        std::cout << "Nombre de threads utilisés : " << nombreDeThreads << std::endl;
    }

    double debutChrono = MPI_Wtime();
    int longueurMax = std::min(LONGUEUR_MAX_CHAINE, static_cast<int>(motDePasseCible.length()));

    std::string afficherCombinaisonsChoix;
    if (rang == 0) {
        // Demander à l'utilisateur s'il veut afficher les combinaisons testées par chaque thread
        std::cout << "Voulez-vous afficher les combinaisons testées par chaque thread ? (o/n) : ";
        std::cin >> afficherCombinaisonsChoix;
        afficherCombinaisons = (afficherCombinaisonsChoix == "o" || afficherCombinaisonsChoix == "O");
    }

    // Exécution de la génération des combinaisons en parallèle
#pragma omp parallel
    {
        genererCombinaisons(combinaisons, longueurMax);
    }

    if (!motDePasseTrouve) {
        // Si le mot de passe n'est pas trouvé
        std::cout << "Mot de passe non trouvé..." << std::endl;
    }

    double finChrono = MPI_Wtime();

    if (rang == 0) {
        // Afficher le temps écoulé
        std::cout << "Temps écoulé : " << finChrono - debutChrono << " secondes" << std::endl;
    }

    // Finaliser MPI
    MPI_Finalize();
    return 0;
}