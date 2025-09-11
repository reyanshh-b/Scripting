#include <iostream>
#include <string>
#include <cstdlib>
#include <thread>
#include <atomic>
#include <chrono>
#include <cctype>
#include <vector>
#include <ctime>
using namespace std;

class MagicAbility {
    public:
        string name;
        string description;
        int damage;
        double damageReduction; // For shield-type abilities
        bool isEvolved;
        bool canBeEvo;

        MagicAbility(const string& n, const string& desc, int dmg = 0, double reduction = 0.0, bool evolved = false, bool evolvable = false)
            : name(n), description(desc), damage(dmg), damageReduction(reduction), isEvolved(evolved), canBeEvo(evolvable) {}
};

struct Ability {
    string name;
    int damage;
    string description;
};

class Weapon{
    public:
        string name;
        string description;
        int damage;
        vector<Ability> abilities;
        Weapon(const string& n, const string& desc, int dmg, const vector<Ability>& abils)
            : name(n), description(desc), damage(dmg), abilities(abils) {}
};

bool isOnlyInt(const string& s) {
    if (s.empty()) return false;
    size_t start = 0;
    if (s[0] == '+' || s[0] == '-') {
        if (s.size() == 1) return false; // just '+' or '-'
        start = 1;
    }
    for (size_t i = start; i < s.size(); ++i) {
        if (!isdigit(s[i])) return false;
    }
    return true; // all digits (with optional sign)
}

std::atomic<int> playerHealth(100);
std::atomic<bool> gameRunning(true);

