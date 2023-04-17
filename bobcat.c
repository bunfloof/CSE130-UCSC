/********************************************************************************* 
 * Joey Ma, jma363
 * 2023 Spring CSE130 project1
 * bobcat.c
 * Unix cat command written in C without OPTIONS
 * 
 * Notes:
 * - Used cat example from Linux Programming by Example textbook as starter code
 * - Keeping consistent convention for termination signals intially given in bobcat.c template
 * - Handles double dash and single dash edge cases; apologies if I overlooked any more edge cases
 * 
 * Usage:
 * ./bobcat [FILE]...
 *
 * Citations:
 * (1) Robbins, Arnold. "4.2. Presenting a Basic Program Structure." 
 *     Linux Programming by Example. Upper Saddle River, NJ: 
 *          Prentice Hall/PTR, 2004. Print.
 *
 * (2) GNU Core Utilities/cat.c
 *
*********************************************************************************/
#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

// helper function to process the file with the provided file descriptor (fd) and filename
void process_file(int fd, const char *filename, bool *error_occurred) {
  char buffer[BUFFER_SIZE];
  ssize_t rcount;

  while ((rcount = read(fd, buffer, BUFFER_SIZE)) > 0) { // read the file in chunks of BUFFER_SIZE
    ssize_t wcount = 0;
    while (wcount < rcount) { // write the read bytes to the standard output in a loop to handle partial writes
      ssize_t result = write(STDOUT_FILENO, buffer + wcount, rcount - wcount);

      if (result < 0) { // handle write errors
        //err(EXIT_FAILURE, "write");
        warn("write");
        *error_occurred = true;
        return;
      }

      wcount += result;
    }
  }

  if (rcount < 0) { // handle read errors
    warn("%s", filename);
    *error_occurred = true;
  }
}

int main(int argc, char * argv[]) {
  bool error_occurred = false;
  bool double_dash = false;
  bool processed_file = false; // var to track if any files have been processed

  if (argc == 1) { // no arguments provided, process stdin
    process_file(STDIN_FILENO, "-", & error_occurred);
  } else {
    for (int i = 1; i < argc; i++) {
      if (!double_dash && strcmp(argv[i], "--") == 0) { // encountered a double dash, mark it and continue
        double_dash = true;
        continue;
      }

      if (!double_dash && strcmp(argv[i], "-") == 0) { // single dash without a double dash, process stdin
        process_file(STDIN_FILENO, "-", & error_occurred);
        processed_file = true; // mark that a file has been processed
      } else {
        int fd = open(argv[i], O_RDONLY);

        if (fd < 0) { // if the file can not be opened
          if (strcmp(argv[i], "-") != 0 || !double_dash) { // if it's not a single dash after a double dash, print a warning
            warn("%s", argv[i]);
            error_occurred = true;
          } else { // if it's a single dash after a double dash, process stdin
            process_file(STDIN_FILENO, "-", & error_occurred);
            processed_file = true; // mark that a file has been processed
          }
        } else { // if the file can be opened, process it
          process_file(fd, argv[i], & error_occurred);
          close(fd);
          processed_file = true; // mark that a file has been processed
        }
      }
    }

    if (!processed_file && double_dash) { // if no files have been processed and a double dash was encountered, process stdin
      process_file(STDIN_FILENO, "-", & error_occurred);
    }
  }

  // set the exit status based on whether an error occurred during processing
  if (error_occurred) return EXIT_FAILURE;
  else return EXIT_SUCCESS;
}
