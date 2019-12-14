#include <stdint.h>
#include "../SSD2119.h"
#include "../Lab.h"

#define SWITCH_PED PF0 // Pedestrian Switch
#define SWITCH_POW PF4 // Power Switch

// Color variables
unsigned short BLACK; // Black
unsigned short WHITE; // White
unsigned short GREEN; // Green
unsigned short DARK_GREEN; // Dark Green
unsigned short RED; // Red
unsigned short DARK_RED; // Dark Red
unsigned short YELLOW; // Yellow
unsigned short DARK_YELLOW; // Dark Yellow

// Declarations
void PORTF_Init(void); // Initialize Port F
void TIMER0_Init(void); // Initialize Timer 0
void TIMER0_start(); // Start Timer 0
void TIMER0_stop(); // Stop Timer 0
void TIMER0_reload(unsigned int val); // Reload Timer 0
void TIMER1_Init(void); // Initialize Timer 1
void TIMER1_start(); // Start Timer 1
void TIMER1_stop(); // Stop Timer 1
void TIMER1_reload(unsigned int val); // Reload Timer 1
void Stoplight_Init(); // Initialize Stoplight
void Stoplight_Draw(); // Draw Stoplight

// Current state
enum state current;
volatile int isOn;
enum bool POW_pressed; // Power button pressed
enum bool PED_pressed; // Pedestrian button pressed

// Enums for simplicity
enum state { // Traffic light states
  GO,
  WARN,
  STOP
};
enum bool { // Boolean
  false,
  true
};

// Main function (Task 3)
int main () {
  // Initializations
  PORTF_Init(); // Initialize Port F
  TIMER0_Init(); // Initialize Timer 0
  TIMER1_Init(); // Initialize Timer 1
  LCD_Init(); // Initialize LCD screen
  Stoplight_Init(); // Initialize Stoplight
  
  current = GO; // Set current state to go
  isOn = false; // System is not on
  POW_pressed = false; // Power is not pressed
  PED_pressed = false; // Pedestrian is not pressed
  // load timer with initial value of 16,000,000 and start the timer
  // gives a 1 sec delay with 16Mhz clock
  TIMER0_reload(16000000 * 5);
  TIMER1_stop(); // Stop Timer 1
  while(1) {
    // Check switch states and restart timer 1 if necessary
    if (~GPIO_PORTF_DATA_R & SWITCH_POW) {
      if (POW_pressed == false) { // Power is not pressed
        POW_pressed = true; // Power is pressed
        TIMER1_reload(16000000 * 2); // Reload Timer 1
      }
    } else if (~GPIO_PORTF_DATA_R & SWITCH_PED) { // Pedestrian switch pressed
      if (PED_pressed == false && current == GO && isOn) { // System is active
        PED_pressed = true; // Pedestrian switch is pressed
        TIMER0_reload(16000000 * 5); // also restart stoplight timer for more time to press
        TIMER1_reload(16000000 * 2); // Reload Timer 1
      }
    } else {
      PED_pressed = false; // Pedestrian not pressed
      POW_pressed = false; // Power not pressed
    }
    if (TIMER1_RIS_R & 0x1 == 0x1) { // check timer 1 flag
      TIMER1_stop(); // Stop Timer 1
      // Power button turns on or off the device
      if (POW_pressed) { // Power is pressed
        if (isOn) { // System is on
          TIMER0_stop(); // Stop Timer 0
          isOn = false; // System off
          Stoplight_Draw(); // Redraw Stoplight
        } else {
          TIMER0_stop(); // Stop Timer 0
          isOn = true; // System on
          current = GO; // Set state to go
          Stoplight_Draw(); // Redraw Stoplight
          TIMER0_reload(16000000 * 5); // Reload Timer 0
        }
      // Pedestrian button moves state to WARN if conditions met
      } else if (PED_pressed && current == GO && isOn) {
        TIMER0_stop(); // Stop Timer 0
        current = WARN; // Set state to warn
        Stoplight_Draw(); // Draw Stoplight
        TIMER0_reload(16000000 * 5); // Reload Timer 0
      }
    }
  }
}

