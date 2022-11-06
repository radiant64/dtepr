#include <testdrive.h>
#include <processor.h>

#include <string.h>

struct buffer_st {
    FILE* handle;
    char* data;
    size_t size;
};

FILE* test_in;
struct buffer_st test_out;
char processed_directive;

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

void test_directive_processor(struct processor_st* processor) {
    processed_directive = processor->directive.c;
}

FIXTURE(test_processor, "Processor")
    struct processor_st processor = { { 0 }, test_directive_processor };
    with_output(&processor);

    SECTION("Reading EOF finishes processing")
        with_input(&processor, "");
        process(&processor);
        REQUIRE(processor.done);
        close_input();
    END_SECTION

    SECTION("Normal text is copied to output")
        with_input(&processor, "test1234");
        process_and_flush(&processor);
        REQUIRE(test_out.size == 8);
        REQUIRE(strcmp(test_out.data, "test1234") == 0);
        close_input();
    END_SECTION

    SECTION("Escaped characters")
        SECTION("Escaped normal character outputs nothing")
            with_input(&processor, "\\e");
            process_and_flush(&processor);
            REQUIRE(test_out.size == 0);
            close_input();
        END_SECTION

        SECTION("Escaped escape character outputs escape character")
            with_input(&processor, "\\\\");
            process_and_flush(&processor);
            REQUIRE(test_out.size == 1);
            REQUIRE(strcmp(test_out.data, "\\") == 0);
            close_input();
        END_SECTION
        
        SECTION("Escaped initial directive character outputs character")
            with_input(&processor, "\\<");
            process_and_flush(&processor);
            REQUIRE(test_out.size == 1);
            REQUIRE(strcmp(test_out.data, "<") == 0);
            close_input();
        END_SECTION
    END_SECTION
    
    SECTION("Directive markers")
        SECTION("Partial marker copies matching part to output")
            with_input(&processor, "<:a:no");
            process_and_flush(&processor);
            REQUIRE(test_out.size == 6);
            REQUIRE(strcmp(test_out.data, "<:a:no") == 0);
            close_input();
        END_SECTION

        SECTION("Complete marker invokes directive processor")
            with_input(&processor, "<:a:>test");
            process_and_flush(&processor);
            REQUIRE(processed_directive == 'a');
            
            SECTION("Text after directive marker is copied to output")
                REQUIRE(test_out.size == 4);
                REQUIRE(strcmp(test_out.data, "test") == 0);
            END_SECTION
        END_SECTION
    END_SECTION

    close_output();
END_FIXTURE

int main(void) {
    return RUN_TEST(test_processor);
}

