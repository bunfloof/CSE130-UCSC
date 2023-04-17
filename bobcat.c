/********************************************************************************* 
 * Joey Ma, jma363
 * 2023 Spring CSE130 project1
 * bobcat.c
 * Unix cat command written in C without OPTIONS
 * 
 * Notes:
 * - Used cat example from Linux Programming by Example textbook as starter code
 * - Handles double dash and single dash edge cases
 * - Handles last argument as double or single dash edge cases
 * - Handles broken pipe errors using errno.h dependency
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
#include <errno.h> // addition of this dependency for error codes

#define BUFFER_SIZE 1024 // theoretically speaking, the memory usage of the bobcat program should always stay below 1 MB, 
                         // as it uses a fixed-size buffer of 1024 bytes (1 KB) for reading and writing data.

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
        //warn("write");
        warn("write error");
        *error_occurred = true;
        if (errno == EPIPE) exit(EXIT_FAILURE); // exit if the error is a broken pipe
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

int main(int argc, char *argv[]) {
  bool error_occurred = false;
  bool double_dash = false;
  bool processed_file = false; // var to track if any files have been processed
  bool non_existent_file = false; // addresses non-existent files edge case

  if (argc == 1) { // no arguments provided, process stdin
    process_file(STDIN_FILENO, "-", &error_occurred);
  } else {
    for (int i = 1; i < argc; i++) {
      if (!double_dash && strcmp(argv[i], "--") == 0) { // encountered a double dash, mark it and continue
        double_dash = true;
        continue;
      }

      if (!double_dash && strcmp(argv[i], "-") == 0) { // single dash without a double dash, process stdin
        process_file(STDIN_FILENO, "-", &error_occurred);
        processed_file = true; // mark that a file has been processed
      } else {
        int fd = open(argv[i], O_RDONLY);

        if (fd < 0) { // if the file cannot be opened
          if (strcmp(argv[i], "-") != 0 || !double_dash) { // if it's not a single dash after a double dash, print a warning
            warn("%s", argv[i]);
            error_occurred = true;
            non_existent_file = true;
          } else { // if it's a single dash after a double dash, process stdin
            process_file(STDIN_FILENO, "-", &error_occurred);
            processed_file = true; // mark that a file has been processed
          }
        } else { // if the file can be opened, process it
          process_file(fd, argv[i], &error_occurred);
          close(fd);
          processed_file = true; // mark that a file has been processed
        }
      }
    }

    if (!processed_file && double_dash && !non_existent_file) {
      process_file(STDIN_FILENO, "-", &error_occurred); // if no files have been processed and a double dash was encountered, process stdin
    } else if (!processed_file && non_existent_file) {
      exit(EXIT_FAILURE);
    }
  }

  // set the exit status based on whether an error occurred during processing
  if (error_occurred) return EXIT_FAILURE;
  else return EXIT_SUCCESS;
}
