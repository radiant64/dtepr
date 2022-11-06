#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define ERROR_MAXLEN 256
#define ERROR(p, msg) p->error = (struct error_st) { msg, strlen(msg), true }

struct directive_st {
    char c;
    uint8_t cursor;
};

struct error_st {
    char message[ERROR_MAXLEN];
    size_t message_len;
    bool is_error;
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
    struct error_st error;
};

void process(struct processor_st* processor);

