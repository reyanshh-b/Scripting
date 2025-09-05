#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>
#include <chrono>
using namespace std;

int main(){
    string test;
    getline(cin, test);
    this_thread::sleep_for(chrono::seconds(5));
    system("clear");
    getline(cin, test);
    this_thread::sleep_for(chrono::seconds(5));
}