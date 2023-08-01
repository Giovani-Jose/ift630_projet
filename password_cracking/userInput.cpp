#include "userInput.h"  // Inclure le fichier header correspondant

UserInput::UserInput() {
}

UserInput::~UserInput() {
}

void UserInput::getPasswordFromUserInput()
{
	cout << "Entrez votre mot de passe :" << endl;
	cin >> motDePasseCible;
}


