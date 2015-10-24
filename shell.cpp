#include <iostream>
#include <string>
#include <unistd.h>
using namespace std;

inline
bool prompt(string &line) {
	cout << "% ";
	getline(cin, line);
	return cin;
}

int main() {
	chdir(RAS_ROOT);
	cout
		<< "****************************************\n"
		<< "** Welcome to the information server. **\n"
		<< "****************************************" << endl;

	string input_line;
	while (prompt(input_line)) {
		cout << input_line << endl;
	}
}
