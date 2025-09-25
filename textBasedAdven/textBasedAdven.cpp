#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <thread>
#include <atomic>
#include <chrono>
#include <cctype>
#include <vector>
#include <ctime>
using namespace std;
struct BossAttack
{
    string name;
    int damage;
    string description;
};
struct Boss
{
    string name;
    int health;
    vector<BossAttack> attacks;
    vector<string> voicelines;
};
struct Ability
{
    string name;
    int damage;
    string description;
    int removeHealth;
};
struct MagicAbilitySkill
{
    string name;
    int damage;
    string description;
    int removeHP;
    float damageReduction;
    bool isBinding;
};
struct Armor{
    string name;
    string description;
    double damageReduction;
    Armor(const string &n, const string &desc, double reduction)
        : name(n), description(desc), damageReduction(reduction) {}
    
};
struct Skill
{
    string name;
    int damage;
    int id;
    double damageReduction;
    int healthRemove;
    bool isBinding;
};

class MagicAbility
{
public:
    string name;
    string description;
    int damage;
    double damageReduction; // For shield-type abilities
    bool isEvolved;
    bool canBeEvo;
    vector<MagicAbilitySkill> abilities;

    MagicAbility(const string &n, const string &desc, int dmg = 0, double reduction = 0.0, bool evolved = false, bool evolvable = false, const vector<MagicAbilitySkill> &abils = {})
        : name(n), description(desc), damage(dmg), damageReduction(reduction), isEvolved(evolved), canBeEvo(evolvable), abilities(abils) {}
};

class Weapon
{
public:
    string name;
    string description;
    int damage;
    vector<Ability> abilities;
    Weapon(const string &n, const string &desc, int dmg, const vector<Ability> &abils)
        : name(n), description(desc), damage(dmg), abilities(abils) {}
};

bool isOnlyInt(const string &s)
{
    if (s.empty())
        return false;
    size_t start = 0;
    if (s[0] == '+' || s[0] == '-')
    {
        if (s.size() == 1)
            return false; // just '+' or '-'
        start = 1;
    }
    for (size_t i = start; i < s.size(); ++i)
    {
        if (!isdigit(s[i]))
            return false;
    }
    return true; // all digits (with optional sign)
}

std::atomic<int> playerHealth(100);
std::atomic<bool> gameRunning(true);

