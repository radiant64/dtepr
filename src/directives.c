#include "directives.h"
#include "executor.h"
#include "processor.h"
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_INIT 4096

void execute_program(struct processor_st* processor) {
    processor->current_out = processor->output_file;
    struct executor_st* etor = processor->executor_factory(processor);
    if (!etor) {
        ERROR(processor, "Failed to create executor");
        return;
    }

    size_t program_size = ftell(processor->program_file);
    char* program = malloc(program_size);
    if (!program) {
        ERROR(processor, "Failed to allocate buffer for executing program");
        return;
    }

    fseek(processor->program_file, 0, SEEK_SET);
    size_t res = fread(program, 1, program_size, processor->program_file);
    if (res != program_size) {
        ERROR(processor, "Error when reading program into buffer"
                " (expected %lu bytes, got %lu)", program_size, res);
        free(program);
        return;
    }

    res = write(etor->cmd_input_fd[1], program, program_size);
    if (res != program_size) {
        ERROR(processor, "Error when sending program to command"
                " (expected %lu bytes, sent %lu)", program_size, res);
        free(program);
        return;
    }
    free(program);
    close(etor->cmd_input_fd[1]);

    // Read back program output into char buffer.
    char* out = malloc(BUF_INIT);
    if (!out) {
        ERROR(processor, "Error when allocating buffer for program output");
        return;
    }
    char* cursor = out;
    size_t bufsize = BUF_INIT;
    size_t size;
    while (!feof(etor->cmd_output_file)) {
        size_t readlen = bufsize - (cursor - out);
        res = fread(cursor, 1, readlen, etor->cmd_output_file);
        cursor += res;
        if (res != readlen) {
            break;
        }
        bufsize *= 2;
        char* new_out = realloc(out, bufsize);
        if (!new_out) {
            ERROR(processor, "Error when resizing buffer for program output");
            free(out);
            return;
        }
        size = cursor - out;
        out = new_out;
        cursor = out + size;
    }

    size = cursor - out;
    if (size) {
        // Strip away trailing whitespace.
        while (size > 1 && isspace(out[size - 1])) {
            size--;
        }

        res = fwrite(out, 1, size, processor->current_out);
        free(out);
        if (res != size) {
            ERROR(processor, "Error writing command output to current output");
            return;
        }
    }

    fclose(etor->cmd_output_file);
    free(etor);

    fseek(processor->program_file, 0L, SEEK_SET);
}

void process_directive(struct processor_st* processor) {
    switch (processor->directive.c) {
    case 'i':
        processor->current_out = NULL;
        break;
    case 'n':
        processor->current_out = processor->output_file;
        break;
    case 'p':
        processor->current_out = processor->program_file;
        break;
    case 'x':
        execute_program(processor);
        break;
    default:
        ERROR(processor, "Unsupported directive: %c", processor->directive.c);
        break;
    }
}

