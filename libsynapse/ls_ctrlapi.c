#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libsynapse_internal.h"
#include "socketcomm.h"
#include "ls_error.h"
#include "ls_idmap.h"
#include "ls_ctrlapi.h"

#include "../src/api_defs.h"

static int labelin = 0;
static char *inlabel;
static int get_label_len;
static int get_button_status;
static int get_progbar;
static int get_scrollbar_value;
static int get_slider_value;

int syn_get_button_status(unsigned long id, int *status)
{
  int *data;
  int dsize;
  header head;

  dsize = sizeof(void *);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }

  head.len = dsize + sizeof(head);
  head.type = API_SYN_GET_BUTTON_STATUS;
  sendmessage(&head, data);
  free(data);

  get_button_status = -2;

  while(get_button_status == -2)
  {
    if(process_connection(1))
      return -1;
  }
  *status = get_button_status;

  return next_error();
}

int syn_set_button_status(unsigned long id, int status)
{
  int *data;
  int dsize;
  header head;

  dsize = sizeof(void *) + sizeof(int);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }
  data[1] = status;

  head.len = dsize + sizeof(head);
  head.type = API_SYN_SET_BUTTON_STATUS;
  sendmessage(&head, data);
  free(data);

  return next_error();
}

int syn_get_progbar(unsigned long id, int *percentage)
{
  int *data;
  int dsize;
  header head;

  dsize = sizeof(void *);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }

  head.len = dsize + sizeof(head);
  head.type = API_SYN_GET_PROGBAR;
  sendmessage(&head, data);
  free(data);

  get_progbar = -2;

  while(get_progbar == -2)
  {
    if(process_connection(1))
      return -1;
  }
  *percentage = get_progbar;

  return next_error();
}

int syn_get_scrollbar_value(unsigned long id, int *value)
{
  int *data;
  int dsize;
  header head;

  dsize = sizeof(void *);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }

  head.len = dsize + sizeof(head);
  head.type = API_SYN_GET_SCROLLBAR_VALUE;
  sendmessage(&head, data);
  free(data);

  get_scrollbar_value = -2;

  while(get_scrollbar_value == -2)
  {
    if(process_connection(1))
      return -1;
  }
  *value = get_scrollbar_value;

  return next_error();
}

int syn_change_slider_value(unsigned long id, int value)
{
  int *data;
  int dsize;
  header head;

  dsize = sizeof(void *) + sizeof(int);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }
  data[1] = value;

  head.len = dsize + sizeof(head);
  head.type = API_SYN_CHANGE_SLIDER_VALUE;
  sendmessage(&head, data);
  free(data);

  return next_error();
}

int syn_get_slider_value(unsigned long id, int *value)
{
  int *data;
  int dsize;
  header head;

  dsize = sizeof(void *);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }

  head.len = dsize + sizeof(head);
  head.type = API_SYN_GET_SLIDER_VALUE;
  sendmessage(&head, data);
  free(data);

  get_slider_value = -2;

  while(get_slider_value == -2)
  {
    if(process_connection(1))
      return -1;
  }
  *value = get_slider_value;

  return next_error();
}

int syn_set_glyph_lock(unsigned long id, int locked)
{
  int *data;
  int dsize;
  header head;

  dsize = sizeof(void *) + sizeof(int);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }
  data[1] = locked;

  head.len = dsize + sizeof(head);
  head.type = API_SYN_SET_GLYPH_LOCK;
  sendmessage(&head, data);
  free(data);

  return next_error();
}

int syn_get_label_len(unsigned long id, int *len)
{
  int *data;
  int dsize;
  header head;

  dsize = sizeof(void *);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }

  head.len = dsize + sizeof(head);
  head.type = API_SYN_GET_LABEL_LEN;
  sendmessage(&head, data);
  free(data);

  get_label_len = -2;

  while(get_label_len == -2)
  {
    if(process_connection(1))
      return -1;
  }
  *len = get_label_len;

  return next_error();
}

