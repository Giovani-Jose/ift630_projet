#pragma once
#include <iostream>
#include <string>
using namespace std;


class UserInput {
private:
    int attributPrive;

public:
    UserInput();
    ~UserInput();
    void getPasswordFromUserInput();

private:
    string motDePasseCible;
};

