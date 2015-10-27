#include "pipe.h"
#include <unistd.h>
#include <fcntl.h>

std::deque<Pipe> Pipe::pending_pipes(1000);

int Pipe::append_at(int at) {
	return pending_pipes[at].write_fd();
}

void Pipe::cancel_at(int at) {
	pending_pipes[at].cancel_write();
}

int Pipe::front() {
	return pending_pipes.front().read_fd();
}

void Pipe::pop() {
	pending_pipes.pop_front();
	pending_pipes.push_back(Pipe());
}

Pipe::Pipe(): input_count(0) {
}

int Pipe::write_fd() {
	if (!input_count) {
		pipe(pipe_fd);
		fcntl(pipe_fd[0], F_SETFD, FD_CLOEXEC);
		fcntl(pipe_fd[1], F_SETFD, FD_CLOEXEC);
	}

	input_count++;
	return pipe_fd[1];
}

void Pipe::cancel_write() {
	if (!--input_count) {
		close(pipe_fd[1]);
		close(pipe_fd[0]);
	}
}

int Pipe::read_fd() {
	return input_count ? pipe_fd[0] : -1;
}

Pipe::~Pipe() {
	if (input_count) {
		close(pipe_fd[1]);
		close(pipe_fd[0]);
	}
}
