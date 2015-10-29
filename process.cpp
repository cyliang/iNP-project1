#include "process.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <cstdlib>

inline
void _convert_argv(const char *argv[], const std::vector<std::string> &vec) {
	for (int i=0; i<vec.size(); i++) {
		argv[i] = vec[i].c_str();
	}

	argv[vec.size()] = NULL;
}

bool create_process(int in_fd, int out_fd, int err_fd, const std::vector<std::string> &argv_vector) {
	const char *argv[argv_vector.size() + 1];
	_convert_argv(argv, argv_vector);

	int pid;
	int check_pipe[2];
	pipe(check_pipe);

	if ((pid = fork()) == 0) {
		// child
		close(check_pipe[0]);
		fcntl(check_pipe[1], F_SETFD, FD_CLOEXEC);

		dup2(in_fd, STDIN_FILENO);
		dup2(out_fd, STDOUT_FILENO);
		dup2(err_fd, STDERR_FILENO);

		execvp(argv[0], (char **) argv);

		write(check_pipe[1], (void *) &errno, sizeof(errno));
		close(check_pipe[1]);
		exit(-1);
	}

	// parent
	typeof(errno) child_err;
	close(check_pipe[1]);
	if (read(check_pipe[0], (void *) &child_err, sizeof(child_err))) {
		// The child fail to exec
		close(check_pipe[0]);
		int exit_status;
		waitpid(pid, &exit_status, 0);
		return false;
	}

	close(check_pipe[0]);
	return true;
}
