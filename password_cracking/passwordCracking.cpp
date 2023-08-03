
#include <iostream>
#include <mpi.h>
#include "userInput.h"
#include "Dictionnary.h"
#include "BruteForce.h"

using namespace std;

int main(int argc, char* argv[])
{
	// Initialize MPI
	MPI_Init(&argc, &argv);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//Entree du mot de passe

	UserInput userInput;
	string password;
	
	if (rank == 0) {
		// Process with rank 0 gets the password input
		password = userInput.getPasswordFromUserInput();
	}

	// Génération de mot de passe candidats

    //étape 1: Fouille de dictionnaire, le mot de passe est il dans le dictionnaire?
	/*Dictionnaire dictionnaire;
	if (dictionnaire.distributeThreadsForSearching(password))
	{
		cout << "le mot de passe a ete trouvee il se trouve dans le fichier " << dictionnaire.locatedPassword() << endl;
	}
	else
		cout << "le mot de passe n'a pas ete trouvee" << endl;*/

	//etape2: generer une liste de combinaison et tester avec chaque combinaison
	string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	size_t length = password.length();

	// Broadcast the input parameters to all processes
	MPI_Bcast(&length, 1, MPI_INT, 0, MPI_COMM_WORLD);
	int alphabetSize = alphabet.length();
	MPI_Bcast(&alphabetSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&password[0], length, MPI_CHAR, 0, MPI_COMM_WORLD);

	// Call the function on all processes
	BruteForce bf;
	string result = bf.generate_all_alphanumeric(length, password, alphabet);

	
	// Print or use the result on the root process
		if (rank == 0) {
			if (!result.empty()) {
				cout << "Password found: " << result << endl;
			}
			else {
				cout << "Password not found." << endl;
			}
		}

	MPI_Finalize();

	return 0;
}