int syn_get_label(unsigned long id, char *label)
{
  int *data;
  int dsize;
  header head;

  dsize = sizeof(void *);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }

  head.len = dsize + sizeof(head);
  head.type = API_SYN_GET_LABEL;
  sendmessage(&head, data);
  free(data);

  inlabel = label;
  labelin = 0;

  while(!labelin)
  {
    if(process_connection(1))
      return -1;
  }

  return next_error();
}

int syn_destroy_object(unsigned long id)
{
  int *data;
  int dsize;
  header head;

  dsize = sizeof(void *) + sizeof(int);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }

  head.len = dsize + sizeof(head);
  head.type = API_SYN_DESTROY_OBJECT;
  sendmessage(&head, data);
  free(data);

  return next_error();
}

int syn_set_visibility(unsigned long id, int visibility)
{
  int *data;
  int dsize;
  header head;

  dsize = sizeof(void *) + sizeof(int);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }
  data[1] = visibility;

  head.len = dsize + sizeof(head);
  head.type = API_SYN_SET_VISIBILITY;
  sendmessage(&head, data);
  free(data);

  return next_error();
}

int syn_reshape_object_wrt(unsigned long id, unsigned long wrt, Point *origin, Point *size, Point *rotation)
{
  int *data;
  int dsize;
  header head;

  dsize = (sizeof(void *) + sizeof(int)*2 + (sizeof(Point)*3));
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }
  data[1] = (int) find_ptr_in_map(wrt, 1);
  if(data[1] == 0)
  {
    free(data);
    return -4;
  }

  data[2] = 0;

  if(origin)
  {
    data[3] = glfloat_to_int(origin->x);
    data[4] = glfloat_to_int(origin->y);
    data[5] = glfloat_to_int(origin->z);
    data[2] |= 1;
  }
  else
    data[3] = data[4] = data[5] = glfloat_to_int(0.0);

  if(size)
  {
    data[6] = glfloat_to_int(size->x);
    data[7] = glfloat_to_int(size->y);
    data[8] = glfloat_to_int(size->z);
    data[2] |= 2;
  }
  else
    data[6] = data[7] = data[8] = glfloat_to_int(0.0);

  if(rotation)
  {
    data[9] = glfloat_to_int(rotation->x);
    data[10] = glfloat_to_int(rotation->y);
    data[11] = glfloat_to_int(rotation->z);
    data[2] |= 4;
  }
  else
    data[9] = data[10] = data[11] = glfloat_to_int(0.0);

  head.len = dsize + sizeof(head);
  head.type = API_SYN_RESHAPE_OBJECT_WRT;
  sendmessage(&head, data);
  free(data);

  return next_error();
}

int syn_reshape_object(unsigned long id, Point *origin, Point *size, Point *rotation)
{
  int *data;
  int dsize;
  header head;

  dsize = (sizeof(void *) + sizeof(int) + (sizeof(Point)*3));
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }

  data[1] = 0;

  if(origin)
  {
    data[2] = glfloat_to_int(origin->x);
    data[3] = glfloat_to_int(origin->y);
    data[4] = glfloat_to_int(origin->z);
    data[1] |= 1;
  }
  else
    data[2] = data[3] = data[4] = glfloat_to_int(0.0);

  if(size)
  {
    data[5] = glfloat_to_int(size->x);
    data[6] = glfloat_to_int(size->y);
    data[7] = glfloat_to_int(size->z);
    data[1] |= 2;
  }
  else
    data[5] = data[6] = data[7] = glfloat_to_int(0.0);

  if(rotation)
  {
    data[8] = glfloat_to_int(rotation->x);
    data[9] = glfloat_to_int(rotation->y);
    data[10] = glfloat_to_int(rotation->z);
    data[1] |= 4;
  }
  else
    data[8] = data[9] = data[10] = glfloat_to_int(0.0);

  head.len = dsize + sizeof(head);
  head.type = 0x3010;
  sendmessage(&head, data);
  free(data);

  return next_error();
}

