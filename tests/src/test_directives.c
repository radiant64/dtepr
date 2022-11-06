#include <testdrive.h>
#include <directives.h>

#include "processor_helpers.h"

FIXTURE(test_directives, "Directives")
    struct processor_st pr = { 0 };
    with_output(&pr);

    SECTION("Unsupported directive causes error")
        pr.directive.c = 'f';
        process_directive(&pr);
        REQUIRE(pr.error.is_error);
        REQUIRE(strcmp(pr.error.message, "Unsupported directive: f") == 0);
    END_SECTION

    close_output();
END_FIXTURE

int main(int argc, char** argv) {
    return RUN_TEST(test_directives);
}
