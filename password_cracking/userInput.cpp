#include "userInput.h"  // Inclure le fichier header correspondant

UserInput::UserInput() {
}

UserInput::~UserInput() {
}

string UserInput::getPasswordFromUserInput()
{
	cout << "Entrez votre mot de passe :" << endl;
	cin >> motDePasseCible;

	return motDePasseCible;
}


