/* resource.c -
 * resource handling functions for System 1.0.
 * Copyright(c) 1998-99 Objective Reality Computer.
 * By Nick McKinney and Max Harris.
 */

#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "resource.h"
#include "resources.h"
#include "tree.h"
#include "synapse.h" // for type bool

GLuint resources[64];
static treenode *resroot;

void init_resources(void)
{
  cache_resources();  // old

  resroot = create_child(&rootnode, sizeof(treenode));
  resroot->type = ROOT;
  resroot->subtype = RESOURCE;
  resroot->visible = FALSE;

  load_resources();
}

resprop *find_resource(char *name)
{
  treenode *thing;

  thing = resroot->child;
  while(thing)
  {
    if(strcmp( ((resprop *)(thing->prop))->resname, name ) == 0)
      return ((resprop *)(thing->prop));
    thing = thing->next;
  }
  return (resprop *)0;
}

int load_resource(treenode *thing)
{
   resprop *that;

   that = (resprop *)(thing->prop);

   that->gl_id = glGenLists(1);

   glNewList(that->gl_id, GL_COMPILE);
     glPushMatrix();
     drawresource(that->filename);
     glPopMatrix();
   glEndList();
   return 0;
}

int load_resources(void)
{
   resprop *that;
   treenode *thing;
   DIR *directory;
   struct dirent *currententry;
   int howmany = 0;

   directory = opendir("resources");
   if (directory == NULL)
   {
      fprintf(stderr, "hmm. can't seem to open resources.\n");
      exit(1);
   }
   fprintf(stderr, "Loading resources...\n");

   currententry = readdir(directory);

   while (currententry != NULL)
   {
      if (strstr(currententry->d_name, ".res") == NULL)
      {
          currententry = readdir(directory);
          continue;
      }

      thing = create_child(resroot, sizeof(treenode));
      thing->type = RESOURCE;
      thing->prop = malloc(sizeof(resprop));
      that = (resprop *)(thing->prop);

      that->filename = (char *) malloc(strlen(currententry->d_name) + strlen("resources/") + sizeof(char));
      strcpy(that->filename, "resources/");
      strcat(that->filename, currententry->d_name);

      that->resname = (char *) malloc(strlen(currententry->d_name) + sizeof(char));
      strcpy(that->resname, currententry->d_name);

      load_resource(thing);

      currententry = readdir(directory);
      howmany++;
   }

   closedir(directory);
   return(howmany);
}


// need more sanity checks in this next function
void drawresource(char *resourcename)
{
   FILE *file;
   GLfloat a[4];
   GLfloat nlen;
   char x, tempchar;

   // int y;
   int begun = FALSE;

   file = fopen(resourcename, "r");
   if (file == NULL)
      return;
   while (!feof(file))
   {
      x = fgetc(file);
      switch (x)
      {
         case 'F':
            if (!begun)
            {
               glBegin(GL_TRIANGLE_FAN);
               begun = TRUE;
            }
            break;

         case 'Q':
            if (!begun)
            {
               glBegin(GL_QUAD_STRIP);
               begun = TRUE;
            }
            break;

         case 'q':
            if (!begun)
            {
               glBegin(GL_QUADS);
               begun = TRUE;
            }
            break;

         case 'T':
            if (!begun)
            {
               glBegin(GL_TRIANGLE_STRIP);
               begun = TRUE;
            }
            break;

         case 'E':
            if (begun)
            {
               glEnd();
               begun = FALSE;
            }
            break;

         case 't':
            if (!begun)
            {
               glBegin(GL_TRIANGLES);
               begun = TRUE;
            }
            break;

         case 'S':
            fscanf(file, "%f", &a[0]);
            fscanf(file, "%f", &a[1]);
            fscanf(file, "%f", &a[2]);

            glScalef(a[0], a[1], a[2]);
            break;

         case 'R':
            fscanf(file, "%f", &a[0]);
            fscanf(file, "%f", &a[1]);
            fscanf(file, "%f", &a[2]);
            fscanf(file, "%f", &a[3]);

            glRotatef(a[0], a[1], a[2], a[3]);
            break;

         case 'c':
            fscanf(file, "%f", &a[0]);
            fscanf(file, "%f", &a[1]);
            fscanf(file, "%f", &a[2]);
            fscanf(file, "%f", &a[3]);

            glColor4fv(a);
            break;

         case '{':
            fscanf(file, "%f", &a[0]);
            fscanf(file, "%f", &a[1]);
            fscanf(file, "%f", &a[2]);

            if (begun)
               glVertex3fv(a);
            break;

         case '[':
            fscanf(file, "%f", &a[0]);
            fscanf(file, "%f", &a[1]);
            fscanf(file, "%f", &a[2]);
            nlen = sqrt((double)(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]));
            if(nlen == 0.0)  // invalid normal
               break;

            a[0] /= nlen;
            a[1] /= nlen;
            a[2] /= nlen;
            glNormal3fv(a);
            break;

         case '#':
            tempchar = fgetc(file);

            while (tempchar != '\n')
            {
               tempchar = fgetc(file);
            }

            break;

         default:;
      }
   }

   fclose(file);

   if (begun)
      glEnd();
}

