#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
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

int main() {
	signal(SIGCHLD, SIG_IGN);
	setenv("PATH", "bin:.", 1);

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
		const char *val = getenv(arg.c_str());
		if (val)
			cout << arg << "=" << val << endl;
	} else if (arg == "setenv") {
		string arg2;

		cmd >> arg >> arg2;
		setenv(arg.c_str(), arg2.c_str(), 1);
	} else {
		cmd.seekg(0);
		return false;
	}

	return true;
}

inline
vector<string> get_argv(istream &cmd) {
	string arg;
	vector<string> argv;
	while ((cmd >> ws) && !strchr("|!>", cmd.peek()) && (cmd >> arg)) {
		argv.push_back(arg);
	}

	return argv;
}

void parse_cmd(istream &cmd) {
	int running_ps = 0;
	bool terminate = false;
	Pipe *inPipe = NULL, *outPipe = NULL;
	
	while (cmd && !terminate) {
		/* Processing command's arguments. */
		vector<string> argv = get_argv(cmd);

		/* Processing command's I/O redirection. */
		int newps_stdin;
		int newps_stdout = STDOUT_FILENO;
		int newps_stderr = STDERR_FILENO;

		inPipe = outPipe;
		outPipe = NULL;
		if (!running_ps) {
			if ((newps_stdin = Pipe::front()) == -1)
				newps_stdin = STDIN_FILENO;
		} else {
			newps_stdin = inPipe->read_fd();
		}

		int stdout_at = -1;
		int stderr_at = -1;
		bool file_redirection = false;
		while ((cmd >> ws) && strchr("|!>", cmd.peek())) {
			switch (cmd.get()) {
				case '|':
					if (isdigit(cmd.peek())) {
						// Number pipe
						int pipe_at;
						cmd >> pipe_at;

						if (newps_stdout == STDOUT_FILENO) {
							stdout_at = pipe_at;
							newps_stdout = Pipe::append_at(pipe_at);
						}
					} else if (newps_stdout == STDOUT_FILENO) {
						outPipe = new Pipe();
						newps_stdout = outPipe->write_fd();
					}

					break;

				case '!':
					if (isdigit(cmd.peek())) {
						int pipe_at;
						cmd >> pipe_at;

						if (newps_stderr == STDERR_FILENO) {
							stderr_at = pipe_at;
							newps_stderr = Pipe::append_at(pipe_at);
						}
					}
					break;

				case '>':
					string filename;
					cmd >> filename; // file name

					if (newps_stdout != STDOUT_FILENO)
						break;

					newps_stdout = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 000777);

					if (newps_stdout == -1) {
						cout << "Cannot open file: " << filename << endl;
						delete inPipe;
						return;
					}
					file_redirection = true;
					break;
			}
		}

		/* Launching the command. */
		if (create_process(newps_stdin, newps_stdout, newps_stderr, argv)) {
			running_ps++;
		} else {
			if (stdout_at != -1)
				Pipe::cancel_at(stdout_at);
			if (stderr_at != -1)
				Pipe::cancel_at(stderr_at);

			cout << "Unknown command: [" << argv[0] << "]." << endl;
			terminate = true;
		}

		delete inPipe;
		if (file_redirection) {
			close(newps_stdout);
		}

		terminate |= file_redirection || stdout_at != -1 || stderr_at != -1;
	}

	delete outPipe;
	if (running_ps)
		Pipe::pop();

	int exit_status;
	while (running_ps--)
		wait(&exit_status);
}