int syn_create_button(unsigned long *id, unsigned long parent, int type, Point *origin, Point *size, Point *rotation, int visible, char *label)
{
  int *data;
  int dsize;
  header head;

  dsize = (sizeof(void *) + sizeof(int)*2) + (sizeof(Point)*3) + ((strlen(label) + 4) & ~3);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(parent, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }
  data[1] = type;
  data[2] = glfloat_to_int(origin->x);
  data[3] = glfloat_to_int(origin->y);
  data[4] = glfloat_to_int(origin->z);
  data[5] = glfloat_to_int(size->x);
  data[6] = glfloat_to_int(size->y);
  data[7] = glfloat_to_int(size->z);
  data[8] = glfloat_to_int(rotation->x);
  data[9] = glfloat_to_int(rotation->y);
  data[10] = glfloat_to_int(rotation->z);
  data[11] = visible;
  strcpy((char *)(&data[12]), label);

  head.len = dsize + sizeof(head);
  head.type = 0x3000;
  sendmessage(&head, data);
  free(data);
  *id = get_next_id();
  add_id_to_waiting_queue(*id);

  return next_error();
}

int syn_reload_fonts(unsigned long id)
{
  int *data;
  int dsize;
  header head;

  dsize = (sizeof(void *) + sizeof(int)*2) + (sizeof(Point)*3);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }

  head.len = dsize + sizeof(head);
  head.type = 0x3102;
  sendmessage(&head, data);
  free(data);

   return next_error();
}

int syn_create_bar(unsigned long *id, unsigned long parent, int type, Point *origin, Point *size, Point *rotation, int min, int max, int value, int window, int visible)
{
  int *data;
  int dsize;
  header head;

  dsize = (sizeof(void *) + sizeof(int)*6) + (sizeof(Point)*3);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(parent, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }
  data[1] = type;
  data[2] = glfloat_to_int(origin->x);
  data[3] = glfloat_to_int(origin->y);
  data[4] = glfloat_to_int(origin->z);
  data[5] = glfloat_to_int(size->x);
  data[6] = glfloat_to_int(size->y);
  data[7] = glfloat_to_int(size->z);
  data[8] = glfloat_to_int(rotation->x);
  data[9] = glfloat_to_int(rotation->y);
  data[10] = glfloat_to_int(rotation->z);
  data[11] = visible;
  data[12] = min;
  data[13] = max;
  data[14] = value;
  data[15] = window;

  head.len = dsize + sizeof(head);
  head.type = 0x3100;
  sendmessage(&head, data);
  free(data);
  *id = get_next_id();
  add_id_to_waiting_queue(*id);

  return next_error();
}

int syn_change_progbar(unsigned long id, int percentage)
{
  int *data;
  int dsize;
  header head;

  dsize = (sizeof(void *) + sizeof(int)*2) + (sizeof(Point)*3);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }
  data[1] = percentage;

  head.len = dsize + sizeof(head);
  head.type = 0x3101;
  sendmessage(&head, data);
  free(data);

  return next_error();
}

int syn_change_scrollbar_value(unsigned long id, int value)
{
  int *data;
  int dsize;
  header head;

  dsize = (sizeof(void *) + sizeof(int));
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }
  data[1] = value;

  head.len = dsize + sizeof(head);
  head.type = 0x3107;
  sendmessage(&head, data);
  free(data);

  return next_error();
}

// this function needs to be smarter:  if the label is larger than a threshold, it needs to send the label in chunks
// (i.e., <-"createtext"  <-"addtolabel"  <-"addtolabel" )
int syn_create_text(unsigned long *id, unsigned long parent, Point *origin, Point *size, Point *rotation, GLfloat pointsize, int subtype, char *fontname, char *label)
{
  int *data;
  int dsize;
  header head;

  dsize = sizeof(void *) + (sizeof(int)*2) + (sizeof(Point)*3) + ((strlen(fontname) + 4) & ~3) + ((strlen(label) + 4) & ~3);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(parent, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }
  data[1] = glfloat_to_int(origin->x);
  data[2] = glfloat_to_int(origin->y);
  data[3] = glfloat_to_int(origin->z);
  data[4] = glfloat_to_int(size->x);
  data[5] = glfloat_to_int(size->y);
  data[6] = glfloat_to_int(size->z);
  data[7] = glfloat_to_int(rotation->x);
  data[8] = glfloat_to_int(rotation->y);
  data[9] = glfloat_to_int(rotation->z);
  data[10] = glfloat_to_int(pointsize);
  data[11] = subtype;
  strcpy((char *)(&data[12]), fontname);
  strcpy((char *)(&data[12 + ((strlen(fontname) + 4) & ~3) / sizeof(int)]), label);

  head.len = dsize + sizeof(head);
  head.type = 0x3103;
  sendmessage(&head, data);
  free(data);
  *id = get_next_id();
  add_id_to_waiting_queue(*id);

  return next_error();
}

