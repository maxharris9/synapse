#include "synapse.h"
#include "tree.h"

#ifndef _FONT_H_
#define _FONT_H_

typedef struct {
  GLfloat xsize, ysize;        // metrics here are in GL units, calculated as if the 24
  GLfloat xbearing, ybearing;  // point font were rendered WYSIWYG 11 units from camera
  GLfloat xMin, yMin, xMax, yMax;  // these make xsize and ysize obsolete...replace someday
  GLfloat xadvance, yadvance;
} glyph_metrics;

typedef struct
{
  char *filename;
  int ptsize;
  char *facename;

  // bitmap representation:
  void *texboxes[256];        // one for each bitmap
  unsigned int textures[256];          // one for each bitmap
  int texwidth[256];
  int texheight[256];
  ipoint origin[256];         // one for each glyph (z = which texture to use)
  ipoint dimensions[256];     // one for each glyph (z is unused)
  glyph_metrics metrics[256]; // one for each glyph

  GLfloat leading, ascent, decent;
  GLfloat base2base;

  // vector representation:
    // not yet
} font;

int load_fonts(treenode *darroot);
int load_font(font *thing);
int reload_fonts(treenode *thing);
int init_ttfonts(void);
font *find_font_by_name(char *name);
int load_font_names(treenode *fontroot);
GLfloat get_string_width(char *text, font *face, GLfloat pt);
GLfloat get_string_height(char *text, font *face, GLfloat pt);

#define H_FOV_RATIO  5.8948822     // 2z * tan(hfov/2)
#define V_FOV_RATIO  8.0073452     // 2z * tan(vfov/2)

#endif
