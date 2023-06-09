# Project 1: Bobcat

<p align="center">
  <img src="https://user-images.githubusercontent.com/10496155/230681849-821e7da9-81b7-4b24-9bab-527969c8df51.jpg" width="200px" /><br>
  <span>Photo by <a href="https://www.flickr.com/photos/becker271/2955468015" rel="noreferrer" target="_blank">Bucker1999</a></span>
</p>

**GOAL**: This project serves as a refresher for building software in the C programming language. In particular, the project will involve a review of Linux system calls, buffering file I/O, memory management, etc.

**IMPORTANT**: Please read [SETUP](https://github.com/UCSC-CSE-130/project1-bobcat/wiki/SETUP) on setting up the development environment.

## Project Details

In this project, you will implement a command-line tool, `bobcat`. `bobcat` should work just like `cat`, except it does not support any of the options.

The command is invoked with zero or more file operands: `./bobcat [file ...]`. The `bobcat` reads files sequentially, writing them to the standard output. The file operands are processed in command-line order. If file is a single dash (`-`) or absent, `bobcat` reads from the standard input.

The `bobcat` utility exits 0 on success, and >0 if an error occurs.

## Examples

Here, we illustrate how your command should operate with examples.

Suppose that you are in a directory that has three files: `file1`, `file2`, and `file3`.

```bash
bobcat file1
```

will print the contents of `file1` to the standard output.

```bash
bobcat file1 - file2 - file3
```

will print the contents of file1, print data it receives from the standard input until it receives an EOF (`^D`) character, print the contents of file2, read and output contents of the standard input again, then finally output the contents of file3.

```bash
bobcat
```

will open the standard input, as file is absent, just like `bobcat -`.

```bash
bobcat file1 hello file2
```

will print the contents of file1. Because `hello` does not exist, it will print the error: `bobcat: hello: No such file or directory` to the standard error. The command should process the rest of the operands, and will print the contents of the `file2`. Because there was an error during the execution, the exit status of the command will be >0.

As stated, `bobcat` should work the same way as `cat`. When in doubt, check the behavior of `cat`.

## Notes

In addition to the functionality described above, `bobcat` should meet the following functionality and limitations:

- `bobcat` must be reasonably time efficient: it should buffer file input and output
- `bobcat` must be reasonably space efficient: it should use at most 1MB of memory regardless of input.
- `bobcat` must not leak any memory (i.e., it should free all memory that it allocates).
- `bobcat` should not crash
- `bobcat` must use the `read` and `write` system calls for reading a file and writing to the standard output.
  - In particular, it cannot use the following functions from the `stdio.h` library: `fwrite`, `fread`, variants of `put` and `get`.
- `bobcat` cannot use functions, like `system(3)`, that execute external programs.
- all source files must be formatted using `clang-format`. run `make format` to format `bobcat.c`.
- You may modify `Makefile` to add scripts, but when you submit, make sure that `make` produces the `bobcat` executable using `clang` with `-Wall -Wextra -Werror -std=c11 -pedantic` flags.

## Hints

- Use `read(2)` to read a file and `write(2)` to write to the standard output.
- Use `warn(3)` to produce warnings. Pass the file name to match the output of `cat`.
- To check the return code of your program, run your program until completion and then type `echo $?` in your terminal.
- Use `valgrind --leak-check=full` to check your program for memory leaks
- Make sure your code works on the environment described in [SETUP](https://github.com/UCSC-CSE-130/project1-bobcat/wiki/SETUP)
