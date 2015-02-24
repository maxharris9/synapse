#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "../tree.h"
#include "../plugin.h"

extern treenode *pluginroot;
static treenode *loading;
static treenode *unloading;

treenode *which_loading(void)
{
  return loading;
}

treenode *which_unloading(void)
{
  return unloading;
}

int load_plugin(char *filename)
{
  treenode *plugin;
  plugin_module *prop;

  plugin = create_child(pluginroot, sizeof(treenode));
  if(!plugin)
    return -1;

  plugin->type = plugin->subtype = 0;
  plugin->prop = malloc(sizeof(plugin_module));
  if(!plugin->prop)
  {
    destroy_node(plugin);
    return -1;
  }
  prop = (plugin_module *)(plugin->prop);

  prop->name = 0;
  prop->api_start = -1;
  prop->api_end = -1;
  prop->node_start = -1;
  prop->node_end = -1;

  prop->filename = strdup(filename);
  loading = plugin;
  prop->handle = dlopen(prop->filename, RTLD_NOW);
  if(!prop->handle)
  {
    free(prop->filename);
    free(prop);
    destroy_node(plugin);
    return -1;
  }

  prop->plugin_init = dlsym(prop->handle, "plugin_init");
  if(dlerror() != NULL)
  {
    fprintf(stderr, "failed to link plugin_init\n");
    unload_plugin(prop->filename);
    return -1;
  }

  prop->plugin_draw = dlsym(prop->handle, "plugin_draw");
  if(dlerror() != NULL)
  {
    fprintf(stderr, "failed to link plugin_draw\n");
    unload_plugin(prop->filename);
    return -1;
  }

  prop->plugin_cleanup = dlsym(prop->handle, "plugin_cleanup");
  if(dlerror() != NULL)
  {
    fprintf(stderr, "failed to link plugin_cleanup\n");
    unload_plugin(prop->filename);
    return -1;
  }

  prop->api_call = dlsym(prop->handle, "plugin_message");
  if(dlerror() != NULL)
  {
    fprintf(stderr, "failed to link plugin_message\n");
    unload_plugin(prop->filename);
    return -1;
  }

  if((*(prop->plugin_init)) () != 0)
  {
    fprintf(stderr, "failed to init\n");
    unload_plugin(prop->filename);
    return -1;
  }

  loading = 0;
  return 0;
}

int unload_plugin(char *name)
{
  treenode *plugin;

  plugin = find_plugin_by_name(name);
  if(!plugin)
  {
    plugin = find_plugin_by_filename(name);
    if(!plugin)
      return -1;
  }

  unloading = plugin;
  dlclose(((plugin_module *)(plugin->prop))->handle);
  unloading = NULL;
  free(((plugin_module *)(plugin->prop))->name);
  free(((plugin_module *)(plugin->prop))->filename);
  free(plugin->prop);
  destroy_node(plugin);
  return 0;
}

treenode *find_plugin_by_name(char *name)
{
  treenode *temp;

  temp = pluginroot->child;
  while(temp)
  {
    if(((plugin_module *)(temp->prop))->name)
      if(strcmp(name, ((plugin_module *)(temp->prop))->name) == 0)
        return temp;
    temp = temp->next;
  }
  return 0;
}

treenode *find_plugin_by_filename(char *name)
{
  treenode *temp;

  temp = pluginroot->child;
  while(temp)
  {
    if(strcmp(name, ((plugin_module *)(temp->prop))->filename) == 0)
      return temp;
    temp = temp->next;
  }
  return 0;
}

