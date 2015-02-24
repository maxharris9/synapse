/* example plugin */

#include <plugin.h>
#include <GL/gl.h>
#include <stdio.h>

char *sp_name = "Template plugin";
char *sp_calls[] = {
  "create_object",  // "standard" messages don't start with an underscore
  "destroy_object",
  "_change_clear_color",  // "custom" messages do start with an underscore
  "_reset_clear_color",
  NULL};
volatile int sp_firstnode;
int sp_numnodes = 1;

static int _change_clear_color(GLfloat *color);
static int _reset_clear_color(void);


void _init(void)
{
  printf("debug: plugin is here\n");
}
