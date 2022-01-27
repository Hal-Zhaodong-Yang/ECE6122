/*
Author: Zhaodong Yang
Class: ECE6122
Last Date Modified: 2021/11/8
Description:
This file is a practice for the using of SFML network. This is the client program.
*/

#include <iostream>
#include <SFML/Network.hpp>
#include <string>
#include <fstream>
#include <ctype.h>

int main(int argc, char* argv[]) 
{
	//invalid entries check
	if (argc < 3) 
	{
		std::cout << "Invalid command line argument detected: ";
		for (int i = 1; i < argc; i++) 
		{
			std::cout << " " << argv[i] << " ";
		}
		std::cout << std::endl << "Please check your values and press any key to end the program!";
		return 1;
	}

	std::string text;

	std::string test;
	test = argv[2];
	bool invalid = false;
	for (std::string::size_type i = 0; i < test.size(); i++) 
	{
		if (!isdigit(test[i])) {
			invalid = true;
		}
	}

	if (invalid == false) 
	{
		int port = std::stoi(argv[2]);
		if (port < 61000 || port > 65535) 
		{
			invalid = true;
		}
	}

	test = argv[1];
	for (std::string::size_type i = 0; i < test.size(); i++)
	{
		if (test == "localhost") {
			break;
		}
		if (!isdigit(test[i])) {
			if (test[i] != '.')
			{
				invalid = true;
			}
		}
	}

	if (invalid) 
	{
		std::cout << "Invalid command line argument detected: ";
		for (int i = 1; i < argc; i++)
		{
			std::cout << " " << argv[i] << " ";
		}
		std::cout << std::endl << "Please check your values and press any key to end the program!";
		return 1;
	}

	//client
	sf::TcpSocket socket;
	sf::Socket::Status status = socket.connect(argv[1], std::stoi(argv[2]));
	if (status != sf::Socket::Done)
	{
		// error...
		std::cout << "Failed to connect to the server at " << argv[1]
			<< " on " << argv[2] << std::endl <<
			"Please check your values and press any key to end program!" << std::endl;

		return 1;
	}


	while (1) {
		std::cout << "Please enter a message:" << std::endl;
		std::getline(std::cin, text);

		if (socket.send(text.c_str(), 100) != sf::Socket::Done)
		{
			// error...
			std::cout << "fail to send message" << std::endl;
		}
	}


	return 0;
}