int syn_set_active(unsigned long id)
{
  int *data;
  int dsize;
  header head;

  dsize = (sizeof(void *) + sizeof(int)*2) + (sizeof(Point)*3);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }

  head.len = dsize + sizeof(head);
  head.type = 0x3104;
  sendmessage(&head, data);
  free(data);

  return next_error();
}

int syn_change_label(unsigned long id, char *label)
{
  int *data;
  int dsize;
  header head;

  dsize = sizeof(void *) + ((strlen(label) + 4) & ~3);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }
  strcpy((char *)(&data[1]), label);

  head.len = dsize + sizeof(head);
  head.type = 0x3108;
  sendmessage(&head, data);
  free(data);

  return next_error();
}

int syn_change_color(unsigned long id, GLfloat *color)
{
  int *data;
  int dsize;
  header head;

  dsize = (sizeof(void *) + sizeof(int)*2) + (sizeof(Point)*3);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }
  data[1] = glfloat_to_int(color[0]);
  data[2] = glfloat_to_int(color[1]);
  data[3] = glfloat_to_int(color[2]);
  data[4] = glfloat_to_int(color[3]);

  head.len = dsize + sizeof(head);
  head.type = 0x3105;
  sendmessage(&head, data);
  free(data);

  return next_error();
}

int syn_create_glyph(unsigned long *id, unsigned long parent, Point *origin, Point *size, Point *rotation, int visible, GLfloat *color, char *name)
{
   int *data;
   int dsize;
   header head;

   dsize = (sizeof(void *) + sizeof(int)*2) + (sizeof(Point)*3) + (sizeof(GLfloat)*4) + ((strlen(name) + 4) & ~3);
   data = malloc(dsize);
   if (!data)
      return -3;

// there has got to be a better way of doing this...
   if (parent != -1)
   {
      data[0] = (int) find_ptr_in_map(parent, 1);
      if(data[0] == 0)
      {
         free(data);
         return -4;
      }
   }
   else
   {
      data[0] = -1;
   } 

   data[1] = 0;  // delete me when there is more time
   data[2] = glfloat_to_int(origin->x);
   data[3] = glfloat_to_int(origin->y);
   data[4] = glfloat_to_int(origin->z);
   data[5] = glfloat_to_int(size->x);
   data[6] = glfloat_to_int(size->y);
   data[7] = glfloat_to_int(size->z);
   data[8] = glfloat_to_int(rotation->x);
   data[9] = glfloat_to_int(rotation->y);
   data[10] = glfloat_to_int(rotation->z);
   data[11] = visible;
   data[12] = glfloat_to_int(color[0]);
   data[13] = glfloat_to_int(color[1]);
   data[14] = glfloat_to_int(color[2]);
   data[15] = glfloat_to_int(color[3]);

   strcpy((char *)(&data[12]), name);

   head.len = dsize + sizeof(head);
   head.type = 0x3106;
   sendmessage(&head, data);
   free(data);
   *id = get_next_id();
   add_id_to_waiting_queue(*id);

   return next_error();
}

int syn_create_region(unsigned long *id, unsigned long parent, int type, Point *origin, Point *size, Point *rotation, int visible)
{
  int *data;
  int dsize;
  header head;

  dsize = (sizeof(void *) + sizeof(int)*2) + (sizeof(Point)*3);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(parent, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }
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
  data[11] = type;  // unused.  needed?

  head.len = dsize + sizeof(head);
  head.type = API_SYN_CREATE_REIGON;
  sendmessage(&head, data);
  free(data);
  *id = get_next_id();
  add_id_to_waiting_queue(*id);

  return next_error();
}

