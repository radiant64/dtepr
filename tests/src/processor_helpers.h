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

void open_buffer(struct buffer_st* buffer) {
    buffer->handle = open_memstream(&buffer->data, &buffer->size);
}

void close_buffer(struct buffer_st* buffer) {
    fclose(buffer->handle);
    free(buffer->data);
}

void with_output(struct processor_st* processor) {
    open_buffer(&test_out);
    processor->output_file = test_out.handle;
    processor->current_out = test_out.handle;
}

void close_output() {
    close_buffer(&test_out);
}

