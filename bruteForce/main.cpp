#include <iostream>
#include <string>
#include <vector>
#include <mpi.h>
#include <omp.h>

const std::string ALPHABET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const int LONGUEUR_MAX_CHAINE = 8;

bool motDePasseTrouve = false;
bool afficherCombinaisons = false;

// Fonction pour g�n�rer les combinaisons de mani�re it�rative
void genererCombinaisons(std::vector<std::string>& combinaisons, int longueurMax) {
    std::string actuelle = "";
    std::vector<int> indices(longueurMax, 0);

    while (!motDePasseTrouve) {
        actuelle = "";
        for (int index : indices) {
            actuelle += ALPHABET[index];
        }

        // Afficher les combinaisons test�es par chaque thread si demand�
        if (afficherCombinaisons) {
#pragma omp critical
            {
                std::cout << "Thread " << omp_get_thread_num() << " a g�n�r� : " << actuelle << std::endl;
            }
        }

        // V�rifier si la cible a �t� trouv�e
        if (actuelle == combinaisons[0]) {
#pragma omp critical
            {
                motDePasseTrouve = true;
                std::cout << "Thread " << omp_get_thread_num() << " a trouv� le mot de passe : " << actuelle << std::endl;
            }
        }

        // Incr�menter les indices pour g�n�rer la prochaine combinaison
        int i = longueurMax - 1;
        while (i >= 0 && ++indices[i] >= ALPHABET.length()) {
            indices[i] = 0;
            i--;
        }

        if (i < 0) {
            break; // Toutes les combinaisons ont �t� g�n�r�es
        }
    }
}

int main(int argc, char** argv) {
    // Initialisation de MPI
    MPI_Init(&argc, &argv);

    int rang, taille;
    // R�cup�rer le rang du processus courant et le nombre total de processus
    MPI_Comm_rank(MPI_COMM_WORLD, &rang);
    MPI_Comm_size(MPI_COMM_WORLD, &taille);

    // V�rifier que le programme est ex�cut� avec un seul processus
    if (taille > 1) {
        if (rang == 0) {
            std::cout << "Erreur : Ce programme doit �tre ex�cut� avec un seul processus." << std::endl;
        }
        // Finaliser MPI et quitter le programme
        MPI_Finalize();
        return 0;
    }

    std::string motDePasseCible;
    if (rang == 0) {
        // Demander � l'utilisateur d'entrer le mot de passe cible
        std::cout << "Entrez le mot de passe � trouver : ";
        std::cin >> motDePasseCible;
    }

    std::vector<std::string> combinaisons;
    combinaisons.push_back(motDePasseCible);

    int nombreDeThreads;
    // R�cup�rer le nombre de threads disponibles
#pragma omp parallel
    {
#pragma omp master
        {
            nombreDeThreads = omp_get_num_threads();
        }
    }

    if (rang == 0) {
        // Afficher le nombre de threads utilis�s
        std::cout << "Nombre de threads utilis�s : " << nombreDeThreads << std::endl;
    }

    double debutChrono = MPI_Wtime();
    int longueurMax = std::min(LONGUEUR_MAX_CHAINE, static_cast<int>(motDePasseCible.length()));

    std::string afficherCombinaisonsChoix;
    if (rang == 0) {
        // Demander � l'utilisateur s'il veut afficher les combinaisons test�es par chaque thread
        std::cout << "Voulez-vous afficher les combinaisons test�es par chaque thread ? (o/n) : ";
        std::cin >> afficherCombinaisonsChoix;
        afficherCombinaisons = (afficherCombinaisonsChoix == "o" || afficherCombinaisonsChoix == "O");
    }

    // Ex�cution de la g�n�ration des combinaisons en parall�le
#pragma omp parallel
    {
        genererCombinaisons(combinaisons, longueurMax);
    }

    if (!motDePasseTrouve) {
        // Si le mot de passe n'est pas trouv�
        std::cout << "Mot de passe non trouv�..." << std::endl;
    }

    double finChrono = MPI_Wtime();

    if (rang == 0) {
        // Afficher le temps �coul�
        std::cout << "Temps �coul� : " << finChrono - debutChrono << " secondes" << std::endl;
    }

    // Finaliser MPI
    MPI_Finalize();
    return 0;
}