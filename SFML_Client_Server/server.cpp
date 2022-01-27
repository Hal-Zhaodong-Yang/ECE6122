/*
Author: Zhaodong Yang
Class: ECE6122
Last Date Modified: 2021/11/8
Description:
This file is a practice for the using of SFML network. This is the server program.
*/

#include <iostream>
#include <SFML/Network.hpp>
#include <string>
#include <fstream>

int main(int argc, char* argv[]) 
{
	if (argc < 2) 
	{
		std::cout << "Invalid command line argument detected: ";
		for (int i = 1; i < argc; i++)
		{
			std::cout << " " << argv[i] << " ";
		}
		std::cout << std::endl << "Please check your values and press any key to end the program!";
		return 1;
	}

	std::string test;
	test = argv[1];
	bool invalid = false;
	for (std::string::size_type i = 0; i < test.size(); i++)
	{
		if (!isdigit(test[i])) {
			invalid = true;
		}
	}

	if (invalid == false)
	{
		int port = std::stoi(argv[1]);
		if (port < 61000 || port > 65535)
		{
			invalid = true;
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

	std::ofstream myfile;
	myfile.open("server.log", std::ios::app);

	sf::TcpListener listener;
	sf::SocketSelector selector;

	std::vector<sf::TcpSocket*> clients;

	char data[100];
	std::size_t received;
	bool server_active = true;

	// bind the listener to a port
	if (listener.listen(std::stoll(argv[1])) != sf::Socket::Done)
	{
		// error...
	}

	selector.add(listener);

	while (server_active) 
	{
		if (selector.wait()) 
		{
			if (selector.isReady(listener)) 
			{
				sf::TcpSocket* client = new sf::TcpSocket;
				listener.accept(*client);

				if (client->receive(data, 100, received) != sf::Socket::Done)
				{
					//no message
				}
				else 
				{
					myfile << data << std::endl;
				}
				clients.push_back(client);
				selector.add(*client);
			}
			else
			{
				for (int i = 0; i < clients.size(); i++)
				{
					if (selector.isReady(*clients[i]))
					{
						if (clients[i]->receive(data, 100, received) != sf::Socket::Done)
						{
							//no message
						}
						else
						{
							myfile << data << std::endl;
						}
					}
				}
			}
			
		}
	}

	for (auto i = clients.begin(); i != clients.end(); i++)
	{
		delete *i;
	}

	myfile.close();
	

	return 0;
}


