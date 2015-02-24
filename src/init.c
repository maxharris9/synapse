/* init.c -
 * initialization routines for Synapse
 * Copyright(c) 1998-99 Objective Reality Computer.
 * By Nick McKinney and Max Harris.
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <GL/glut.h>
#include <stdio.h> // for printf()
#include "init.h" // for Window
#include "display.h" // for reshape(), display(), maincamera
#include "input.h" // for key(), specialkey(), clickonstuff(), mousedownmove(), and initmouse()
#include "resource.h" // for drawresource(), cache_resource()
#include "menu.h" // for add_menu_entry(), make_menu()
#include "font.h" // for init_output()
#include "window.h" // for init_windows(), create_window()
#include "deepwindow.h" // for create_deepwindow()
#include "tree.h" // for inittree()
#include "text.h" // for create_text()
#include "conn.h" // for sockets stuff
#include "coord.h" // for make_bbox()
#include "bar.h" // for create_bar()
#include "glyph.h" // for create_glyph()
#include "plugin.h"

int __synwidth, __synheight;
int killme = 0;
int Window;
int ScreenWidth, ScreenHeight;

void darquit(void)
{
  unload_plugins();  // don't ask me why i do this...it's for those
                     // "someone's going to write a plugin that stores
                     // a static value upon unloading" situations
  glutDestroyWindow(Window);
  exit(0);
}

void sigchild(int sig)
{
  killme = 1;
  wait(NULL);
}

void init(void)
{
   char *shell;

   glEnable(GL_POLYGON_SMOOTH);
   glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
   glEnable(GL_BLEND);
   
   glEnable(GL_COLOR_MATERIAL);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   glClearColor(0.0, 0.0, 0.0, 1);

   inittree();
   init_ttfonts();
   init_resources();
   init_plugins();

   set_current_camera(new_create_camera(&rootnode, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)); // sanity?

   new_mouse = new_create_pointer((treenode *)current_camera(), 0, 0, -10, 0, 0, 0, 0);
   if(!new_mouse)
   {
      printf("error creating pointer\n");
      exit(1);
   }

   if(!getenv("SYNAPSE_SHELL"))
   {
      printf("no shell set, exiting\n");
      exit(1);
   }

   shell = strdup(getenv("SYNAPSE_SHELL"));
   if(fork() == 0)
   {
      sleep(2);
      execl(shell, shell, NULL);
      printf("an error occurred running the shell\n");
      exit(1);
   }

   free(shell);

   signal(SIGCHLD, sigchild);
   signal(SIGPIPE, SIG_IGN);
}

void idle_(void)
{
  if(killme)
    darquit();
  process_connections();
  if(conn_waiting())
    new_conn();

  glutPostRedisplay();
}

int main(int argc, char **argv)
{
   WIDTH = 800;
   HEIGHT = 600;
   if(argc > 1)
      WIDTH = atoi(argv[1]);
   if(argc > 2)
      HEIGHT = atoi(argv[2]);

   if(WIDTH < 80)
   {
      fprintf(stderr, "error: minimum width is 80\n");
      exit(1);
   }

   if(HEIGHT < 60)
   {
      fprintf(stderr, "error: minimum height is 60\n");
      exit(1);
   }

   init_sockets();

// init_animation();

   glutInit(&argc, argv);
   glutInitWindowPosition(0, 0);
   glutInitWindowSize(WIDTH, HEIGHT);

   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_ALPHA | GLUT_DEPTH);

   Window = glutCreateWindow("Synapse 3DUI v0.3");
   if (!Window)
   {
      printf("Error: couldn't open window\n");
      exit(1);
   }
// glutFullScreen(); // doesn't go fullscreen in software if resolution is different
   glutSetCursor(GLUT_CURSOR_NONE);

   init();

   glutReshapeFunc(reshape);
   glutKeyboardFunc(key);
   glutSpecialFunc(specialkey);
   glutIdleFunc(idle_);
   glutMouseFunc(clickonstuff);
   glutDisplayFunc(display);
   glutMotionFunc(mousedownmove);
   glutPassiveMotionFunc(mousemove);
   glutMainLoop();
   return 0;
}
