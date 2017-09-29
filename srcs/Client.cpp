#include "Client.hpp"
#include "Server.hpp"

Client::Client ( int fd, struct sockaddr_in	&in, Server *server)
{
	this->fd = fd;
	this->in = in;
	this->address = inet_ntoa(this->in.sin_addr);
	this->server = server;

	std::thread thread(Client::clientThread, this);
	thread.detach();
}

std::string Client::getAddress()
{
	return this->address;
}

Client::Client ( Client const & src )
{
	*this = src;
	return ;
}

Client &				Client::operator=( Client const & rhs )
{
	if (this != &rhs)
	{
	}
	return (*this);
}

Client::~Client ( void )
{
	return ;
}

std::ostream &				operator<<(std::ostream & o, Client const & i)
{
	(void)i;
	return (o);
}

void Client::clientThread(Client *client)
{
	char buffer[CLIENT_BUFFER];
	int res = 0;
	while (true)
	{
		res = recv(client->fd, buffer, CLIENT_READ, 0);
		std::string data = buffer;
		if (data[data.size() - 1] == '\n')
			data[data.size()- 1] = '\0';
		if (res) {
			send(client->fd, "SLT", 3, 0);
			memset(&buffer, 0, CLIENT_READ);
		} else if (res <= 0) {
			client->server->removeClient(client);
			break;
		}
	}
}
