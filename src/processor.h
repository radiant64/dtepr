#pragma once

#include <stdbool.h>
#include <stdint.h>

struct directive_st {
    char c;
    uint8_t cursor;
};

struct processor_st {
    struct directive_st directive;
    int output_fd;
    int program_fd;
    int arg_fd;
    bool escaped;
    bool done;
};

void process();

