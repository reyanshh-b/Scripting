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
    atomic<bool> travelerEventPending = false;
    string travelerPrompt;
    int travelerRandomChance = 0;

    User(int initial_fuel) : fuel(initial_fuel), running(false), randEventRunning(false) {}

    ~User() {
        stopFuelThread();
        stopRandEventThread();
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

    void startRandEventThread(){
        if (randEventThread.joinable()) return; // already running
        cout << "Random event thread started." << endl;
        randEventRunning = true;
        randEventThread = thread(&User::randEventTread, this);
    }

    void stopRandEventThread(){
        randEventRunning = false;
        if (randEventThread.joinable()) {
            randEventThread.join();
        }
    }

private:
    thread fuelThread;
    thread randEventThread;
    atomic<bool> running;
    atomic<bool> randEventRunning;

    void consumeFuel() {
        while (running) {
            if (fuel > 0) {
                fuel--;
                cout << fuel << " units of fuel remaining." << endl;
            } else {
                cout << "Out of fuel!" << std::endl;
                cout << "\033[2K"; // Clear line
                cout << "\033[1A"; // Move cursor up
                cout << "\033[2K"; // Clear line again
            }
            this_thread::sleep_for(chrono::seconds(1));
        }
    }

    void randEventTread(){
    while(randEventRunning){
        if(rand() % 100 < 30) { // 30% chance for a random event
            cout << "\033[31m" << "A random event has occurred!" << "\033[32m" << endl;
            const int randEvent = rand() % 5; //numbers 0-4
            switch(randEvent){
                case 0:
                    cout << "\033[34m" << "GAS LEAK! You lose 10 fuel!" << "\033[32m" << endl;
                    fuel -= 10;
                    if(fuel < 0){
                        fuel = 0;
                    }
                    break;
                case 1:
                    cout << "\033[34m" << "You found a fuel barrel! +10 fuel" << "\033[32m" << endl;
                    fuel += 10;
                    if(fuel > 100){
                        fuel = 100;
                    }
                    break;
                case 2: {
                    travelerEventPending = true;
                    travelerPrompt = "\033[34mA traveler finds you and asks if you can give them some fuel [yes/no]\033[32m";
                    travelerRandomChance = (rand() % 2) + 1;
                    break;
                }
                case 3: {
                    const int randomSuperRareChance = rand() % 100;
                    if(randomSuperRareChance < 1){
                        cout << "\033[31m" << "A tree falls on your car and hits you square on the head, GAME OVER!" << "\033[32m" << endl;
                    }
                    break;
                }
                case 4:
                    cout << "\033[34m" << "You found a hidden fuel stash! +50 fuel" << "\033[32m" << endl;
                    fuel += 50;
                    if(fuel > 100){
                        fuel = 100;
                    }
                    break;
                default:
                    break;
            }
        }
        this_thread::sleep_for(chrono::seconds(5));
    }
}
};


int main(){
    srand(time(nullptr));
    system("clear");
    cout << "\033[32m" << endl; // Green output text
    vector<string> actions = {
        "turn on car",
        "turn off car",
        "look for oil",
        "list"
    };
    User user(100);

    //color variables
    const string red = "\033[31m";
    const string reset = "\033[32m";
    const string yellow = "\033[33m";
    const string blue = "\033[34m";
    while(true){
        string action;
            if (user.travelerEventPending) {
            cout << user.travelerPrompt << endl; // Show prompt first
            getline(cin, action);                // Get user input
            transform(action.begin(), action.end(), action.begin(), ::tolower);

            if(user.travelerRandomChance == 1){
                if(action == "yes"){
                    cout << blue << "The traveler thanks you and gives you a map to a hidden fuel stash! +20 fuel" << reset << endl;
                    user.fuel += 20;
                    if(user.fuel > 100) user.fuel = 100;
                }else if (action == "no"){
                    cout << blue << "The traveller curses you and you lost half your fuel!" << reset << endl;
                    user.fuel /= 2;
                    if(user.fuel < 0) user.fuel = 0;
                }
            }else{
                if(action == "yes"){
                    cout << blue << "The traveler turns out to be a bandit and steals a quarter of your fuel!" << reset << endl;
                    user.fuel -= user.fuel / 4;
                    if(user.fuel < 0) user.fuel = 0;
                }else if (action == "no"){
                    cout << blue << "The traveler thanks you graciously and gives you a magic fuel can! No fuel is lost for 15 seconds!" << reset << endl;
                    user.stopFuelThread();
                    this_thread::sleep_for(chrono::seconds(15));
                    user.startFuelThread();
                }
            }
            user.travelerEventPending = false;
            continue; // skip normal action prompt
        }   
        cout << "Enter an action (list to see all actions): " << endl;
        getline(cin, action);

        for(const auto& act : actions){
            if(action == act){
                if (action == "list"){
                    system("clear");
                    cout << "Available actions:" << endl;
                    for(const auto& a : actions){
                        cout << "- " << a << endl;
                    }
                }else if (action == "turn on car"){
                    system("clear");
                    cout << red << "car sputters for a little but then runs" << reset << endl;
                    user.startFuelThread();
                    user.startRandEventThread();
                }else if( action == "turn off car"){
                    system("clear");
                    cout << red << "car shuts off" << reset << endl;
                    user.stopFuelThread();
                    user.stopRandEventThread();
                }else if(action == "look for oil"){
                    system("clear");
                    int chance = rand() % 100;
                    if(chance < 30){
                        cout << yellow << "You found some oil!" << reset << endl;
                        user.fuel += 20;
                    }else{
                        cout << red << "You didn't find any oil." << reset << endl;
                    }
                }else if(action == "exit"){
                    user.stopFuelThread();
                    user.stopRandEventThread();
                    return 0;
                }
            }
        }
     }
}