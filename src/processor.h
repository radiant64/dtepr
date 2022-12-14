#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <string.h>

#define ERROR_MAXLEN 256
#define ERROR(p, msg, ...) do {\
        snprintf(p->error.message, ERROR_MAXLEN, msg,## __VA_ARGS__);\
        p->error.message_len = strlen(p->error.message);\
        p->error.is_error = true;\
    } while (0);

struct executor_st;

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
    struct executor_st* (*executor_factory)(struct processor_st* processor);
    const char** command;
    FILE* current_out;
    FILE* input_file;
    FILE* output_file;
    FILE* program_file;
    bool escaped;
    bool done;
    struct error_st error;
};

void process(struct processor_st* processor);

