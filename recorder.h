#pragma once
#include "contiki.h"
#include <stdio.h>

/*----------------------------Globals....................................*/
// 10000 ms dived into 100 time slots. When button is preseed, the time pressed in stored in its time slot
// tick for 10 sec = CLOCK_CONF_SECOND *10 = 1280
// 1 time slot = 1280ticks/100slots = 12,8
// [1-12.8, 12.8-25.6, ... 1267.2-1280] : size 100
// setting timer to 10 sec is 1280 ticks
#define SIZE 13
static const int TICKS = CLOCK_CONF_SECOND *10;
static char password_storage[SIZE];
static char login_password_storage[SIZE];

/*----------------------------Proto types....................................*/

/*
    Stores a secret button press within the correct timeslot

    $$Params$$
        clock_time_t : time left in ticks (1280 ticks = 10s)

    $$return$$
        void
*/
void record_secret_password_sequence(clock_time_t time_left);


/*
    Outputs the stored secret password

    $$return$$
        void
*/
void log_secret_password_sequence();


/*
    Stores a login button press within the correct timeslot

    $$Params$$
        clock_time_t : time left in ticks (1280 ticks = 10s)

    $$return$$
        void
*/
void record_login_password_sequence(clock_time_t time_left);


/*
    Outputs the stored login password

    $$return$$
        void
*/
void log_login_sequence();


/*
    clears in stored login cache

    $$return$$
        void
*/
void clear_login();


/*------------IMPLEMENTATION START--------------*/
/*------------IMPLEMENTATION START--------------*/
/*------------IMPLEMENTATION START--------------*/
void record_secret_password_sequence(clock_time_t time_remaining )
{
  clock_time_t current_time = TICKS - time_remaining;
  int ticks_per_sec = CLOCK_CONF_SECOND;
  int recording_index = current_time/ticks_per_sec;
  password_storage[recording_index] = 1;
}

void log_secret_password_sequence()
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

void record_login_password_sequence(clock_time_t time_remaining )
{
   clock_time_t current_time = TICKS - time_remaining;
  int ticks_per_sec = CLOCK_CONF_SECOND;
  int recording_index = current_time/ticks_per_sec;
  login_password_storage[recording_index] = 1;
}

void log_login_sequence()
{
  int counter = 0;
  int i = 0;
  while (i < SIZE)
  {
    if (counter <= 26)
    {
      counter++;
      printf("%d-", (int)login_password_storage[i]);
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

void clear_login()
{
  int i = 0;
  while(i<SIZE)
  {
    login_password_storage[i] = 0;
    i++;
  }
}