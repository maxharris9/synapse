#include <synapse/wmanager.h>
#include <synapse/controls.h>
#include <stdio.h>
#include <unistd.h>
#include <synapse/objecttypes.h>

unsigned long id8;

void event(int id, int type, int modifier, Point *where, int usec)
{
   return;
}

void idle(void)
{
   static int direction = 0, i = 0;

   syn_change_progbar(id8, i);
   sleep(0.1);


   if (i > 99)
      direction = 1;

   if (i < 0)
      direction = 0;

   if (direction == 0)
      i++;
   else
      i--;
}

void init(int argc, char **argv)
{
   Point origin, size, rot;
   unsigned long id1, id2, id3, id4, id5, id6, id7;
   GLfloat colour[4] = {0.6, 0.4, 0.0, 0.0};

   origin.z = -10; origin.x = origin.y = -1;
   size.x = 4.25;  size.y = 3.6;
   rot.x = rot.y = rot.z = 0;
   syn_create_flatwindow(&id1, DIALOG, &origin, &size, &rot, 1, "Dialog window");

   origin.z = 0; origin.x = origin.y = 0.25;
   size.x = 2;  size.y = 2.0;
   syn_create_group(&id2, id1, 0, &origin, &size, &rot, 1, "Group label");

   origin.z = 0; origin.x = 2.5; origin.y = 0.25;
   size.x = 1.5;  size.y = 2;
   syn_create_group(&id3, id1, 0, &origin, &size, &rot, 1, "Group label 2");

   origin.x = 0.25; origin.y = 0.25; origin.z = 0.1;
   size.x = 0.5; size.y = 0.35;
   syn_create_button(&id4, id3, PUSH, &origin, &size, &rot, 1, "Login");

   origin.x = 0.25; origin.y = 0.9; origin.z = 0.1;
   size.x = 0.1; size.y = 0.1;
   syn_create_button(&id5, id3, CHECK, &origin, &size, &rot, 1, "Checkbox");

// origin.x = 0.25; origin.y = 1.5; origin.z = 0.1;
// size.x = 0.1; size.y = 0.1;
// syn_create_button(&id5, id3, RADIO, &origin, &size, &rot, 1, "Radio");

// origin.x = 0.25; origin.y = 1.2; origin.z = 0.1;
// size.x = 0.1; size.y = 0.1;
// syn_create_button(&id5, id3, RADIO, &origin, &size, &rot, 1, "Radio 2");

   origin.x = 2.75; origin.y = 2.85; origin.z = 0.0;
   size.x = 1.25; size.y = 0.25;
   syn_create_text(&id6, id1, &origin, &size, &rot, 10, BOX, "20thcent.ttf", "Single line text box");

   origin.x = 0.25; origin.y = 0.25; origin.z = 0.25;
   size.x = 1.25; size.y = 0.25;
   syn_create_text(&id5, id2, &origin, &size, &rot, 8, LABEL, "20thcent.ttf", "Single line text label");

   origin.x = 0.25; origin.y = 2.85; origin.z = 0.0;
   size.x = 2.25; size.y = 0.25;
   syn_create_bar(&id7, id1, SCROLL, &origin, &size, &rot, 0, 4, 2, 1, 1);

   origin.x = 0.25; origin.y = 2.5; origin.z = 0.0;
   size.x = 3.75; size.y = 0.125;
   syn_create_bar(&id8, id1, PROGRESS, &origin, &size, &rot, 0, 4, 2, 1, 1);
   syn_change_color(id8, colour);
}

int main(int argc, char **argv)
{
   syn_set_callback_init(init);
   syn_set_callback_idle(idle);
   syn_set_callback_event(event);
   syn_main(argc, argv);

   return 0;
}
