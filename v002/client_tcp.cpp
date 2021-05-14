#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include "battleship.cpp"


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 1024
//#define DEFAULT_PORT "27015"

using std::cout;
using std::endl;
using std::string;
using std::stoi;
using std::ofstream;
using std::ifstream;

// Function to chat with server.
void chat(SOCKET ConnectSocket, BattleShip &battleship){
    char buff[DEFAULT_BUFLEN];
    int n, iResult, number = -1, letter = -1, client_number, client_letter;
    string tmp;
    BattleShip enemy_battleship;

    cout << "Press P to print boards..." << endl;
    cout << "Type exit to leave..." << endl;
    // Infinite loop during chat.
    for(;;){

        // Clear buffer.
        memset(buff, 0, DEFAULT_BUFLEN);
        cout << "Enter the shot: ";
        n = 0;

        // Copy client message in the buffer.
        while((buff[n++] = getchar()) != '\n');

        // If message contains "Exit" the server exit and chat ended.
        if(strncmp("exit", buff, 4) == 0){
            printf("Client Exit...\n");
            // Send that buffer to client ask to close and close here to.
            iResult = send( ConnectSocket, buff, sizeof(buff), 0);
            if (iResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ConnectSocket);
                WSACleanup();
                break;
            }
            break;
        }

        // If 'P' is pressed print my board and the server board.
        if(buff[0] == 'P'){
            cout << "<<\tMy Board\t>>" << endl;
            battleship.printBoard();
            cout << endl << endl;
            cout << "<<\tKnown Enemy Board\t>>" << endl;
            enemy_battleship.printBoard();

            // Clear buffer.
            memset(buff, 0, DEFAULT_BUFLEN);
            // Re-DO that operation to get shoot data.
            cout << "Enter the shot: ";
            n = 0;
            while((buff[n++] = getchar()) != '\n');
        }

        // Store my own shot.
        client_number = stoi(string(1, buff[0]));
        client_letter = stoi(string(1, buff[1]));

        // Receive the shot from the server.
        if(battleship.shoot(number,letter)){
            buff[2] = 'H'; buff[3] = 'I'; buff[4] = 'T'; buff[5] = ' ';
        } else{
            buff[2] = 'M'; buff[3] = 'I'; buff[4] = 'S'; buff[5] = 'S';
        }

        cout << endl; //Screen ajustments.
        
        // Check every shoot if the game ends.
        if(battleship.checkEndGame()){
            cout << "Server WIN!\n";
            strcpy(buff, "win");
            // Send that buffer to client ask to close and close here to.
            iResult = send( ConnectSocket, buff, sizeof(buff), 0);
            if (iResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ConnectSocket);
                WSACleanup();
                break;
            }
            cout << "Client Exiting...\n";
            break;
        }

        // Send that buffer to server.
        iResult = send(ConnectSocket, buff, sizeof(buff), 0);
        if (iResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            break;
        }
        memset(buff, 0, DEFAULT_BUFLEN);
        
        // Read the message from server.
        recv(ConnectSocket, buff, sizeof(buff), 0);

        // If message contains "win" close connection
        if(strncmp("win", buff, 3) == 0){
            cout << "Congratulations you win!\n";
            cout << "Client Exit...\n";
            break;
        }

        // Define tmp string to threat data.
        tmp = buff;
        // If Hit was received. Mark in you enemy board.
        if(strncmp(tmp.substr(2,4).c_str(),"HIT", 3) == 0){
            enemy_battleship.board[client_number][client_letter] = 1;
            //cout << "My shot is: " << number << letter << endl;
            //cout << "I hit a enemy ship!" << endl;
            //enemy_battleship.printBoard();
        }

        // Print buffer which contains the client contents.
        cout << "Message from server..." << endl; 
        cout << "You " << tmp.substr(2,4) << " the target at: " << client_number << "," << client_letter << " Attack: " << tmp.substr(0,1) << "," << tmp.substr(1,1) << endl << endl;

        // Receive the shot from the server.
        number = stoi(string(1, buff[0]));
        letter = stoi(string(1, buff[1]));

        // Print Board my own board.
        //battleship.printBoard();

        // Print the board when receive "P".
        if(strncmp("P", buff, 1) == 0){
            battleship.printBoard();
        }
    }
}

// Function to place ships.
void place_ships(BattleShip &battleship){ 
    // Variables.
    ifstream file("ship_position.txt");
    string line;
    int type, number, letter;
    bool horizontal;
    // Read file.
    if(file.is_open()){
        while(getline(file, line)){
            // Threat data.
            horizontal = false;
            type = stoi(string(1, line[0]));
            //Check 10.
            if(line[2] == '1' && line[3] == '0'){
                number = stoi(string(2, line[2]))-2;
                letter = (int)line[5]-65;
                if(line[7] == 'h'){
                    horizontal = true;
                }
            } else{
                number = stoi(string(1, line[2]))-1;
                letter = (int)line[4]-65;
                if(line[6] == 'h'){
                    horizontal = true;
                } 
            }
            
            // Place the ships.
            battleship.placeShip(type, number, letter, horizontal);
        }
    } else{
        cout << "Error open file!";
    }
    
    // Close file.
    file.close();
}

int __cdecl main(int argc, char **argv) 
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    const char* DEFAULT_PORT = argv[2];
    
    BattleShip battleship;
    place_ships(battleship);

    // Validate the parameters
    if (argc != 3) {
        printf("Usage: %s client <ip/name> <port>\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Chat with the server.
    chat(ConnectSocket, battleship);

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Check the connection is closed in a proper way.
    if ( iResult == 0 )
        printf("Connection closed.\n");
    else
        printf("recv failed with error: %d\n", WSAGetLastError());

    // Cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}