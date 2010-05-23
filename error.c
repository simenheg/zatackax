#include "error.h"

/**
 * Prints a file not found error message.
 *
 * @param filepath Path of the file that was not found.
 */
void fileNotFound(char *filepath)
{
    fprintf(stderr, "ERROR: Could not open file '%s'.\n", filepath);
}
