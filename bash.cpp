#include <iostream>
#include <string>
#include <cstdlib>
#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#endif

using namespace std;

int main(){
    string command;
    string user;
#if defined(__unix__) || defined(__APPLE__)
    if(getlogin() != nullptr){
        user = getlogin();
    } else {
        cout << "Enter username: ";
        getline(cin, user);
    }
#else 
    cout << "Enter username: ";
    getline(cin, user);
#endif
    const string red = "\033[31m";
    const string reset = "\033[0m";

    while(true){
        cout << red << user << reset << "@bash >";
        if(!getline(cin, command)) break;

        if(command == "exit") break;

        int result = system(command.c_str());
        if(result == -1){
            cout << "Error completeting command \n";
        }
    }
}