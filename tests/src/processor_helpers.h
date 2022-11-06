#pragma once
#include <processor.h>

struct buffer_st {
    FILE* handle;
    char* data;
    size_t size;
};

FILE* test_in;
struct buffer_st test_out;

void with_input(struct processor_st* processor, const char* text) {
    test_in = fmemopen((void*) text, strlen(text), "r");
    processor->input_file = test_in;
}

void close_input() {
    fclose(test_in);
}

void with_output(struct processor_st* processor) {
    test_out.handle = open_memstream(&test_out.data, &test_out.size);
    processor->output_file = test_out.handle;
    processor->current_out = test_out.handle;
}

void close_output() {
    fclose(test_out.handle);
    free(test_out.data);
}

void process_and_flush(struct processor_st* processor) {
    while (!processor->done) {
        process(processor);
    }
    fflush(test_out.handle);
}

