#pragma once
#include <deque>

class Pipe {
public:
	Pipe();
	~Pipe();
	int write_fd();
	int read_fd(); // return -1 if pipe isn't required
	void cancel_write();

	static int append_at(int at);
	static void cancel_at(int at);
	static int front(); // return -1 if pipe isn't required
	static void pop();

private:
	int pipe_fd[2]; // read; write
	int input_count;
	static std::deque<Pipe> pending_pipes;
};
