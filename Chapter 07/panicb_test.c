#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <limits.h>

int main (int argc, char **argv)
{
  int b, fd = open(argv[1], O_RDONLY);

  if (fd < 0) {
    perror("open()");
    return EXIT_FAILURE;
  }

  if (ioctl(fd, 0, &b)) {
    perror("ioctl");
    close(fd);
    return EXIT_FAILURE;
  }

  printf ("button= %d\n", b);	

  close(fd);

  return EXIT_SUCCESS;
}
