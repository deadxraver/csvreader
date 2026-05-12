#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  int fd = -1;
  int ret = 0;
  if (argc != 2) {
    fprintf(stderr, "Wrong number of args\n");
    ret = EINVAL;
    goto end;
  }
  if ((fd = open(argv[1], O_RDONLY)) < 0) {
    fprintf(stderr, "Failed to open file %s\n", argv[1]);
    ret = ENOENT;
    goto end;
  }
  // TODO:
end:
  if (fd >= 0)
    close(fd);
  fd = -1;
  return ret;
}
