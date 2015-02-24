#ifndef __EACHFILE_H_
#define __EACHFILE_H_

void set_file_callback(int (*cb) (char *path));
int do_file_search(char *path, char *ext);

#endif
