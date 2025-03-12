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
int tetRow = 4;
int tetCol = 4;
vector<vector<bool>> tetModels(tetRow, vector<bool>(tetCol));
class tetBlocks{
    public:
        void operator[](int index){
            switch(index){
                case 0: //2x2
                    for(int i = 0; i < 2; i++){
                        for(int j = 0; j < 2; j++){
                            tetModels[i][j] = true;
                        }
                    }
                    break;
                case 1:
                    
                    break;
                
            }
        }
};


// Function to print matrix
void printMatrix() {
    for (int i = 0; i < bRow; i++) {
        for (int j = 0; j < bCol; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

void clearTerm(){
    system("clear");
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
    }
    return 0;
}
