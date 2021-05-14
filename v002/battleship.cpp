#include "battleship.h"

// Constructor.
BattleShip::BattleShip() {

    for(int i=0; i<10; i++){
        for(int j=0; j<10; j++){
            board[i][j] = 0;
        }
    } 
}

// Function to place a Ship.
bool BattleShip::placeShip(int type, int number, int letter, bool horizontal){
    // Check Ship type the switch case will only result in true if it is possible to place a ship.
    // It will check the board limits and check if it is no ship at the same spot.
    switch(type){
        case 1: // Aerocarrier.
            for(int i=0; i<5; i++){
                if(horizontal){
                    if(letter+5 > 9 || board[number][letter+i] == 1){
                        return false;
                    }                    
                    board[number][letter+i] = 1;
                } else{
                    if(number+5 > 9 || board[number+i][letter] == 1){
                        return false;
                    }
                    board[number+i][letter] = 1;
                }
            }
            return true;
        case 2: // Tankers Ship.
            for(int i=0; i<4; i++){
                if(horizontal){
                    if(letter+4 > 9 || board[number][letter+i] == 1){
                        return false;
                    }
                    board[number][letter+i] = 1;
                } else{
                    if(number+4 > 9 || board[number+i][letter] == 1){
                        return false;
                    }
                    board[number+i][letter] = 1;
                }
            }        
            return true;
        case 3: // Destroyer.
            for(int i=0; i<3; i++){
                if(horizontal){
                    if(letter+3 > 9 || board[number][letter+i] == 1){
                        return false;
                    }
                    board[number][letter+i] = 1;
                } else{
                    if(number+3 > 9 || board[number+i][letter] == 1){
                        return false;
                    }
                    board[number+i][letter] = 1;
                }
            }
            return true;
        case 4: // Submarine.
            for(int i=0; i<2; i++){
                if(horizontal){
                    if(letter+2 > 9 || board[number][letter+i] == 1){
                        return false;
                    }                    
                    board[number][letter+i] = 1;
                } else{
                    if(number+2 > 9 || board[number+i][letter] == 1){
                        return false;
                    }
                    board[number+i][letter] = 1;
                }
            }
            return true;
        default:
            std::cout << "ERROR";
            return false;
    }
}

// Function to shoot.
bool BattleShip::shoot(int number, int letter){
    // Hit.
    if(board[number][letter] == 1){
        board[number][letter] = 0;
        
        return true;
    } else{
        return false;
    }
}

// Function to check end game.
bool BattleShip::checkEndGame(){
    for(int i=0; i<10; i++){
        for(int j=0; j<10; j++){
            if(board[i][j] == 1){
                return false;
            }
        }
    }
    return true;
}

// Function to print the board.
void BattleShip::printBoard(){
    cout<<"   A B C D E F G H I J\n";
    
    for(int i=0; i<10; i++){
        if(i==9){
            cout<<i+1<<" ";
        } else{
            cout<<i+1<<"  ";
        }
        for(int j=0; j<10; j++){
            std::cout << board[i][j] << " ";
        }
        cout << "\n";
    }
}