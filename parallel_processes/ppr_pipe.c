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

void atexit_handler(void);

void start_conv(void);
void start_log(void);
void start_statistics(void);
void start_report(void);

void report_sighandler(int sig);
void log_sighandler(int sig);

static int pipe_conv_to_log[2];
static int pipe_conv_to_statistics[2];
static int pipe_statistics_to_report[2];

static char *app_name;

int
main(int argc, char *argv[])
{
  app_name = strdup(basename(argv[0]));
  atexit(atexit_handler);

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

      write(pipe_conv_to_log[1], &num, sizeof(num));
      write(pipe_conv_to_statistics[1], &num, sizeof(num));
    }
}

void start_log(void)
{
  char *log_file_path;
  static FILE *log_f;
  int num;
  size_t log_file_path_len;

  close(pipe_conv_to_log[1]);

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
      read(pipe_conv_to_log[0], &num, sizeof(num));
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

  close(pipe_statistics_to_report[1]);

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
      read(pipe_statistics_to_report[0], &min_num, sizeof(min_num));
      read(pipe_statistics_to_report[0], &max_num, sizeof(max_num));
      read(pipe_statistics_to_report[0], &mean_num, sizeof(mean_num));

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

  close(pipe_conv_to_statistics[1]);

  count = 0;
  min_num = max_num = 0;
  sum = 0;
  mean_num = 0.0;

  pid = fork();
  if (!pid)
    {
      start_report();
    }
  else
    {
      close(pipe_conv_to_statistics[1]);
      close(pipe_statistics_to_report[0]);
    }

  while(1)
    {
      read(pipe_conv_to_statistics[0], &num, sizeof(num));
      ++count;
      sum += num;
      if (num < min_num)
	min_num = num;
      if (num > max_num)
	max_num = num;
      mean_num = ((double) sum) / ((double) count);

      write(pipe_statistics_to_report[1], &min_num, sizeof(min_num));
      write(pipe_statistics_to_report[1], &max_num, sizeof(max_num));
      write(pipe_statistics_to_report[1], &mean_num, sizeof(mean_num));
    }
}

void log_sighandler(int sig)
{
  static FILE *log_f;
  close(pipe_conv_to_log[0]);
  fclose(log_f);
}

void report_sighandler(int sig)
{
  static FILE *report_f;
  close(pipe_statistics_to_report[0]);
  fclose(report_f);
}

void atexit_handler(void)
{
  free(app_name);
}
