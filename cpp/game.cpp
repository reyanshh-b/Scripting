#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <cstdlib>
#include <algorithm>
#include <cctype>
using namespace std;

class User {
public:
    int fuel;

    User(int initial_fuel) : fuel(initial_fuel), running(false) {}

    ~User() {
        stopFuelThread();
    }

    void startFuelThread() {
        if (fuelThread.joinable()) return; // already running

        running = true;
        fuelThread = thread(&User::consumeFuel, this);
    }

    void stopFuelThread() {
        running = false;
        if (fuelThread.joinable()) {
            fuelThread.join();
        }
    }

private:
    thread fuelThread;
    atomic<bool> running;

    void consumeFuel() {
        while (running) {
            if (fuel > 0) {
                fuel--;
                cout << fuel << " units of fuel remaining." << endl;
                cout << "\033[2K"; // Clear line
                cout << "\033[1A"; // Move cursor up
                cout << "\033[2K"; // Clear line again

            } else {
                cout << "Out of fuel!" << std::endl;
                cout << "\033[2K"; // Clear line
                cout << "\033[1A"; // Move cursor up
                cout << "\033[2K"; // Clear line again
            }
            this_thread::sleep_for(chrono::seconds(1));
        }
    }
};


int main(){
    system("clear");
    cout << "\033[32m" << endl; // Green output text
    vector<string> actions = {
        "turn on car",
        "turn off car",
        "look for oil",
        "list"
    };
    //create a car object
    User user(100);

    //color variables
    const string red = "\033[31m";
    const string reset = "\033[32m";
    const string yellow = "\033[33m";
    const string blue = "\033[34m";
    while(true){
        string action;
        cout << "Enter an action (list to see all actions): " << endl;
        getline(cin, action);

        if(rand() % 100 < 50){
            cout << red << "A random event has occurred!" << reset << endl;
            const int randEvent = rand() % 5; //numbers 0-4
            switch(randEvent){
                case 0:
                    cout << blue << "GAS LEAK! You lose 10 fuel!" << reset << endl;
                    user.fuel -= 10;
                    if(user.fuel < 0){
                        user.fuel = 0;
                    }
                    break;
                case 1:
                    cout << blue << "You found a fuel barrel! +10 fuel" << reset << endl;
                    user.fuel += 10;
                    if(user.fuel > 100){
                        user.fuel = 100;
                    }
                    break;
                case 2: {
                    string userInput;
                    cout << blue << "A traveler finds you and asks if you can give them some fuel [yes/no]" << reset << endl;
                    getline(cin, userInput);
                    const int randomChance = (rand() % 2) + 1; // numbers 1-2
                    transform(userInput.begin(), userInput.end(), userInput.begin(), ::tolower);
                    switch(randomChance){
                        case 1:
                            if(userInput == "yes"){
                                cout << blue << "The traveler thanks you and gives you a map to a hidden fuel stash! +20 fuel" << reset << endl;
                                user.fuel += 20;
                                if(user.fuel > 100){
                                    user.fuel = 100;
                                }
                            }else if (userInput == "no"){
                                cout << blue << "The traveller curses you and you lost half your fuel!" << reset << endl;
                                user.fuel /= 2;
                                if(user.fuel < 0){
                                    user.fuel = 0;
                                }
                            }
                            break;
                        case 2:
                            if(userInput == "yes"){
                                cout << blue << "The traveler turns out to be a bandit and steals a quarter of your fuel!" << reset << endl;
                                user.fuel -= user.fuel / 4;
                                if(user.fuel < 0){
                                    user.fuel = 0;
                                }
                            }else if (userInput == "no"){
                                cout << blue << "The traveler thanks you graciously and gives you a magic fuel can! No fuel is lost for 15 seconds!" << reset << endl;
                                user.stopFuelThread();
                                this_thread::sleep_for(chrono::seconds(15));
                                user.startFuelThread();
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                }
                case 3: {
                    const int randomSuperRareChance = rand() % 100;
                    if(randomSuperRareChance < 0.25){
                        cout << red << "A tree falls on your car and hits you square on the head, GAME OVER!" << reset << endl;
                    }
                    break;
                }
                case 4:
                    cout << blue << "You found a hidden fuel stash! +50 fuel" << reset << endl;
                    user.fuel += 50;
                    if(user.fuel > 100){
                        user.fuel = 100;
                    }
                    break;
                default:
                    break;
            }
        }

        for(const auto& act : actions){
            if(action == act){
                if (action == "list"){
                    system("clear");
                    //list all actions
                    cout << "Available actions:" << endl;
                    for(const auto& a : actions){
                        cout << "- " << a << endl;
                    }
                }else if (action == "turn on car"){
                    system("clear");
                    cout << red << "car sputters for a little but then runs" << reset << endl;
                    user.startFuelThread();
                }else if( action == "turn off car"){
                    system("clear");
                    cout << red << "car shuts off" << reset << endl;
                    user.stopFuelThread();
                }else if(action == "look for oil"){
                    system("clear");
                    //30% chance to find oil
                    int chance = rand() % 100;
                    if(chance < 30){
                        cout << yellow << "You found some oil!" << reset << endl;
                        user.fuel += 20;
                    }else{
                        cout << red << "You didn't find any oil." << reset << endl;
                    }
                }else if(action == "exit"){
                    user.stopFuelThread();
                    return 0;
                }
            }
        }
     }
}