void cache_resources(void)
{
   resources[RES_CORNERBL1] = glGenLists(1);
   glPushMatrix();
   glNewList(resources[RES_CORNERBL1], GL_COMPILE);
   drawresource("resources/cornerbl.res");
   glEndList();
   glPopMatrix();

   resources[RES_POINTER] = glGenLists(1);
   glNewList(resources[RES_POINTER], GL_COMPILE);
   drawresource("resources/pointer.res");
   glEndList();

   resources[RES_GRIPPY] = glGenLists(1);
   glNewList(resources[RES_GRIPPY], GL_COMPILE);
   glPushMatrix();
   drawresource("resources/grippy.res");
   glTranslatef(0.0, 0.02666, 0.0);
   drawresource("resources/grippy.res");
   glTranslatef(0.0, 0.02666, 0.0);
   drawresource("resources/grippy.res");
   glTranslatef(0.0, 0.02667, 0.0);
   drawresource("resources/grippy.res");
   glPopMatrix();
   glEndList();

   resources[RES_UPGLYPH] = glGenLists(1);
   glNewList(resources[RES_UPGLYPH], GL_COMPILE);
   drawresource("resources/upglyph.res");
   glEndList();

   resources[RES_CHECK] = glGenLists(1);
   glNewList(resources[RES_CHECK], GL_COMPILE);
   drawresource("resources/check.res");
   glEndList();

   resources[RES_DEEPGRIP] = glGenLists(1);
   glNewList(resources[RES_DEEPGRIP], GL_COMPILE);
   drawresource("resources/deepgrip.res");
   glEndList();

   resources[RES_CLOSEGRIP] = glGenLists(1);
   glNewList(resources[RES_CLOSEGRIP], GL_COMPILE);
   glPushMatrix();
   glScalef(1.5, 1.5, 1.5);
   glTranslatef(-0.0135, 0.02666, 0.0);
   drawresource("resources/grippy.res");
   glPopMatrix();
   glEndList();

   resources[RES_SQUAREGRIP] = glGenLists(1);
   glNewList(resources[RES_SQUAREGRIP], GL_COMPILE);
   glPushMatrix();
   glScalef(1.5, 1.5, 1.5);

   glTranslatef(0.015, 0.015, 0.0);
   drawresource("resources/square.res");
   glTranslatef(0.0, 0.02666, 0.0);
   drawresource("resources/square.res");
   glTranslatef(0.02666, 0.0, 0.0);
   drawresource("resources/square.res");
   glTranslatef(0, -0.02666, 0.0);
   drawresource("resources/square.res");
   glPopMatrix();
   glEndList();

   resources[RES_RADIO] = glGenLists(1);
   glNewList(resources[RES_RADIO], GL_COMPILE);
   glPushMatrix();
   glTranslatef(0.049, 0.075, -0.1);
   glutSolidSphere(0.06, 12, 2);
   glPopMatrix();
   glEndList();
}
