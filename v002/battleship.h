#include <iostream>
//using namespace std;

#ifndef BATTLESHIP_H
#define BATTLESHIP_H

using std::cout;

class BattleShip {

public:
    // Construtors.
    BattleShip();
    // Destructor
    //~BattleShip();
    
    // Variables.
    int board[10][10];
    /*
    struct Ship{
        int type;
        int letter;
        int number;
        bool horizontal;
    };
    
    Ship aerocarrier {1, -1, -1, false}; // 5 squares.
    Ship tankers_ship {2, -1, -1, false}; // 4 squares.
    Ship destroyer {3, -1, -1, false}; // 3 squares.
    Ship submarines {4, -1, -1, false}; // 2 squares.
    */

    // Functions
    bool placeShip(int, int, int, bool);
    bool shoot(int letter, int number);
    bool checkEndGame();
    void printBoard();
    
//protected:

//private:

};
#endif //BATTLESHIP_H