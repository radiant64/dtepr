#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "directives.h"
#include "executor.h"
#include "processor.h"
#include "version.h"

struct buffer_st {
    FILE* handle;
    char* data;
    size_t size;
};

struct buffer_st* create_buffer() {
    struct buffer_st* buf = malloc(sizeof(struct buffer_st));
    if (!buf) {
        return NULL;
    }

    buf->handle = open_memstream(&buf->data, &buf->size);
    if (!buf->handle) {
        free(buf);
        return NULL;
    }

    return buf;
}

void close_buffer(struct buffer_st* buf) {
    fclose(buf->handle);
    free(buf);
}

void print_usage(const char* name) {
    fprintf(stderr, "Usage: %s (-v  | <command> [ARG [ARG ...]])\n", name);
}

void print_version() {
    fprintf(stdout, "dtepr version %s\n", dtepr_version);
}

int main(int argc, const char** argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        exit(1);
    }

    if (argv[1][0] == '-' && argv[1][1] == 'v') {
        print_version();
        exit(0);
    }

    struct buffer_st* program_file = create_buffer();
    if (!program_file) {
        perror("Failed to create program buffer");
        exit(2);
    }

    struct processor_st processor = {
        { 0 },
        process_directive,
        fork_and_execute_factory,
        &argv[1],
        stdout,
        stdin,
        stdout,
        program_file->handle
    };

    while (!processor.done) {
        process(&processor);
    }
    if (processor.error.is_error) {
        fprintf(stderr, "Error while processing: %s", processor.error.message);
        exit(3);
    }

    return 0;
}

