#define _XOPEN_SOURCE 500

#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "systemd/sd-daemon.h"

int
main(int argc, char *argv[])
{
	int interval;

	int location = argc;
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "---") == 0)
		{
			location = i;
			break;
		}
	}

	if (argc <= location + 1)
	{
		fprintf(stderr, "Must specify a command to wrap\n");
		exit(EXIT_FAILURE);
	}

	// Parse command
	char **watchdog_cmd = malloc(location * sizeof(char *));
	for (int i = 0; i < location - 1; i++)
		watchdog_cmd[i] = argv[i + 1];
	watchdog_cmd[location - 1] = NULL;

	// Parse wrapped command
	char **cmd = malloc((argc - location) * sizeof(char *));
	for (int i = 0; i < argc - location; i++)
		cmd[i] = argv[location + i + 1];
	cmd[argc - location - 1] = NULL;

	char *env = getenv("WATCHDOG_USEC");
	if (env == NULL) {
		fprintf(stderr, "WatchdogSec must be set\n");
		exit(EXIT_FAILURE);
	}
	interval = atoi(env) / 2;

	pid_t cpid = fork();
	if (cpid == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else if (cpid == 0)
	{
		if (execve(cmd[0], cmd, NULL) == -1)
		{
			perror("execve");
			exit(EXIT_FAILURE);
		}

		exit(EXIT_SUCCESS);
	}

	sd_notify(0, "READY=1");
	sd_notify(0, "WATCHDOG=1");

	while (1)
	{
		usleep(interval);

		int wstatus;
		pid_t wpid, w;

		wpid = fork();
		if (wpid == -1)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		else if (wpid == 0)
		{
			if (execve(watchdog_cmd[0], watchdog_cmd, NULL) == -1)
			{
				perror("execve");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			do {
				if ((w = waitpid(wpid, &wstatus, WUNTRACED | WCONTINUED)) == -1) {
					perror("waitpid");
					exit(EXIT_FAILURE);
				}
			} while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));

			sd_notifyf(0, "STATUS=watchdog returned with exit code %d\n",
				WEXITSTATUS(wstatus));

			if (WEXITSTATUS(wstatus) != 0)
				exit(EXIT_FAILURE);
		}
		
		sd_notify(0, "WATCHDOG=1");
	}

	return 0;
}
