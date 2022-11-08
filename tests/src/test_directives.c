#include <testdrive.h>
#include <directives.h>
#include <executor.h>

#include "processor_helpers.h"

static char dummy[] = "dummy";
static const char test_etor_out[] = "test1234";
static struct executor_st* test_etor = NULL;

static void close_etor() {
    fclose(test_etor->cmd_output_file);
    free(test_etor);
}

struct executor_st* test_executor_factory(struct processor_st* processor) {
    test_etor = malloc(sizeof(struct executor_st));
    *test_etor = (struct executor_st) {
        { 0 },
        { 0 },
        fmemopen((void*) test_etor_out, sizeof(test_etor_out) - 1, "r")
    };
    return test_etor;
}

FIXTURE(test_directives, "Directives")
    struct processor_st pr = { 0 };
    pr.executor_factory = test_executor_factory;
    with_output(&pr);

    SECTION("Unsupported directive causes error")
        pr.directive.c = 'f';
        process_directive(&pr);
        REQUIRE(pr.error.is_error);
        REQUIRE(strcmp(pr.error.message, "Unsupported directive: f") == 0);
    END_SECTION

    SECTION("Redirection directives")
        SECTION("'i' directive redirects to NULL")
            pr.directive.c = 'i';
            process_directive(&pr);
            REQUIRE(pr.current_out == NULL);
        END_SECTION
        SECTION("'n' directive redirects to regular output")
            pr.output_file = (FILE*) 123;
            pr.directive.c = 'n';
            process_directive(&pr);
            REQUIRE(pr.current_out == (FILE*) 123);
        END_SECTION
        SECTION("'p' directive redirects to program buffer")
            pr.program_file = (FILE*) 123;
            pr.directive.c = 'p';
            process_directive(&pr);
            REQUIRE(pr.current_out == (FILE*) 123);
        END_SECTION
    END_SECTION

    SECTION("Execute directive reads from executor's output")
        pr.program_file = fmemopen(dummy, sizeof(dummy) - 1, "r");
        pr.directive.c = 'x';
        process_directive(&pr);
        fflush(test_out.handle);
        REQUIRE(strcmp(test_etor_out, test_out.data) == 0);
    END_SECTION

    close_output();
END_FIXTURE

int main(int argc, char** argv) {
    return RUN_TEST(test_directives);
}
