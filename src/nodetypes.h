#ifndef _NODETYPES_H_
#define _NODETYPES_H_

enum type
{
   ROOT, // done
   POINTER, // done
   CAMERA, // done
   APP, // done
   BUTTON, // near done
   FLATWINDOW, // done
   DEEPWINDOW,
   BAR, // done
   TEXT, // done
   GLYPH, // done
   GROUP,
   MENU,
   LIST,
   REIGON // done
};

enum subtype
{
   NONE, // NONE, done
   PUSH, // BUTTON, done
   CHECK, // BUTTON, done
   RADIO, // BUTTON
   BORDER, // BUTTON, done
   CORNER, // BUTTON, done
   MENUBUTTON, // BUTTON
   FILLER, // BUTTON,
   DIALOG, // FLATWINDOW, done
   MINI, // FLATWINDOW, done
   REGULAR, // FLATWINDOW, done
   LABEL, // TEXT, done
   BOX, // TEXT,
   MULTIBOX, // TEXT, done
   SIX, // MENU
   FACE, // MENU
   PROGRESS, // BAR, done
   SLIDER, // BAR, done
   VSLIDER, // BAR, done
   SCROLL, // BAR, done
   VSCROLL, // BAR, done
   FONT, // ROOT, done
   RESOURCE, // ROOT, done
   SUBREIGON // REIGON, done
};

void printnodetype(int type);
void printnodesubtype(int subtype);

#endif
