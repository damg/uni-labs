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
#include <sys/shm.h>
#include <sys/mman.h>

#define BUF_SIZE 32

void main_sighandler(int);

void start_conv(void);
void start_log(void);
void start_statistics(void);
void start_report(void);

void report_sighandler(int sig);
void log_sighandler(int sig);

static sem_t *shm_log_sem_rd;
static sem_t *shm_stat_sem_rd;
static sem_t *shm_rep_sem_rd;
static sem_t *shm_log_sem_wr;
static sem_t *shm_stat_sem_wr;
static sem_t *shm_rep_sem_wr;
static sem_t *shm_log_sem_ac;
static sem_t *shm_stat_sem_ac;
static sem_t *shm_rep_sem_ac;

static int shm_log;
static int shm_stat;
static int shm_rep;

static volatile size_t rdpos_log_buf = 0;
static volatile size_t rdpos_stat_buf = 0;
static volatile size_t rdpos_rep_buf = 0;
static volatile size_t wrpos_log_buf = 0;
static volatile size_t wrpos_stat_buf = 0;
static volatile size_t wrpos_rep_buf = 0;

struct report_msgbuf_s
{
  int min_num;
  int max_num;
  double mean_num;
};

static char *app_name;

#define P(sem_name) \
  sem_wait((sem_name))

#define V(sem_name) \
  sem_post((sem_name))

