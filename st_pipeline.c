
// usleep need this
#if !defined(_XOPEN_SOURCE) && !defined(_POSIX_C_SOURCE)
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600 /* SUS v3, POSIX 1003.1 2004 (POSIX 2001 + Corrigenda) */
#else
#define _XOPEN_SOURCE 500 /* SUS v2, POSIX 1003.1 1997 */
#endif                    /* __STDC_VERSION__ */
#endif                    /* !_XOPEN_SOURCE && !_POSIX_C_SOURCE */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

typedef struct _Node
{
  void *data;
  struct _Node *next;
} Node, *PNode;

typedef struct _BlockingQueue
{
  PNode head;
  PNode tail;
  pthread_mutex_t mutex_lock_queue;
  pthread_cond_t condition;
} BlockingQueue, *PBlockingQueue;

typedef void (*PFunc)(void *);

typedef struct _ActiveObject
{
  pthread_t thread;
  PBlockingQueue queue;
  PFunc func;
} ActiveObject, *PActiveObject;

typedef struct _Task
{
  unsigned int num_of_tasks;
  unsigned int _data;
} Task, *PTask;

PBlockingQueue queueCreate()
{
  PBlockingQueue queue = (PBlockingQueue)malloc(sizeof(BlockingQueue));

  if (queue == NULL)
    return NULL;

  queue->head = NULL;
  queue->tail = NULL;

  pthread_mutex_init(&queue->mutex_lock_queue, NULL);
  pthread_cond_init(&queue->condition, NULL);

  return queue;
}

void queueDestroy(PBlockingQueue queue)
{
  if (queue == NULL)
    return;
  pthread_mutex_lock(&queue->mutex_lock_queue);
  PNode node = queue->head;
  while (node != NULL)
  {
    PNode next = node->next;
    free(node->data);
    free(node);
    node = next;
  }
  pthread_mutex_unlock(&queue->mutex_lock_queue);
  pthread_mutex_destroy(&queue->mutex_lock_queue);
  pthread_cond_destroy(&queue->condition);
  free(queue);
}

void queueEnqueue(PBlockingQueue queue, void *data)
{
  if (queue == NULL)
    return;
  PNode node = (PNode)malloc(sizeof(Node));
  if (node == NULL)
    return;
  node->data = data;
  node->next = NULL;
  pthread_mutex_lock(&queue->mutex_lock_queue);
  if (queue->head == NULL)
  {
    queue->head = node;
    queue->tail = node;
    pthread_cond_signal(&queue->condition);
  }
  else
  {
    queue->tail->next = node;
    queue->tail = node;
  }
  pthread_mutex_unlock(&queue->mutex_lock_queue);
}

void *queueDequeue(PBlockingQueue queue)
{
  if (queue == NULL)
    return NULL;
  pthread_mutex_lock(&queue->mutex_lock_queue);
  while (queue->head == NULL)
    pthread_cond_wait(&queue->condition, &queue->mutex_lock_queue);
  PNode node = queue->head;
  void *data = node->data;
  queue->head = node->next;
  if (queue->head == NULL)
    queue->tail = NULL;
  free(node);
  pthread_mutex_unlock(&queue->mutex_lock_queue);
  return data;
}

void *funcThread(void *activeObject)
{
  if (activeObject == NULL)
    return NULL;

  PActiveObject ao = (PActiveObject)activeObject;
  PBlockingQueue queue = ao->queue;
  void *task = NULL;

  if (queue == NULL)
    return NULL;

  while ((task = queueDequeue(queue)) != NULL)
    ao->func(task);

  return activeObject;
}

PActiveObject CreateActiveObject(PFunc func)
{
  if (func == NULL)
    return NULL;

  PActiveObject activeObject = (PActiveObject)malloc(sizeof(ActiveObject));

  if (activeObject == NULL)
    return NULL;

  activeObject->queue = queueCreate();

  if (activeObject->queue == NULL)
  {
    free(activeObject);
    return NULL;
  }

  activeObject->func = func;

  if (pthread_create(&activeObject->thread, NULL, funcThread, activeObject) != 0)
  {
    queueDestroy(activeObject->queue);
    free(activeObject);
    return NULL;
  }

  return activeObject;
}

