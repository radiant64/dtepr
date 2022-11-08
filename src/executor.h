#pragma once

#include <stdio.h>

struct processor_st;

struct executor_st {
    int cmd_input_fd[2];
    int cmd_output_fd[2];
    FILE* cmd_output_file;
};

struct executor_st* fork_and_execute_factory(struct processor_st* processor);

struct executor_st* get_executor(struct processor_st* processor);

