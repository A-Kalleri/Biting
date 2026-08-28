#include "reader.h"
#include "helper.h"

int main (void) {

    source_buf_t *sbuf = source_buf_constructor(256);
    load_file(sbuf, "test");

    for (;;) {

        if (read_to_buffer(sbuf) != 0) {
            break;
        }
        
        if (sbuf -> length == 0) {
            write_stderr("\nEOF\n");
            break;
        }

        write_stderr("%.*s", (int)sbuf -> length, sbuf -> buffer);

    }

    source_buf_destructor(&sbuf);
    return 0;

}