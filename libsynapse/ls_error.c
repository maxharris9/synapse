#include <stdlib.h>
#include "libsynapse_internal.h"
#include "ls_error.h"

typedef struct error_queue
{
  struct error_queue *next;
  int error;
} error_queue;

static error_queue *errors = 0;

int error_queue_len(void)
{
  error_queue *e;
  int count = 0;

  e = errors;
  while(e)
  {
    count++;
    e = e->next;
  }
  return count;
}

int next_error(void)
{
  int i;
  error_queue *e;

  if(errors)
  {
    i = errors->error;
    e = errors;
    errors = errors->next;
    free(e);
    return i;
  }
  return 0;  // if no return codes are waiting, return 0
}

int add_error_to_queue(int error)
{
  error_queue *e;

  if(!errors)
  {
    errors = (error_queue *) malloc(sizeof(error_queue));
    errors->next = 0;
    errors->error = error;
    return 0;
  }

  e = errors;
  while(e->next)
    e = e->next;
  e->next = (error_queue *) malloc(sizeof(error_queue));
  e = e->next;
  e->next = 0;
  e->error = error;
  return 0;
}
