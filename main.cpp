#include <SFML/Graphics.hpp>
#include <cstring> 
#include <string>
#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include "position.h"
#include <arpa/inet.h>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Circle Move");

    sf::CircleShape circle1(50.0f);
    circle1.setFillColor(sf::Color::Green);

    sf::CircleShape circle2(50.0f); // Second circle
    circle2.setFillColor(sf::Color::Blue);
    Position ownPosition;
    ownPosition.x = 400.0f;
    ownPosition.y = 400.0f;
    circle1.setPosition(ownPosition.x, ownPosition.y);

    Position opponentPosition;
    opponentPosition.x = 400.0f;
    opponentPosition.y = 200.0f;
    circle2.setPosition(opponentPosition.x, opponentPosition.y);

    float speed = 5.0f;

    const sf::Time frameTime = sf::seconds(1.0f/30.0f);

    sf::Clock clock;
    sf::Time elapsed;

    // creating socket 
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0); 
  
    // specifying address 
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(8080); 
    serverAddress.sin_addr.s_addr = inet_addr("SERVER IP"); 
  
    // sending connection request 
    connect(clientSocket, (struct sockaddr*)&serverAddress, 
            sizeof(serverAddress)); 
  
    // sending data 
    const char* message = "Hello, server!"; 
    send(clientSocket, message, strlen(message), 0); 

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        elapsed = clock.restart();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            if (ownPosition.x >= 5.0f)
                ownPosition.x -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            if (ownPosition.x <= 695.0f)
                ownPosition.x += speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            if (ownPosition.y >= 5.0f)
                ownPosition.y -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            if (ownPosition.y <= 495.0f)
                ownPosition.y += speed;

        circle1.setPosition(ownPosition.x, ownPosition.y);


        // Send the position to the server
        send(clientSocket, &ownPosition, sizeof(Position), 0);

        // Receive the position of the other client
        Position buffer;
        int bytesRead = recv(clientSocket, &buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            std::cerr << "Error receiving data from the server" << std::endl;
            break;
        } else if (bytesRead == 0) {
            std::cout << "Server disconnected. Exiting the client." << std::endl;
            break;
        }
        opponentPosition.x = 800.0f - buffer.x;
        opponentPosition.y = 600.0f - buffer.y;
        std::cout << "Received position from the other client: " << std::endl;
        circle2.setPosition(opponentPosition.x, opponentPosition.y);

        window.clear();
        window.draw(circle1);
        window.draw(circle2);
        window.display();

        sf::sleep(frameTime - elapsed);
    }

    return 0;
}
