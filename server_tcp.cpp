#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <charconv>

#include "battleship.cpp"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 1024
//#define DEFAULT_PORT "27015"

using std::stoi; 
using std::string;
using std::cout;
using std::endl;
using std::to_chars;

//Function to chat with the client.
void chat(SOCKET ClientSocket, BattleShip &battleship){
    char buff[DEFAULT_BUFLEN];
    int number = -1, letter = -1, server_number = -1, server_letter = -1;
    int n, iResult;
    string tmp;
    srand(time(0));
    // Infinite loop during chat.
    for(;;){
        // Clear buffer.
        memset(buff, 0, DEFAULT_BUFLEN);

        // Read the message from client and copy it in buffer.
        recv(ClientSocket, buff, sizeof(buff)+1, 0);

        // If message contains "Exit" the server exit and chat ended.
        if(strncmp("exit", buff, 4) == 0){
            printf("Server Exiting...\n");
            break;
        }

        // If message contains "win" close connection
        if(strncmp("win", buff, 3) == 0){
            cout << "Congratulations you win!\n";
            cout << "Server Exiting...\n";
            break;
        }

        // Print buffer which contains the client contents.
        tmp = buff;
        cout << "Message from client..." << endl; 
        cout << "You " << tmp.substr(2,4) << " the target at: " << server_number << "," << server_letter << endl;
        //cout << "From client: " << tmp.substr(0,2) << " - " << tmp.substr(2,4) << endl;
        
        // Receive the shoot from the client.
        number = stoi(string(1, buff[0]));
        letter = stoi(string(1, buff[1]));

        // Clear buffer.
        memset(buff, 0, DEFAULT_BUFLEN);

        // Check if HIT is received from client. If HIT next position will be +1.
        // If not, get two random numbers less than 10.
        if(strncmp(tmp.substr(2,4).c_str(),"HIT", 3) == 0){
            // Check the borders.
            if(server_number < 9){
                server_number++;
            } else{
                if(server_letter < 9){
                    server_letter++;
                }
            }
            
            // Char conversion on C++17.
            to_chars(buff, buff+1,server_number);
            to_chars(buff+1, buff+2,server_letter);
        } else{
            server_number = rand()%10;
            //server_number = 9;
            server_letter = rand()%10;
            //server_letter = 4;

            // Char conversion on C++17.
            to_chars(buff, buff+1,server_number);
            to_chars(buff+1, buff+2,server_letter);
        }

        // Receive the shoot from the client.
        if(battleship.shoot(number,letter)){
            buff[2] = 'H'; buff[3] = 'I'; buff[4] = 'T'; buff[5] = ' ';
        } else{
            buff[2] = 'M'; buff[3] = 'I'; buff[4] = 'S'; buff[5] = 'S';
        }

        // Check every shoot if the game ends.
        if(battleship.checkEndGame()){
            cout << "Client WIN!\n";
            strcpy(buff, "win");
            // Send that buffer to client ask to close and close here to.
            iResult = send( ClientSocket, buff, sizeof(buff), 0);
            if (iResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                break;
            }
            cout << "Server Exiting...\n";
            break;
        }

        // Print Board.
        battleship.printBoard();
        
        tmp = buff;
        cout << "Attack: " << tmp.substr(0,1) << "," << tmp.substr(1,1) << endl << endl;
        //cout << "To client: " << tmp.substr(0,2) << " - " << tmp.substr(2,4) << endl << endl;

        // Send that buffer to client.
        iResult = send( ClientSocket, buff, sizeof(buff), 0);
        if (iResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            break;
        }
    }
}

// Function to start the game.
void place_ships(BattleShip &battleship){ 
    // Variables.
    int type, number, letter;
    bool horizontal;
    srand(time(0));
    int n = 0;
    
    // Aerocarrier.
    do {
        number = rand()%10;
        letter = rand()%10;
        horizontal = rand()%2;
    } while(!battleship.placeShip(1, number, letter, horizontal));
    n++;
    
    // Tanker Ships.
    for(int i=0; i<2; i++){
        do {
            number = rand()%10;
            letter = rand()%10;
            horizontal = rand()%2;
        } while(!battleship.placeShip(2, number, letter, horizontal));
        n++;
    }
    // Destroyer.
    for(int i=0; i<3; i++){
        do {
            number = rand()%10;
            letter = rand()%10;
            horizontal = rand()%2;
        } while(!battleship.placeShip(3, number, letter, horizontal));
        n++;
    }
    
    // Submarine.
    for(int i=0; i<4; i++){
        do {
            number = rand()%10;
            letter = rand()%10;
            horizontal = rand()%2;
        } while(!battleship.placeShip(4, number, letter, horizontal));
        n++;
    }
    
}

int __cdecl main(int argc, char **argv) 
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    const char* DEFAULT_PORT = argv[1];

    BattleShip battleship;
    place_ships(battleship);
    
    // Validate the parameters
    if (argc != 2) {
        printf("Usage: %s server <port>\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    // Chat with client.
    chat(ClientSocket, battleship);

    // Check if connection is closing in a proper way.
    if (iResult == 0)
            printf("Connection closing...\n");
        else  {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
    }

    // Shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // Cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}