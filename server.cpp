#include <cstring> 
#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <thread>
#include "position.h"

using namespace std; 
  
void handleClient(int clientSocket, int opponentClientSocket) {
    Position position;
    char buffer[512];
    while (true) {
        // Exchange positions between clients
        

        // Receive position from client 1
        int bytesRead = recv(clientSocket, &position, sizeof(Position), 0);
        if (bytesRead == -1) {
            std::cerr << "Error receiving position from client 1" << std::endl;
        } else {
            buffer[bytesRead] = '\0';
            std::cout << "Received position from client 1: " << position.x << std::endl;

            // Check for the exit condition
            if (strstr(buffer, "exit") != nullptr) {
                std::cout << "Received exit message from client 1. Exiting the loop." << std::endl;
                break;  // Exit the loop
            }

            // Send position to client 2
            send(opponentClientSocket, &position, sizeof(Position), 0);
        }
    }

    close(clientSocket);

}
int main() 
{ 
    // creating socket 
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); 

    if (serverSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }
  
    // specifying the address 
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(8080); 
    serverAddress.sin_addr.s_addr = INADDR_ANY; 
  
    // binding socket. 
    bind(serverSocket, (struct sockaddr*)&serverAddress, 
         sizeof(serverAddress)); 
  
    // listening to the assigned socket 
    listen(serverSocket, 2); 

    // Accept the first client
    sockaddr_in client1Address;
    socklen_t client1AddrSize = sizeof(client1Address);
    int client1Socket = accept(serverSocket, (struct sockaddr*)&client1Address, &client1AddrSize);
    if (client1Socket == -1) {
        std::cerr << "Error accepting connection from client 1" << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "Client 1 connected." << std::endl;
    
    // Accept the second client
    sockaddr_in client2Address;
    socklen_t client2AddrSize = sizeof(client2Address);
    int client2Socket = accept(serverSocket, (struct sockaddr*)&client2Address, &client2AddrSize);
    if (client2Socket == -1) {
        std::cerr << "Error accepting connection from client 2" << std::endl;
        close(serverSocket);
        close(client1Socket);
        return -1;
    }

    std::cout << "Client 2 connected." << std::endl;


    

    // Close the server socket (we don't need it in the loop)
    

    thread clientThread1(handleClient, client1Socket, client2Socket);

    thread clientThread2(handleClient, client2Socket, client1Socket);

    clientThread1.join();
    clientThread2.join();

    close(serverSocket);
  
    return 0; 
}