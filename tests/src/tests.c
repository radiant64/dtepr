#include <testdrive.h>
#include <processor.h>

#include <string.h>

#define TEST_BUFFER_SIZE 1024

FILE* test_in;

struct buffer_st {
    FILE* handle;
    char* data;
    size_t size;
};

struct buffer_st test_out;
struct buffer_st test_pgm;
struct buffer_st test_arg;

void open_input(const char* text) {
    test_in = fmemopen((void*) text, strlen(text), "r");
}

void close_input() {
    fclose(test_in);
}

void with_input(struct processor_st* processor, const char* text) {
    open_input(text);
    processor->input_file = test_in;
}

void open_out(struct buffer_st* buffer) {
    buffer->handle = open_memstream(&buffer->data, &buffer->size);
}

void close_out(struct buffer_st* buffer) {
    fclose(buffer->handle);
    free(buffer->data);
}

void test_directive_processor(struct processor_st* processor) {
}

FIXTURE(test_processor, "Processor")
    struct processor_st processor = { 0 };

    SECTION("Reading EOF finishes processing")
        with_input(&processor, "");
        process(&processor);
        REQUIRE(processor.done);
        close_input();
    END_SECTION
END_FIXTURE

int main(void) {
    return RUN_TEST(test_processor);
}

