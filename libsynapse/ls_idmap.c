/* functions for mapping logical node ids to pointers returned by synapse
 * (the intent is to avoid a lot of latency issues related to networking
 * with the side effect of increasing stability)
 */

#include <stdio.h>
#include <stdlib.h>
#include "libsynapse_internal.h"
#include "socketcomm.h"
#include "ls_idmap.h"

typedef struct id_queue
{
  struct id_queue *next;
  int id;
  void *ptr;
} id_queue;

static id_queue *waiting = 0;
static id_queue *map = 0;
static unsigned long nextid = 0;

int id_queue_len(void)
{
  id_queue *e;
  int count = 0;

  e = waiting;
  while(e)  
  {
    count++;
    e = e->next;
  }
  return count;
}

int is_in_queue(id_queue *queue, unsigned long id)
{
  id_queue *tmp;

  tmp = queue;

  while(tmp)
  {
    if(tmp->id == id)
      return 1;
    tmp = tmp->next;
  }

  return 0;
}

unsigned long get_next_id(void)
{
  unsigned long tmp;

  while(is_in_queue(waiting, nextid))
    nextid++;  // this will wrap around to 0 automagically

  tmp = nextid;
  nextid++;
  return tmp;
}

int add_id_to_waiting_queue(unsigned long id)
{
  id_queue *tmp = waiting;

  if(!tmp)
    waiting = tmp = (id_queue *) malloc(sizeof(id_queue));
  else
  {
    while(tmp->next)
      tmp = tmp->next;
    tmp->next = (id_queue *) malloc(sizeof(id_queue));
    tmp = tmp->next;
  }
  tmp->id = id;
  tmp->next = 0;
  return 0;
}

int wait_for_id(unsigned long id)
{
  while(is_in_queue(waiting, id))
  {
    if(process_connection(1))
      return -1;
  }
  return 0;
}

void move_to_map(id_queue *thing)
{
  id_queue *tmp = map;
  id_queue *prev;

// special cases for list lengths of 0 or 1, and if thing belongs at the beginning of the list
  if(!tmp)  // map is empty
  {
    map = thing;
    map->next = 0;
    return;
  }

  if(tmp->id > thing->id) // thing goes at the beginnng
  {
    thing->next = tmp;
    map = thing;
    return;
  }

  if(!tmp->next) // map has one element and thing goes after it
  {
    tmp->next = thing;
    thing->next = 0;
    return;
  }

  prev = tmp;
  tmp = tmp->next;
  while(tmp)
  {
    if(tmp->id > thing->id)
      break;
    prev = tmp;
    tmp = tmp->next;
  }

  thing->next = tmp;
  prev->next = thing;
  return;
}

int process_id_message(void *ptr)
{
  id_queue *tmp;

  tmp = waiting;
  waiting = waiting->next;
  tmp->ptr = (void *)(*(int *)ptr);
  move_to_map(tmp);
  return 0;
}

void remove_ptr_from_map(void *ptr)
{
  id_queue *tmp;
  id_queue *prev;

  tmp = map;
  prev = 0;
  while(tmp)
  {
    if(tmp->ptr == ptr)
    {
      if(prev)
        prev->next = tmp->next;
      if(tmp == map)
        map = tmp->next;
      free(tmp);
      return;
    }
    prev = tmp;
    tmp = tmp->next;
  }
  // it will never happen that something is destroyed before i get the
  // ptr to it, because the pointer will always be further downstream
  return;
}

void * find_ptr_in_map(unsigned long id, int waitforit)
{
  id_queue *tmp;

  if(waitforit)
    wait_for_id(id);

  tmp = map;
  while(tmp)
  {
    if(tmp->id == id)
      return tmp->ptr;
    if(tmp->id > id)
      break;  // this works becuase map is a sorted list
    tmp = tmp->next;
  }
  return 0;
}

int find_id_in_map(void *ptr)
{
  id_queue *tmp;

  tmp = map;
  while(tmp)
  {
    if(tmp->ptr == ptr)
      return tmp->id;
    tmp = tmp->next;
  }
  return -1;
}
