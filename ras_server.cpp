#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[]) {
	if(argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	char *execute_directory = getcwd(NULL, 0);
	char shell_path[strlen(execute_directory) + 10];
	sprintf(shell_path, "%s/shell", execute_directory);
	free(execute_directory);

	if (chdir(RAS_ROOT) == -1) {
		fprintf(stderr, "Unable to change current directory to \"%s\".\n", RAS_ROOT);
		exit(1);
	}
	signal(SIGCHLD, SIG_IGN);

	int listen_fd;
	struct sockaddr_in listen_addr;
	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	listen_addr.sin_port = htons(atoi(argv[1]));

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_fd == -1) {
		fputs("Cannot create socket.\n", stderr);
		exit(2);
	}
	if(bind(listen_fd, (struct sockaddr *) &listen_addr, sizeof(listen_addr)) == -1) {
		fputs("Cannot bind.\n", stderr);
		exit(2);
	}
	if(listen(listen_fd, 1024) == -1) {
		fputs("Cannot listen.\n", stderr);
		exit(2);
	}

	while(1) {
		int accept_fd = accept(listen_fd, NULL, NULL);
		if (accept_fd >= 0 && fork() == 0) {
			dup2(accept_fd, STDOUT_FILENO);
			dup2(accept_fd, STDERR_FILENO);
			dup2(accept_fd, STDIN_FILENO);

			close(accept_fd);
			close(listen_fd);

			execlp(shell_path, shell_path, NULL);
			fputs("Fail to start the shell", stderr);
			exit(3);
		}

		close(accept_fd);
	}
}
