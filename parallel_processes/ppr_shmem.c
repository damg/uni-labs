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

void main_sighandler(int);

void start_conv(void);
void start_log(void);
void start_statistics(void);
void start_report(void);

void report_sighandler(int sig);
void log_sighandler(int sig);

static sem_t *shm_log_sem;
static sem_t *shm_stat_sem;
static sem_t *shm_rep_sem;

static int shm_log;
static int shm_stat;
static int shm_rep;

struct report_msgbuf_s
{
  int min_num;
  int max_num;
  double mean_num;
};

static char *app_name;

int
main(int argc, char *argv[])
{
  app_name = strdup(basename(argv[0]));
  signal(SIGINT, main_sighandler);

  shm_log_sem = mmap(NULL, sizeof *shm_log_sem, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  shm_stat_sem = mmap(NULL, sizeof *shm_stat_sem, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  shm_rep_sem = mmap(NULL, sizeof *shm_rep_sem, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  pid_t pid;

  srand(time(0));

  pid = fork();

  if (!pid)
    {
      start_log();
    }
  else
    {
      sem_init(shm_log_sem, 1, 0);
      shm_log = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    }

  pid = fork();
  if (!pid)
    {
      start_statistics();
    }
  else
    {
      sem_init(shm_stat_sem, 1, 0);
      shm_stat = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
      start_conv();
    }
  return EXIT_SUCCESS;
}

void start_conv(void)
{
  int num;
  int* shmptr;

  while(1)
    {
      usleep(1000000);
      num = rand() % 256;

      shmptr = shmat(shm_log, 0, 0);
      *shmptr = num;
      sem_post(shm_log_sem);

      printf("%d\n", num);

      shmptr = shmat(shm_stat, 0, 0);
      *shmptr = num;
      sem_post(shm_stat_sem);
    }
}

void start_log(void)
{
  char *log_file_path;
  static FILE *log_f;
  int num;
  int* shmptr;
  size_t log_file_path_len;

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
      sem_wait(shm_log_sem);
      shmptr = shmat(shm_log, 0, 0);
      perror("HAI");
      num = *shmptr;
      fprintf(log_f, "[%ld] %d\n", time(0), num);
      fflush(log_f);
    }
}

void start_report(void)
{
  size_t report_file_path_len;
  char *report_file_path;
  static FILE *report_f;
  struct report_msgbuf_s *shmptr;

  int min_num, max_num, num;
  double mean_num;

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
      sem_wait(shm_rep_sem);
      shmptr = shmat(shm_rep, 0, 0);
      min_num = shmptr->min_num;
      max_num = shmptr->max_num;
      mean_num = shmptr->mean_num;

      fprintf(report_f, "[%ld] %i %i %f\n", time(0), min_num, max_num, mean_num);
      fflush(report_f);
    }
}

void start_statistics(void)
{
  pid_t pid;
  int num, *shmiptr;
  struct report_msgbuf_s *shmptr;
  struct report_msgbuf_s buf;

  int min_num, max_num;
  size_t count;
  int sum;
  double mean_num;

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
        sem_init(shm_rep_sem, 1, 0);
	shm_rep = shmget(IPC_PRIVATE, sizeof(struct report_msgbuf_s), IPC_CREAT | 0600);
    }

  while(1)
    {
      sem_wait(shm_stat_sem);
      shmiptr = shmat(shm_stat, 0, 0);
      num = *shmiptr;

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

      shmptr = shmat(shm_rep, 0, 0);
      *shmptr = buf;
      sem_post(shm_rep_sem);
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
  shmctl(shm_log, IPC_RMID, 0);
  shmctl(shm_stat, IPC_RMID, 0);
  shmctl(shm_rep, IPC_RMID, 0);
  sem_destroy(shm_log_sem);
  sem_destroy(shm_stat_sem);
  sem_destroy(shm_rep_sem);

  free(app_name);
  exit(0);
}
