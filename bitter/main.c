#include <stddef.h>
#include <string.h>

#include "parse.h"
#include "helper.h"


static int has_bitb_extension(const char *filename) {
        const char *ext = strrchr(filename, '.');
        return ext != NULL && strcmp(ext, ".bitb") == 0;
}

int main (int argc, char **argv) {

        if (argc == 1) {
                write_stderr("Usage: bitter <file>\n");
                return 1;
        }

        int status = 0;

        for (char **arg = argv + 1; *arg != NULL; ++arg) {

                const char *file = *arg;
                if (!has_bitb_extension(file)) {
                        write_stderr("Unsupported file extension on '%s'.\n", file);
                        return 1;
                }

                parse_t *parser = parse_constructor(file);
                if (!parser) {
                        status = PAR_INIT_FAILED;
                        break;
                }

                int result = parse_start(parser);
                parse_destructor(&parser);

                if (result != 0) {
                        status = result;
                        break;
                }

        }

        return status;

}