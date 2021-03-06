#include "recorder.h"
#include "dev/button-sensor.h"
#include "leds.h"
#include "sys/log.h"
#include "dynamic_Time_Warping.h"

/*----------------------------Build commands---------------------------------*/
// Make and upload telos b
// make TARGET=sky MOTES=/dev/ttyUSB0 knockDetector.upload login
// Check serial port:
// - make TARGET=sky motelist
// compile and upload program
//-  make TARGET=sky PORT=/dev/ttyUSB0 knockDetector.upload
// ssh on the sensor
// - make TARGET=sky PORT=/dev/ttyUSB0 login
// Clean build
//- make TARGET=sky distclean
// local sky build
// - make TARGET=sky

/*----------------------------Global Vars------------------------------------*/
#define ACCEPT_THRESHOLD 6

// Events
static process_event_t start_recording_event;
static process_event_t end_recording_event;
static process_event_t end_led_indicator_event;
static process_event_t start_login_recorder_event;
static process_event_t end_login_recorder_event;

// LED pins
static const leds_mask_t BLUE = 0x40;
static const leds_mask_t RED = 0x10;
static const leds_mask_t GREEN = 0x20;

/*----------------------------Processes--------------------------------------*/
/*
    Holds the logic for communication between processes (Who starts when)
*/
PROCESS(main_process, "main_process");

/*
    Records the secret password
*/
PROCESS(record_password, "password recorder");

/*
    Start blinking leds, to indicate recording starup
*/
PROCESS(led_start_up_indicater, "Led start up indicator");

/*
    Records the secret password. Uses DTW to check whether the login password matches the secret password.
    If the password matches within an error of 6, the password is accepted and the program terminates
*/
PROCESS(login_procees, "login_procees");

/*
    Starts up proceses on boot. (Some will instantly go to sleep, and wait for start event from main_process)
*/
AUTOSTART_PROCESSES(&record_password, &main_process, &login_procees);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(main_process, ev, data)
{
  PROCESS_BEGIN();
  LOG_DBG("Init - main \n");

  // Startup led indicator // wait for finish event
  process_start(&led_start_up_indicater, NULL);
  PROCESS_WAIT_EVENT_UNTIL(ev == end_led_indicator_event);

  // start up password recording // wait for finnish
  start_recording_event = process_alloc_event();
  process_post(&record_password, start_recording_event, NULL);
  PROCESS_WAIT_EVENT_UNTIL(ev == end_recording_event);
  LOG_DBG("Recording password ended \n");

  // login process incomming
  start_login_recorder_event = process_alloc_event();
  process_post(&login_procees, start_login_recorder_event, NULL);
  PROCESS_WAIT_EVENT_UNTIL(ev == end_login_recorder_event);

  LOG_DBG("Main finish \n");

  PROCESS_END();
}

PROCESS_THREAD(led_start_up_indicater, ev, data)
{
  // Declare variables as static to ensure their value are kept between kernal calls;
  // None static vars, will be reset each time the kernel
  static struct etimer timer;

  // none static variables are recomputed at every run (at each event)
  // Must be static
  static uint counter = 0;

  PROCESS_BEGIN();
  LOG_DBG("Starting led bootup indicator process \n");
  // toggle blue led every 1 sec to indicate password recording (for 6 sec)

  while (counter <= 5)
  {
    if (counter % 2 == 0)
    {
      LOG_DBG("Blink! \n");
    }

    leds_toggle(BLUE);
    etimer_set(&timer, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);
    counter++;
  }
  leds_on(RED);
  leds_on(BLUE);
  leds_on(GREEN);
  counter = 0;

  // alloc led indicator finnished
  end_led_indicator_event = process_alloc_event();

  // send led_indicator_finished event to main
  process_post(&main_process, end_led_indicator_event, NULL);
  PROCESS_END();
}

PROCESS_THREAD(record_password, ev, data)
{
  // a simple timer, without built-in notification (caller must check if expired).
  static struct timer timer;
  static bool is_recording = true;

  PROCESS_BEGIN();
  LOG_DBG("Init record_password process \n");

  PROCESS_WAIT_EVENT_UNTIL(ev == start_recording_event);
  LOG_DBG("Recording password has started \n");

  SENSORS_ACTIVATE(button_sensor);
  // Start timer 10 sec
  timer_set(&timer, CLOCK_SECOND * 10);

  while (is_recording == true)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
    if (!timer_expired(&timer))
    {
      clock_time_t current_time = timer_remaining(&timer);
      printf("time left %u \n", (unsigned int)current_time);
      record_secret_password_sequence(current_time);
      LOG_DBG("Secret press : Button press recorded \n");
    }
    else
    {
      is_recording = false;
    }
  }

  leds_on(RED);
  leds_off(BLUE);
  leds_off(GREEN);
  LOG_DBG("Password was succesfully recorded \n");
  LOG_DBG("Current password is : \n");
  log_secret_password_sequence();

  // alloc led indicator finnished
  end_recording_event = process_alloc_event();

  // send end_recording_event event to main
  process_post(&main_process, end_recording_event, NULL);

  PROCESS_END();
}

PROCESS_THREAD(login_procees, ev, data)
{
  // a simple timer, without built-in notification (caller must check if expired).
  static struct timer timer;
  static bool is_recording = true;
  static bool is_correct = false;

  PROCESS_BEGIN();
  LOG_DBG("Init login recorder \n");

  PROCESS_WAIT_EVENT_UNTIL(ev == start_login_recorder_event);
  LOG_DBG("Press button to start login sequence \n");

  SENSORS_ACTIVATE(button_sensor);
  // press button to start login recording.
  PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
  leds_off(RED);
  leds_on(BLUE);
  // Start timer 10 sec

  while (is_correct == false)
  {
    timer_set(&timer, CLOCK_SECOND * 10);

    while (is_recording == true)
    {
      PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
      if (!timer_expired(&timer))
      {
        clock_time_t current_time = timer_remaining(&timer);
        printf("time left %u \n", (unsigned int)current_time);
        record_login_password_sequence(current_time);
        LOG_DBG("Login press: Button press recorded \n");
      }
      else
      {
        is_recording = false;
      }
    }

    LOG_DBG("Current login is : \n");
    log_login_sequence();

    int **cost_matrix = compute_accumulated_cost_matrix(password_storage, login_password_storage);

    print_matrix(cost_matrix);
    int min_cost = min_cost_warping_path(cost_matrix);
    printf("min cost : %d \n", min_cost);

    if(min_cost <=ACCEPT_THRESHOLD)
    {
      LOG_DBG("Password is correct! \n");
      leds_off(BLUE);
      leds_on(GREEN);
      is_correct = true;  
    }
    else
    {
      is_recording = true;
      clear_login();
      timer_reset(&timer);
    }
    free_matrix(cost_matrix);
  }
  // alloc led indicator finnished
  end_login_recorder_event = process_alloc_event();
  // send end_recording_event event to main
  process_post(&main_process, end_login_recorder_event, NULL);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