// ************** TIMER0_Handler ***************************
// - Interrupt handler for the timer 0
//   - handles the stoplight color rotation logic
// *********************************************************
// Input: none
// Output: none
// *********************************************************
void TIMER0_Handler () {
  if (isOn) { // System is on
    // Adjust state appropriately
    if (current == GO) { // State is go
      current = STOP; // Set state to stop
    } else if (current == WARN) { // State is warn
      current = STOP; // Set state to stop
    } else if (current == STOP) { // State is stop
      current = GO; // Set state to go
    }
    Stoplight_Draw(); // Draw Stoplight
  }
  TIMER0_start(); // Start Timer 0
}

/* Initializes the necessary registers to enable the onboard switches
 */
void PORTF_Init(void) {
  unsigned short wait = 0;                          // Waiting variable
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;             // enable Port F GPIO
  wait++;
  wait++;
  // initialize switches
  GPIO_PORTF_DIR_R &= ~(SWITCH_PED + SWITCH_POW);   // set direction of Port F pins to input
  GPIO_PORTF_DEN_R |= SWITCH_PED + SWITCH_POW;      // enable digital on all used switches
  GPIO_PORTF_LOCK_R = GPIO_LOCK_UNLOCK;             // unlocks the GPIO_CR register for writing
  GPIO_PORTF_CR_R |= SWITCH_PED + SWITCH_POW;       // enable commits for switch 1 and 2
  GPIO_PORTF_PUR_R |= SWITCH_PED + SWITCH_POW;      // enabled weak pull-up resistors for SW1, SW2
  GPIO_PORTF_ICR_R = SWITCH_PED + SWITCH_POW;       // clear any interrupts from these switches
  GPIO_PORTF_IM_R = SWITCH_PED + SWITCH_POW;        // unmask the interrupt from the controller
}

// ************** TIMTER0_Init *****************************
// - Initializes the Timer 0
// *********************************************************
// Input: none
// Output: none
// *********************************************************
void TIMER0_Init(void) {
  unsigned short wait = 0;
  SYSCTL_RCGCTIMER_R |= (1 << 0);      // enable 16/32 bit timer 0
  wait++;
  wait++;
  TIMER0_CTL_R  = 0x0;                 // disable timer 0's A and B timers
  TIMER0_CFG_R  = 0x0;                 // put timer 0 into concatenated mode
  TIMER0_CTL_R  |= (1 << 5);           // enable timer to trigger ADC
  TIMER0_TAMR_R |= (0x2 << 0);         // set timer to periodic mode    (TAMR = 0x2)
  TIMER0_TAMR_R &= ~(1 << 4);          // set timer to count down mode  (TCDIR = 0)
  TIMER0_IMR_R |= 0x1;                 // enable interrupt for the timer 0
  NVIC_EN0_R |= 0x80000;
}

/* Starts timer 0
 */
void TIMER0_start() {
  TIMER0_ICR_R |= 0x1;     // clear the timer flag
  TIMER0_CTL_R |= 0x1;     // enable the timer
}

/* Stops timer 0
 */
void TIMER0_stop() {
  TIMER0_CTL_R &= ~0x1;    // disable the timer
  TIMER0_ICR_R |= 0x1;     // clear the timer flag
}

/* Loads a value into timer 0 and restarts it
 *  @param - val is the initial value to load into the timer
 */
void TIMER0_reload(unsigned int val) {
  TIMER0_CTL_R &= ~(1 << 0);   // disable the timer
  TIMER0_TAILR_R = val;        // load new initial value
  TIMER0_ICR_R |= (1 << 0);    // clear the timer flag
  TIMER0_CTL_R |= (1 << 0);    // enable the timer
}

