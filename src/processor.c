#include "processor.h"

static const char const directive_tpl[] = "<: :>";
static const uint8_t directive_size = sizeof(directive_tpl);
static const uint8_t directive_locus = 2;
static const char escape_char = '\\';

static bool is_complete_directive(const struct processor_st* processor) {
    return processor->directive.cursor == directive_size;
}

bool match_directive(char c, struct processor_st* processor) {
    struct directive_st* d = &processor->directive;
    if (d->cursor == directive_locus) {
        d->c = c;
        return true;
    }

    if (c == directive_tpl[d->cursor]) {
        d->cursor++;
        return true;
    }
    return false;
}

void unmatch_partial_directive(const struct processor_st* processor) {
    for (int i = 0; i < processor->directive.cursor; ++i) {
        if (i == directive_locus) {
            fwrite(&processor->directive.c, 1, 1, processor->output_file);
        } else {
            fwrite(&directive_tpl[i], 1, 1, processor->output_file);
        }
    }
}

void process(struct processor_st* processor) {
    char c;
    if (fread(&c, 1, 1, processor->input_file) == 0) {
        processor->done = true;
        return;
    }

    if (processor->escaped) {
        processor->escaped = false;
        if (c != directive_tpl[0] && c != escape_char) {
            // Can only escape start of directive, and escape character. Other
            // characters will be silenced.
            return;
        }
    } else if (c == escape_char) {
        processor->escaped = true;
        return;
    } else if (match_directive(c, processor)) {
        if (is_complete_directive(processor)) {
            processor->directive_processor_impl(processor);
            processor->directive = (struct directive_st) { 0 };
            return;
        }
        // Potential directive; don't copy to output (for now).
        return;
    }

    if (processor->directive.cursor > 0) {
        // Dump the partial (but non-matching) directive to the output.
        unmatch_partial_directive(processor);
    }

    int res = fwrite(&c, 1, 1, processor->output_file);
    if (res == 0) {
        // TODO: Something went wrong when writing.
    }
}

