#include "recorder.h"
#include "dev/button-sensor.h"
#include "leds.h"
#include "sys/log.h"

#define LOG_MODULE "Knock log"
#define LOG_LEVEL LOG_LEVEL_DBG

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
// Events
static process_event_t start_recording_event;
static process_event_t end_recording_event;
static process_event_t end_led_indicator_event;

// LED pins
static const leds_mask_t BLUE = 0x40;
static const leds_mask_t RED = 0x10;
static const leds_mask_t GREEN = 0x20;
  
/*----------------------------Processes--------------------------------------*/
PROCESS(main_process, "main_process");
PROCESS(record_password, "password recorder");
PROCESS(led_start_up_indicater, "Led start up indicator");
AUTOSTART_PROCESSES(&record_password, &main_process);
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

  // login process incomming
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

  while (counter <=5)
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
  bool is_recording = true;

  PROCESS_BEGIN();
  LOG_DBG("Init record_password process \n");

  PROCESS_WAIT_EVENT_UNTIL(ev == start_recording_event ); 
  LOG_DBG("Recording password has started \n");
  
  SENSORS_ACTIVATE(button_sensor);
  // Start timer 10 sec
  timer_set(&timer, CLOCK_SECOND*10);

  while (is_recording == true)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
    if(!timer_expired(&timer))
    {
      clock_time_t current_time = timer_remaining(&timer);
      printf("time left %u \n", (unsigned int)current_time);
      add_press_within_timeslot(current_time);
      LOG_DBG("Button press recorded \n");
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
  log_password();

    // alloc led indicator finnished
  end_recording_event = process_alloc_event();

  // send end_recording_event event to main
  process_post(&main_process, end_recording_event, NULL);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/


