#include <TimerOne.h>                                 // Header file for TimerOne library

const int trigPin = 9;
const int echoPin = 10;
float duration, distance;
#define echo_int 0                                    // Interrupt id for echo pulse

#define TIMER_US 50                                   // 50 uS timer duration 
#define TICK_COUNTS 4000                              // 200 mS worth of timer ticks

volatile long echo_start = 0;                         // Records start of echo pulse 
volatile long echo_end = 0;                           // Records end of echo pulse
volatile long echo_duration = 0;                      // Duration - difference between end and start
volatile int trigger_time_count = 0;                  // Count down counter to trigger pulse time
volatile long range_flasher_counter = 0;              // Count down counter for flashing distance LED


// ----------------------------------
// setup() routine called first.
// A one time routine executed at power up or reset time.
// Used to initialise hardware.
// ----------------------------------
void setup() 
{
  pinMode(trigPin, OUTPUT);                           // Trigger pin set to output
  pinMode(echoPin, INPUT);                            // Echo pin set to input
 
  Timer1.initialize(TIMER_US);                        // Initialise timer 1
  Timer1.attachInterrupt( timerIsr );                 // Attach interrupt to the timer service routine 
  attachInterrupt(echo_int, echo_interrupt, CHANGE);  // Attach interrupt to the sensor echo input
  Serial.begin (9600);                                // Initialise the serial monitor output
}

// ----------------------------------
// loop() Runs continuously in a loop.
// This is the background routine where most of the processing usualy takes place.
// Non time critical tasks should be run from here.
// ----------------------------------
void loop()
{
  duration = pulseIn(echoPin, HIGH);
  distance = (duration*.0343)/2;
  Serial.print("Distance: ");
  Serial.println(distance);
    delay(100);                                       // every 100 mS
}

// --------------------------
// timerIsr() 50uS second interrupt ISR()
// Called every time the hardware timer 1 times out.
// --------------------------
void timerIsr()
{
    trigger_pulse();                                 // Schedule the trigger pulses
}

// --------------------------
// trigger_pulse() called every 50 uS to schedule trigger pulses.
// Generates a pulse one timer tick long.
// Minimum trigger pulse width for the HC-SR04 is 10 us. This system
// delivers a 50 uS pulse.
// --------------------------
void trigger_pulse()
{
      static volatile int state = 0;                 // State machine variable

      if (!(--trigger_time_count))                   // Count to 200mS
      {                                              // Time out - Initiate trigger pulse
         trigger_time_count = TICK_COUNTS;           // Reload
         state = 1;                                  // Changing to state 1 initiates a pulse
      }
    
      switch(state)                                  // State machine handles delivery of trigger pulse
      {
        case 0:                                      // Normal state does nothing
            break;
        
        case 1:                                      // Initiate pulse
           digitalWrite(trigPin, HIGH);              // Set the trigger output high
           state = 2;                                // and set state to 2
           break;
        
        case 2:                                      // Complete the pulse
        default:      
           digitalWrite(trigPin, LOW);               // Set the trigger output low
           state = 0;                                // and return state to normal 0
           break;
     }
}

// --------------------------
// echo_interrupt() External interrupt from HC-SR04 echo signal. 
// Called every time the echo signal changes state.
//
// Note: this routine does not handle the case where the timer
//       counter overflows which will result in the occassional error.
// --------------------------
void echo_interrupt()
{
  switch (digitalRead(echoPin))                     // Test to see if the signal is high or low
  {
    case HIGH:                                      // High so must be the start of the echo pulse
      echo_end = 0;                                 // Clear the end time
      echo_start = micros();                        // Save the start time
      break;
      
    case LOW:                                       // Low so must be the end of hte echo pulse
      echo_end = micros();                          // Save the end time
      echo_duration = echo_end - echo_start;        // Calculate the pulse duration
      break;
  }
}