#include "nodetypes.h"
#include <stdio.h>

#define caseof(type, label) case type : \
  printf(label); \
  break

void printnodetype(int type)
{
  switch(type)
  {
    caseof(ROOT, "root node");
    caseof(POINTER, "pointer");
    caseof(CAMERA, "camera");
    caseof(APP, "connection");
    caseof(BUTTON, "button");
    caseof(FLATWINDOW, "flat window");
    caseof(DEEPWINDOW, "deep window");
    caseof(BAR, "bar");
    caseof(TEXT, "text");
    caseof(GLYPH, "glyph");
    caseof(GROUP, "group");
    caseof(REIGON, "reigon");
    caseof(MENU, "menu");
    caseof(LIST, "list");

    default:
      printf("unknown type");
      break;
  }
}

void printnodesubtype(int type)
{
  switch(type)
  {
    caseof(NONE, "none");
    caseof(PUSH, "push");
    caseof(CHECK, "check");
    caseof(RADIO, "radio");
    caseof(BORDER, "border");
    caseof(CORNER, "corner");
    caseof(FILLER, "filler");
    caseof(DIALOG, "dialog");
    caseof(MINI, "mini");
    caseof(REGULAR, "regular");
    caseof(LABEL, "label");
    caseof(BOX, "box");
    caseof(MULTIBOX, "multibox");
    caseof(SIX, "six");
    caseof(SUBREIGON, "subreigon");
    caseof(FACE, "face");
    caseof(PROGRESS, "progress");
    caseof(SLIDER, "slider");
    caseof(VSLIDER, "vertical slider");
    caseof(SCROLL, "scroll");
    caseof(VSCROLL, "vertical scroll");
    caseof(FONT, "font");
    caseof(RESOURCE, "resource");

    default:
      printf("unknown subtype");
      break;
  }
}
