#include "directives.h"
#include "processor.h"

#include <strings.h>

void process_directive(struct processor_st* processor) {
    switch (processor->directive.c) {
    case 'a':
        break;
    case 'e':
        break;
    case 'i':
        break;
    case 'n':
        break;
    case 'p':
        break;
    case 'x':
        break;
    default:
        ERROR(processor, "Unsupported directive: %c", processor->directive.c);
        break;
    }
}

