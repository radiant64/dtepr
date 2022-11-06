#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

struct directive_st {
    char c;
    uint8_t cursor;
};

struct processor_st {
    struct directive_st directive;
    void (*directive_processor_impl)(struct processor_st* processor);
    FILE* current_out;
    FILE* input_file;
    FILE* output_file;
    FILE* program_file;
    FILE* arg_file;
    bool escaped;
    bool done;
};

void process(struct processor_st* processor);

