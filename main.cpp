#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <cstdlib>
using namespace std;
using namespace this_thread;
using namespace chrono;

// Initialize board
int bRow = 16;
int bCol = 8;
int sizeOfValidBlock;
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
        case 0: //2x2
            for(int i = 0; i < 2; i++){
                for(int j = 0; j < 2; j++){
                    tetModels[i][j] = true;
                    //set the size of the valid block
                    sizeOfValidBlock = 2;
                }
            }
            break;
        case 1://2x3
            for(int i = 0; i < 2; i++){
                for(int j = 0; j < 3; j++){
                    tetModels[i][j] = true;
                    sizeOfValidBlock = 3;
                }
            }
            break;
        case 2://3x3
            for(int i = 0; i < 3; i++){
                for(int j = 0; j < 3; j++){
                    tetModels[i][j] = true;
                    sizeOfValidBlock = 3;
                }
            }
            break;
        case 3: //right 4x3 block
            for(int i = 0; i < 4; i++){ //horizonal line for block
                tetModels[0][i] = true;
            }
            for(int i = 0; i < 2; i++){ //vertical line for block
                tetModels[i][3] = true;
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

void blockMovement() {
    bool isBlock = false;
    int blockRow = 0;
    int blockCol = (bCol - sizeOfValidBlock) / 2; // Center block horizontally

    while (true) {
        // Check if block reaches bottom or another block
        for (int i = tetRow - 1; i >= 0; i--) {
            for (int j = 0; j < tetCol; j++) {
                if (tetModels[i][j]) {
                    int newRow = blockRow + i + 1;  
                    int newCol = blockCol + j + 4;

                    cout << i << " :: " << newRow << " + " << newCol << endl;

                    if (newRow >= bRow || (matrix[newRow][newCol] == "X" && !tetModels[i][j])) {
                        cout << "db: " << newRow << endl;
                        isBlock = true;
                    }
                }
            }
        }

        if (isBlock) break;

        // Clear previous position
        for (int i = 0; i < tetRow; i++) {
            for (int j = 0; j < tetCol; j++) {
                if (tetModels[i][j] && blockRow + i < bRow) {
                    matrix[blockRow + i][blockCol + j] = "_";
                }
            }
        }

        // Move block down
        blockRow++;

        // Place block in new position
        for (int i = 0; i < tetRow; i++) {
            for (int j = 0; j < tetCol; j++) {
                if (tetModels[i][j] && matrix[blockRow + i][blockCol + j] == "_") {
                    matrix[blockRow + i][blockCol + j] = "X";
                }
            }
        }

        printMatrix();
        sleep_for(seconds(1));
    }
}



int main() {
    cout << "\033[32m"; // Green output text
    string playerIn;
    
    clearTerm();
    cout << "-------------TETRIS-------------" << endl;
    cout << "Enter 'play' to begin." << endl;
    cout << "> ";
    cin >> playerIn;
    
    if(playerIn == "play") {
        cout << "Initializing board...\n";
        sleep_for(seconds(1));
        printMatrix();

        for (int blockType = 0; blockType < 2; blockType++) { // Drop 2 blocks
            initializeTetBlocks(blockType);

            blockMovement();
        }
    }
    return 0;
}
