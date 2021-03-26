#pragma once
#include "contiki.h"
#include <stdio.h>

/*----------------------------Globals....................................*/
// 10000 ms dived into 100 time slots. When button is preseed, the time pressed in stored in its time slot
// tick for 10 sec = CLOCK_CONF_SECOND *10 = 1280
// 1 time slot = 1280ticks/100slots = 12,8
// [1-12.8, 12.8-25.6, ... 1267.2-1280] : size 100
// setting timer to 10 sec is 1280 ticks
static const int SIZE = 130;
static const int TICKS = CLOCK_CONF_SECOND *10;
static char password_storage[130];

/*----------------------------Proto types....................................*/
void add_press_within_timeslot(clock_time_t time_left);
void log_password();

void add_press_within_timeslot(clock_time_t time_remaining )
{
  clock_time_t current_time = TICKS - time_remaining;
  int recording_index = current_time/10;
  password_storage[recording_index] = 1;
}


void log_password()
{
  int counter = 0;
  int i = 0;
  while (i < SIZE)
  {
    if (counter <= 26)
    {
      counter++;
      printf("%d-", (int)password_storage[i]);
    }
    else
    {
      counter = 0;
      printf("\n");
    }
    i++;
  }
  printf("\n");
}
