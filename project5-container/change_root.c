#define _GNU_SOURCE

#include "change_root.h"

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

/**
 * `change_root` uses the pivot_root syscall to change the root of the file
 * system to the given path.
 * In addition, it mounts the new `/dev` and `/proc`, and configures `PATH`.
 */
void change_root(const char* path) {
  /* before calling `pivot_root`, unmount `/proc` */
  if (umount2("/proc", MNT_DETACH) < 0) {
    err(1, "unmount proc");
  }

  /* now, use `pivot_root` */

  char old_path[PATH_MAX];
  char old_root[PATH_MAX];
  char new_root[PATH_MAX];

  realpath(path, new_root);
  sprintf(old_path, "%s/.old_root", new_root);
  realpath(old_path, old_root);

  if (mount(new_root, new_root, "bind", MS_BIND | MS_REC, "") < 0) {
    err(1, "Failed to bind mount new root");
  }
  if (mkdir(old_root, 0700) < 0 && errno != EEXIST) {
    err(1, "Failed to create a directory to store old root");
  }

  if (syscall(SYS_pivot_root, new_root, old_root) < 0) {
    err(1, "Failed to pivot_root");
  }

  chdir("/");

  if (umount2("/.old_root", MNT_DETACH) < 0) {
    err(1, "Failed to unmount old root");
  }
  if (rmdir("/.old_root") < 0) {
    err(1, "Failed to remove old root");
  }

  /* few more things to make the container work properly */

  // mount new /dev
  if (mount("devtmpfs", "/dev", "devtmpfs", MS_NOSUID | MS_RELATIME, NULL) <
      0) {
    err(1, "mount devtmpfs");
  }
  // mount new /proc
  if (mount("proc", "/proc", "proc",
            MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_RELATIME, NULL) < 0) {
    err(1, "mount proc");
  }
  // set `PATH` so container can execute commands
  setenv("PATH", "/bin:/sbin:/usr/bin:/usr/local/sbin:/usr/local/bin", 1);
}
