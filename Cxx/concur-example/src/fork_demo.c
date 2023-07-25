#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  pid_t pid;
  int v = 100;

  if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
    perror("signal");
    exit(EXIT_FAILURE);
  }
  printf("Before Fork, v: %d\n", v);
  pid = fork();
  printf("After Fork, PID: %jd, v: %d\n", (intmax_t)pid, v);
  switch (pid) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);
    case 0:
      puts("Child exiting.");
      exit(EXIT_SUCCESS);
    default:
      printf("Child is PID %jd\n", (intmax_t)pid);
      puts("Parent exiting.");
      exit(EXIT_SUCCESS);
  }
}
