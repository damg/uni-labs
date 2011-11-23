#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

void start_conv(void);
void start_log(void);
void start_statistics(void);
void start_report(void);

int
main(int argc, char *argv[])
{
  pid_t pid;

  srand(time(0));

  pid = fork();
  if (pid)
    start_log();

  pid = fork();
  if (pid)
    start_statistics();

  start_conv();
  return EXIT_SUCCESS;
}

void start_conv(void)
{
  int num;

  while(1)
    {
      num = rand() % sizeof(unsigned char);
      usleep(300000);
    }
}

void start_log(void)
{
  while(1)
    {
    }
}

void start_report(void)
{
  while(1)
    {
    }
}

void start_statistics(void)
{
  pid_t pid;

  pid = fork();
  if (pid)
    start_report();

  while(1)
    {
    }
}

