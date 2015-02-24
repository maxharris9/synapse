#include "libsynapse_internal.h"

#ifndef _LS_WMAPI_H_
#define _LS_WMAPI_H_

int syn_create_flatwindow(unsigned long *id, int wintype, Point *origin, Point *size, Point *rotation, int visible, char *title);

int syn_create_deepwindow(unsigned long *id, int wintype, Point *origin, Point *size, Point *rotation, int visible, char *title);

int process_wmanager_message(unsigned int len, int type, void *message);

#endif