void healthMonitor()
{
    while (gameRunning)
    {
        if (playerHealth <= 0)
        {
            cout << "\n\033[1;31mGAME OVER!\033[0m" << endl;
            gameRunning = false;
            exit(0); // forcefully exit the program
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
void useInventoryItems(vector<string> &plyrInventory, vector<MagicAbility> &playerMagics, const string &red, const string &green, const string &yellow, const string &blue, const string &reset)
{
    cout << yellow << "Enter the item number to use: " << reset << endl;
    string itemNumStr;
    getline(cin, itemNumStr);
    if (isOnlyInt(itemNumStr))
    {
        int itemNum = stoi(itemNumStr);
        if (itemNum < 1 || itemNum > static_cast<int>(plyrInventory.size()))
        {
            cout << red << "Invalid item number." << reset << endl;
        }
        else if (plyrInventory[itemNum - 1] == "Health Potion")
        {
            playerHealth += 20;
            if (playerHealth > 100)
                playerHealth = 100;
            cout << green << "You used a Health Potion. Health +20" << reset << endl;
            cout << red << "Health: " << playerHealth << reset << endl;
            plyrInventory.erase(plyrInventory.begin() + itemNum - 1);
        }
        else if (plyrInventory[itemNum - 1] == "Weird Skull")
        {
            int randomSkullOutcome = rand() % 2;
            if (randomSkullOutcome == 0)
            {
                cout << red << "You examine the skull, and at first nothing happens, but then a bright flash of light erupts from the skull, when you come back to your senses" << green << " , all wounds are healed" << reset << endl;
                playerHealth = 100;
                cout << red << "Health: " << playerHealth << reset << endl;
                plyrInventory.erase(plyrInventory.begin() + itemNum - 1);
            }
            else if (randomSkullOutcome == 1)
            {
                cout << red << "You examine the skull, at first nothing happens but then the eyesockets glow a bright red color and a beam of light shines at you, " << red << "You lose half your health!" << reset << endl;
                playerHealth = playerHealth / 2;
                cout << red << "Health: " << playerHealth << reset << endl;
                plyrInventory.erase(plyrInventory.begin() + itemNum - 1);
            }
        }
        else if (plyrInventory[itemNum - 1] == "Mysterious Item")
        {
            cout << green << "You hold the item firmly in your hand, and as it melts in your palm, with your body aborbing its energy, " << blue << "New magic unlocked: Shield (reduces damage by 40%, only used during battles)" << reset << endl;
            plyrInventory.erase(plyrInventory.begin() + itemNum - 1);
            playerMagics.push_back(MagicAbility(
                "Shield",
                "Reduces damage by 40 percent during battles",
                0,
                0.4,
                false,
                true,
                {{"Shield", 0, "Reduces damage by 40 percent during battles", false, false}}));
        }
    }
}

vector<string> preBattleSequence(vector<string> &plyrInventory, vector<MagicAbility> &playerMagics, const string &red, const string &green, const string &yellow, const string &blue, const string &cyan, const string &reset, vector<Armor> &playerArmors)
{
    cout << yellow << "Inventory:" << endl;
    if (plyrInventory.empty())
    {
        cout << red << "Your inventory is empty. Good Luck." << reset << endl;
        cout << yellow << endl;
        return {};
    }
    else
    {
        for (size_t i = 0; i < plyrInventory.size(); ++i)
        {
            cout << blue << i + 1 << ". " << plyrInventory[i] << endl;
        }
        vector<string> battleItems;
        while (true)
        { // for ITEMS, next while loop is for MAGIC
            cout << red << "Enter items to use during battle (say exit to stop selecting or continue to boss battle)" << reset << endl;
            string useItem;
            getline(cin, useItem);
            if (useItem == "exit"){
                cout << cyan << "You decide not to use any more items." << reset << endl;
                break;
            }
            else{
                // convert to int and use item
                int itemNum;
                try{
                    itemNum = stoi(useItem);
                }catch(const exception &e){
                    cout << red << "Invalid input, enter a valid number or enter 'exit' to exit the prompt" << reset << endl;
                    continue;
                }
                if (itemNum < 1 || itemNum > static_cast<int>(plyrInventory.size()))
                {
                    cout << red << "Invalid item number." << reset << endl;
                }
                else if (plyrInventory[itemNum - 1] == "Health Potion")
                {
                    // check number of health potions in user inventory
                    int potionCount = 0;
                    for (const string &item : plyrInventory)
                    {
                        if (item == "Health Potion")
                        {
                            potionCount++;
                        }
                    }
                    // if user tried to put more potions than they have in battle items
                    int battlePotionCount = 0;
                    for (const string &item : battleItems)
                    {
                        if (item == "Health Potion")
                        {
                            battlePotionCount++;
                        }
                    }
                    if (battlePotionCount >= potionCount)
                    {
                        cout << red << "You can only use " << potionCount << " Health Potion(s) during battle" << reset << endl;
                        continue;
                    }
                    battleItems.push_back("Health Potion");
                    cout << cyan << "Health Potion will be used during battle" << reset << endl;
                }
                else if (plyrInventory[itemNum - 1] == "Sword of Destiny")
                {
                    // check if already in battle items
                    bool alreadyInBattle = false;
                    for (const string &item : battleItems)
                    {
                        if (item == "Sword of Destiny")
                        {
                            alreadyInBattle = true;
                            break;
                        }
                    }
                    if (alreadyInBattle)
                    {
                        cout << red << "You can only use one " << cyan << "Sword of Destiny" << red << " during battle" << reset << endl;
                        continue;
                    }
                    battleItems.push_back("Sword of Destiny");
                    cout << cyan << "Sword of Destiny will be used during battle" << reset << endl;
                }else if(plyrInventory[itemNum - 1] == "Armor of Exodius"){
                    bool alreadyInBattle = false;
                    for(const string &item : battleItems){
                        if(item == "Armor of Exodius"){
                            alreadyInBattle = true;
                            break;
                        }
                    }
                    if(alreadyInBattle){
                        cout << red << "You can only use one" << cyan << "Armor of Exodius" << red << "during battle" << reset << endl;
                        continue;
                    }
                    battleItems.push_back("Armor of Exodius");
                    cout << cyan << "Armor of Exodius will be used during battle" << reset << endl;
                }
                else
                {
                    cout << red << "Item cannot be used during battle" << reset << endl;
                }
            }
        }

        return battleItems;
    }
}

// function to create dynamic text that somes out letter by letter
void dynamicText(const string &text, int delayMs, const string &colorCode, bool nextLine)
{
    for (char c : text)
    {
        cout << colorCode << c << flush;                       // Print each character with color
        this_thread::sleep_for(chrono::milliseconds(delayMs)); // Delay between characters
    }
    if (nextLine)
    {
        cout << endl;
    }
}

void bossBattle(
    Boss &boss,
    vector<Weapon> &playerWeapons,
    vector<MagicAbility> &playerMagics,
    vector<string> &itemsInBattle,
    vector<string> &plyrInventory,
    vector<Armor> &playerArmors,
    int playerHealth,
    bool &hasPotion,
    const string &red, const string &green, const string &yellow, const string &blue, const string &magenta, const string &cyan, const string &white, const string &reset, const string &darkRed, // colors
    bool &isDead
)
{
    while (boss.health > 0 && playerHealth > 0)
    {
        // display health of exodius and player
        cout << red << "-----------------------------------" << endl;
        cout << darkRed << boss.name << " Health: " << boss.health << endl;
        cout << blue << "Your health: " << playerHealth << endl;
        cout << reset << "-----------------------------------" << endl;
        cout << yellow << "Current items in use:" << endl;
        cout << blue;
        for (size_t i = 0; i < itemsInBattle.size(); ++i)
        {
            cout << i + 1 << ". " << itemsInBattle[i] << endl;
        }
        cout << reset << "-----------------------------------" << endl;
        cout << cyan << "Magic Abilities: " << endl;
        cout << endl;
        string input;
        int moveList = 0;
        int chosenMove = 0;
        vector<Skill> chosenSkills;
        for (const auto &magic : playerMagics)
        {
            cout << magic.name << ":" << endl;
            if (magic.damageReduction > 0.0)
            {
                moveList++;
                cout << moveList << ". Damage reduction: " << magic.damageReduction << endl;
                chosenSkills.push_back({magic.name, magic.damage, moveList, magic.damageReduction, false});
            }
            if (magic.damage > 0)
            {
                moveList++;
                cout << moveList << ". Damage: " << magic.damage << endl;
                chosenSkills.push_back({magic.name,
                                        magic.damage,
                                        moveList,
                                        0.0,
                                        false});
            }
            cout << " > Description: " << magic.description << endl;
        }

        cout << blue << "------------WEAPONS--------------" << endl;

        // list abilities for weapons
        for (const auto &weapon : playerWeapons)
        {
            if(find(itemsInBattle.begin(), itemsInBattle.end(), weapon.name) != itemsInBattle.end()){
                cout << cyan << weapon.name << endl;
                for (const auto &ability : weapon.abilities)
                {
                    moveList++;
                    cout << blue << moveList << ". " << ability.name << " " << ability.damage << " damage : " << ability.description << endl;
                    chosenSkills.push_back({ability.name, ability.damage, moveList, 0.0, false});
                }
                cout << endl;
            }
        }

        cout << blue << "------------ARMOR--------------" << endl;
        for(const auto& armor : playerArmors){
            if(find(itemsInBattle.begin(), itemsInBattle.end(), armor.name) != itemsInBattle.end()){
                cout << " - " << armor.name << ": " << armor.description << " (Damage Reduction: " << armor.damageReduction * 100 << "%)" << reset << endl;
            }
            cout << endl;
        }

        double totalArmorReduction = 0.0;
        for (const auto &armor : playerArmors){
            totalArmorReduction += armor.damageReduction;
        }
        

        cout << green << "Enter the move id (number before the move) you want to use: " << endl;

        getline(cin, input);
        try
        {
            chosenMove = stoi(input);
        }
        catch (const exception &e)
        {
            cout << "Invalid input, enter a valid number" << endl;
            continue;
        }
        cout << darkRed << "Chosen move: " << chosenSkills[chosenMove - 1].name << endl;
        int damage = chosenSkills[chosenMove - 1].damage;
        double damageReduction = 0.0;
        if (chosenSkills[chosenMove - 1].damageReduction > 0.0)
        {
            damageReduction = chosenSkills[chosenMove - 1].damageReduction;
        }

        this_thread::sleep_for(chrono::milliseconds(250));

        cout << darkRed << "You attempt to use " << chosenSkills[chosenMove - 1].name << " on " << boss.name << " ..." << reset << endl;

        if (damage > 0 && damageReduction == 0.0)
        { // only damage
            // 50% chance attack works or fails
            int randChance = rand() % 10;
            if (randChance != 0)
            {
                // rand crit hit 20% chance
                int critChance = rand() % 5;
                if (critChance == 0)
                {
                    string textOutput = "CRITICAL HIT! " + chosenSkills[chosenMove - 1].name + " hits " + boss.name + " for " + to_string(damage + (damage / 2)) + " damage!";
                    dynamicText(textOutput, 50, darkRed, true);
                    boss.health -= damage + (damage / 2);
                }
                else
                {
                    string textOutput = chosenSkills[chosenMove - 1].name + " hits " + boss.name + " for " + to_string(damage) + " damage!";
                    dynamicText(textOutput, 50, darkRed, true);
                    boss.health -= damage;
                }
            }
            else
            {
                string textOutput = chosenSkills[chosenMove - 1].name + " missed! 0 damage dealt!";
                dynamicText(textOutput, 50, red, true);
            }
        }
        else if (damageReduction > 0.0 && damage == 0)
        { // only damage reduction
            dynamicText("You brace yourself...", 50, cyan, true);
        }

        if (boss.health <= 0)
        {
            break;
        }

        int voiceLineIndex = rand() % boss.voicelines.size();
        dynamicText(boss.voicelines[voiceLineIndex], 75, darkRed, true);

        int attackIndex = rand() % boss.attacks.size();
        const BossAttack &attack = boss.attacks[attackIndex];
        dynamicText(boss.name + " uses " + attack.name + " " + attack.description, 50, darkRed, true);
        int damageTaken = attack.damage;
        damageTaken -= static_cast<int>(attack.damage * (damageReduction * totalArmorReduction));
        if(damageTaken < 0) damageTaken = 0;
        playerHealth -= damageTaken;
        cout << red << "You lose " << damageTaken << " health!" << reset << endl;

        // act player if they want to use potion
        string usePotion;
        if (hasPotion)
        {
            // search for potion in battle items
            for (const string &item : itemsInBattle)
            {
                if (item == "Health Potion")
                {
                    cout << yellow << "Do you want to use a Health Potion? (yes/no): " << reset << endl;
                    getline(cin, usePotion);
                    for (char &c : usePotion)
                        c = tolower(c); // convert to lowercase
                    break;
                }
            }
            if (usePotion == "yes")
            {
                playerHealth += 20;
                if (playerHealth > 100)
                    playerHealth = 100;
                cout << green << "Potion used! Health +20" << reset << endl;
                // remove one potion from battle items and inventory
                for (auto it = itemsInBattle.begin(); it != itemsInBattle.end(); ++it)
                {
                    if (*it == "Health Potion")
                    {
                        itemsInBattle.erase(it);
                        auto invIt = find(plyrInventory.begin(), plyrInventory.end(), "Health Potion");
                        if (invIt != plyrInventory.end())
                        {
                            plyrInventory.erase(invIt);
                        }
                        break;
                    }
                }
            }
        }
    }

    
    if(playerHealth <= 0){
        isDead = true;
        playerHealth = 0;
    }
}

int main()
{
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
    vector<string> potions;
    vector<Armor> playerArmors;
    bool isDead = false;
    bool hasPotion = false;

    getline(cin, playerName);

    if (isOnlyInt(playerName))
    {
        cout << red << "Invalid name (cannot be only numbers or epty), defaulting to Player" << reset << endl;
        playerName = "Player";
    }
    else if (playerName.empty() || playerName == " ")
    {
        cout << red << "Invalid name (cannot be only numbers or epty), defaulting to Player" << reset << endl;
        playerName = "Player";
    }
    else
    {
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
        "You enter the skull room and find a chest, you open it and find a health potion"};
    vector<string> possibleOutcomesHeart = {
        "You enter a room thats filled with heart decals on the wall, a faint perfume fills the air, you feel relaxed but then pass out, it turns out the perfume was laced with a sedative and you lose 15 health",
        "You enter the room and find a living room setup with a mysterious drink on the table",
        "You enter the room and a pink figure appears, it attacks you in a blink of an eye, you lose 25 health",
        "You enter a pink room -- Nothing happens"};
    vector<string> possibleOutcomesStar = {
        "You enter a room filled with stars on the wall, you feel a sense of calm wash over you, you gain 10 health",
        "You enter the room and find a telescope pointed at a window, you look through it and see a beautiful starry sky, you look at it long enough and a mysterious item appears in your inventory",
        "You enter the room and find a chest, you open it and find a health potion",
        "You enter the room and is blinded by a flash of light, you lose 5 health"};

    if (plrChoice == "1")
    {
        lastRoom = "Skull Room";
        cout << green << "Entering the " << red << "Skull Room" << reset << endl;
        int outcomeIndex = rand() % possibleOutcomesSkull.size();
        cout << magenta << possibleOutcomesSkull[outcomeIndex] << reset << endl;
        if (outcomeIndex == 0)
        {
            playerHealth -= 20;
            cout << red << "Health: " << playerHealth << reset << endl;
        }
        else if (outcomeIndex == 1)
        {
            cout << yellow << "Do you want to take the skull? (yes/no): " << reset << endl;
            string takeSkull;
            getline(cin, takeSkull);
            for (char &c : takeSkull)
                c = tolower(c);
            if (takeSkull == "yes")
            {
                plyrInventory.push_back("Weird Skull");
                cout << cyan << "Skull added to inventory" << reset << endl;
            }
            else if (takeSkull == "no")
            {
                cout << cyan << "You leave the skull alone" << reset << endl;
            }
            else
            {
                cout << red << "Invalid input, leaving skull alone" << reset << endl;
            }
        }
        else if (outcomeIndex == 2)
        {
            playerHealth -= 10;
            cout << red << "Health: " << playerHealth << reset << endl;
        }
        else if (outcomeIndex == 3)
        {
            cout << yellow << "Do you want to keep the potion? (yes/no): " << reset << endl;
            string takePotion;
            getline(cin, takePotion);
            for (char &c : takePotion)
                c = tolower(c);
            if (takePotion == "yes")
            {
                plyrInventory.push_back("Health Potion");
                hasPotion = true;
                potions.push_back("Health Potion");
                cout << cyan << "Health Potion added to inventory" << reset << endl;
            }
            else if (takePotion == "no")
            {
                cout << cyan << "You leave the potion" << reset << endl;
            }
            else
            {
                cout << red << "Invalid input, leaving potion" << reset << endl;
            }
        }
    }
    else if (plrChoice == "2")
    {
        lastRoom = "Heart Room";
        cout << green << "Entering the " << red << "Heart Room" << reset << endl;
        int outcomeIndex = rand() % possibleOutcomesHeart.size();
        cout << magenta << possibleOutcomesHeart[outcomeIndex] << reset << endl;
        if (outcomeIndex == 0)
        {
            playerHealth -= 15;
            cout << red << "Health: " << playerHealth << reset << endl;
        }
        else if (outcomeIndex == 1)
        {
            cout << red << "Do you want to drink it? (yes/no): " << reset << endl;
            string drink;
            getline(cin, drink);
            for (char &c : drink)
                c = tolower(c);
            if (drink == "yes")
            {
                int drinkOutcome = rand() % 2;
                if (drinkOutcome == 0)
                {
                    playerHealth += 20;
                    cout << green << "You feel rejuvenated! Health +20" << reset << endl;
                    if (playerHealth > 100)
                        playerHealth = 100;
                    cout << red << "Health: " << playerHealth << reset << endl;
                }
                else
                {
                    playerHealth -= 20;
                    cout << red << "The drink was poisoned! Health -20" << reset << endl;
                    cout << red << "Health: " << playerHealth << reset << endl;
                }
            }
            else if (drink == "no")
            {
                cout << cyan << "You leave the drink alone" << reset << endl;
            }
        }
        else if (outcomeIndex == 2)
        {
            playerHealth -= 25;
            cout << red << "Health: " << playerHealth << reset << endl;
        }
    }
    else if (plrChoice == "3")
    {
        lastRoom = "Star Room";
        cout << green << "Entering the " << yellow << "Star room" << reset << endl;
        int outcomeIndex = rand() % possibleOutcomesStar.size();
        cout << magenta << possibleOutcomesStar[outcomeIndex] << reset << endl;
        if (outcomeIndex == 0)
        {
            playerHealth += 10;
            if (playerHealth > 100)
                playerHealth = 100;
            cout << red << "Health: " << playerHealth << reset << endl;
        }
        else if (outcomeIndex == 1)
        {
            plyrInventory.push_back("Mysterious Item");
            cout << cyan << "Mysterious Item added to inventory" << reset << endl;
        }
        else if (outcomeIndex == 2)
        {
            cout << yellow << "Do you want to keep the potion? (yes/no): " << reset << endl;
            string takePotion;
            getline(cin, takePotion);
            for (char &c : takePotion)
                c = tolower(c);
            if (takePotion == "yes")
            {
                plyrInventory.push_back("Health Potion");
                hasPotion = true;
                potions.push_back("Health Potion");
                cout << cyan << "Health Potion added to inventory" << reset << endl;
            }
            else if (takePotion == "no")
            {
                cout << cyan << "You leave the potion" << reset << endl;
            }
            else
            {
                cout << red << "Invalid input, leaving potion" << reset << endl;
            }
        }
        else if (outcomeIndex == 3)
        {
            playerHealth -= 5;
            cout << red << "Health: " << playerHealth << reset << endl;
        }
    }
    else
    {
        cout << red << "Invalid choice, you stumble and fall into the void, losing all your health!" << reset << endl;
        playerHealth = 0;
    }

    cout << yellow << "You leave the room and find yourself in a long hallway with two paths:" << endl;
    currentRoom = "Hallway";
    while (true)
    {
        cout << yellow << endl;
        cout << "What do you do?" << endl;
        cout << "1. Check/Use inventory items" << endl;
        cout << "2. Inspect stats" << endl;
        cout << "3. Continue down the hallway" << reset << endl;
        string hallwayChoice;
        getline(cin, hallwayChoice);
        if (hallwayChoice == "1")
        {
            cout << endl;
            cout << yellow << "Inventory:" << endl;
            if (plyrInventory.empty())
            {
                cout << red << "Your inventory is empty." << reset << endl;
                cout << yellow << endl;
            }
            else
            {
                for (size_t i = 0; i < plyrInventory.size(); ++i)
                {
                    cout << blue << i + 1 << ". " << plyrInventory[i] << endl;
                }
                cout << red << "Do you want to use an item? (yes/no): " << reset << endl;
                string useItem;
                getline(cin, useItem);
                if (useItem == "no")
                {
                    cout << cyan << "You decide not to use any items." << reset << endl;
                }
                else if (useItem == "yes")
                {
                    useInventoryItems(plyrInventory, playerMagics, red, green, yellow, blue, reset);
                }
            }
        }
        else if (hallwayChoice == "2")
        {
            cout << endl;
            cout << green << "Health: " << playerHealth << endl;
            cout << blue << "Inventory items: " << plyrInventory.size() << endl;
            cout << cyan << "Current Room: " << currentRoom << yellow << endl;
            cout << endl;
        }
        else if (hallwayChoice == "3")
        {
            bool hasKey = false;
            break;
        }
    }
    cout << magenta << "You walk down the hallway reluctantly and find two large wooden doors." << reset << endl;
    vector<string> itemsInBattle;
    while (true)
    {
        cout << yellow << "Do you want to go through the left door or the right door? (left/right): " << reset << endl;
        string doorChoice;
        getline(cin, doorChoice);
        if (doorChoice == "left")
        {
            cout << red << "Door is locked! Maybe the key is in the other room?" << reset << endl;
        }
        else if (doorChoice == "right")
        {
            // cout << red << "An eerie vibe comes from the room, as you walk in, you see a silver blue chest infront of you, when you open it," << blue << " you find a sword emitting a powerful aura from it, you reach out to take it and feel a sensation flow through your arms." << reset << endl;
            dynamicText("An eerie vibe comes from the room, as you walk in, you see a silver blue chest infront of you, when you open it,", 60, red, false);
            dynamicText(" you find a sword emitting a powerful aura from it, you reach out to take it and feel a sensation flow through your arms.", 55, blue, true);
            playerWeapons.push_back(Weapon(
                "Sword of Destiny",
                "Sword obtained in Lord Exodius's room",
                25,
                {{"Uppercut", 50, "A swift slice upwards, striking enemy from the chin up", false},
                 {"Heavy Slash", 75, "You grab the sword by its edge for maximum leverage and use all your might to swing as powerful as you can", false},
                 {"Normal Slash", 25, "A quick slash", false}}));
            plyrInventory.push_back("Sword of Destiny");
            dynamicText("Sword of Destiny added to inventory", 35, cyan, true);
            dynamicText("You continue walking down the room which gets darker and darker as you get futhur from the entrance, which has now disapeared, a deep voice fills the room", 60, red, true);
            dynamicText("[???]: I've been waiting for you... ", 100, green, false);
            dynamicText(playerName, 100, red, true);
            dynamicText("The ground shakes at the boom of his voice", 10, red, true);
            dynamicText("[You]: Who... who are you?", 100, blue, true);
            dynamicText(
                "[???]: I am Lord Exodious, you enter my domain, now you'll never leave.",
                85,
                green,
                true);
            dynamicText("A giant figure materializes infront of you, towering over you, his eyes glow a bright red color, and his armor is black as night", 70, red, true);
            dynamicText("[Lord Exodius]: Prepare to meet your demise!", 10, darkRed, true);

            // intialize pre-battle sequence
            itemsInBattle = preBattleSequence(plyrInventory, playerMagics, red, green, yellow, blue, cyan, reset, playerArmors);
            cout << cyan << "Magics will automatically be equipped in battle" << endl;
            cout << magenta << "Beginning battle..." << reset << endl;

            break;
        }
    }

    // boss battle

    system("clear"); // clear terminal
    Boss exodius = {
        "Lord Exodius",
        250,
        {{"Midnight Blade", 30, "dealing 30 damage!"},
         {"Shadow Slash", 20, "dealing 20 damage!"},
         {"Dark Pulse", 25, "dealing 25 damage!"}},
        {
            "[Lord Exodius]: I see how it is...",
            "[Lord Exodius]: Is that all you've got?",
            "[Lord Exodius]: Hah. Weak.",
            "[Lord Exodius]: You'll have to do better than that!",
            "[Lord Exodius]: Pathetic.",
            "[Lord Exodius]: I am.. inevitable.",
        }
    };

    bossBattle(exodius, playerWeapons, playerMagics, itemsInBattle, plyrInventory, playerArmors, playerHealth.load(), hasPotion, red, green, yellow, blue, magenta, cyan, white, reset, darkRed, isDead);

    

    //problem - playerhealth becomes 100 out of nowhere after battle - fix by 

    if(playerHealth == 0 || playerHealth < 0 || isDead){
        dynamicText("You have been defeated by Lord Exodius...", 50, red, true);
        dynamicText("Game Over", 100, darkRed, true);
        monitorThread.detach(); // stop health monitor thread
        return 0;
    }

    //clear itemsInBattle
    itemsInBattle.clear();

    // wow that alot of code for a boss battle
    dynamicText("ALL HEALTH RESTORED - ONE TIME EVENT, NO HEALTH RESTORATION AFTER BOSS BATTLES IN THE FUTURE", 25, green, true);
    playerHealth = 100;
    dynamicText("As Exodius falls to the ground, hand cletching againts his chest, he looks up at you with final words", 65, red, true);
    dynamicText("[Lord Exodius]: Heh... here I thought I was immortal... I thought I was.. a god...", 75, darkRed, true);
    dynamicText("With that, he takes his last breath and falls silent", 75, red, true);
    dynamicText("His remains disintegrate into a pile of dust, leaving behind a glowing sword with a green gem in its hilt, a dark orb carrying his magic essence, and his armor", 75, yellow, true);
    dynamicText("You pick up the sword, feeling its warmth and power coursing through your veins", 75, magenta, true);

    playerWeapons.push_back(Weapon(
        "Sword of Exodius",
        "Sword obtained from the remains of Lord Exodius, radiating his power when wielded",
        40,
        {
            {"Midnight Cleave", 80, "A powerful cleave that channels the dark energy of Exodius, takes away 15 hp to use", 15},
            {"Shadow Strike", 60, "A swift strike that harnesses the shadows, dealing heavy damage", 0},
            {"Dark Wave", 100, "Unleash a wave of dark energy that damages all enemies, takes away 20 hp to use", 20}
        }
    ));

    dynamicText("Sword of Exodius added to inventory", 25, cyan, true);

    dynamicText("After, you pick up the orb, feeling his power flow through your soul", 75, green, true);

    playerMagics.push_back(MagicAbility(
        "Exodius's Shadow",
        "Magic obtained from the essence of Lord Exodius, grants powerful dark abilities",
        0,
        0,
        false,
        false,
        {
            {"Cloak", 0, "50/50 chance to dodge next attack", 0, 1.0},
            {"Dark Strike", 70, "Materialize a spear in the palm of your hand and throw it at the enemy", 0, 0.0},
            {"Shadow Bind", 0, "Bind the enemy in shadows, preventing them from attacking next turn", 0, 1.0, true}
        }
    ));

    dynamicText("New Magic unlocked: Exodius's Shadow", 25, green, true);

    dynamicText("You pick up the armor, feeling a surge of protection", 75, blue, true);

    playerArmors.push_back(Armor(
        "Armor of Exodius",
        "Armor obtained from the remains of Exodius, Reduces damage taken by 15% (during any fights)",
        0.15
    ));
    dynamicText("Armor of Exodius added to inventory", 25, blue, true);
    dynamicText("A chest appears infront of you while you begin to leave, inside of it appears a key... maybe it unlocks the other door?", 75, yellow, true);
    plyrInventory.push_back("Old Key");
    dynamicText("Old Key added to inventory", 25, cyan, true);
    dynamicText("You walk back into the hallway, you face against the locked wooden door and slide the key inside the keyhole, it struggles at first but then unlocks with a loud click", 75, yellow, true);
    //remove key from inv
    auto keyIt = find(plyrInventory.begin(), plyrInventory.end(), "Old Key");
    if (keyIt != plyrInventory.end())
    {
        plyrInventory.erase(keyIt);
    }

    dynamicText("You open the door reluctantly, and a bright light flashes your eyes, when you come to your senses, you appear in a large room with another locked door infront of you.. but a sense of", 75, yellow, false);
    dynamicText(" impending doom flows over your shoulders", 75, darkRed, true);

    //wave of enemies
    dynamicText("Suddenly, multiple creatures appear infront of you, faces full of revenge", 50, red, true);
    dynamicText("[Mysterious Creature]: HOW DARE YOU DEFEAT OUR LORD! MEET YOUR DOOM!", 100, darkRed, true);
    bool isWaveOver = false;
    while(!isWaveOver){ //WAVE LOOP FOR FUTURE REFERENCES
        int wave = 1;
        int numEnemies;
        int eachEnemyHealth;
        int damageToNextWave = numEnemies * eachEnemyHealth;
        vector<int> usedMoves;
        while(true){
            if(wave == 1){
                numEnemies = 5;
                eachEnemyHealth = 20;
            }else if(wave == 2){
                numEnemies = 10;
                eachEnemyHealth = 35;
            }else if(wave == 3){
                numEnemies = 20;
                eachEnemyHealth = 45;
            }
            
            string in;
            int chosenMove;
            int movelist = 0;
            vector<Skill> chosenSkills;
            //cant use the same move twice in a row
            
            cout << cyan << "-----------------INFO------------------" << endl;
            cout << green << "Health: " << playerHealth << endl;
            cout << yellow << "Wave: " << wave << endl;
            cout << red << "Enemies remaining: " << numEnemies << endl;
            cout << cyan << "------------MAGICS--------------" << endl;
            for (const auto& magic : playerMagics) {
                cout << magic.name << ":" << endl;
                if (magic.damageReduction > 0.0) {
                    movelist++;
                    cout << "[" << movelist << "] " << "Damage reduction: " << magic.damageReduction << endl;
                    chosenSkills.push_back({magic.name, magic.damage, movelist, magic.damageReduction, 0, false});
                }
                for(const auto& magicabil : magic.abilities){
                    movelist++;
                    cout << "[" << movelist << "] " << " " << magicabil.name << " (" << magicabil.damage << " damage): " << magicabil.description;
                    if(magicabil.isBinding) cout << " [Binding Ability]";
                    cout << endl;
                    chosenSkills.push_back({magicabil.name, magicabil.damage, movelist, magicabil.damageReduction, magicabil.removeHP, magicabil.isBinding});
                }
                if(magic.damage > 0) {
                    movelist++;
                    cout << "[" << movelist << "] " << "Damage: " << magic.damage << endl;
                    chosenSkills.push_back({magic.name, magic.damage, movelist, magic.damageReduction, 0, false});
                }
                cout << " > Description: " << magic.description << endl;
            }
            cout << blue << "------------WEAPONS--------------" << endl;
            for(const auto &weapon : playerWeapons){
                cout << cyan << weapon.name << endl;
                for(const auto &ability : weapon.abilities){
                    movelist++;
                    cout << blue << "[" << movelist << "] " << ability.name << " " << ability.damage << " damage : " << ability.description << endl;
                    chosenSkills.push_back({ability.name, ability.damage, movelist, 0.0, ability.removeHealth, false});
                }
            }
            getline(cin, in);

            try{
                chosenMove = stoi(in);

            }catch (const exception &e){
                cout << "Invalid input, enter a valid number" << endl;
                continue;
            }
            cout << red << "Chosen move: " << chosenSkills[chosenMove - 1].name << endl;
            cout << "DEBUG: DMG -- " << chosenSkills[chosenMove - 1].damage << endl;
            /*for(int i = 0; i < usedMoves.size(); i++){ //debugging start
                cout << usedMoves[i] << " ";
            }
            cout << endl;
            cout << usedMoves[usedMoves.size()] << endl;
            cout << usedMoves[usedMoves.size() - 1] << endl; */ //debugging end */

            if(usedMoves.size() > 1){
                cout << "DEBUG: USEDMOVES.BACK() -- " << usedMoves.back() << endl;
            }

            if(usedMoves.size() > 1){//check if its not empty
                if(usedMoves.back() == chosenMove or usedMoves[usedMoves.size() - 1] == chosenMove){
                    dynamicText("You cannot use the same move twice in a row!", 50, red, true);
                    usedMoves.pop_back();
                    continue;
                }
            }

            usedMoves.push_back(chosenMove);
            cout << "DEBUG: USED MOVE LIST-- ";
            for(auto i = 0; i < usedMoves.size(); i++){
                cout << usedMoves[i] << " ";
            }
            cout << endl;

            /*if(usedMoves[usedMoves.size() - 2] == chosenMove && usedMoves.size() > 1){
                dynamicText("You cannot use the same move twice in a row!", 50, red, true);
                usedMoves.pop_back();
                continue;
            }*/ //--------> follows main principle, attempting to improve


            /*if(usedMoves.back() == chosenMove){
                dynamicText("You cannot use the same move twice in a row!", 50, red, true);
                continue;
            }
            usedMoves.push_back(chosenMove); */ //broken code --- intended purpose -> cant use same move twice in a row

            int damage = chosenSkills[chosenMove - 1].damage;
            int damageReduction = 0;
            damageReduction = chosenSkills[chosenMove - 1].damageReduction;

            //check if the move removes health
            if(chosenSkills[chosenMove - 1].healthRemove > 0){
                if(playerHealth <= chosenSkills[chosenMove - 1].healthRemove){
                    cout << darkRed << "idiot u had less health than the move takes away, u died lol" << reset << endl;
                    playerHealth = 0;
                    isDead = true;
                    break;
                }else{
                    playerHealth -= chosenSkills[chosenMove - 1].healthRemove;
                }
            }

            //damage the enemies
            if(damage > 0){
                int enemiesKilled = damage / eachEnemyHealth;
                dynamicText("Your move killed " + to_string(enemiesKilled) + " enemies!", 50, red, true);
                numEnemies -= enemiesKilled;
            }

            if(numEnemies <= 0){
                if(wave < 3){
                    dynamicText("You attempt to take a breather, but suddenly more appear!", 50, darkRed, true);
                    wave++;
                    continue;
                }else if(wave > 3){
                    isWaveOver = true;
                    break;
                }
            }

            vector<string> enemyVoicelines = {
                "[Mysterious Creature]: FOR EXODIUS!!",
                "[Mysterious Creature]: ugh!",
                "[Mysterious Creature]: GRAAAHHH!",
                "[Mysterious Creature]: ARRGGHH!",
                "[Mysterious Creature]: YOU WILL PAY FOR THIS!",
            };

            int vcIndex = rand() % enemyVoicelines.size();
            dynamicText(enemyVoicelines[vcIndex], 65, darkRed, true);

            if(damageReduction > 0){
                dynamicText("You brace yourself...", 50, cyan, true);
            }
            //enemies attack
            struct enemyAttack{
                int id;
                int dmg;
                string desc;
            };

            cout << "DEBUG: num enemi is " << numEnemies << endl;
            vector<enemyAttack> enemyAttacks = {
                {1, 50 / numEnemies, "An enemy does a switch punch to your face -" + to_string( 50 / numEnemies) + " hp"},
                {2, 55 / numEnemies, "An enemy does a swift kick to your gut -" + to_string( 55 / numEnemies) + " hp"},
                {3, 65 / numEnemies, "An enemy does a heavy slam to your back -" + to_string( 65 / numEnemies) + " hp"},
                {4, 70 / numEnemies, "A group of enemies tackle you and pin you on the ground -" + to_string( 70 / numEnemies) + " hp"}
            };

            //first check if the user used a binding move
            bool isBinded = chosenSkills[chosenMove - 1].isBinding;
            if(!isBinded){
                int totalEnemyDmg = 0;
                for(int i = 0; i < numEnemies; i++){
                    int attackIndex = rand() % enemyAttacks.size();
                    totalEnemyDmg += enemyAttacks[attackIndex].dmg;
                    dynamicText(enemyAttacks[attackIndex].desc, 50, red, true);
                }
                totalEnemyDmg -= static_cast<int>(totalEnemyDmg * (damageReduction));
                if(totalEnemyDmg < 0) totalEnemyDmg = 0;
                playerHealth -= totalEnemyDmg;
                dynamicText("You took " + to_string(totalEnemyDmg) + " damage!", 50, red, true );
            }else{
                dynamicText("Enemies are binded, no attacks this turn.", 50, green, true);
                chosenSkills[chosenMove - 1].isBinding = false; //reset binding
                continue;
            }

            if(numEnemies <= 0){
                if(wave < 3){
                    dynamicText("You attempt to take a breather, but suddenly more appear!", 50, darkRed, true);
                    wave++;
                    continue;
                }else if(wave > 3){
                    isWaveOver = true;
                    break;
                }
            }
        }
    }

    gameRunning = false;  // Tell the monitor thread to stop
    monitorThread.join(); // Wait for it to finish
    return 0;
}
