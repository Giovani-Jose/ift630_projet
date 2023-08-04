
#include <iostream>
#include "userInput.h"
#include "Dictionnary.h"
using namespace std;

int main()
{
	//Entree du mot de passe

	UserInput userInput;
	string password = userInput.getPasswordFromUserInput();

	// Génération de mot de passe candidats

	//étape 1: Fouille de dictionnaire, le mot de passe est il dans le dictionnaire?
	Dictionnaire dictionnaire;
	if (dictionnaire.distributeThreadsForSearching(password))
	{
		cout << "le mot de passe a ete trouvee il se trouve dans le fichier " << dictionnaire.locatedPassword();
	}
	else
		cout << "le mot de passe n'a pas ete trouvee" << endl;

	//etape2: generer une liste de combinaison et tester avec chaque combinaison


}