PBlockingQueue getQueue(PActiveObject activeObject)
{
  if (activeObject == NULL)
    return NULL;

  return activeObject->queue;
}

void stop(PActiveObject activeObject)
{
  if (activeObject == NULL)
    return;
  pthread_cancel(activeObject->thread);
  pthread_join(activeObject->thread, NULL);
  queueDestroy(activeObject->queue);
  free(activeObject);
}

PActiveObject ActiveObjects[4] = {NULL};

int isprime(unsigned int num)
{
  if (num <= 1 || num % 2 == 0)
    return 0;

  unsigned int root = (unsigned int)sqrt(num);

  for (unsigned int i = 3; i <= root; i += 2)
  {
    if (num % i == 0)
      return 0;
  }

  return 1;
}

void Task1(void *task)
{
  PTask task_init = (PTask)task;

  unsigned int n = task_init->num_of_tasks, seed = task_init->_data;

  srand(seed != 0 ? seed : time(NULL));

  for (unsigned int i = 0; i < n; i++)
  {
    unsigned int num = (rand() % 900000) + 100000;

    PTask task_data = (PTask)malloc(sizeof(Task));

    if (task_data == NULL)
      exit(1);

    task_data->num_of_tasks = n;
    task_data->_data = num;

    queueEnqueue(getQueue(ActiveObjects[1]), task_data);

    usleep(1000);
  }

  stop(ActiveObjects[0]);
}

void Task2(void *task)
{
  static unsigned int count = 0;
  PTask task_data = (PTask)task;

  unsigned int iterations = task_data->num_of_tasks, num = task_data->_data;

  fprintf(stdout, "%u\n%s\n", num, isprime(num) ? "true" : "false");

  task_data->_data += 11;

  queueEnqueue(getQueue(ActiveObjects[2]), task_data);

  if (iterations <= ++count)
    stop(ActiveObjects[1]);
}

void Task3(void *task)
{
  static unsigned int count = 0;
  PTask task_data = (PTask)task;

  unsigned int iterations = task_data->num_of_tasks, num = task_data->_data;

  fprintf(stdout, "%u\n%s\n", num, isprime(num) ? "true" : "false");

  task_data->_data -= 13;

  queueEnqueue(getQueue(ActiveObjects[3]), task_data);

  if (iterations <= ++count)
    stop(ActiveObjects[2]);
}

void Task4(void *task)
{
  static unsigned int count = 0;
  PTask task_data = (PTask)task;

  unsigned int iterations = task_data->num_of_tasks, num = task_data->_data;

  printf("%u\n%s\n%u\n", num, isprime(num) ? "true" : "false", (num + 2));
  free(task_data);

  if (iterations <= ++count)
    stop(ActiveObjects[3]);
}

int main(int argc, char **args)
{
  if (argc > 3 || argc < 2)
  {
    printf("Usage: %s <n> [<seed>]\n", *args);
    return 1;
  }
  unsigned int n = atoi(args[1]), seed = 0;
  if (argc == 3)
  {
    seed = atoi(args[2]);

    if (seed == 0)
    {
      printf("Seed must be positive!");
      return 1;
    }
  }
  if (n == 0)
  {
    printf("Number must be positive!");
    return 1;
  }
  PFunc Tasks[4] = {Task1, Task2, Task3, Task4};
  for (int i = 0; i < 4; i++)
  {
    ActiveObjects[i] = CreateActiveObject(Tasks[i]);
    if (ActiveObjects[i] == NULL)
      return 1;
  }
  PTask task_data = (PTask)malloc(sizeof(Task));
  if (task_data == NULL)
    exit(1);
  task_data->num_of_tasks = n;
  task_data->_data = seed;
  queueEnqueue(getQueue(ActiveObjects[0]), task_data);
  for (int i = 0; i < 4; i++) pthread_join(ActiveObjects[i]->thread, NULL);

  return 0;
}