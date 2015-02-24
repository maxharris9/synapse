
#include <synapse/synapse.h>

#ifndef _CONTROLS_H_
#define _CONTROLS_H_

int syn_create_button(unsigned long *id, unsigned long parent, int type, Point *origin, Point *size, Point *rotation, int visible, char *label);
int syn_get_button_status(unsigned long id, int *status);
int syn_set_button_status(unsigned long id, int status);

int syn_create_bar(unsigned long *id, unsigned long parent, int type, Point *origin, Point *size, Point *rotation, int min, int max, int value, int window, int visible);

int syn_change_progbar(unsigned long id, int percentage);
int syn_get_progbar(unsigned long id, int *percentage);
int syn_change_scrollbar_value(unsigned long id, int value);
int syn_get_scrollbar_value(unsigned long id, int *value);
int syn_change_slider_value(unsigned long id, int value);
int syn_get_slider_value(unsigned long id, int *value);

int syn_create_text(unsigned long *id, unsigned long parent, Point *origin, Point *size, Point *rotation, GLfloat pointsize, int subtype, char *fontname, char *label);
int syn_add_to_text(unsigned long id, char *buf, unsigned long size);

int syn_set_active(unsigned long id);
int syn_set_visibility(unsigned long id, int visibility);

int syn_change_color(unsigned long id, GLfloat *color);
int syn_change_label(unsigned long id, char *label);
int syn_get_label(unsigned long id, char *label);
int syn_get_label_len(unsigned long id, int *len);

int syn_reshape_object(unsigned long id, Point *origin, Point *size, Point *rotation);
int syn_reshape_object_wrt(unsigned long id, unsigned long wrt, Point *origin, Point *size, Point *rotation);

int syn_create_glyph(unsigned long *id, long parent, Point *origin, Point *size, Point *rotation, int visible, GLfloat *color, char *label);
int syn_set_glyph_lock(unsigned long id, int locked);

int syn_create_region(unsigned long *id, unsigned long parent, int type, Point *origin, Point *size, Point *rotation, int visible);

int syn_create_group(unsigned long *id, unsigned long parent, int type, Point *origin, Point *size, Point *rotation, int visible, char *label);

int syn_create_list(unsigned long *id, unsigned long parent, int type, Point *origin, Point *size, Point *rotation, int columns, int visible);

int syn_destroy_object(unsigned long id);

#endif
