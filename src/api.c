/* public api decoding functions */

#include <GL/gl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"
#include "api_defs.h"
#include "conn.h"
#include "window.h"
#include "wmanager.h"
#include "button.h"
#include "tree.h"
#include "synapse.h"
#include "bar.h"
#include "barmanager.h"
#include "font.h"
#include "text.h"
#include "deepwindow.h"
#include "glyph.h"
#include "reigon.h"
#include "resource.h"
#include "group.h"
#include "list.h"

int change_label(treenode *thing, char *label)
{
  switch(thing->type)
  {
    case TEXT:
      return change_text_label(thing, label);

    case FLATWINDOW:
      if(thing->subtype == REGULAR)
      {
        return change_button_label(((regwinprop *)(thing->prop))->titlebar, label);
      }
      return -1;

    default:
      return -1;
  }
}

char *get_label(treenode *thing)
{
  switch(thing->type)
  {
    case TEXT:
      return strdup(((text *)(thing->prop))->label);

//  case GLYPH:
//    if (!thing->child)
//       return 0;
//
//    return strdup(((text *)(thing->child->prop))->label);

    default:
      return 0;
  }
}

int reshape_object(treenode *thing, int which, GLfloat x, GLfloat y, GLfloat z,
                   GLfloat width, GLfloat height, GLfloat depth,
                   GLfloat rotx, GLfloat roty, GLfloat rotz)
{
  if(!thing)
    return -1;

  switch(thing->type)
  {
    case BAR:
      return resize_bar(thing, x, y, z, width, height, rotx, roty, rotz);

    case TEXT:
      return reshape_text(thing, which, x, y, z, width, height, rotx, roty, rotz);

    case REIGON:
    {
      point foo;
      int i;

      if(which & 1)
      {
        foo.x = x;
        foo.y = y;
        foo.z = z;
        i = move_reigon(thing, &foo);
        if(i)
          return i;
      }

      if(which & 2)
      {
        foo.x = width;
        foo.y = height;
        foo.z = depth;
        i = resize_reigon(thing, &foo);
        if(i)
          return i;
      }
      return 0;
    }

    case BUTTON:
      return reshape_button(thing, which, x, y, z, width, height, rotx, roty, rotz);

    default:
      return -1;
  }
}

int reshape_object_wrt(treenode *thing, treenode *wrt, int which, GLfloat x, GLfloat y, GLfloat z,
                       GLfloat width, GLfloat height, GLfloat depth,
                       GLfloat rotx, GLfloat roty, GLfloat rotz)
{
  if(!wrt)
    return -1;

  x += wrt->origin.x;
  y += wrt->origin.y;
  z += wrt->origin.z;

  rotx += wrt->rotation.x;
  roty += wrt->rotation.y;
  rotz += wrt->rotation.z;

  return reshape_object(thing, which, x, y, z, width, height, depth, rotx, roty, rotz);  // 'which'?  ok
}

// ok, here are the rules:
//
//  - ptr_message[0] is the length of the _return_ message, in bytes.  most of the
//    time, this will be 12 (3 integers)
//  - all return messages _must_ be at least 12 bytes long (one int for length, one for
//    type, and one for error code (probably 0)).
//  - yes, i promise to write a better send_message function  :)   [njm]

