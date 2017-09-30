#include "RemoteShell.hpp"
#include "Client.hpp"

RemoteShell::RemoteShell (Client *client)
{
	this->client = client;
	this->handleShell();
}

RemoteShell::RemoteShell ( RemoteShell const & src )
{
	*this = src;
}

RemoteShell &				RemoteShell::operator=( RemoteShell const & rhs )
{
	return (*this);
}

RemoteShell::~RemoteShell ()
{
	return ;
}

std::ostream &				operator<<(std::ostream & o, RemoteShell const & i)
{

	return (o);
}

void RemoteShell::printPrompt()
{
	std::string print = "-> ";
	send(this->client->fd, print.c_str(), print.length(), 0);
}

void RemoteShell::handleShell()
{
	char buffer[CLIENT_BUFFER];
	int res = 0;

	this->printPrompt();
	memset(&buffer, 0, CLIENT_READ);
	while (true)
	{
		res = recv(this->client->fd, buffer, CLIENT_READ, 0);
		if (res)
		{
			std::string data = buffer;
			if (data[data.size() - 1] == '\n')
				data[data.size()- 1] = '\0';
			if (!strcmp(data.c_str(), "exit")) {
				break;
			}
			redi::ipstream proc(buffer, redi::pstreams::pstdout | redi::pstreams::pstderr);
			std::string line;
			while (std::getline(proc.out(), line)) {
				line += "\n";
				send(this->client->fd, line.c_str(), line.length(), 0);
			}
			this->printPrompt();
		}
		memset(&buffer, 0, CLIENT_READ);
	}
}
