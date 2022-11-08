#include "directives.h"
#include "executor.h"
#include "processor.h"
#include <stdlib.h>
#include <unistd.h>

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
        goto cleanup_etor;
    }

    fseek(processor->program_file, 0, SEEK_SET);
    size_t res = fread(program, 1, program_size, processor->program_file);
    if (res != program_size) {
        ERROR(processor, "Error when reading program into buffer"
                " (expected %lu bytes, got %lu)", program_size, res);
        goto cleanup_program;
    }

    res = write(etor->cmd_input_fd[1], program, program_size);
    if (res != program_size) {
        ERROR(processor, "Error when sending program to command"
                " (expected %lu bytes, sent %lu)", program_size, res);
        goto cleanup_program;
    }
    free(program);
    close(etor->cmd_input_fd[1]);

    char* result;
    int size;
    res = fscanf(etor->cmd_output_file, "%ms%n", &result, &size);
    if (res != 1) {
        ERROR(processor, "Error when reading output from command");
        goto cleanup_program;
    }

    fclose(etor->cmd_output_file);
    free(etor);

    fseek(processor->program_file, 0L, SEEK_SET);

    res = fwrite(result, 1, size, processor->current_out);
    if (res != size) {
        ERROR(processor, "Error when writing command output to current output"
                " (expected %d bytes, wrote %lu)", size, res);
        goto cleanup_program;
    }
    return;

cleanup_program:
    free(program);

cleanup_etor:
    free(etor);
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

