#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <conio.h>
#include <cstdlib>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

	// connect to server
	/*	const unsigned short port = 50001;
		std::string serverIp = "127.0.0.1";
		sf::IpAddress server = serverIp;

		printf("Connecting to %s...\n", serverIp.c_str());
	
	// tell the server the client connected
		char sendMsg[1024];
		sprintf(sendMsg, "+Inzuki");
	
		sf::UdpSocket socket;
		socket.send(sendMsg, sizeof(sendMsg), server, port);
		socket.setBlocking(false);

	// receive from the server if the client successfully connected
		char buffer[1024];
		std::size_t received;
		sf::IpAddress sender;
		unsigned short senderPort;
		socket.receive(buffer, sizeof(buffer), received, sender, senderPort);

		// if the client successfully connected, the server should send back a success signal
		if(buffer[0] == '0'){
			char buff[1024];
			for(int i = 0; i < sizeof(buffer); i++)
				buff[i] = buffer[i + 1];

			ID = atoi(buff);

			printf("Connected to %s successfully (given ID %i).\n", serverIp.c_str(), ID);
		}

		// socket.setBlocking(false);*/