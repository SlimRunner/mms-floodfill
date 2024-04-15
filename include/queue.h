#ifndef QUEUE_H
#define QUEUE_H

void initQueue(int count, int size);
void freeQueue();

int qpush(int value);
int qpop(int * value);

#endif // QUEUE_H
