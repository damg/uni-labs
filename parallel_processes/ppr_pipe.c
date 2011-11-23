#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>

void start_conv(void);
void start_log(void);
void start_statistics(void);
void start_report(void);

void log_sighandler(int sig);

static int pipe_conv_to_log[2];
static int pipe_conv_to_statistics[2];
static int pipe_statistics_to_report[2];

static FILE *log_f;

int
main(int argc, char *argv[])
{
  pid_t pid;

  srand(time(0));

  pipe(pipe_conv_to_log);
  pipe(pipe_conv_to_statistics);
  pipe(pipe_statistics_to_report);

  pid = fork();
  if (!pid)
    {
      start_log();
    }
  else
    {
      close(pipe_conv_to_log[0]);
    }

  pid = fork();
  if (!pid)
    {
      start_statistics();
    }
  else
    {
      close(pipe_conv_to_statistics[0]);
    }

  start_conv();
  return EXIT_SUCCESS;
}

void start_conv(void)
{
  int num;

  while(1)
    {
      num = rand() % 256;
      usleep(300000);

      write(pipe_conv_to_log[1], &num, sizeof(num));
      write(pipe_conv_to_statistics[1], &num, sizeof(num));
    }
}

void start_log(void)
{
  int num;

  close(pipe_conv_to_log[1]);

  log_f = fopen("/tmp/ppr.log", "w");
  signal(SIGINT, log_sighandler);

  while(1)
    {
      read(pipe_conv_to_log[0], &num, sizeof(num));
      fprintf(log_f, "[%ld] %d\n", time(0), num);
      fflush(log_f);
    }
}

void start_report(void)
{
  close(pipe_statistics_to_report[1]);

  while(1)
    {
    }
}

void start_statistics(void)
{
  pid_t pid;
  int num;

  close(pipe_conv_to_statistics[1]);

  pid = fork();
  if (!pid)
    {
      start_report();
    }
  else
    {
      close(pipe_statistics_to_report[0]);
    }

  while(1)
    {
      read(pipe_statistics_to_report[0], &num, sizeof(num));
    }
}

void log_sighandler(int sig)
{
  close(pipe_conv_to_log[0]);
  fclose(log_f);
}
