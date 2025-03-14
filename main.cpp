#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <cstdlib>
using namespace std;
using namespace this_thread;
using namespace chrono;

//

// Initialize board
int bRow = 16;
int bCol = 8;
vector<vector<string>> matrix(bRow, vector<string>(bCol, "_")); // Set default value to "_"

//handle block models (in true/false models)
int tetRow = 8;
int tetCol = 8;
vector<vector<bool>> tetModels(tetRow, vector<bool>(tetCol));
void initializeTetBlocks(int index){
    //empty matrix for new blocks
    for(int i = 0; i < tetRow; i++){
        for(int j = 0; j < tetCol; j++){
            tetModels[i][j] = false;
        }
    }

    //initialize true/false statements
    switch(index){
        case 0:
            for(int i = 0; i < 2; i++){
                for(int j = 0; j < 2; j++){
                    tetModels[i][j] = true;
                }
            }
            break;
        default:
            cout << "invalid request statement, can't fullfill index requirements." << endl;
            break;
    }
}


void clearTerm(){
    system("clear");
}


// Function to print matrix
void printMatrix() {
    clearTerm();
    for (int i = 0; i < bRow; i++) {
        for (int j = 0; j < bCol; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

int main() {
    string playerIn;
    //temporary vars
    clearTerm();
    cout << "-------------TETRIS-------------" << endl;
    cout << "Enter 'play' to begin." << endl;
    cout << "> ";
    cin >> playerIn;
    if(playerIn == "play"){
        cout << "Intializing board...\n";
        sleep_for(seconds(2));
        printMatrix();
        initializeTetBlocks(0);
        for(int i = 0; i < tetRow; i++){
            for(int j = 0; j < tetCol; j++){
                if(tetModels[i][j]){
                    matrix[i][j] = "X";
                }
            }
        }
        printMatrix();
    }
    return 0;
}
