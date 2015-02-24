/* font.c -
 * font routines for System 1.0.
 * Copyright(c) 1998 Objective Reality Computer.
 * By Nick McKinney and Max Harris. 
 */

#include <stdio.h> // for printf
#include <GL/gl.h>
#include <GL/glu.h>
#include <sys/types.h> // for open()
#include <sys/stat.h> // for open()
#include <fcntl.h> // for open()
#include <dirent.h> // for getdirentries()
#include <stdlib.h> // for malloc()
#include <string.h> // for strstr()
#include "font.h" // for text
#include "tree.h"
#include "/usr/include/freetype1/freetype/freetype.h"
#include "bar.h"
#include <GL/glut.h> // for glut
#include "display.h"

#define LBUF 5

static TT_Engine engine;
static treenode *fontroot;

int init_ttfonts(void)
{
  TT_Init_FreeType(&engine);
  fontroot = create_child(&rootnode, sizeof(treenode));
  load_fonts(fontroot);
  return 0;
}

int reload_fonts(treenode *fontbar)
{
   treenode *that;
   int howmany, remaining;

   howmany = load_font_names(fontroot);
   that = fontroot->child;

   remaining = howmany;
   change_progress_bar(fontbar, 0);
   display();

   while (that != NULL)
   {
//       printf("Loading font face %s...", ((font *)(that->prop))->facename);
       fflush(stdout);
       load_font((font *)(that->prop));
//       printf("\n");
       that = that->next;

       remaining--;
       change_progress_bar(fontbar, ((float)(howmany - remaining) / howmany) * 100);
       display();
   }

   return 0;
}

GLfloat get_string_width(char *text, font *face, GLfloat pt)
{
  GLfloat width = 0.0;
  char current;

  current = *text++;
  while(current)
  {
    width += face->metrics[(int)current].xadvance * (pt / 24.0);
    current = *text++;
  }
  return width;
}

// this function currently ignores control characters (like CR)
GLfloat get_string_height(char *text, font *face, GLfloat pt)
{
  GLfloat height = 0.0;
  char current;

  current = *text++;
  height = face->base2base * (pt / 24.0);
  while(current)
  {
    if(current == '\n')
      height += face->base2base * (pt / 24.0);
    current = *text++;
  }

  height -= face->leading;
  return height;
}

font *find_font_by_name(char *name)
{
  treenode *thing;

  thing = fontroot->child;
  while(thing)
  {
    if(strcmp( ((font *)(thing->prop))->facename, name ) == 0)
      return ((font *)(thing->prop));
    thing = thing->next;
  }
  return 0;
}

int load_font_names(treenode *fontroot)
{
   font *that;
   treenode *thing;
   DIR *directory;
   struct dirent *currententry;
   int howmany = 0;

   directory = opendir("resources");
   if (directory == NULL)
   {
      printf("hmm. can't seem to open resources.\n");
      exit(1);
   }

   currententry = readdir(directory);

   while (currententry != NULL)
   {
      if (strstr(currententry->d_name, ".ttf") == NULL)
      {
          currententry = readdir(directory);
          continue;
      }

      thing = create_child(fontroot, sizeof(treenode));
      thing->prop = malloc(sizeof(font));
      that = (font *)(thing->prop);

      that->filename = (char *) malloc(strlen(currententry->d_name) + strlen("resources/") + sizeof(char));
      strcpy(that->filename, "resources/");
      strcat(that->filename, currententry->d_name);

      that->facename = (char *) malloc(strlen(currententry->d_name) + sizeof(char));
      strcpy(that->facename, currententry->d_name);

      currententry = readdir(directory);
      howmany++;
   }

   closedir(directory);
   return(howmany);
}

