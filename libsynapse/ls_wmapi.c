#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>
#include "libsynapse_internal.h"
#include "socketcomm.h"
#include "ls_error.h"
#include "ls_idmap.h"
#include "ls_wmapi.h"

int syn_create_flatwindow(unsigned long *id, int wintype, Point *origin, Point *size, Point *rotation, int visible, char *title)
{
  int *data;
  int dsize;
  header head;

  dsize = (sizeof(int)*3) + (sizeof(Point)*3) + ((strlen(title) + 4) & ~3);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = wintype;
  data[1] = glfloat_to_int(origin->x);
  data[2] = glfloat_to_int(origin->y);
  data[3] = glfloat_to_int(origin->z);
  data[4] = glfloat_to_int(size->x);
  data[5] = glfloat_to_int(size->y);
  data[6] = glfloat_to_int(size->z);
  data[7] = glfloat_to_int(rotation->x);
  data[8] = glfloat_to_int(rotation->y);
  data[9] = glfloat_to_int(rotation->z);
  data[10] = visible;
  strcpy((char *)(&data[11]), title);

  head.len = dsize + sizeof(head);
  head.type = 0x1000;
  sendmessage(&head, data);
  *id = get_next_id();
  add_id_to_waiting_queue(*id);

  return next_error();
}

int syn_create_deepwindow(unsigned long *id, int wintype, Point *origin, Point *size, Point *rotation, int visible, char *title)
{
  int *data;
  int dsize;
  header head;

  dsize = (sizeof(int)*2) + (sizeof(Point)*3) + ((strlen(title) + 4) & ~3);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = wintype;
  data[1] = glfloat_to_int(origin->x);
  data[2] = glfloat_to_int(origin->y);
  data[3] = glfloat_to_int(origin->z);
  data[4] = glfloat_to_int(size->x);
  data[5] = glfloat_to_int(size->y);
  data[6] = glfloat_to_int(size->z);
  data[7] = glfloat_to_int(rotation->x);
  data[8] = glfloat_to_int(rotation->y);
  data[9] = glfloat_to_int(rotation->z);
  data[10] = visible;
  strcpy((char *)(&data[11]), title);

  head.len = dsize + sizeof(head);
  head.type = 0x1100;
  sendmessage(&head, data);
  *id = get_next_id();
  add_id_to_waiting_queue(*id);

  return next_error();
}

int process_wmanager_message(unsigned int len, int type, void *message)
{
  if(*((int *)(message)) == 0)
    add_error_to_queue(-1);
  else
  {
    process_id_message(message);
    add_error_to_queue(0);
  }
  return 0;
}