int
main(int argc, char *argv[])
{
  app_name = strdup(basename(argv[0]));
  signal(SIGINT, main_sighandler);

  shm_log_sem_rd = mmap(NULL, sizeof *shm_log_sem_rd, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  shm_log_sem_wr = mmap(NULL, sizeof *shm_log_sem_wr, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  shm_log_sem_ac = mmap(NULL, sizeof *shm_log_sem_ac, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  shm_stat_sem_rd = mmap(NULL, sizeof *shm_stat_sem_rd, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  shm_stat_sem_wr = mmap(NULL, sizeof *shm_stat_sem_wr, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  shm_stat_sem_ac = mmap(NULL, sizeof *shm_stat_sem_ac, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  shm_rep_sem_rd = mmap(NULL, sizeof *shm_rep_sem_rd, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  shm_rep_sem_wr = mmap(NULL, sizeof *shm_rep_sem_wr, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  shm_rep_sem_ac = mmap(NULL, sizeof *shm_rep_sem_ac, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  pid_t pid;

  srand(time(0));

  shm_log = shmget(IPC_PRIVATE, sizeof(int)*BUF_SIZE, IPC_CREAT | 0600);
  fprintf(stderr, "Created shmem for log: %d\n", shm_log);
  sem_init(shm_log_sem_rd, 1, 0);
  sem_init(shm_log_sem_wr, 1, BUF_SIZE);
  sem_init(shm_log_sem_ac, 1, 1);

  pid = fork();
  if (!pid)
    start_log();

  shm_stat = shmget(IPC_PRIVATE, sizeof(int)*BUF_SIZE, IPC_CREAT | 0600);
  fprintf(stderr, "Created shmem for stat: %d\n", shm_stat);
  sem_init(shm_stat_sem_rd, 1, 0);
  sem_init(shm_stat_sem_wr, 1, BUF_SIZE);
  sem_init(shm_stat_sem_ac, 1, 1);

  shm_rep = shmget(IPC_PRIVATE, sizeof(struct report_msgbuf_s)*BUF_SIZE, IPC_CREAT | 0600);
  fprintf(stderr, "Created shmem for rep: %d\n", shm_rep);
  sem_init(shm_rep_sem_rd, 1, 0);
  sem_init(shm_rep_sem_wr, 1, BUF_SIZE);
  sem_init(shm_rep_sem_ac, 1, 1);

  pid = fork();
  if (!pid)
    start_statistics();
  else
    start_conv();

  return EXIT_SUCCESS;
}

void start_conv(void)
{
  int num, i;
  static int *conv_shmptr_log, *conv_shmptr_stat;

  conv_shmptr_log = shmat(shm_log, 0, 0);
  conv_shmptr_stat = shmat(shm_stat, 0, 0);

  while(1)
    {
      num = rand() % 256;

      P(shm_stat_sem_wr);
      P(shm_stat_sem_ac);
      conv_shmptr_stat[wrpos_log_buf] = num; //
      wrpos_log_buf = (wrpos_log_buf + 1) % BUF_SIZE;
      V(shm_stat_sem_ac);
      V(shm_stat_sem_rd);

      P(shm_log_sem_wr);
      P(shm_log_sem_ac);
      conv_shmptr_log[wrpos_rep_buf] = num; //
      wrpos_rep_buf = (wrpos_rep_buf + 1) % BUF_SIZE;
      V(shm_log_sem_ac);
      V(shm_log_sem_rd);
    }
}

void start_log(void)
{
  char *log_file_path;
  static FILE *log_f;
  int num;
  static int* log_shmptr;
  size_t log_file_path_len;

  log_file_path_len = strlen(app_name) + 9 + 1;
  log_file_path = malloc(log_file_path_len);
  memset(log_file_path, 0, log_file_path_len);
  sprintf(log_file_path, "/tmp/%s.log", app_name);
  fprintf(stderr, "Log file: %s\n", log_file_path);
  log_f = fopen(log_file_path, "w");
  free(log_file_path);

  log_shmptr = shmat(shm_log, 0, 0);
  signal(SIGINT, log_sighandler);

  while(1)
    {
      P(shm_log_sem_rd);
      P(shm_log_sem_ac);

      num = log_shmptr[rdpos_log_buf];
      rdpos_log_buf = (rdpos_log_buf + 1) % BUF_SIZE;
      fprintf(log_f, "[%ld] %d\n", time(0), num);
      fflush(log_f);

      V(shm_log_sem_ac);
      V(shm_log_sem_wr);
    }
}

void start_report(void)
{
  size_t report_file_path_len;
  char *report_file_path;
  static FILE *report_f;
  static struct report_msgbuf_s *rep_shmptr;

  int min_num, max_num;
  double mean_num;

  report_file_path_len = strlen(app_name) + 9 + 1;
  report_file_path = malloc(report_file_path_len);
  memset(report_file_path, 0, report_file_path_len);
  sprintf(report_file_path, "/tmp/%s.rep", app_name);
  fprintf(stderr, "Report file: %s\n", report_file_path);
  report_f = fopen(report_file_path, "w");
  free(report_file_path);

  rep_shmptr = shmat(shm_rep, 0, 0);
    
  signal(SIGINT, report_sighandler);

  while(1)
    {
      P(shm_rep_sem_rd);
      P(shm_rep_sem_ac);
      
      min_num = rep_shmptr[rdpos_rep_buf].min_num;
      max_num = rep_shmptr[rdpos_rep_buf].max_num;
      mean_num = rep_shmptr[rdpos_rep_buf].mean_num;
      rdpos_rep_buf = (rdpos_rep_buf + 1) % BUF_SIZE;

      fprintf(report_f, "[%ld] %i %i %f\n", time(0), min_num, max_num, mean_num);
      fflush(report_f);

      V(shm_rep_sem_ac);
      V(shm_rep_sem_wr);
    }
}

void start_statistics(void)
{
  pid_t pid;
  int num;
  static int *stat_shmptr_conv;
  static struct report_msgbuf_s *stat_shmptr_rep;
  struct report_msgbuf_s buf;

  int min_num, max_num;
  size_t count;
  int sum;
  double mean_num;

  count = 0;
  min_num = 256;
  max_num = -1;
  sum = 0;
  mean_num = 0.0;

  stat_shmptr_conv = shmat(shm_stat, 0, 0);
  stat_shmptr_rep = shmat(shm_rep, 0, 0);

  pid = fork();
  if (!pid)
      start_report();
    
  while(1)
    {
      P(shm_stat_sem_rd);
      P(shm_stat_sem_ac);
      P(shm_rep_sem_wr);
      P(shm_rep_sem_ac);

      num = stat_shmptr_conv[rdpos_stat_buf];
      rdpos_stat_buf = (rdpos_stat_buf + 1) % BUF_SIZE;

      ++count;
      sum += num;
      if (num < min_num)
	min_num = num;

      if (num > max_num)
	max_num = num;
      mean_num = ((double) sum) / ((double) count);

      buf.min_num = min_num;
      buf.max_num = max_num;
      buf.mean_num = mean_num;

      stat_shmptr_rep[wrpos_rep_buf] = buf;
      wrpos_rep_buf = (wrpos_rep_buf + 1) % BUF_SIZE;
      V(shm_stat_sem_ac);
      V(shm_rep_sem_ac);
      V(shm_stat_sem_wr);
      V(shm_rep_sem_rd);
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
  static int *conv_shmptr_log, *conv_shmptr_stat, *log_shmptr, *stat_shmptr_conv;
  static struct report_msgbuf_s *rep_shmptr, *stat_shmptr_rep;
  
  shmdt(conv_shmptr_log);
  shmdt(conv_shmptr_stat);
  shmdt(log_shmptr);
  shmdt(rep_shmptr);
  shmdt(stat_shmptr_conv);
  shmdt(stat_shmptr_rep);

  shmctl(shm_log, IPC_RMID, 0);
  shmctl(shm_stat, IPC_RMID, 0);
  shmctl(shm_rep, IPC_RMID, 0);

  sem_destroy(shm_log_sem_rd);
  sem_destroy(shm_stat_sem_rd);
  sem_destroy(shm_rep_sem_rd);
  sem_destroy(shm_log_sem_wr);
  sem_destroy(shm_stat_sem_wr);
  sem_destroy(shm_rep_sem_wr);
  sem_destroy(shm_log_sem_ac);
  sem_destroy(shm_stat_sem_ac);
  sem_destroy(shm_rep_sem_ac);

  free(app_name);
  exit(0);
}