int syn_create_group(unsigned long *id, unsigned long parent, int type, Point *origin, Point *size, Point *rotation, int visible, char *label)
{
  int *data;
  int dsize;
  header head;

  dsize = (sizeof(void *) + sizeof(int)*2) + (sizeof(Point)*3) + ((strlen(label) + 4) & ~3);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(parent, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }
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
  strcpy((char *)(&data[11]), label);

  head.len = dsize + sizeof(head);
  head.type = API_SYN_CREATE_GROUP;
  sendmessage(&head, data);
  free(data);
  *id = get_next_id();
  add_id_to_waiting_queue(*id);

  return next_error();
}

int syn_create_list(unsigned long *id, unsigned long parent, int type, Point *origin, Point *size, Point *rotation, int columns, int visible)
{
   int *data;
   int dsize;
   header head;

   dsize = (sizeof(void *) + (sizeof(int)*2) + (sizeof(Point)*3) + (sizeof(int)*2));
   data = malloc(dsize);
   if (!data)
      return -3;

// there has got to be a better way of doing this...
   data[0] = (int) find_ptr_in_map(parent, 1);
   if (data[0] == 0)
   {
      free(data);
      return -4;
   }
   data[1] = glfloat_to_int(origin->x);
   data[2] = glfloat_to_int(origin->y);
   data[3] = glfloat_to_int(origin->z);
   data[4] = glfloat_to_int(size->x);
   data[5] = glfloat_to_int(size->y);
   data[6] = glfloat_to_int(size->z);
   data[7] = glfloat_to_int(rotation->x);
   data[8] = glfloat_to_int(rotation->y);
   data[9] = glfloat_to_int(rotation->z);
   data[10] = columns;
   data[11] = visible;

   head.len = dsize + sizeof(head);
   head.type = API_SYN_CREATE_LIST;
   sendmessage(&head, data);
   free(data);
   *id = get_next_id();
   add_id_to_waiting_queue(*id);

   return next_error();
}

int syn_add_to_text(unsigned long id, char *buffer, unsigned long actual)
{
  int *data;
  int dsize;
  header head;

  dsize = (sizeof(void *) + sizeof(int)*2 + sizeof(char)*actual);
  data = malloc(dsize);
  if(!data)
    return -3;

// there has got to be a better way of doing this...
  data[0] = (int) find_ptr_in_map(id, 1);
  if(data[0] == 0)
  {
    free(data);
    return -4;
  }

  data[1] = actual;
  strncpy((char *)(&data[2]), buffer, actual);

  head.len = dsize + sizeof(head);
  head.type = 0x3112;
  sendmessage(&head, data);
  free(data);

  return next_error();
}

int process_control_message(unsigned int len, int type, void *message)
{
  switch(type)
  {
    case API_SYN_GET_BUTTON_STATUS:
      get_button_status = ((int *)message)[0];
      break;

    case API_SYN_GET_PROGBAR:
      get_progbar = ((int *)message)[0];
      break;

    case API_SYN_GET_SCROLLBAR_VALUE:
      get_scrollbar_value = ((int *)message)[0];
      break;

    case API_SYN_GET_SLIDER_VALUE:
      get_slider_value = ((int *)message)[0];
      break;

    case API_SYN_GET_LABEL_LEN:
      get_label_len = ((int *)message)[0];
      break;

    case API_SYN_GET_LABEL:
      strcpy(inlabel, (char *)(&((int *)message)[0]));
      labelin = 1;
      break;

    case 0x3101: // change progress bar
    case 0x3102: // reload_fonts
    case 0x3010: // reshape object
    case API_SYN_SET_VISIBILITY:
    case API_SYN_RESHAPE_OBJECT_WRT:
    case 0x3107: // change scrollbar value
    case 0x3104: // set active
    case 0x3108: // change label
    case 0x3112: // add to text
    case 0x3105: // change color
    case API_SYN_SET_BUTTON_STATUS:
    case API_SYN_CHANGE_SLIDER_VALUE:
      add_error_to_queue(*((int *)(message)));
      break;

    default:
      if(*((int *)(message)) == 0)
        add_error_to_queue(-1);
      else
      {
        process_id_message(message);
        add_error_to_queue(0);
      }
      break;
  }
  return 0;
}
