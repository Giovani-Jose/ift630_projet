#pragma once
#include "Dictionnary.h"
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

bool Dictionnaire::distributeThreadsForSearching(string password) {
    auto startingTime = system_clock::now();
    int threadNumber = 1;

    vector<thread> threads;
    for (int i = 1; i <= 11; i++) {
        threads.push_back(thread(&Dictionnaire::search, this, i, password,threadNumber));
        threadNumber++;
    }
    
    cout << "En recherche...." << endl;

    for (auto& t : threads) {
        t.join();
    }

    auto stopingTime = system_clock::now();

    auto duree = duration_cast<milliseconds>(startingTime - stopingTime);

    temps_exec = duree.count();

    return isPasswordFound;
}

string Dictionnaire::locatedPassword()
{
    return foundPasswordFileName;
}

void::Dictionnaire::showProgress(int num)
{
    if (!isPasswordFound)
        cout <<endl<< "Le thread numero " << num << " a terminee sa recherche et n'a pas trouvee le mot de passe" << endl;
    else
    {
        cout << endl << "le thread numero " << num << " a trouvee le mot de passe " << endl;
        return;
    }
}


void Dictionnaire::search(int numFile, string password,int threadNumber) {
    FILE* stream;
    string filename = to_string(numFile) + ".txt";

    // ouvre le fichier en mode lecture
    ifstream infile(filename);

    // vérifie si l'ouverture du fichier a réussi
    if (!infile) {
        cout << "Erreur lors de l'ouverture du fichier" << endl;
        return;
    }
    else {

        // lit le fichier ligne par ligne
        string line;
        while (getline(infile, line) && isPasswordFound == false) {

            if (line[line.length()] == 0)
                line = line.substr(0, line.size() - 1);

            //comparer le mot de passe avec chaque mot du dictionnaire

            if (line == password) {
                mtx.lock();
                if (searchingPassword == "") searchingPassword = line;
                isPasswordFound = true;
                shouldShowProgress = false;
                foundPasswordFileName = filename;
                showProgress(threadNumber);
                mtx.unlock();
                break;
            }
        
        }
        if(shouldShowProgress)showProgress(threadNumber);

        // ferme le fichier
        infile.close();
    }
}