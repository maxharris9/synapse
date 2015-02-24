#include <stdarg.h>
#include "tree.h"

#ifndef _PLUG_IN_H_
#define _PLUG_IN_H_

#define FIRST_NODE 0x10000000
#define FIRST_API  0x10000000

typedef struct msg_header
{
  int id;
  int len;
} msg_header;

typedef struct plugin_module
{
  char *name;
  char *filename;
  void *handle;
  int (*api_call) (msg_header *header, void *msgdata); // this is used for all plug-in functionality, regardless of whether it comes from an internal or external source (resemble orkernel messages?)
  int (*plugin_init) (void);
  void (*plugin_draw) (treenode *object);
  int (*plugin_event) (treenode *target, treenode *node, int event, int modifier, point *where, unsigned long usec);
  void (*plugin_cleanup) (void);
  int api_start;  // this is abstracted from the plug-in...to it, all api calls start at #0
  int api_end;
  int node_start;
  int node_end;
  int usage;  // how many external proggies are currently using me
} plugin_module;

int load_plugin(char *filename);
int unload_plugin(char *name);
void unload_plugins(void);
treenode *find_plugin_by_name(char *name);
treenode *find_plugin_by_filename(char *name);
int send_plugin_message(treenode *plugin, unsigned int message, unsigned int size, void *msgdata);
int init_plugins(void);
treenode *which_loading(void);    // someone please think of a better way
treenode *which_unloading(void);  // to track these values

int spa_init(int msgids, int nodes, char *name, int *nodestart);
void spa_close(void);

treenode *find_plugin_by_nodetype(int type);
void send_plugin_draw(plugin_module *plugin, treenode *node);
int send_plugin_event(plugin_module *plugin, treenode *try, treenode *node, int event, int modifier, point *where, unsigned long usec);
void send_plugin_destroy(plugin_module *plugin, treenode *node);

#endif