void healthMonitor() {
    while (gameRunning) {
        if (playerHealth <= 0) {
            cout << "\n\033[1;31mGAME OVER!\033[0m" << endl;
            gameRunning = false;
            exit(0); // forcefully exit the program
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
void useInventoryItems(vector<string>& plyrInventory, vector<MagicAbility>& playerMagics, const string& red, const string& green, const string& yellow, const string& blue, const string& reset){
    cout << yellow << "Enter the item number to use: " << reset << endl;
    string itemNumStr;
    getline(cin, itemNumStr);
    if(isOnlyInt(itemNumStr)){
        int itemNum = stoi(itemNumStr);
        if(itemNum < 1 || itemNum > static_cast<int>(plyrInventory.size())){
            cout << red << "Invalid item number." << reset << endl;
        }else if(plyrInventory[itemNum - 1] == "Health Potion"){
            playerHealth += 20;
            if(playerHealth > 100) playerHealth = 100;
            cout << green << "You used a Health Potion. Health +20" << reset << endl;
            cout << red << "Health: " << playerHealth << reset << endl;
            plyrInventory.erase(plyrInventory.begin() + itemNum - 1);
        }else if(plyrInventory[itemNum - 1] == "Weird Skull"){
            int randomSkullOutcome = rand() % 2;
            if(randomSkullOutcome == 0){
                cout << red << "You examine the skull, and at first nothing happens, but then a bright flash of light erupts from the skull, when you come back to your senses" << green << " , all wounds are healed" << reset << endl;
                playerHealth = 100;
                cout << red << "Health: " << playerHealth << reset << endl;
                plyrInventory.erase(plyrInventory.begin() + itemNum - 1);
            }else if(randomSkullOutcome == 1){
                cout << red << "You examine the skull, at first nothing happens but then the eyesockets glow a bright red color and a beam of light shines at you, " << red << "You lose half your health!" << reset << endl;
                playerHealth = playerHealth / 2;
                cout << red << "Health: " << playerHealth << reset << endl;
                plyrInventory.erase(plyrInventory.begin() + itemNum - 1);
            }   
        }else if(plyrInventory[itemNum - 1] == "Mysterious Item"){
            cout << green << "You hold the item firmly in your hand, and as it melts in your palm, with your body aborbing its energy, " << blue << "New magic unlocked: Shield (reduces damage by 20%, only used during battles)" << reset << endl;
            plyrInventory.erase(plyrInventory.begin() + itemNum - 1);
            playerMagics.push_back(MagicAbility(
                "Shield",
                "Reduces damage by 20 percent during battles",
                0,
                0.2,
                false,
                true
            ));
        }
    }
}


vector<string> preBattleSequence(vector<string>& plyrInventory, vector<MagicAbility>& playerMagics, const string& red, const string& green, const string& yellow, const string& blue, const string& cyan, const string& reset){
    cout << yellow << "Inventory:" << endl;
    if(plyrInventory.empty()){
        cout << red << "Your inventory is empty. Good Luck." << reset << endl;
        cout << yellow << endl;
        return {};
    }else{
        for(size_t i = 0; i < plyrInventory.size(); ++i){
            cout << blue << i+1 << ". " << plyrInventory[i] << endl;
        }
        vector<string> battleItems;
        while(true){ //for ITEMS, next while loop is for MAGIC
            cout << red << "Enter items to use during battle (say exit to stop selecting)" << reset << endl;
            string useItem;
            getline(cin, useItem);
            if(useItem == "exit"){
                cout << cyan << "You decide not to use any more items." << reset << endl;
                break;
            }else{
                //convert to int and use item
                int itemNum = stoi(useItem);
                if(itemNum < 1 || itemNum > static_cast<int>(plyrInventory.size())){
                    cout << red << "Invalid item number." << reset << endl;
                }else if(plyrInventory[itemNum - 1] == "Health Potion"){
                    //check number of health potions in user inventory
                    int potionCount = 0;
                    for(const string& item : plyrInventory){
                        if(item == "Health Potion"){
                            potionCount++;
                        }
                    }
                    //if user tried to put more potions than they have in battle items
                    int battlePotionCount = 0;
                    for(const string& item : battleItems){
                        if(item == "Health Potion"){
                            battlePotionCount++;
                        }
                    }
                    if(battlePotionCount >= potionCount){
                        cout << red << "You can only use " << potionCount << " Health Potion(s) during battle" << reset << endl;
                        continue;
                    }
                    battleItems.push_back("Health Potion");
                    cout << cyan << "Health Potion will be used during battle" << reset << endl;


                }else if(plyrInventory[itemNum - 1] == "Sword of Destiny"){
                    //check if already in battle items
                    bool alreadyInBattle = false;
                    for(const string& item : battleItems){
                        if(item == "Sword of Destiny"){
                            alreadyInBattle = true;
                            break;
                        }
                    }
                    if(alreadyInBattle){
                        cout << red << "You can only use one" << cyan << "Sword of Destiny" << red << "during battle" << reset << endl;
                        continue;
                    }
                    battleItems.push_back("Sword of Destiny");
                    cout << cyan << "Sword of Destiny will be used during battle" << reset << endl;
                }else{
                    cout << red << "Item cannot be used during battle" << reset << endl;
                }
            }
        }

        return battleItems;

    }
}



//function to create dynamic text that somes out letter by letter
void dynamicText(const string& text, int delayMs, const string& colorCode, bool nextLine) {
    for(char c : text) {
        cout << colorCode << c << flush; // Print each character with color
        this_thread::sleep_for(chrono::milliseconds(delayMs)); // Delay between characters
    }
    if(nextLine){
        cout << endl;
    }
}

int main() {
    srand(time(0));
    system("clear");
    //----------color ASCII---------- 
    const string red = "\033[1;31m"; 
    const string green = "\033[1;32m"; 
    const string yellow = "\033[1;33m"; 
    const string blue = "\033[1;34m"; 
    const string magenta = "\033[1;35m"; 
    const string cyan = "\033[1;36m"; 
    const string white = "\033[1;37m"; 
    const string reset = "\033[0m";
    const string darkRed = "\033[0;31m";
    //-----------Initizilize Game------------
    cout << red << "Hello Player, Provide a username to begin: " << reset << endl;
    string playerName;
    vector<string> plyrInventory;
    string lastRoom = "";
    string currentRoom = "";
    vector<MagicAbility> playerMagics;
    vector<Weapon> playerWeapons;

    getline(cin, playerName);

    if (isOnlyInt(playerName)) {
        cout << red << "Invalid name (cannot be only numbers or epty), defaulting to Player1" << reset << endl;
        playerName = "Player1";
    }else if(playerName.empty() || playerName == " ") {
        cout << red << "Invalid name (cannot be only numbers or epty), defaulting to Player1" << reset << endl;
        playerName = "Player1";
    }else{
        system("clear");
        cout << green << "Welcome, " << playerName << "!" << reset << endl;
    }

    std::thread monitorThread(healthMonitor);
    //-----------Game Start------------
    cout << yellow << "You're lost in a dungeon, with 3 doors infront of you" << endl;
    cout << "1. A door with a skull on it" << endl;
    cout << "2. A door with a heart on it" << endl;
    cout << "3. A door with a star on it" << reset << endl;
    cout << cyan << "Choose a door (1, 2, or 3): " << reset << endl;
    string plrChoice;
    getline(cin, plrChoice);
    vector<string> possibleOutcomesSkull = {
        "You enter the skull room but step over a tripmine, a dart shoots out and hits you in the leg! You lose 20 health.",
        "You enter the skull room and find a weird skull on a pedestal",
        "You enter the skull room and your vision gets blurry, you feel dizzy and fall to the ground, losing 10 health, you wake up 15 minutes later",
        "You enter the skull room and find a chest, you open it and find a health potion"
    };
    vector<string> possibleOutcomesHeart = {
        "You enter a room thats filled with heart decals on the wall, a faint perfume fills the air, you feel relaxed but then pass out, it turns out the perfume was laced with a sedative and you lose 15 health",
        "You enter the room and find a living room setup with a mysterious drink on the table",
        "You enter the room and a pink figure appears, it attacks you in a blink of an eye, you lose 25 health",
        "You enter a pink room -- Nothing happens"
    };
    vector<string> possibleOutcomesStar = {
        "You enter a room filled with stars on the wall, you feel a sense of calm wash over you, you gain 10 health",
        "You enter the room and find a telescope pointed at a window, you look through it and see a beautiful starry sky, you look at it long enough and a mysterious item appears in your inventory",
        "You enter the room and find a chest, you open it and find a health potion",
        "You enter the room and is blinded by a flash of light, you lose 5 health"
    };

    if(plrChoice == "1"){
        lastRoom = "Skull Room";
        cout << green << "Entering the " << red << "Skull Room" << reset << endl;
        int outcomeIndex = rand() % possibleOutcomesSkull.size();
        cout << magenta << possibleOutcomesSkull[outcomeIndex] << reset << endl;
        if(outcomeIndex == 0){
            playerHealth -= 20;
            cout << red << "Health: " << playerHealth << reset << endl;
        }else if(outcomeIndex == 1){
            cout << yellow << "Do you want to take the skull? (yes/no): " << reset << endl;
            string takeSkull;
            getline(cin, takeSkull);
            for(char& c : takeSkull) c = tolower(c);
            if(takeSkull == "yes"){
                plyrInventory.push_back("Weird Skull");
                cout << cyan << "Skull added to inventory" << reset << endl;
            }else if (takeSkull == "no"){
                cout << cyan << "You leave the skull alone" << reset << endl;
            }else{
                cout << red << "Invalid input, leaving skull alone" << reset << endl;
            }
        }else if (outcomeIndex == 2){
            playerHealth -= 10;
            cout << red << "Health: " << playerHealth << reset << endl;
        }else if (outcomeIndex == 3){
            cout << yellow << "Do you want to keep the potion? (yes/no): " << reset << endl;
            string takePotion;
            getline(cin, takePotion);
            for(char& c : takePotion) c = tolower(c);
            if(takePotion == "yes"){
                plyrInventory.push_back("Health Potion");
                cout << cyan << "Health Potion added to inventory" << reset << endl;
            }else if (takePotion == "no"){
                cout << cyan << "You leave the potion" << reset << endl;
            }else{
                cout << red << "Invalid input, leaving potion" << reset << endl;
            }
        }
    }else if (plrChoice == "2"){
        lastRoom = "Heart Room";
        cout << green << "Entering the " << red << "Heart Room" << reset << endl;
        int outcomeIndex = rand() % possibleOutcomesHeart.size();
        cout << magenta << possibleOutcomesHeart[outcomeIndex] << reset << endl;
        if(outcomeIndex == 0){
            playerHealth -= 15;
            cout << red << "Health: " << playerHealth << reset << endl;
        }else if(outcomeIndex == 1){
            cout << red << "Do you want to drink it? (yes/no): " << reset << endl;
            string drink;
            getline(cin, drink);
            for(char& c : drink) c = tolower(c);
            if(drink == "yes"){
                int drinkOutcome = rand() % 2;
                if(drinkOutcome == 0){
                    playerHealth += 20;
                    cout << green << "You feel rejuvenated! Health +20" << reset << endl;
                    if(playerHealth > 100) playerHealth = 100;
                    cout << red << "Health: " << playerHealth << reset << endl;
                }else{
                    playerHealth -= 20;
                    cout << red << "The drink was poisoned! Health -20" << reset << endl;
                    cout << red << "Health: " << playerHealth << reset << endl;
                }
            }else if (drink == "no"){
                cout << cyan << "You leave the drink alone" << reset << endl;
            }
        }else if (outcomeIndex == 2){
            playerHealth -= 25;
            cout << red << "Health: " << playerHealth << reset << endl;
        }
    }else if (plrChoice == "3"){
        lastRoom = "Star Room";
        cout << green << "Entering the " << yellow << "Star room" << reset << endl;
        int outcomeIndex = rand() % possibleOutcomesStar.size();
        cout << magenta << possibleOutcomesStar[outcomeIndex] << reset << endl;
        if(outcomeIndex == 0){
            playerHealth += 10;
            if(playerHealth > 100) playerHealth = 100;
            cout << red << "Health: " << playerHealth << reset << endl;
        }else if (outcomeIndex == 1){
            plyrInventory.push_back("Mysterious Item");
            cout << cyan << "Mysterious Item added to inventory" << reset << endl;
        }else if(outcomeIndex == 2){
            cout << yellow << "Do you want to keep the potion? (yes/no): " << reset << endl;
            string takePotion;
            getline(cin, takePotion);
            for(char& c : takePotion) c = tolower(c);
            if(takePotion == "yes"){
                plyrInventory.push_back("Health Potion");
                cout << cyan << "Health Potion added to inventory" << reset << endl;
            }else if (takePotion == "no"){
                cout << cyan << "You leave the potion" << reset << endl;
            }else{
                cout << red << "Invalid input, leaving potion" << reset << endl;
            }
        }else if (outcomeIndex == 3){
            playerHealth -= 5;
            cout << red << "Health: " << playerHealth << reset << endl;
        }
    }else{
        cout << red << "Invalid choice, you stumble and fall into the void, losing all your health!" << reset << endl;
        playerHealth = 0;
    }

    
    cout << yellow << "You leave the room and find yourself in a long hallway with two paths:" << endl;
    currentRoom = "Hallway";
    while(true){
        cout << yellow << endl;
        cout << "What do you do?" << endl;
        cout << "1. Check/Use inventory items" << endl;
        cout << "2. Inspect stats" << endl;
        cout << "3. Continue down the hallway" << reset << endl;
        string hallwayChoice;
        getline(cin, hallwayChoice);
        if(hallwayChoice == "1"){
            cout << endl;
            cout << yellow << "Inventory:" << endl;
            if(plyrInventory.empty()){
                cout << red << "Your inventory is empty." << reset << endl;
                cout << yellow << endl;
            }else{
                for(size_t i = 0; i < plyrInventory.size(); ++i){
                    cout << blue << i+1 << ". " << plyrInventory[i] << endl;
                }
                cout << red << "Do you want to use an item? (yes/no): " << reset << endl;
                string useItem;
                getline(cin, useItem);
                if(useItem == "no"){
                    cout << cyan << "You decide not to use any items." << reset << endl;
                }else if(useItem == "yes"){
                    useInventoryItems(plyrInventory, playerMagics, red, green, yellow, blue, reset);
                }
            }
        }else if(hallwayChoice == "2"){
            cout << endl;
            cout << green << "Health: " << playerHealth << endl;
            cout << blue << "Inventory items: " << plyrInventory.size() << endl;
            cout << cyan <<"Current Room: " << currentRoom << yellow << endl;
            cout << endl;
        }else if (hallwayChoice == "3"){
            bool hasKey = false;
            break;
        }
    }
    cout << magenta << "You walk down the hallway reluctantly and find two large wooden doors." << reset << endl;
    vector<string> itemsInBattle;
    while(true){
        cout << yellow << "Do you want to go through the left door or the right door? (left/right): " << reset << endl;
        string doorChoice;
        getline(cin, doorChoice);
        if(doorChoice == "left"){
            cout << red << "Door is locked! Maybe the key is in the other room?" << reset << endl;
        }else if (doorChoice == "right"){
            //cout << red << "An eerie vibe comes from the room, as you walk in, you see a silver blue chest infront of you, when you open it," << blue << " you find a sword emitting a powerful aura from it, you reach out to take it and feel a sensation flow through your arms." << reset << endl;
            dynamicText("An eerie vibe comes from the room, as you walk in, you see a silver blue chest infront of you, when you open it,", 100, red, false);
            dynamicText(" you find a sword emitting a powerful aura from it, you reach out to take it and feel a sensation flow through your arms.", 100, blue, true);
            playerWeapons.push_back(Weapon(
                "Sword of Destiny",
                "Sword obtained in Lord Exodius's room",
                25,
                {
                    {"Uppercut", 25, "A swift slice upwards, striking enemy from the chin up"},
                    {"Heavy Slash", 15, "You grab the sword by its edge for maximum leverage and use all your might to swing as powerful as you can"},
                }
            ));
            plyrInventory.push_back("Sword of Destiny");
            dynamicText("Sword of Destiny added to inventory", 35, cyan, true);
            dynamicText("You continue walking down the room which gets darker and darker as you get futhur from the entrance, which has now disapeared, a deep voice fills the room", 100, red, true);
            dynamicText("[???]: I've been waiting for you... ", 200, green, false);
            dynamicText(playerName, 200, red, true);
            dynamicText("The ground shakes at the boom of his voice", 10, red, true);
            dynamicText("[You]: Who... who are you?", 150, blue, true);
            dynamicText(
                "[???]: I am Lord Exodious, you enter my domain, now you'll never leave.",
                90,
                green,
                true
            );
            dynamicText("A giant figure materializes infront of you, towering over you, his eyes glow a bright red color, and his armor is black as night", 100, red, true);
            dynamicText("[Lord Exodius]: You entered my domain, now you'll never leave!", 175, darkRed, true);

            //intialize pre-battle sequence
            itemsInBattle = preBattleSequence(plyrInventory, playerMagics, red, green, yellow, blue, cyan, reset);
            cout << cyan << "Magics will automatically be equipped in battle" << endl;
            cout << magenta << "Beginning battle..." << reset << endl;

            break;
        }
    }

    //boss battle

    system("clear"); //clear terminal
    int exodiusHealth = 200;
     while(true){ //boss battle
        cout << darkRed << "Lord Exodius Health: " << exodiusHealth << endl;
        cout << blue << "Your health: " << playerHealth << endl;
        cout << reset << "-----------------------------------" << endl;
        cout << yellow << "Current items in use:" << endl;
        cout << blue;  
        for(size_t i = 0; i < itemsInBattle.size(); ++i){
            cout << i+1 << ". " << itemsInBattle[i] << endl;
        }
        cout << reset << "-----------------------------------" << endl;
        cout << cyan << "Magic Abilities: " << endl;
        cout << endl;
        string input;
        int moveList = 0;
        int chosenMove = 0;
        vector<pair<int, string>> chosenSkills;
        for(const auto& magic: playerMagics){
            cout << magic.name << ":" << endl;
            if(magic.damageReduction > 0.0){
                moveList++;
                cout << moveList << ". Damage reduction: " << magic.damageReduction << endl;
                chosenSkills.push_back({moveList, magic.name});
            }
            if(magic.damage > 0){
                moveList++;
                cout << moveList << ". Damage: " << magic.damage << endl;
                chosenSkills.push_back({moveList, magic.name});
            }
            cout << " > Description: " << magic.description << endl;
        }

        cout << blue << "------------WEAPONS--------------" << endl;

        //list abilities for weapons
        for(const auto& weapon : playerWeapons){
            cout << cyan << weapon.name << endl;
            for(const auto& ability : weapon.abilities){
                moveList++;
                cout << blue << moveList << ". " << ability.name << " " << ability.damage << " damage : " << ability.description << endl;
                chosenSkills.push_back({moveList, ability.name});
            }
            cout << endl;
        }

        cout << green << "Enter a move you want to use: " << endl;

        getline(cin, input);
        try{
            chosenMove = stoi(input);
        }catch(const exception& e){
            cout << "Invalid input, enter a valid number" << endl;
            continue;
        }
        cout << darkRed << "Chosen move: " << chosenSkills[chosenMove - 1].second << endl;

        break;

        if(exodiusHealth <= 0){ //when exodius is dead
            dynamicText("LORD EXODIUS DEFEATED", 60, green, true);
            break;
        }
    }
    
    gameRunning = false;      // Tell the monitor thread to stop
    monitorThread.join();     // Wait for it to finish
    return 0;
}
