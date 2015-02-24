/* plugin.c
  portable plug-in interface code
  copyright (c) 1999 objective reality computer, llc.
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "plugin.h"
#include "tree.h"
#include "eachfile.h"

treenode *pluginroot;

int init_plugins(void)
{
  pluginroot = create_child(&rootnode, sizeof(treenode));
  pluginroot->type = ROOT;
  pluginroot->subtype = 0;

  fprintf(stderr, "Looking for plugins... ");
  fflush(stderr);

  set_file_callback(&load_plugin);
  do_file_search("/usr/lib/synapse/plugins", ".so");
  return 0;
}

void unload_plugins(void)
{
  while(pluginroot->child)
    unload_plugin(((plugin_module *)(pluginroot->child->prop))->name);
}

int send_plugin_message(treenode *plugin, unsigned int message, unsigned int size, void *msgdata)
{
  int ret;
  msg_header msghead;

  msghead.id = message;
  msghead.len = size + sizeof(msg_header);

  if(plugin)
    ret = (*(((plugin_module *)(plugin->prop))->api_call)) (&msghead, msgdata);
  else
    ret = -1;

  return ret;
}

treenode *find_plugin_by_nodetype(int type)
{
  return 0;
}

void send_plugin_draw(plugin_module *plugin, treenode *node)
{
}

void send_plugin_destroy(plugin_module *plugin, treenode *node)
{
}

int get_msgid_set(int num)
{
  static int nextnum = FIRST_API;
  int ret;

  if((unsigned int)(nextnum) + (unsigned int)(num) > (unsigned int)(0xFFFFFFFF))
    return -1;  // yah right, but _i_ try to write rabbit-free code

  ret = nextnum;
  nextnum += num;
  return ret;
}

int get_node_set(int num)
{
  static int nextnum = FIRST_NODE;
  int ret;

  if((unsigned int)(nextnum) + (unsigned int)(num) > (unsigned int)(0xFFFFFFFF))
    return -1;  // yah right, but _i_ try to write rabbit-free code

  ret = nextnum;
  nextnum += num;
  return ret;
}

int spa_init(int msgtypes, int nodetypes, char *name, int *nodestart)
{
  treenode *loading;
  plugin_module *plugin;

  loading = which_loading();
  if(!loading)
    return -1;

  if(find_plugin_by_name(name))
    return -1;

  plugin = (plugin_module *)(loading->prop);

  if(msgtypes == 0)
    plugin->api_start = plugin->api_end == 0;
  else
  {
    plugin->api_start = get_msgid_set(msgtypes);
    if(plugin->api_start < 0)
      return -1;
    plugin->api_end = plugin->api_start + msgtypes;
  }

  if(nodetypes == 0)
    plugin->node_start = plugin->node_end = 0;
  else
  {
    plugin->node_start = get_node_set(nodetypes);
    if(plugin->node_start < 0)
      return -1;
    plugin->node_end = plugin->node_start + nodetypes;
  }
  *nodestart = plugin->node_start;

  plugin->name = strdup(name);
  printf("Registering plugin %s\n", plugin->name);

  return 0;
}
