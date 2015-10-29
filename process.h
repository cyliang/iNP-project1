#pragma once
#include <vector>
#include <string>

bool create_process(
		int stdin_fd, int stdout_fd, int stderr_fd,
		const std::vector<std::string> &argv
);