int load_font(font *thing)
{
  int xmax = 0, ymax = 0, rowymax = 0;
//int xpos = 0, ypos = 0;
  int xpos = 1, ypos = 1;
  int lastxchar = 0, lastychar = 0, lastxmax = 0;
  int lastrowymax = 0;
  TT_Instance instance;
  TT_Glyph glyph;
  TT_CharMap charmap;
  TT_Face face;
  TT_Face_Properties fprops;
  TT_Glyph_Metrics metrics;
  TT_Instance_Metrics imetrics;
  TT_Raster_Map rastermap;
  int i, j;
  int totaltex = 0;
  int textures = 0;
  int xbearings[256], ybearings[256];
  unsigned char palette[5] = {0, 63, 127, 191, 255};
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  TT_Open_Face(engine, thing->filename, &face);
  TT_New_Instance(face, &instance);
  TT_Set_Instance_CharSize(instance, 24 * 64);
  TT_New_Glyph(face, &glyph);

  TT_Get_Face_Properties(face, &fprops);
  TT_Get_Instance_Metrics(instance, &imetrics);

  thing->leading = fprops.os2->sTypoLineGap;
  thing->leading = (thing->leading * imetrics.y_scale) / (0x10000 * 64.0);
  thing->leading = (thing->leading * V_FOV_RATIO) / 600;

  thing->ascent = fprops.os2->sTypoAscender;
  thing->ascent = (thing->ascent * imetrics.y_scale) / (0x10000 * 64.0);
  thing->ascent = (thing->ascent * V_FOV_RATIO) / 600;

  thing->decent = fprops.os2->sTypoDescender;
  thing->decent = (thing->decent * imetrics.y_scale) / (0x10000 * 64.0);
  thing->decent = (thing->decent * V_FOV_RATIO) / 600;

  thing->base2base = thing->ascent - thing->decent + thing->leading;

  for(i= 0; i < fprops.num_CharMaps; ++i)
  {
    short unsigned platform= 0;
    short unsigned encoding= 0;
    TT_Get_CharMap_ID( face, i, &platform, &encoding );
    if ( (platform == 3 && encoding == 1 )  ||
         (platform == 0 && encoding == 0 ) )
    {
      TT_Get_CharMap( face, i, &charmap );
      break;
    }
  }
  if(i == fprops.num_CharMaps)
  {
    printf("no unicode encoding found\n");
    exit(1);  // too harsh to leave in...for debugging only
  }

  // PASS 1
  // repeat for each glyph
  for(i = 32; i < 256; i++)
  {
    TT_Load_Glyph(instance, glyph, TT_Char_Index(charmap, i), TTLOAD_SCALE_GLYPH | TTLOAD_HINT_GLYPH);
    TT_Get_Glyph_Metrics(glyph, &metrics);
    xbearings[i] = metrics.bearingX / 64;
    ybearings[i] = metrics.bearingY / 64;

    thing->metrics[i].xbearing = (xbearings[i] * H_FOV_RATIO) / 800;
    thing->metrics[i].ybearing = (ybearings[i] * V_FOV_RATIO) / 600;
    thing->metrics[i].xMin = (metrics.bbox.xMin / 64 * H_FOV_RATIO) / 800;
    thing->metrics[i].yMin = (metrics.bbox.yMin / 64 * V_FOV_RATIO) / 600;
    thing->metrics[i].xMax = (metrics.bbox.xMax / 64 * H_FOV_RATIO) / 800;
    thing->metrics[i].yMax = (metrics.bbox.yMax / 64 * V_FOV_RATIO) / 600;
    thing->metrics[i].xsize = (((metrics.bbox.xMax - metrics.bbox.xMin)/64) * H_FOV_RATIO) / 800;
    thing->metrics[i].ysize = (((metrics.bbox.yMax - metrics.bbox.yMin)/64) * V_FOV_RATIO) / 600;
    thing->metrics[i].xadvance = ((metrics.advance / 64) * H_FOV_RATIO) / 800;

    thing->origin[i].y = ypos;
    if(xpos + (metrics.bbox.xMax - metrics.bbox.xMin)/64 + LBUF> 256)
    {
      xpos = LBUF;
      if(ypos + rowymax + LBUF > 255)   // > 256
      {
        for(j = lastychar; j < i; j++)
          thing->origin[i].y -= lastrowymax;
        for(j = 1; j < lastxmax; j <<= 1);
        thing->texwidth[textures] = j;
        for(j = 1; j < ymax; j <<= 1);
        thing->texheight[textures] = j;
        thing->texboxes[textures] = calloc(1, thing->texwidth[textures] * thing->texheight[textures]);
        totaltex += thing->texwidth[textures] * thing->texheight[textures];
        textures++;
        xmax = 0;
        ymax = 0;
        ypos = LBUF;
      }
      ypos += rowymax + LBUF;
      thing->origin[i].y = ypos;
      lastrowymax = rowymax;
      rowymax = 0;
      lastychar = lastxchar;
      lastxmax = xmax;
      if(ypos > ymax)
        ymax = ypos;
    }
    thing->origin[i].x = xpos;
    thing->origin[i].z = textures;
    thing->dimensions[i].x = (metrics.bbox.xMax - metrics.bbox.xMin)/64;
    thing->dimensions[i].y = (metrics.bbox.yMax - metrics.bbox.yMin)/64;
    xpos += (metrics.bbox.xMax - metrics.bbox.xMin)/64 + LBUF;
    lastxchar = i;
    if(rowymax < (metrics.bbox.yMax - metrics.bbox.yMin)/64)
      rowymax = (metrics.bbox.yMax - metrics.bbox.yMin)/64;
    if(xpos > xmax)
      xmax = xpos;
  }

  if(xmax > lastxmax)
    lastxmax = xmax;
  for(j = 1; j < lastxmax; j <<= 1);
  thing->texwidth[textures] = j;
  for(j = 1; j < ymax; j <<= 1);
  thing->texheight[textures] = j;
  thing->texboxes[textures] = calloc(1, thing->texwidth[textures] * thing->texheight[textures]);
  totaltex += thing->texwidth[textures]  * thing->texheight[textures];
  textures++;


  TT_Set_Raster_Gray_Palette(engine, palette);

  // PASS 2
  // repeat for each glyph
  for(i = 32; i < 256; i++)
  {
    TT_Load_Glyph(instance, glyph, TT_Char_Index(charmap, i), TTLOAD_SCALE_GLYPH | TTLOAD_HINT_GLYPH);
    rastermap.rows = thing->texheight[thing->origin[i].z];
    rastermap.cols = thing->texwidth[thing->origin[i].z];
    rastermap.width = thing->texwidth[thing->origin[i].z];
    rastermap.size = rastermap.rows * rastermap.cols;
    rastermap.bitmap = thing->texboxes[thing->origin[i].z];
    rastermap.flow = TT_Flow_Up;
    TT_Get_Glyph_Pixmap(glyph, &rastermap, (thing->origin[i].x - xbearings[i]) * 64, (thing->origin[i].y + (thing->dimensions[i].y - ybearings[i])) * 64);
  }

  // PASS 3
  // repeat for each texbox
  for(i = 0; i < textures; i++)
  {
    glGenTextures(1, &thing->textures[i]);
    glBindTexture(GL_TEXTURE_2D, thing->textures[i]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, thing->texwidth[i], thing->texheight[i], 0, GL_ALPHA, GL_UNSIGNED_BYTE, thing->texboxes[i]);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_ALPHA, thing->texwidth[i], thing->texheight[i], GL_ALPHA, GL_UNSIGNED_BYTE, thing->texboxes[i]);
  }

  TT_Close_Face(face);
  return 0;
}

int load_fonts(treenode *darroot)
{
   treenode *that;
   
// font *dat;
// treenode *thing;

// static int when = 0;

// if (when != 0)
      load_font_names(fontroot);
/* else
   {
      thing = create_child(fontroot, sizeof(treenode));
      thing->prop = malloc(sizeof(font));
      dat = (font *)(thing->prop);
      dat->facename = (char *) malloc(9 + sizeof(char));
      dat->filename = (char *) malloc(20 + sizeof(char));

      strcpy(dat->facename, "20thcent.ttf");
      strcpy(dat->filename, "resources/20thcent.ttf");
   }
*/
   that = fontroot->child;

   while (that != NULL)
   {
      printf("Loading font face %s...", ((font *)(that->prop))->facename);
      fflush(stdout); 
      load_font((font *)(that->prop));
      printf("\n");
      that = that->next;
   }

// when++;
   return 0;
}
