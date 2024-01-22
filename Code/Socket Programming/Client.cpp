#include <iostream>
#include <WinSock2.h>
#include <random>

#pragma comment(lib, "ws2_32.lib")
int getRandomNumber();

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock.\n";
        return 1;
    }

    // Create a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create client socket.\n";
        WSACleanup();
        return 1;
    }

    // Connect to the server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // Assuming server is running on the same machine
    serverAddress.sin_port = htons(12345);

    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to the server.\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Initialize speed
    int speed = 0;

    int bytesRead;
    int receivedNumber;
    while(bytesRead = recv(clientSocket, reinterpret_cast<char*>(&receivedNumber), sizeof(int), 0)){

            if (bytesRead > 0) {
             if(receivedNumber==0)
            {
        
            std::cout << "Client Received Platoon Formation Request" << "\n";
            std::cout << "Client Accepting request" << "\n";
            const char* response = "Accepting request";
            send(clientSocket, response, strlen(response), 0);
            std::cout << "Client sent: " << response << "\n";
            std::cout << "---------------------------------------------------------------" << "\n";

            }
            if(receivedNumber==1)
            {
                std::cout << "Child Truck Accelerating:\n";
                    for (int i = 0; i < 5; ++i) {
                        // Increase speed by 5 for each iteration
                        speed += 5;
                        std::cout << "Accelerating. Speed: " << speed << "\n";
                    }
                std::cout << "---------------------------------------------------------------" << "\n";
            }
             if(receivedNumber==3)
            {
                std::cout << "Child Truck Breaking/deaccelerating:\n";
                    for (int i = 0; i < 5; ++i) {
                        // Decrease speed by 5 for each iteration
                        speed -= 5;
                        std::cout << "Speed: " << speed << "\n";
                    }
                std::cout << "---------------------------------------------------------------" << "\n";
            }
             if(receivedNumber==2)
            {
                std::cout << "Child Truck moving in Constant Velocity\n";
                for (int i = 0; i < 5; ++i) {
                    std::cout << "Speed: " << speed << "\n";
                }
            std::cout << "---------------------------------------------------------------" << "\n";
            }
             if(receivedNumber==4)
            {
                std::cout << "Journey Ended.....\n";
                break;
            }
             if(receivedNumber==6)
            {
                std::cout << "---------------------------------------------------------------" << "\n";
                std::cout << "Receiving Turning Info(Turing angle & Location data)\n";
                std::cout << "Turning on the exact location\n";
                std::cout << "---------------------------------------------------------------" << "\n";
            }
             if(receivedNumber==5)
            {
                int DC=getRandomNumber();
                std::cout<< DC<<"Distance Check:\n";
                if(DC==1)
                {
                    std::cout<<"Distance not optimal:\nReducing/Increasing Speed to maintain optimal distance:\n";
                    for (int i = 0; i < 2; ++i) {
                            std::cout << "adjusting speed" <<"\n";
                        }
                    std::cout<<"Optimal Distance between trucks are maintained.\n";
                }
                else{
                    //std::cout<<"The distance between trucks is optimal.... "<<DC<<"\n";
                }
                
            }
        } 
        int Obs=getRandomNumber();
           std::cout<< "Checking for Obstacle:\n";
            if(Obs==1)
            {
                std::cout<<"Obstacle detected:\nReducing Speed:\n";
                for (int i = 0; i < 2; ++i) {
                        // Decrease speed by 5 for each iteration
                        speed -= 2;
                        std::cout << "Speed: " << speed << "\n";
                    }
                std::cout<<"Obstacle gone:\nIncreasing to speed to maintain optimal distance:\n";
                for (int i = 0; i < 2; ++i) {
                        // Decrease speed by 5 for each iteration
                        speed += 2;
                        //std::cout << "Speed: " << speed << "\n";
                    }
            }
            else{
                //std::cout<<"No obstacle detected: "<<Obs<<"\n";
            }

    } 
    // Close the socket and clean up
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
int getRandomNumber() {
    // Generate a random number between 1 and 3
    return std::rand() % 3 + 1;
}

