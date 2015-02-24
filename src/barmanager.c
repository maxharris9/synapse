/* implements a "bar manager"-like functionality */

#include <stdlib.h>
#include <stdio.h>
#include "synapse.h"
#include "tree.h"
#include "event.h"
#include "bar.h"
#include "barmanager.h"
#include "button.h"
#include "resources.h"
#include "coord.h"
#include "glyph.h"

extern GLuint resources[64];

int resize_scrollbar(treenode *thing, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat rotx, GLfloat roty, GLfloat rotz)
{
  scrollbarprop *props;
  bar *bprops;

  props = (scrollbarprop *)(thing->prop);
  bprops = (bar *)(props->slider->prop);

  thing->rotation.x = rotx;
  thing->rotation.y = roty;
  thing->rotation.z = rotz;

  switch(thing->subtype)
  {
    case SCROLL:
      thing->origin.x = x;
      thing->origin.y = y;

      if(width < 0.75)
        width = 0.75;
      height = 0.25;

      thing->dimentions.x = width;
      thing->dimentions.y = height;
      props->upbutton->origin.x = width - 0.25;
      props->downbutton->origin.x = width - 0.5;
      props->slider->dimentions.x = width - 0.5;

      break;

    case VSCROLL:
      thing->origin.x = x;
      thing->origin.y = y;

      if(height < 0.75)
        height = 0.75;
      width = 0.25;
  
      thing->dimentions.x = width;
      thing->dimentions.y = height;
      props->slider->dimentions.y = height - 0.5;
    
      break;
  }

  make_bbox(props->upbutton);
  make_bbox(props->downbutton);
  make_bbox(props->slider);

  change_slider_values(props->slider, bprops->min, bprops->max, bprops->window, bprops->value);

  make_bbox(thing);
  return 0;
}

int scrollbar_event(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec)
{
  switch(event)
  {
    case EVENT_CHANGE:
      if(((scrollbarprop *)(target->prop))->change_handler)
        ((scrollbarprop *)(target->prop))->change_handler(target, node, event, modifier, where, usec);
      else
        send_node_event(target->parent, target, EVENT_CHANGE, ((bar *)(((scrollbarprop *)(target->prop))->slider->prop))->value, NULL);
      break;

    case EVENT_CLICK:
      if(node == ((scrollbarprop *)(target->prop))->upbutton)
      {
        treenode *scroller = ((scrollbarprop *)(target->prop))->slider;
        if(!change_slider_values(scroller, ((bar *)(scroller->prop))->min, ((bar *)(scroller->prop))->max, ((bar *)(scroller->prop))->window, ((bar *)(scroller->prop))->value + 1))
          send_node_event(target, target, EVENT_CHANGE, ((bar *)(scroller->prop))->value, NULL);
      }
      else if(node == ((scrollbarprop *)(target->prop))->downbutton)
      {
        treenode *scroller = ((scrollbarprop *)(target->prop))->slider;
        if(!change_slider_values(scroller, ((bar *)(scroller->prop))->min, ((bar *)(scroller->prop))->max, ((bar *)(scroller->prop))->window, ((bar *)(scroller->prop))->value - 1))
          send_node_event(target, target, EVENT_CHANGE, ((bar *)(scroller->prop))->value, NULL);
      }
      break;

    default:
      return 1;
  }
  return 0;
}

int change_progress_bar(treenode *that, GLfloat percentage)
{
   if(percentage > 100)
     percentage = 100;
   else if(percentage < 0)
     percentage = 0;

   that->child->dimentions.x = ((that->dimentions.x / 100) * percentage);
   ((bar *)(that->prop))->value = percentage;

   if (that->child->dimentions.x < 0.115)
      that->child->visible = FALSE;
   else
      that->child->visible = TRUE;
   return 0;
}

int change_slider_values(treenode *thing, int min, int max, int window, int value)
{
  treenode *thumb;
  int rvalue = 0;
  int oldvalue;

  thumb = thing->child;
  oldvalue = ((bar *)(thing->prop))->value;

  if(window > (max - min))
    window = max - min;
  if(window < 1)
    window = 1;

  if(value > (max - window))
    value = max - window;
  else if(value < min)
    value = min;

  if(value == oldvalue)
    rvalue = 1;

  ((bar *)(thing->prop))->min = min;
  ((bar *)(thing->prop))->max = max;
  ((bar *)(thing->prop))->window = window;
  ((bar *)(thing->prop))->value = value;

  switch(thing->subtype)
  {
    case SLIDER:
      thumb->dimentions.x = ((float)window / (float)(max - min)) * thing->dimentions.x;
      if(thumb->dimentions.x < 0.25)
        thumb->dimentions.x = 0.25;

//    thumb->origin.x = (thing->dimentions.x) * ((float)value / (float)(max - min));
      thumb->origin.x = thing->dimentions.x * (value - min) / (float)(max - min);

      thumb->child->origin.x = thumb->dimentions.x / 2.0 + 0.045;

      break;

    case VSLIDER:
      thumb->dimentions.y = ((float)window / (float)(max - min)) * thing->dimentions.y;
      if(thumb->dimentions.y < 0.25)
        thumb->dimentions.y = 0.25;
//    thumb->origin.y = (thing->dimentions.y) * ((float)(max - min - value) / (float)(max - min)) - thumb->dimentions.y ;
      thumb->origin.y = thing->dimentions.y * ((float)(max - value)/(float)(max - min)) - thumb->dimentions.y;

      thumb->child->origin.y = thumb->dimentions.y / 2.0 - 0.045;
     
      break;
  }

  make_bbox(thumb->child);
  make_bbox(thumb);

  return rvalue;
}

