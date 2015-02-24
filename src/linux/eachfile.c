/* eachfile.c -
 * if you want to do something to each file in a directory, use this file
 * Copyright(c) 1999 Objective Reality Computer.
 * By Nick McKinney and Max Harris.
 */

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

static int (*callback) (char *path);

void set_file_callback(int (*cb) (char *path))
{
  callback = cb;
}

int do_file_search(char *path, char *ext)
{
  DIR *directory;
  struct dirent *currententry;
  int howmany = 0;

  if(!callback)
    return -1;
  if(!path)
    return -1;
  directory = opendir(path);
  if (directory == NULL)
    return -1;

  currententry = readdir(directory);

  while (currententry != NULL)
  {
    static char *name;

    if(ext)
      if(strcmp(&(currententry->d_name[strlen(currententry->d_name) - strlen(ext)]), ext) != 0)
      {
        currententry = readdir(directory);
        howmany++;
        continue;
      }
    name = malloc(strlen(path) + strlen(currententry->d_name) + 2); // 1 for null term, 1 for slash
    strcpy(name, path);
    strcat(name, "/");
    strcat(name, currententry->d_name);
    if(callback(name) != 0)
      break;
    free(name);

    currententry = readdir(directory);
    howmany++;
  }

  closedir(directory);
  return(howmany);
}
