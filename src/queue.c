#include "queue.h"

#include <stdlib.h>

static int * queue = NULL;
static int max_size;
int lnode = 0, rnode = 0, qsize = 0;

void initQueue(int count) {
  if (queue) return;
  int size = sizeof(int);
  max_size = count;
  queue = calloc(count, size);
  lnode = 0;
  rnode = 0;
  qsize = 0;
}

void freeQueue() {
  if (!queue) return;
  free(queue);
  queue = NULL;
}

int qpush(int value) {
  if (max_size == qsize) return 0;

  queue[rnode++] = value;
  rnode %= max_size;
  ++qsize;
  return 1;
}

int qpop(int * value) {
  if (!qsize) return 0;

  *value = queue[lnode++];
  lnode %= max_size;
  --qsize;
  return 1;
}
