/*********************************************************************************
 * Joey Ma
 * 2023 Spring CSE130 project5
 * container.c
 * entry file for container
 *
 * Notes:
 * - Not tested on windows or docker container
 *
 * Usage:
 * sudo ./container [ID] [IMAGE] [CMD]...
 *
 * Citations:
 * - Dongjing tutor
 * - Rohan tutor
 *
 *********************************************************************************/

#define _GNU_SOURCE

#include <err.h>
#include <errno.h>
#include <linux/limits.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <wait.h>

#include "change_root.h"

#define CONTAINER_ID_MAX 16
#define CHILD_STACK_SIZE 4096 * 10

typedef struct container {
  char id[CONTAINER_ID_MAX];
  // TODO: Add fields
  char image[PATH_MAX];
  char** cmd;
  char cwd[PATH_MAX];
} container_t;

/**
 * `usage` prints the usage of `client` and exists the program with
 * `EXIT_FAILURE`
 */
void usage(char* cmd) {
  printf("Usage: %s [ID] [IMAGE] [CMD]...\n", cmd);
  exit(EXIT_FAILURE);
}

/**
 * `container_exec` is an entry point of a child process and responsible for
 * creating an overlay filesystem, calling `change_root` and executing the
 * command given as arguments.
 */
int container_exec(void* arg) {
  container_t* container = (container_t*)arg;
  // this line is required on some systems
  if (mount("/", "/", "none", MS_PRIVATE | MS_REC, NULL) < 0) {
    err(1, "mount / private");
  }

  // printf("Creating overlay filesystem...\n");

  // TODO: Create a overlay filesystem
  // `lowerdir`  should be the image directory: ${cwd}/images/${image}
  // `upperdir`  should be `/tmp/container/{id}/upper`
  // `workdir`   should be `/tmp/container/{id}/work`
  // `merged`    should be `/tmp/container/{id}/merged`
  // ensure all directories exist (create if not exists) and
  // call `mount("overlay", merged, "overlay", MS_RELATIME,
  //    lowerdir={lowerdir},upperdir={upperdir},workdir={workdir})`

  // overlayFS directory paths
  // --------------------------------------------------
  char lowerdir[PATH_MAX];  // base image directory path for the container
                            // filesystem
  char upperdir[PATH_MAX];  // directory where changes to the filesystem will be
                            // written
  char workdir[PATH_MAX];   // working directory for the overlay filrsystem
  char merged[PATH_MAX];  // directory that will present the merged view of the
                          // filesystem

  // Constructors
  // ---------------------------------------------------------------
  sprintf(lowerdir, "%s/images/%s", container->cwd, container->image);
  sprintf(upperdir, "/tmp/container/%s/upper", container->id);
  sprintf(workdir, "/tmp/container/%s/work", container->id);
  sprintf(merged, "/tmp/container/%s/merged", container->id);

  char* dirs[] = {lowerdir, upperdir, workdir,
                  merged};  // array of dir paths needed to be created
  for (int i = 0; i < (int)(sizeof(dirs) / sizeof(char*));
       ++i) {                     // loop through everything in dirs arr
    char* dir = dirs[i];          // get current dir path
    char* tmp_str = strdup(dir);  // mutable copy of dir path

    for (char* p = tmp_str + 1; *p;
         p++) {         // loop through each character in dir path
      if (*p == '/') {  // if current character is '/' that means a subdirectory
        *p = '\0';  // temporarily end the string sitting here to isolate subdir
        if (mkdir(tmp_str, S_IRWXU) != 0 &&
            errno != EEXIST) {  // permission mod S_IRWXU-try to create subdir;
                                // if exists, then ignore error
          perror("mkdir error");
          return -1;
        }
        *p = '/';  // restore '/' in the directory path before continuing
      }
    }  // proceed below after all subdirs created -------------------

    if (mkdir(tmp_str, S_IRWXU) != 0 &&
        errno != EEXIST) {  // create the final dir in the path
      perror("mkdir error");
      return -1;
    }

    struct stat st = {0};  // check if final dir exists
    if (stat(tmp_str, &st) == -1) {
      printf("%s does not exist\n", tmp_str);
      return -1;
    }

    free(tmp_str);  // free temp mutable copy of dir path
  }

  char options[PATH_MAX];  // construct options string for overlayFS
  sprintf(options, "lowerdir=%s,upperdir=%s,workdir=%s", lowerdir, upperdir,
          workdir);

  // printf("Mounting overlay filesystem with options: %s\n", options);

  if (mount("overlay", merged, "overlay", MS_RELATIME, options) < 0) {
    err(1, "Failed to mount overlay filesystem");
  }

  // TODO: Call `change_root` with the `merged` directory
  // printf("Changing root...\n");
  change_root(merged);

  // TODO: use `execvp` to run the given command and return its return value
  // printf("Executing command...\n");
  if (execvp(container->cmd[0], container->cmd) < 0) {
    err(1, "Failed to execute cummand");
  }

  return 0;
}

int main(int argc, char** argv) {
  if (argc < 4) {
    usage(argv[0]);
  }

  /* Create tmpfs and mount it to `/tmp/container` so overlayfs can be used
   * inside Docker containers */
  if (mkdir("/tmp/container", 0700) < 0 && errno != EEXIST) {
    err(1, "Failed to create a directory to store container file systems");
  }
  if (errno != EEXIST) {
    if (mount("tmpfs", "/tmp/container", "tmpfs", 0, "") < 0) {
      err(1, "Failed to mount tmpfs on /tmp/container");
    }
  }

  /* cwd contains the absolute path to the current working directory which can
   * be useful constructing path for image */
  char cwd[PATH_MAX];
  getcwd(cwd, PATH_MAX);

  // TODO: store all necessary information to `container`
  container_t container;
  strncpy(container.id, argv[1], CONTAINER_ID_MAX);
  strncpy(container.image, argv[2], PATH_MAX);
  container.cmd = argv + 3;
  getcwd(container.cwd, PATH_MAX);  // store cwd in container

  /* Use `clone` to create a child process */
  char child_stack[CHILD_STACK_SIZE];  // statically allocate stack for child
  int clone_flags = SIGCHLD | CLONE_NEWNS | CLONE_NEWPID;
  int pid = clone(container_exec, &child_stack, clone_flags, &container);
  if (pid < 0) {
    err(1, "Failed to clone");
  }

  waitpid(pid, NULL, 0);
  return EXIT_SUCCESS;
}
