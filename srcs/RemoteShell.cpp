#include "RemoteShell.hpp"
#include "Client.hpp"

RemoteShell::RemoteShell (Client *client)
{
	this->client = client;
	this->initShell();
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

void RemoteShell::initShell()
{
	pid_t	pid = fork();
	if (pid == 0) {
		this->handleShell();
	} else if (pid < 0) {

	} else {
		int returnStatus;
    	waitpid(pid, &returnStatus, 0);
	}
}

std::string RemoteShell::getAbovePath()
{
	int				end	= this->path.length() - 1;
	std::string		above;

	while (end) {
		if (this->path[end] == '/') {
			break;
		}
		end--;
	}
	for (int i = 0; i < end; i++) {
		above += this->path[i];
	}

	return (above.length() > 0) ? above : "/";
}

bool RemoteShell::pathExist(std::string path)
{
	struct stat statStruct;
	stat(path.c_str(), &statStruct);

	if (S_ISDIR(statStruct.st_mode)) {
	    return true;
	} else {
		return false;
	}
}

void RemoteShell::handleChdir(std::string buf)
{
	char *position = strstr((char*)buf.c_str(), "cd");
	if (position != NULL) {
		std::vector<std::string> path = Utils::split(position, ' ');
		if (path.size() >= 2 && strcmp(path[1].c_str(), ".")) {
			std::string new_path = path[1];
			if (new_path[0] == '/' && this->pathExist(new_path)) {
				this->path = new_path;
			} else if (!strcmp(new_path.c_str(), "..")) {
				this->path = this->getAbovePath();
			} else {
				if (this->path.length() > 1 && this->pathExist(this->path + "/" + new_path)) {
					this->path = this->path + "/" + new_path;
				}
				else if (this->pathExist(this->path + new_path)) {
					this->path += new_path;
				}
			}
		}
	}
	chdir(this->path.c_str());
}

void RemoteShell::handleShell()
{
	char buffer[CLIENT_BUFFER];
	int res = 0;

	this->path = "/"; // start path on the remote shell
	this->printPrompt();
	memset(&buffer, 0, CLIENT_READ);
	while (true)
	{
		res = recv(this->client->fd, buffer, CLIENT_READ, 0);
		if (res)
		{
			std::string data = buffer;
			if (data[data.size() - 1] == '\n')
				data[data.size() - 1] = '\0';
			this->handleChdir(data);
			if (!strcmp(data.c_str(), "exit")) {
				exit(0);
			}
			redi::ipstream proc(buffer, redi::pstreams::pstdout | redi::pstreams::pstderr);
			std::string line;
			while (std::getline(proc.out(), line)) {
				line += "\n";
				send(this->client->fd, line.c_str(), line.length(), 0);
			}
			this->printPrompt();
		} else if (res <= 0) {
			exit(0);
		}
		memset(&buffer, 0, CLIENT_READ);
	}
}
