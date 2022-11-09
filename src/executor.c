#include "executor.h"
#include "processor.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static struct executor_st* executor = NULL;

static void free_executor() {
    free(executor);
}

struct executor_st* fork_and_execute_factory(struct processor_st* processor) {
    struct executor_st* etor = malloc(sizeof(struct executor_st));
    int cmd_input_fd[2];
    int cmd_output_fd[2];
    if (pipe(etor->cmd_input_fd) == -1) {
        ERROR(processor, "Failed to create pipe: %s", strerror(errno));
        return NULL;
    }
    if (pipe(etor->cmd_output_fd) == -1) {
        ERROR(processor, "Failed to create pipe: %s", strerror(errno));
        return NULL;
    }

    pid_t pid = fork();
    switch (pid) {
    case -1:
        ERROR(processor, "Failed to fork child process: %s", strerror(errno));
        return NULL;
    case 0: // Child
        close(etor->cmd_input_fd[1]); // Close for writing.
        // Duplicate input file descriptor to fd 0 (stdin) and close the
        // (now redundant) file descriptor.
        dup2(etor->cmd_input_fd[0], 0);
        close(etor->cmd_input_fd[0]);
        close(etor->cmd_output_fd[0]); // Close for reading.
        dup2(etor->cmd_output_fd[1], 1);
        close(etor->cmd_output_fd[1]);
        execvp(processor->command[0], (char* const*) processor->command);
        // If this context still exists, exec() failed.
        ERROR(processor, "Failed to execute command: %s", strerror(errno));
        return NULL;
    default: // Parent
        close(etor->cmd_input_fd[0]); // Close for reading.
        close(etor->cmd_output_fd[1]); // Close for writing.
        etor->cmd_output_file = fdopen(etor->cmd_output_fd[0], "r");
        atexit(free_executor);
        return etor;
    }

}

