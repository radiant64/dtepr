#include <testdrive.h>
#include <directives.h>
#include <executor.h>
#include <processor.h>
#include <string.h>
#include <unistd.h>

#include "processor_helpers.h"

const char* test_command[] = {"sh", NULL};
const char testdoc_execute[] = "<:p:>echo test1234<:x:>";
static struct buffer_st test_program;

void process_and_flush(struct processor_st* processor) {
    while (!processor->done) {
        process(processor);
    }
    fflush(test_out.handle);
}

FIXTURE(integration_tests, "Integration")
    open_buffer(&test_out);
    open_buffer(&test_program);
    struct processor_st pr = {
        { 0 },
        process_directive,
        fork_and_execute_factory,
        test_command,
        test_out.handle,
        NULL,
        test_out.handle,
        test_program.handle
    };

    SECTION("Execution test")
        FILE* input_file = fmemopen(
                (void*) testdoc_execute, sizeof(testdoc_execute) - 1, "r");
        pr.input_file = input_file;
        process_and_flush(&pr);
        REQUIRE(strcmp("test1234", test_out.data) == 0);
        fclose(input_file);
    END_SECTION

    close_buffer(&test_program);
    close_buffer(&test_out);
END_FIXTURE

int main(int argc, char** argv) {
    return RUN_TEST(integration_tests);
}

