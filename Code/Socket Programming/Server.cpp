#include <iostream>
#include <fstream>
#include <string>
#include <WinSock2.h>
#include <omp.h>
#include <list>


#pragma comment(lib, "ws2_32.lib")

// Declaration of the communicator function
void communicator(int valueToSend, SOCKET clientSocket);
int commcheck(int valueToSend, SOCKET clientSocket);
class FIFOList {
public:
    void appendValue(int value) {
        myList.push_back(value);
    }

    int fetchFirstValue() {
        if (!myList.empty()) {
            int lastValue = myList.front();
            myList.pop_front(); // Remove the fetched value
            return lastValue;
        } else {
            std::cerr << "The list is empty.\n";
            return -1; // Return a sentinel value to indicate an empty list
        }
    }

    bool isEmpty() {
        return myList.empty();
    }

private:
    std::list<int> myList;
};



int main()
{
    WSADATA wsaData; //--> supports socket pro on windows envi
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize Winsock.\n";
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//**
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create server socket.\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;//Assigning the address of my own machine
    serverAddress.sin_port = htons(12345);//-->specifying network byte order

    if (bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind the server socket.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Error while listening for connections.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Server listening on port 12345...\n";

    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to accept the client connection.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }


    int ttt=0;
    FIFOList myFIFOList;

    const char *message = "Platoon Formation Request";
    int plr = 0;

    send(clientSocket, reinterpret_cast<const char *>(&plr), sizeof(int), 0);
    std::cout << "Server sent: " << message << "\n";

    char buffer[256];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead == SOCKET_ERROR)
    {
        std::cerr << "Error while receiving data from the Client.\n";                   
    }
    else
    {
        buffer[bytesRead] = '\0';
        std::cout << "Server received: " << buffer << "\n";
        if (strcmp(buffer, "Accepting request") == 0)
            {
                std::cout << "Canged ttt\n";
                ttt=1;
            }
    }

    // Opening Sample file
    std::ifstream inputFile("sample.txt");
    if (!inputFile.is_open())
    {
        std::cerr << "Failed to open the file.\n";
        closesocket(clientSocket);
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Initialize speed
    int speed = 0;

    // Initialize flag to control communication in Thread 1
    int lst=1;

// Parallel region with two threads
#pragma omp parallel num_threads(2)
    {
#pragma omp sections
        {

// Section 1: Read file (handled by Thread 1)
#pragma omp section
            {
                std::string line;
                while (std::getline(inputFile, line))
                {
                    if (line == "Accelerate")
                    {
                        std::cout << "Parent Truck Accelerating:\n";
                        int t = 1;
                        //communicator(t, clientSocket);
                        myFIFOList.appendValue(1);
                        for (int i = 0; i < 5; ++i)
                        {
                            speed += 5;
                            std::cout << "Accelerating. Speed: " << speed << "\n";
                        }
                    }
                    else if (line == "Break")
                    {
                        std::cout << "Parent Truck Breaking/deaccelerating:\n";
                        myFIFOList.appendValue(3);
                        for (int i = 0; i < 5; ++i)
                        {
                            speed -= 5;
                            std::cout << "Speed: " << speed << "\n";
                        }
                    }
                     else if (line == "Turn")
                    {
                        myFIFOList.appendValue(6);
                        std::cout<<"Turning\n";
                        std::cout<<"Updating Turning angles and Location data in Server\n";

                    }
                    else if (line == "Stop")
                    {
                        myFIFOList.appendValue(4);
                        std::cout<<"Journey Ended\n";
                        closesocket(serverSocket);
                        break;
                    }
                    else
                    {
                        std::cout << "Parent moving in Constant Velocity\n";
                        myFIFOList.appendValue(2);
                        for (int i = 0; i < 3; ++i)
                        {
                            std::cout << "Speed: " << speed << "\n";
                        }
                    }
                    //Distance Check(5);
                    myFIFOList.appendValue(5);

                }
            }
// Section 0: Communication (handled by Thread 0)
#pragma omp section
            {
                while (!myFIFOList.isEmpty())
                {
                  if(myFIFOList.isEmpty())
                  {
                    std::cout<<"List is empty";
                    lst=0;
                  }

                   int firstValue = myFIFOList.fetchFirstValue();
                   communicator(firstValue, clientSocket);
                }
            }

        }
    }

    // Close the sockets and clean up
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

// Definition of the communicator function
void communicator(int valueToSend, SOCKET clientSocket)
{
    // Send the received value to the client
    send(clientSocket, reinterpret_cast<const char *>(&valueToSend), sizeof(int), 0);

}
int commcheck(int valueToSend, SOCKET clientSocket)
{
    // Send the received value to the client
    int x=100;
    send(clientSocket, reinterpret_cast<const char *>(&x), sizeof(int), 0);
    int receivedNumber;
    recv(clientSocket, reinterpret_cast<char*>(&receivedNumber), sizeof(int), 0);
    return receivedNumber;

}