int do_api(connection *this, int size, int type, void *message)
{
   int er;
   int x;
   GLint *ip;
   GLfloat *fp;
   char *cp;
   GLfloat red[4] = {1.0, 0.0, 0.0, 1.0};
   GLfloat color[4] = {1.0, 1.0, 1.0, 1.0};
   unsigned int *ptr_message;

   ip = (int *)malloc(size);
   for (x = 0; x < (size / sizeof(int)); x++)
       ip[x] = ntohl(((int *)message)[x]);
   fp = (float *)ip;
   cp = (char *)ip;

   switch(type)
   {
      case API_SYN_GET_BUTTON_STATUS:
      {
         ptr_message = malloc(12);
         ptr_message[0] = 12;
         ptr_message[1] = API_SYN_GET_BUTTON_STATUS;
         ptr_message[2] = get_button_status((treenode *)(ip[0]));

         er = sendmessage(this, ptr_message, ptr_message[0]);
         free(ptr_message);
         return er;
         break;
      }

      case API_SYN_SET_BUTTON_STATUS:
      {
         ptr_message = malloc(12);
         ptr_message[0] = 12;
         ptr_message[1] = API_SYN_SET_BUTTON_STATUS;
         ptr_message[2] = set_button_status((treenode *)(ip[0]), ip[1]);

         er = sendmessage(this, ptr_message, ptr_message[0]);
         free(ptr_message);
         return er;
         break;
      }

      case API_SYN_CREATE_LIST:
      {
         ptr_message = malloc(12);
         ptr_message[0] = 12;
         ptr_message[1] = API_SYN_CREATE_LIST;
         printf("huh?\n");
         ptr_message[2] = (unsigned int) create_list((treenode *)ip[0],
                                        inttofloat(ip[1]),
                                        inttofloat(ip[2]),
                                        inttofloat(ip[3]),
                                        inttofloat(ip[4]),
                                        inttofloat(ip[5]),
                                       inttofloat(ip[7]),
                                       inttofloat(ip[8]),
                                       inttofloat(ip[9]),
                                       red,
                                      ip[10],
                                      ip[11]);

         er = sendmessage(this, ptr_message, ptr_message[0]);
         free(ptr_message);
         return er;
      }
      break;

    case API_SYN_GET_PROGBAR:
    {
      ptr_message = malloc(12);
      ptr_message[0] = 12;
      ptr_message[1] = API_SYN_GET_PROGBAR;
      ptr_message[2] = get_progbar((treenode *)(ip[0]));

      er = sendmessage(this, ptr_message, ptr_message[0]);
      free(ptr_message);
      return er;
      break;
    }

    case API_SYN_GET_SCROLLBAR_VALUE:
    {
      ptr_message = malloc(12);
      ptr_message[0] = 12;
      ptr_message[1] = API_SYN_GET_SCROLLBAR_VALUE;
      ptr_message[2] = get_scrollbar_value((treenode *)(ip[0]));

      er = sendmessage(this, ptr_message, ptr_message[0]);
      free(ptr_message);
      return er;
      break;
    }

    case API_SYN_CHANGE_SLIDER_VALUE:
    {
      ptr_message = malloc(12);
      ptr_message[0] = 12;
      ptr_message[1] = API_SYN_CHANGE_SLIDER_VALUE;
      ptr_message[2] = change_slider_value((treenode *)(ip[0]), ip[1]);

      er = sendmessage(this, ptr_message, ptr_message[0]);
      free(ptr_message);
      return er;
      break;
    }

    case API_SYN_GET_SLIDER_VALUE:
    {
      ptr_message = malloc(12);
      ptr_message[0] = 12;
      ptr_message[1] = API_SYN_GET_SLIDER_VALUE;
      ptr_message[2] = get_slider_value((treenode *)(ip[0]));

      er = sendmessage(this, ptr_message, ptr_message[0]);
      free(ptr_message);
      return er;
      break;
    }

    case API_SYN_SET_GLYPH_LOCK:
       ptr_message = malloc(12);
       ptr_message[0] = 12;
       ptr_message[1] = API_SYN_SET_GLYPH_LOCK;
       if(((treenode *)(ip[0]))->type == GLYPH)
       {
         set_glyph_lock((treenode *)(ip[0]), ip[1]);
         ptr_message[2] = 0;
       }
       else
         ptr_message[2] = -1;
       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
       break;


    case API_SYN_SET_VISIBILITY:
       ptr_message = malloc(12);
       ptr_message[0] = 12;
       ptr_message[1] = API_SYN_SET_VISIBILITY;
       ((treenode *)(ip[0]))->visible = ip[1];
       ptr_message[2] = 0;

       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
       break;

    case API_SYN_GET_LABEL:
       {
         char *lbl;

         lbl = get_label((treenode *)(ip[0]));
         if(lbl)
         {
           ptr_message = malloc(8 + ((strlen(lbl) + 4) & ~3));
           ptr_message[0] = 8 + ((strlen(lbl) + 4) & ~3);
           ptr_message[1] = API_SYN_GET_LABEL;
           strcpy((char *)(&ptr_message[2]), lbl);
           free(lbl);
         }
         else
         {
           ptr_message = malloc(12);
           ptr_message[0] = 12;
           ptr_message[1] = API_SYN_GET_LABEL;
           ptr_message[2] = 0;
         }
       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
         break;
       }

    case API_SYN_GET_LABEL_LEN:
       {
         char *lbl;

         ptr_message = malloc(12);
         lbl = get_label((treenode *)(ip[0]));
         ptr_message[0] = 12;
         ptr_message[1] = API_SYN_GET_LABEL_LEN;
         if(lbl)
         {
           ptr_message[2] = strlen(lbl);
           free(lbl);
         }
         else
           ptr_message[2] = -1;
       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
         break;
       }

    case API_SYN_DESTROY_OBJECT:
       {
         ptr_message = malloc(12);
         ptr_message[0] = 12;
         ptr_message[1] = API_SYN_DESTROY_OBJECT;
         ptr_message[2] = destroy_object((treenode *)(ip[0]));
       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
         break;
       }

    case API_SYN_RESHAPE_OBJECT_WRT:
      ptr_message = malloc(12);
      ptr_message[0] = 12;
      ptr_message[1] = API_SYN_RESHAPE_OBJECT_WRT;
      ptr_message[2] = reshape_object_wrt((treenode *)ip[0], (treenode *)ip[1], ip[2],
                        inttofloat(ip[3]),
                        inttofloat(ip[4]),
                        inttofloat(ip[5]),
                        inttofloat(ip[6]),
                        inttofloat(ip[7]),
                        inttofloat(ip[8]),
                        inttofloat(ip[9]),
                        inttofloat(ip[10]),
                        inttofloat(ip[11]));

       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
      break;

    case API_SYN_CREATE_GROUP:
      ptr_message = malloc(12);
      ptr_message[0] = 12;
      ptr_message[1] = API_SYN_CREATE_GROUP;
      ptr_message[2] = (unsigned int) create_group((treenode *)ip[0],
                                     inttofloat(ip[1]),
                                     inttofloat(ip[2]),
                                     inttofloat(ip[3]),
                                     inttofloat(ip[4]),
                                     inttofloat(ip[5]),
                                     inttofloat(ip[7]),
                                     inttofloat(ip[8]),
                                     inttofloat(ip[9]),
                                      (char *)(&ip[11]), ip[10]);

       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
      break;

    case 0x1000:  // new window
//    someday, every one of these should look like this:

//    return api_syn_create_flatwindow(this, size, type, message);  // keep type in...could be useful for
                                                                    // multipurpose message-answering functions
      ptr_message = malloc(12);
      ptr_message[0] = 12;
      ptr_message[1] = 0x1000;
      ptr_message[2] = (unsigned int) create_window((treenode *) this,
                        inttofloat(ip[1]),
                        inttofloat(ip[2]),
                        inttofloat(ip[3]),
                        inttofloat(ip[4]),
                        inttofloat(ip[5]),
                        inttofloat(ip[7]),
                        inttofloat(ip[8]),
                        inttofloat(ip[9]),
                        red, FLATWINDOW, ip[0], (char *)(&ip[11]), ip[10]);
       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
      break;

    case 0x1100:  // new deepwindow
      ptr_message = malloc(12);
      ptr_message[0] = 12;
      ptr_message[1] = 0x1100;
      ptr_message[2] = (unsigned int) create_deepwindow((treenode *) this,
                        inttofloat(ip[1]),
                        inttofloat(ip[2]),
                        inttofloat(ip[3]),
                        inttofloat(ip[4]),
                        inttofloat(ip[5]),
                        inttofloat(ip[6]),
                        inttofloat(ip[7]),
                        inttofloat(ip[8]),
			inttofloat(ip[9]),
                        red, DEEPWINDOW, ip[0], (char *)(&ip[11]));
       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
      break;

    case 0x3000:  // new button
      ptr_message = malloc(12);
      ptr_message[0] = 12;
      ptr_message[1] = 0x3000;
      ptr_message[2] = (unsigned int) create_button((treenode *) ip[0],
                        inttofloat(ip[2]),
                        inttofloat(ip[3]),
                        inttofloat(ip[4]),
                        inttofloat(ip[5]),
                        inttofloat(ip[6]),
                        inttofloat(ip[8]),
                        inttofloat(ip[9]),
                        inttofloat(ip[10]),
                        0, BUTTON, ip[1], (char *)(&ip[12]), ip[11]);

       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
      break;

    case 0x3010:  // reshape object
      ptr_message = malloc(12);
      ptr_message[0] = 12;
      ptr_message[1] = 0x3010;
      ptr_message[2] = reshape_object((treenode *)ip[0], ip[1], 
                        inttofloat(ip[2]),
                        inttofloat(ip[3]),
                        inttofloat(ip[4]),
                        inttofloat(ip[5]),
                        inttofloat(ip[6]),
                        inttofloat(ip[7]),
                        inttofloat(ip[8]),
                        inttofloat(ip[9]),
                        inttofloat(ip[10]));

       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
      break;

    case 0x3100:  // new bar
      ptr_message = malloc(12);
      ptr_message[0] = 12;
      ptr_message[1] = 0x3100;
      ptr_message[2] = (unsigned int) create_bar((treenode *) ip[0],
                        inttofloat(ip[2]),
                        inttofloat(ip[3]),
                        inttofloat(ip[4]),
                        inttofloat(ip[5]),
                        inttofloat(ip[6]),
                        inttofloat(ip[8]),
                        inttofloat(ip[9]),
                        inttofloat(ip[10]),
                        ip[12], // min
                        ip[13], // max
                        ip[14], // value
                        ip[15], // window
                        0, BAR, ip[1], ip[11]);

       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
      break;

    case 0x3101: // change progress bar
    {
      ptr_message = malloc(12);
       ptr_message[0] = 12;
       ptr_message[1] = 0x3101;
       ptr_message[2] = change_progress_bar((treenode *) ip[0], ip[1]);

       er = sendmessage(this, ptr_message, sizeof(int) * 3);
       free(ptr_message);
       return er;
       break;
    }

    case 0x3102: // reload_fonts
    {
      ptr_message = malloc(12);
       ptr_message[0] = 12;
       ptr_message[1] = 0x3102;
       ptr_message[2] = reload_fonts((treenode *) ip[0]);
 
       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
       break;
    }

    case 0x3103:  // new text
    {
      ptr_message = malloc(12);
       ptr_message[0] = 12;
       ptr_message[1] = 0x3103;
       ptr_message[2] = (unsigned int) create_text((treenode *) ip[0],
                                                     inttofloat(ip[1]),
                                                     inttofloat(ip[2]),
                                                     inttofloat(ip[3]), 
                                                     inttofloat(ip[4]),
                                                     inttofloat(ip[5]),
                                                     inttofloat(ip[6]),
                                                     inttofloat(ip[7]),
                                                     inttofloat(ip[8]),
                                                     inttofloat(ip[9]),
                                                     inttofloat(ip[10]), 
                                                     ip[11], find_font_by_name((char *)(&ip[12])), (char *)(&ip[12 + ((strlen((char *)(&ip[12])) + 4) & ~3) / sizeof(int)]));
       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
       break;
    }

    case 0x3104: // set active control
    {
      ptr_message = malloc(12);
       ptr_message[0] = 12;
       ptr_message[1] = 0x3104;
       ptr_message[2] = set_active((treenode *) ip[0]);

       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
       break;
    }
  
    case 0x3105: // change object's color
    {
       color[0] = inttofloat(ip[1]),
       color[1] = inttofloat(ip[2]),
       color[2] = inttofloat(ip[3]),
       color[3] = inttofloat(ip[3]);
 
      ptr_message = malloc(12);
       ptr_message[0] = 12;
       ptr_message[1] = 0x3105;
       ptr_message[2] = color_object((treenode *) ip[0], color);
       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
       break;
    }

    case 0x3106:  // new glyph
    {
      ptr_message = malloc(12);
       ptr_message[0] = 12;
       ptr_message[1] = 0x3106;

       color[0] = inttofloat(ip[12]);
       color[1] = inttofloat(ip[13]);
       color[2] = inttofloat(ip[14]);
       color[3] = inttofloat(ip[15]);

       if (ip[0] == -1)
       {
          resprop *foo;
          GLint res;

          foo = find_resource((char *)(&ip[12]));
          if(foo)
          {
            res = foo->gl_id;
            ptr_message[2] = (unsigned int) create_glyph((treenode *) this,
                                                         inttofloat(ip[2]),
                                                         inttofloat(ip[3]),
                                                         inttofloat(ip[4]), 
                                                         inttofloat(ip[5]),
                                                         inttofloat(ip[6]),
                                                         inttofloat(ip[8]),
                                                         inttofloat(ip[9]),
                                                         inttofloat(ip[10]),
                                                   0, color, res, ip[11]);
          }
          else
            ptr_message[2] = 0;
       }

       else
       {
          resprop *foo;
          GLint res;

          foo = find_resource((char *)(&ip[12]));
          if(foo)
          {
            res = foo->gl_id;
            ptr_message[2] = (unsigned int) create_glyph((treenode *) ip[0],
                                                         inttofloat(ip[2]),
                                                         inttofloat(ip[3]),
                                                         inttofloat(ip[4]), 
                                                         inttofloat(ip[5]),
                                                         inttofloat(ip[6]),
                                                         inttofloat(ip[8]),
                                                         inttofloat(ip[9]),
                                                         inttofloat(ip[10]),
                                                   0, color, res, ip[11]);
          }
          else
            ptr_message[2] = 0;
       }

       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
    }
       break;

    case 0x3109: // change starting color
    {
       color[0] = inttofloat(ip[1]),
       color[1] = inttofloat(ip[2]),
       color[2] = inttofloat(ip[3]),
       color[3] = inttofloat(ip[3]);
 
      ptr_message = malloc(12);
       ptr_message[0] = 12;
       ptr_message[1] = 0x3109;
//     ptr_message[2] = color_application_start(color);
       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
       break;
    }

    case 0x3108: // change label
    {
      ptr_message = malloc(12);
       ptr_message[0] = 12;
       ptr_message[1] = 0x3108;
       ptr_message[2] = change_label((treenode *)ip[0], (char *)(&ip[1]));
       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
       break;
    }

    case 0x3107: // change scroll bar value
    {
       treenode *scroller;
      ptr_message = malloc(12);
       ptr_message[0] = 12;
       ptr_message[1] = 0x3107;
       scroller = (treenode *)ip[0];
       scroller = ((scrollbarprop *)(scroller->prop))->slider;
       change_slider_values(scroller, ((bar *)(scroller->prop))->min, ((bar *)(scroller->prop))->max, ((bar *)(scroller->prop))->window, ip[1]);
       ptr_message[2] = 0;
       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
       break;
    }

    case API_SYN_CREATE_REIGON:
    {
      ptr_message = malloc(12);
       ptr_message[0] = 12;
       ptr_message[1] = API_SYN_CREATE_REIGON;
       ptr_message[2] = (unsigned int) create_reigon((treenode *) ip[0],
                                                       inttofloat(ip[1]),
                                                       inttofloat(ip[2]),
                                                       inttofloat(ip[3]),
                                                       inttofloat(ip[4]),
                                                       inttofloat(ip[5]),
                                                       inttofloat(ip[7]),
                                                       inttofloat(ip[8]),
                                                       inttofloat(ip[9]),
                                                       ip[10]);
       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
    }
       break;

    case 0x3111: // get object type
    {
      ptr_message = malloc(12);
       ptr_message[0] = 12;
       ptr_message[1] = 0x3111;
       ptr_message[2] = ((treenode *) ip[0])->type;
       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
       break;
    }

    case 0x3112: // add to text label
      ptr_message = malloc(12);
       ptr_message[0] = 12;
       ptr_message[1] = 0x3112;
       ptr_message[2] = labelcat((treenode *)ip[0], ip[1], (char *)(&ip[2]));
       er = sendmessage(this, ptr_message, ptr_message[0]);
       free(ptr_message);
       return er;
       break;

    default:
    {
       break;
    }
  }
  return 0;
}
