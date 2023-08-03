#include <mpi.h>
#include <iostream>
#include <string>
#include <chrono>
#include "BruteForce.h"

using namespace std;
using namespace std::chrono;

BruteForce::BruteForce() {
}

void BruteForce::generate_alphanumeric(int length, string passwd, string current, string alphabet, bool& passFoundAllComb) {
    
    if (current.length() == length || passFoundAllComb) {    
        
        if (passwd == current) {
            passFoundAllComb = true;
            mot = current;
        }
    }
    else {
        for (int i = 0; i < alphabet.length(); i++) {
            string new_current = current + alphabet[i];
            generate_alphanumeric(length, passwd, new_current, alphabet, passFoundAllComb);
        }
    }
}

string BruteForce::generate_all_alphanumeric(int length, string passwd, string alphabet) {
    int rank, size;
    bool passFoundAllComb = false;
    string localMot = "";

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int chunkSize = alphabet.length() / size;
    int startIndex = rank * chunkSize;
    int endIndex = (rank == size - 1) ? alphabet.length() : (rank + 1) * chunkSize;

    // Log the start of processing for each process
    MPI_Barrier(MPI_COMM_WORLD);
    cout << "Process " << rank << " started processing. Range: [" << startIndex << ", " << endIndex - 1 << "]" << endl;

    for (int i = startIndex; i < endIndex; i++) {
        string current = string(1, alphabet[i]);
        generate_alphanumeric(length, passwd, current, alphabet, passFoundAllComb);
        if (passFoundAllComb) {
            localMot = mot;
            break;
        }
    }

    // Prepare an array to hold the localMot string
    char* localMotArray = new char[localMot.size() + 1]; // +1 for null-terminator
    strcpy_s(localMotArray, localMot.size() + 1, localMot.c_str());

    // Gather all localMot arrays into a 2D array on the root process
    char* allLocalMot = nullptr;
    if (rank == 0) {
        allLocalMot = new char[size * (localMot.size() + 1)]; // Allocate space for all localMot strings
    }
    MPI_Gather(localMotArray, localMot.size() + 1, MPI_CHAR, allLocalMot, localMot.size() + 1, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Log the end of processing for each process
    MPI_Barrier(MPI_COMM_WORLD);
    cout << "Process " << rank << " finished processing." << endl;

    // On the root process, find the correct password from the gathered strings
    string globalMot;
    if (rank == 0) {
        for (int i = 0; i < size; i++) {
            if (allLocalMot[i * (localMot.size() + 1)] != '\0') {
                globalMot = &allLocalMot[i * (localMot.size() + 1)];
                break;
            }
        }
    }

    delete[] localMotArray;
    delete[] allLocalMot;

    return globalMot;
}



