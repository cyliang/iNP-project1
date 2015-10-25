#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <signal.h>
#include "pipe.h"
#include "process.h"
using namespace std;

inline
bool prompt(string &line) {
	cout << "% ";
	getline(cin, line);
	return cin;
}

bool special_cmd(istream &cmd);
void parse_cmd(istream &cmd);

map<string, string> env_variables;

int main() {
	chdir(RAS_ROOT);
	signal(SIGCHLD, SIG_IGN);
	env_variables["PATH"] = "bin:.";

	cout
		<< "****************************************\n"
		<< "** Welcome to the information server. **\n"
		<< "****************************************" << endl;

	string input_line;
	while (prompt(input_line)) {
		istringstream iss(input_line);

		if (!special_cmd(iss))
			parse_cmd(iss);
	}
}

bool special_cmd(istream &cmd) {
	string arg;

	cmd >> arg;
	if (arg == "exit") {
		exit(0);
	} else if (arg == "printenv") {
		cmd >> arg;
		if (env_variables.count(arg))
			cout << arg << "=" << env_variables[arg] << endl;
	} else if (arg == "setenv") {
		string arg2;

		cmd >> arg >> arg2;
		env_variables[arg] = arg2;
	} else {
		cmd.seekg(0);
		return false;
	}

	return true;
}

void parse_cmd(istream &cmd) {
	string arg;
	vector<string> argv;
	while ((cmd >> arg) && !strchr("|!>", arg[0])) {
		argv.push_back(arg);
	}

	if (!create_process(STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO, argv, env_variables["PATH"].c_str())) {
		cout << "Unknown command: [" << argv[0] << "]." << endl;
		return;
	}

	int exit_status;
	wait(&exit_status);
}
