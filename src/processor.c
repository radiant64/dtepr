#include "processor.h"

static const char const directive_tpl[] = "<: :>";
static const uint8_t directive_size = sizeof(directive_tpl) - 1;
static const uint8_t directive_locus = 2;
static const char escape_char = '\\';

static bool is_complete_directive(const struct processor_st* processor) {
    return processor->directive.cursor == directive_size;
}

void write_to_current(struct processor_st* processor, char c) {
    if (processor->current_out) {
         if (fwrite(&c, 1, 1, processor->current_out) == 0) {
             ERROR(processor, "Failed to write to output stream");
         }
    }
}

bool match_directive(char c, struct processor_st* processor) {
    struct directive_st* d = &processor->directive;
    if (d->cursor == directive_locus) {
        d->c = c;
        d->cursor++;
        return true;
    }

    if (c == directive_tpl[d->cursor]) {
        d->cursor++;
        return true;
    }
    return false;
}

void unmatch_partial_directive(struct processor_st* processor) {
    int res = 1;
    for (int i = 0; i < processor->directive.cursor; ++i) {
        if (i == directive_locus) {
            write_to_current(processor, processor->directive.c);
        } else {
            write_to_current(processor, directive_tpl[i]);
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
            // Escape breakout/invalid escape: Print the escape character.
            write_to_current(processor, escape_char);
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
        processor->directive = (struct directive_st) { 0 };
    }

    if (processor->current_out) {
        write_to_current(processor, c);
    }
}

