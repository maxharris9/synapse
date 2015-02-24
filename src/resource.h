#ifndef _RESOURCE_H_
#define _RESOURCE_H_

typedef struct resprop {
  char *filename;
  char *resname;
  GLint gl_id;
} resprop;

void drawresource(char *);
void cache_resources(void);
resprop *find_resource(char *name);
void init_resources(void);
int load_resources(void);

#endif

