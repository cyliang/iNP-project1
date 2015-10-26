#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
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
	/* Processing command's arguments. */
	string arg;
	vector<string> argv;
	while ((cmd >> ws) && !strchr("|!>", cmd.peek()) && (cmd >> arg)) {
		argv.push_back(arg);
	}

	/* Processing command's I/O redirection. */
	int newps_stdin = STDIN_FILENO;
	int newps_stdout = STDOUT_FILENO;
	int newps_stderr = STDERR_FILENO;

	bool file_redirection = false;
	while ((cmd >> ws) && strchr("|!>", cmd.peek())) {
		switch (cmd.get()) {
			case '>':
				cmd >> arg; // file name

				if (newps_stdout != STDOUT_FILENO)
					break;

				newps_stdout = open(arg.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 000777);

				if (newps_stdout == -1) {
					cout << "Cannot open file: " << arg << endl;
					return;
				}
				file_redirection = true;
				break;

			case '|':
				break;

			case '!':
				break;
		}
	}

	/* Launching the command. */
	if (create_process(newps_stdin, newps_stdout, newps_stderr, argv, env_variables["PATH"].c_str())) {
		int exit_status;
		wait(&exit_status);
	} else {
		cout << "Unknown command: [" << argv[0] << "]." << endl;
	}

	if (file_redirection) {
		close(newps_stdout);
	}
}
