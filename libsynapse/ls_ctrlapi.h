#include "libsynapse_internal.h"

#ifndef _LS_CTRLAPI_H_
#define _LS_CTRLAPI_H_

int syn_get_label_len(unsigned long id, int *len);
int syn_get_label(unsigned long id, char *label);

int syn_create_button(unsigned long *id, unsigned long parent, int type, Point *origin, Point *size, Point *rotation, int visible, char *label);

int syn_create_bar(unsigned long *id, unsigned long parent, int type, Point *origin, Point *size, Point *rotation, int min, int max, int value, int window, int visible);

int syn_change_progbar(unsigned long id, int percentage);
int syn_change_scrollbar_value(unsigned long id, int value);
int process_control_message(unsigned int len, int type, void *message);

int syn_create_text(unsigned long *id, unsigned long parent, Point *origin, Point *size, Point *rotation, GLfloat pointsize, int subtype, char *fontname, char *label);
int syn_add_to_label(unsigned long id, char *buffer, unsigned long actual);

int syn_set_active(unsigned long id);

int syn_change_color(unsigned long id, GLfloat *color);

int syn_reshape_object(unsigned long id, Point *origin, Point *size, Point *rotation);

int syn_create_glyph(unsigned long *id, unsigned long parent, Point *origin, Point *size, Point *rotation, int visible, GLfloat *color, char *label);
int syn_set_glyph_lock(unsigned long id, int locked);

int syn_create_region(unsigned long *id, unsigned long parent, int type, Point *origin, Point *size, Point *rotation, int visible);
int syn_create_group(unsigned long *id, unsigned long parent, int type, Point *origin, Point *size, Point *rotation, int visible, char *label);

int syn_destroy_object(unsigned long id);

#endif
