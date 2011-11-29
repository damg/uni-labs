#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <libgen.h>
#include <fcntl.h>

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

struct linked_list_s
{
  void *data;
  struct linked_list_s *next;
};

struct linked_list_s*
linked_list_last(struct linked_list_s* l);

struct linked_list_s*
linked_list_append(struct linked_list_s* dst, void *data);

size_t
linked_list_len(struct linked_list_s* l);

struct queue_s
{
  struct linked_list_s *head;
  struct linked_list_s *tail;
};

size_t
queue_len(struct queue_s* q);

struct queue_s*
queue_enqueue(struct queue_s* q, void *data);

void*
queue_peek(struct queue_s* q);

void*
queue_dequeue(struct queue_s* q);

int
queue_empty(struct queue_s* q);

struct flock_queue_s
{
  struct mtx *mux;
  struct queue_s *q;
};

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
      usleep(1000000);

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

  int min_num, max_num, num;
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

struct linked_list_s*
linked_list_last(struct linked_list_s* l)
{
  if (!l)
    return NULL;

  while(l->next)
    l = l->next;

  return l;
}

struct linked_list_s*
linked_list_append(struct linked_list_s* dst, void *data)
{
  struct linked_list_s* last;
  struct linked_list_s* node;

  node = malloc(sizeof(struct linked_list_s));
  node->data = data;
  node->next = NULL;

  last = linked_list_last(dst);
  if (!last)
    return node;
  
  last->next = node;
  return dst;
}

size_t
linked_list_len(struct linked_list_s* l)
{
  size_t len;

  if (!l)
    return 0;
  
  len = 1;
  while(l->next)
    {
      l = l->next;
      len++;
    }

  return len;
}

size_t
queue_len(struct queue_s* q)
{
  if (!q)
    return 0;
  return linked_list_len(q->head);
}

struct queue_s*
queue_enqueue(struct queue_s* q, void *data)
{
  if (!q)
    {
      q = malloc(sizeof(struct queue_s));
      q->head = NULL;
      q->tail = NULL;
    }

  q->head = linked_list_append(q->head, data);
  q->tail = linked_list_last(q->head);

  return q;
}

void*
queue_peek(struct queue_s* q)
{
  if (!q)
    return NULL;
  if (!q->head)
    return NULL;
  return q->head->data;
}

void*
queue_dequeue(struct queue_s* q)
{
  void *result;
  struct linked_list_s* node;

  if (!q)
    return NULL;
  if (!q->head)
    return NULL;

  node = q->head;
  q->head = node->next;
  result = node->data;
  free(node);
  return result;
}

int
queue_empty(struct queue_s* q)
{
  return (q != NULL) && (q->head != NULL);
}
