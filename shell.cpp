#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <cstdlib>
#include <unistd.h>
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
		return false;
	}

	return true;
}

void parse_cmd(istream &cmd) {
	string arg;
	cout << "Not parsed yet!" << endl;
}
