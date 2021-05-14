#include "battleship.cpp"
#include <iostream>

using std::cout;

int main(int argc, char **argv){
    
    BattleShip battleship;
    
    //battleship.printBoard();

    //battleship.placeShip(1, 0, 0, true);
    //battleship.placeShip(4,2,3,false);
    //cout << battleship.shoot(4,4) << "\n";

    //battleship.printBoard();

    for(int i; i<1000; i++){
        //if(rand()%10 == 10)
            cout << rand()%10 << "\n";
            //cout << "AQUI";
    }
    
    return 0;
}