// ************** TIMTER1_Init *****************************
// - Initializes the Timer 1
// *********************************************************
// Input: none
// Output: none
// *********************************************************
void TIMER1_Init(void) {
  unsigned short wait = 0;
  SYSCTL_RCGCTIMER_R |= (1 << 1);      // enable 16/32 bit timer 1
  wait++;
  wait++;
  TIMER1_CTL_R  = 0x0;                 // disable timer 0's A and B timers
  TIMER1_CFG_R  = 0x0;                 // put timer 1 into concatenated mode
  TIMER1_CTL_R  |= (1 << 5);           // enable timer to trigger ADC
  TIMER1_TAMR_R |= (0x2 << 0);         // set timer to periodic mode    (TAMR = 0x2)
  TIMER1_TAMR_R &= ~(1 << 4);          // set timer to count down mode  (TCDIR = 0)
}

/* Starts timer 1
 */
void TIMER1_start() {
  TIMER1_ICR_R |= 0x1;     // clear the timer flag
  TIMER1_CTL_R |= 0x1;     // enable the timer
}

/* Stops timer 1
 */
void TIMER1_stop() {
  TIMER1_CTL_R &= ~0x1;    // disable the timer
  TIMER1_ICR_R |= 0x1;     // clear the timer flag
}

/* Loads a value into timer 1 and restarts it
 *  @param - val is the initial value to load into the timer
 */
void TIMER1_reload(unsigned int val) {
  TIMER1_CTL_R &= ~(1 << 0);   // disable the timer
  TIMER1_TAILR_R = val;        // load new initial value
  TIMER1_ICR_R |= (1 << 0);    // clear the timer flag
  TIMER1_CTL_R |= (1 << 0);    // enable the timer
}

// Initialize Stoplight
void Stoplight_Init() {
  // Color calculations
  BLACK = 0;
  WHITE = convertColor(0xFF, 0xFF, 0xFF);
  GREEN = convertColor(0x55, 0xFF, 0x55);
  DARK_GREEN = convertColor(0x33, 0x66, 0x33);
  RED = convertColor(0xFF, 0x33, 0x33);
  DARK_RED = convertColor(0x88, 0x33, 0x33);
  YELLOW = convertColor(0xFF, 0xFF, 0x33);
  DARK_YELLOW = convertColor(0x66, 0x66, 0x11);
  // Fill a black background
  LCD_ColorFill(BLACK);
  // Draw white circles for the stoplight frames
  LCD_DrawFilledCircle(55, 120, 45, WHITE); // Green light circle
  LCD_DrawFilledCircle(160, 120, 45, WHITE); // Yellow light circle
  LCD_DrawFilledCircle(265, 120, 45, WHITE); // Red light circle
  Stoplight_Draw(); // Draw stoplight
}

// Draw Stoplight
void Stoplight_Draw() {
  if (isOn) { // System is on
    switch(current) {
      case GO: // State go
        LCD_DrawFilledCircle(265, 120, 40, DARK_RED); // Red light off
        LCD_DrawFilledCircle(55, 120, 40, GREEN); // Green light on
        LCD_DrawFilledCircle(160, 120, 40, DARK_YELLOW); // Yellow light off
        break;
      case WARN: // State warn
        LCD_DrawFilledCircle(55, 120, 40, DARK_GREEN); // Green light off
        LCD_DrawFilledCircle(160, 120, 40, YELLOW); // Yellow light on
        LCD_DrawFilledCircle(265, 120, 40, DARK_RED); // Red light on
        break;
      case STOP: // State stop
        LCD_DrawFilledCircle(160, 120, 40, DARK_YELLOW); // Yellow light off
        LCD_DrawFilledCircle(55, 120, 40, DARK_GREEN); // Green light off
        LCD_DrawFilledCircle(265, 120, 40, RED); // Red light on
    }
  } else {
    LCD_DrawFilledCircle(55, 120, 40, DARK_GREEN); // Green light off
    LCD_DrawFilledCircle(160, 120, 40, DARK_YELLOW); // Yellow light off
    LCD_DrawFilledCircle(265, 120, 40, DARK_RED); // Red light off
  }
}