int thumb_event(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec)
{
   static point dar;

   switch(event)
   {
      case EVENT_MOUSEDRAG:
         dar.x += where->x;
         dar.y += where->y;
         dar.z += where->z;

         move_slider(node->parent, &dar);
         break;

      case EVENT_MOUSEDOWN:
         dar = node->origin;
         break;

      case EVENT_MOUSEUP:
         break;
   }
   return 0;
}

int create_progress(treenode *temp)
{
   treenode *prog;

   prog = create_button(temp, 0, 0, 0, 0.1, temp->dimentions.y, 0, 0, 0, temp->color, BUTTON, PUSH, "", 1);
   prog->mask = MASK_ALL;

// prog->eventmask &= ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG);

   ((bar *)(temp->prop))->value = 0.1;   // __HARDCODE__

   return(0);
}

int create_slider(treenode *temp, int min, int max, int window, int value)
{
   GLfloat gray[4] = {0.95, 0, 0.0, 1.0};
   treenode *thumb;

   gray[0] = temp->color[0]; // - 0.1;
   gray[1] = temp->color[1]; // - 0.1;
   gray[2] = temp->color[2]; // - 0.1;
   gray[3] = temp->color[3];

   thumb = create_button(temp, 0, 0, 0, 0.25, 0.25, 0, 0, 0, gray, BUTTON, PUSH, NULL, 1);
   if(temp->subtype == SLIDER)
     create_glyph(thumb, 0.07, 0.07, 0.2, 0.2, 0.2, 0, 0, 90, 0, gray, resources[RES_GRIPPY], 1);
   else
     create_glyph(thumb, 0.07, 0.07, 0.2, 0.2, 0.2, 0, 0, 0, 0, gray, resources[RES_GRIPPY], 1);

   change_slider_values(temp, min, max, window, value);

   thumb->mask = MASK_ALL & ~(MASK_MOUSEDOWN | MASK_MOUSEUP | MASK_MOUSEDRAG);
   thumb->event = thumb_event;

   make_bbox(temp);
   return 0;
}

int create_scroll(treenode *temp, int min, int max, int window, int value)
{
   GLfloat black[4] = {0.0, 0.0, 0.0, 1.0};
   GLfloat dar[4] = {0.95, 0, 0.0, 1.0};
   scrollbarprop *sbp;

// black[0] = temp->color[0]; // - 0.1;
// black[1] = temp->color[1]; // - 0.1;
// black[2] = temp->color[2]; // - 0.1;
// black[3] = temp->color[3];
   temp->autoresolve = TRUE;

   free(temp->prop);  // super-cheap
   temp->prop = malloc(sizeof(scrollbarprop));
   sbp = (scrollbarprop *)(temp->prop);
   sbp->change_handler = 0;

   switch(temp->subtype)
   {
      case SCROLL:
         sbp->upbutton = create_button(temp, temp->dimentions.x - 0.25, 0, 0, 0.25, 0.25, 0, 0, 0, 0, BUTTON, PUSH, NULL, 1);
         create_glyph(sbp->upbutton, 0.05, 0.2, 0.21, 0.15, 0.15, 0.0, 0.0, -90.0, 0, black, resources[RES_UPGLYPH], 1);
         make_bbox(sbp->upbutton);

         sbp->downbutton = create_button(temp, temp->dimentions.x - 0.5, 0, 0, 0.25, 0.25, 0, 0, 0, 0, BUTTON, PUSH, NULL, 1);
         create_glyph(sbp->downbutton, 0.2, 0.05, 0.21, 0.15, 0.15, 0.0, 0.0, 90.0, 0, black, resources[RES_UPGLYPH], 1);
         make_bbox(sbp->downbutton);

         sbp->slider = create_bar(temp, 0, 0, 0, temp->dimentions.x - 0.5, 0.25, 0, 0, 0, min, max, value, window, dar, BAR, SLIDER, 1);

         temp->mask &= ~MASK_CLICK;
         temp->mask &= ~MASK_CHANGE;
         temp->event = scrollbar_event;
         make_bbox(temp);

         break;

      case VSCROLL:
         sbp->downbutton = create_button(temp, 0, 0.25, 0, 0.25, 0.25, 0, 0, 0, 0, BUTTON, PUSH, NULL, 1);
         create_glyph(sbp->downbutton, 0.05, 0.05, 0.21, 0.15, 0.15, 0.0, 0.0, 0.0, 0, black, resources[RES_UPGLYPH], 1);
         make_bbox(sbp->downbutton);

         sbp->upbutton = create_button(temp, 0, 0.0, 0, 0.25, 0.25, 0, 0, 0, 0, BUTTON, PUSH, NULL, 1);
         create_glyph(sbp->upbutton, 0.2, 0.2, 0.21, 0.15, 0.15, 0.0, 0.0, 180.0, 0, black, resources[RES_UPGLYPH], 1);
         make_bbox(sbp->upbutton);

         sbp->slider = create_bar(temp, 0, 0.5, 0, 0.25, temp->dimentions.y - 0.5, 0, 0, 0, min, max, value, window, dar, BAR, VSLIDER, 1);

         temp->mask &= ~MASK_CLICK;
         temp->mask &= ~MASK_CHANGE;
         temp->event = scrollbar_event;
         make_bbox(temp);
       
         break;
   }
   return(0);
}
