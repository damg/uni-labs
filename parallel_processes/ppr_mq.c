/*
    Copyright (C) 2011  Dmitri Bachtin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <libgen.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void main_sighandler(int);

void start_conv(void);
void start_log(void);
void start_statistics(void);
void start_report(void);

void report_sighandler(int);
void log_sighandler(int);

static int pipe_conv_to_log;
static int pipe_conv_to_statistics;
static int pipe_statistics_to_report;

extern int errno;

struct conv_msgbuf
{
  long mtype;
  int mnum;
};

struct stat_msgbuf
{
  long mtype;
  int mmin_num;
  int mmax_num;
  double mmean_num;
};

static char *app_name;

int
main(int argc, char *argv[])
{
  app_name = strdup(basename(argv[0]));
  signal(SIGINT, main_sighandler);

  pid_t pid;

  srand(time(0));

  pipe_conv_to_log = msgget(IPC_PRIVATE, IPC_CREAT | 0777);
  pipe_conv_to_statistics = msgget(IPC_PRIVATE, IPC_CREAT | 0777);
  pipe_statistics_to_report = msgget(IPC_PRIVATE, IPC_CREAT | 0777);

  pid = fork();
  if (!pid)
      start_log();

  pid = fork();
  if (!pid)
      start_statistics();

  start_conv();
  return EXIT_SUCCESS;
}

void start_conv(void)
{
  int num, i;
  struct conv_msgbuf buf;
  
  for(i = 0; i < 1000000; ++i)
//  while(1)
    {
      num = rand() % 256;

      buf.mtype = 1;
      buf.mnum = num;

      msgsnd(pipe_conv_to_log, &buf, sizeof(buf.mnum), 0);
      msgsnd(pipe_conv_to_statistics, &buf, sizeof(buf.mnum), 0);
    }
}

void start_log(void)
{
  char *log_file_path;
  static FILE *log_f;
  int num;
  size_t log_file_path_len;
  struct conv_msgbuf buf;

  log_file_path_len = strlen(app_name) + 9 + 1;
  log_file_path = malloc(log_file_path_len);
  memset(log_file_path, 0, log_file_path_len);
  sprintf(log_file_path, "/tmp/%s.log", app_name);
  fprintf(stderr, "Log file: %s\n", log_file_path);
  log_f = fopen(log_file_path, "w");
  free(log_file_path);

  signal(SIGINT, log_sighandler);

  while(1)
    {
      msgrcv(pipe_conv_to_log, &buf, sizeof(buf.mnum), 1, 0);
      num = buf.mnum;
      fprintf(log_f, "[%ld] %d\n", time(0), num);
      fflush(log_f);
    }
}

void start_report(void)
{
  size_t report_file_path_len;
  char *report_file_path;
  static FILE *report_f;

  int min_num, max_num;
  double mean_num;
  struct stat_msgbuf buf;

  report_file_path_len = strlen(app_name) + 9 + 1;
  report_file_path = malloc(report_file_path_len);
  memset(report_file_path, 0, report_file_path_len);
  sprintf(report_file_path, "/tmp/%s.rep", app_name);
  fprintf(stderr, "Report file: %s\n", report_file_path);
  report_f = fopen(report_file_path, "w");
  free(report_file_path);

  signal(SIGINT, report_sighandler);

  while(1)
    {
      msgrcv(pipe_statistics_to_report, &buf, sizeof(buf)-sizeof(long), 2, 0);
      min_num = buf.mmin_num;
      max_num = buf.mmax_num;
      mean_num = buf.mmean_num;

      fprintf(report_f, "[%ld] %i %i %f\n", time(0), min_num, max_num, mean_num);
      fflush(report_f);
    }
}

void start_statistics(void)
{
  pid_t pid;
  int num;

  int min_num, max_num;
  size_t count;
  int sum;
  double mean_num;

  struct conv_msgbuf rcvbuf;
  struct stat_msgbuf sndbuf;

  count = 0;
  min_num = max_num = 0;
  sum = 0;
  mean_num = 0.0;

  pid = fork();
  if (!pid)
      start_report();

  while(1)
    {
      msgrcv(pipe_conv_to_statistics, &rcvbuf, sizeof(int), 0, 0);
      num = rcvbuf.mnum;

      ++count;
      sum += num;
      if (num < min_num)
	min_num = num;
      if (num > max_num)
	max_num = num;
      mean_num = ((double) sum) / ((double) count);

      sndbuf.mtype = 2;
      sndbuf.mmin_num = min_num;
      sndbuf.mmax_num = max_num;
      sndbuf.mmean_num = mean_num;
      msgsnd(pipe_statistics_to_report, &sndbuf, sizeof(sndbuf)-sizeof(long), 0);
    }
}

void log_sighandler(int sig)
{
  static FILE *log_f;
  fclose(log_f);
}

void report_sighandler(int sig)
{
  static FILE *report_f;
  fclose(report_f);
}

void main_sighandler(int sig)
{
  msgctl(pipe_conv_to_log, IPC_RMID, NULL);
  msgctl(pipe_conv_to_statistics, IPC_RMID, NULL);
  msgctl(pipe_statistics_to_report, IPC_RMID, NULL);

  free(app_name);

  printf("BAI\n");
  exit(0);
}
