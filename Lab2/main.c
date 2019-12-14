#include <custom_header.h>
#include <stdint.h>

//User LED controls
// Turn on red LED
void redLEDOn() {
  GPIO_DATA_PORTF = RED;
}
// Turn on yellow LED
void yellowLEDOn() {
  GPIO_DATA_PORTF = YELLOW;
}
// Turn on green LED
void greenLEDOn() {
  GPIO_DATA_PORTF = GREEN;
}
// Turn on teal LED
void tealLEDOn() {
  GPIO_DATA_PORTF = TEAL;
}
// Turn on blue LED
void blueLEDOn() {
  GPIO_DATA_PORTF = BLUE;
}// Turn on purple LED
void purpleLEDOn() {
  GPIO_DATA_PORTF = PURPLE;
}
// Turn on white LED
void whiteLEDOn() {
  GPIO_DATA_PORTF = WHITE;
}
// Turn off blue LED
void blueLEDOff() {
  GPIO_DATA_PORTF &= ~BLUE;
}
// Turn off red LED
void redLEDOff() {
  GPIO_DATA_PORTF &= ~RED;
}

//Breadboard LED controls
// turn on green LED
void GREEN_LED_on(void) {
  GPIO_DATA_PORTA |= PA5; // set PA5 bit to 1 (on)
}
// turn on yellow LED
void YELLOW_LED_on(void) {
  GPIO_DATA_PORTA |= PA6; // set PA6 bit to 1 (on)
}
// turn on red LED
void RED_LED_on(void) {
  GPIO_DATA_PORTA |= PA7; // set PA7 bit to 1 (on)
}
// turn off green LED
void GREEN_LED_off(void) {
  GPIO_DATA_PORTA &= ~PA5;  // set PA5 bit to 0 (off)
}
// turn off yellow LED
void YELLOW_LED_off(void) {
  GPIO_DATA_PORTA &= ~PA6;  // set PA6 bit to 0 (off)
}
// turn off red LED
void RED_LED_off(void) {
  GPIO_DATA_PORTA &= ~PA7;  // set PA7 bit to 0 (off)
}

//Initializations
// Initialize GPIO Port F
void GPIOPortFInit() {
  RCGC2_REGISTER = RCGC2_PORTF; // enable Port F GPIO
  GPIO_DEN_PORTF = PF0_TO_PF4_DEN; // enable digital pin PF1- PF3
  GPIO_DIR_PORTF = RGB; // set PF1 - PF3 as output
}
// Initialize GP Timer
void GPTMInit() {
  RCGCTIMER |= GPTM0_16_32; // Enable 16/32 Timer 0
  GPTMCTL = GPTMA_DISABLE; // Disable Timer A
  GPTMCFG = TM_MODE_32; // Select 32-bit mode
  GPTMTAMR |= TAMR_PER_TM_MODE; // Set periodic timer mode
  GPTMTAMR &= ~TACDIR_COUNT_UP; // Configure TACDIR to count down
  GPTMTAILR = N16_MIL; // Load value of 16 million into GPTMTAILR
  GPTMICR |= RESET_TA; // Reset timer
  GPIOICR_PORTF |= RESET_TA; // Reset timer Port F
  GPTMCTL |= GPTMA_ENABLE; // Enable Timer A
}
// Initialize interrupts
void interruptInit() {
  GPTMIMR = TATOIM_ENABLE; // Enable Timer A Time-Out Interrupt
  GPTMIMR_PORTF = 0x11; // Enable Timer A Time-Out Interrupt on Port F
  INTERRUPT_ENABLE = INT_19_AND_30_ENABLE; // Enable interrupts 19 and 30
  GPIOIS_PORTF &= ~PF0_AND_PF4; // Detect edge on PF0 and PF4 pins
  GPIOIBE_PORTF = IEV_CTL_PF0_AND_PF4; // Ports PF0 and PF4 detect both edges
  GPIOIEV_PORTF |= RISEDGE_PF0_AND_PF4; // Rising edge triggers interrupt
}
// Initialize user switches
void userSwitchInit() {
  GPIO_LOCK_PORTF = GPIO_UNLOCK_KEY; // unlock port F to allow writing to register
  GPIOCR_PORTF = PF0_AND_PF4; // configure register to write to GPIOPUR
  GPIOPUR_PORTF = PF0_AND_PF4; // enable pads for PF0 and PF4
}
// initialize LEDs
void LED_init(void) {
  volatile unsigned long delay;
  RCGC2_REGISTER |= 0x01; // activate clock for Port A
  delay = RCGC2_REGISTER; // allow time for clock to start
  GPIO_PCTL_PORTA &= ~0x00000F00; // regular GPIO
  GPIO_AMSEL_PORTA &= ~PA5_TO_PA7; // disable analog function of PA5-PA7
  GPIO_DIR_PORTA |= PA5_TO_PA7; // set PA5-PA7 to output
  GPIO_AFSEL_PORTA &= ~PA5_TO_PA7; // regular port function
  GPIO_DEN_PORTA |= PA5_TO_PA7; // enable digital output on PA5-PA7
}
// initialize switches
void switch_init(void) {
  volatile unsigned long delay;
  RCGC2_REGISTER |= 0x00000001; // activate clock for Port A
  delay = RCGC2_REGISTER; // allow time for clock to start
  GPIO_AMSEL_PORTA &= ~PA2_AND_PA3; // disable analog on PA2, PA3
  GPIO_PCTL_PORTA &= ~0x0000FF00; // PCTL GPIO on PA2, PA3
  GPIO_DIR_PORTA &= ~PA2_AND_PA3; // set PA2, PA3 to input
  GPIO_AFSEL_PORTA &= ~PA2_AND_PA3; // PA2, PA3 regular port function
  GPIO_DEN_PORTA |= PA2_AND_PA3; // enable PA2, PA3 as digital port
}

// return whether the power switch is pressed or not
unsigned long STARTSTOP_switch_input(void) {
  return (GPIO_DATA_PORTA & PA2); // 0x4 (pressed) or 0 (not pressed)
}
// return whether the pedestrian switch is pressed or not
unsigned long PED_switch_input(void) {
  return (GPIO_DATA_PORTA & PA3); // 0x8 (pressed) or 0 (not pressed)
}

// Reset timer when 1 sec cycle finishes
int timerCycled() {
  if (GPTMRIS & 0x1 == 0x1) {
    GPTMICR |= RESET_TA; // reset timer
    return 1; // cycle has finished
  } else {
    return 0; // cycle has not finished
  }
}
// 5 second delay
void fiveSeconds() {
  for (int i = 0; i < 5; i ++) {
    while (!timerCycled()) {}
  }
}

// Cycle through 7 LED colors in an infinite loop
void lightCycle() {
  GPIOPortFInit(); // initialize Port F
  GPTMInit(); // initialize GP timer
  while(1)
  {
    whiteLEDOn(); // turn on white LED
    while (!timerCycled()) {} // cycle clock
    redLEDOn(); // turn on red LED
    while (!timerCycled()) {} // cycle clock
    yellowLEDOn(); // turn on yellow LED
    while (!timerCycled()) {} // cycle clock
    greenLEDOn(); // turn on green LED
    while (!timerCycled()) {} // cycle clock
    tealLEDOn(); // turn on teal LED
    while (!timerCycled()) {} // cycle clock
    blueLEDOn(); // turn on blue LED
    while (!timerCycled()) {} // cycle clock
    purpleLEDOn(); // turn on purple LED
    while (!timerCycled()) {} // cycle clock
  }
}

// initialize enum for different states in cycle
enum colors{white, red, yellow, green, teal, blue, purple}color;
// Cycle through 7 LED colors
void Timer_Handler(void) {
  switch (color)
    {
    case white:
      whiteLEDOn(); // Turn on white LED
      color = red; // Change state to red
      break;
    case red:
      redLEDOn(); // Turn on red LED
      color = yellow; // Change state to yellow
      break;
    case yellow:
      yellowLEDOn(); // Turn on yellow LED
      color = green; // Change state to green
      break;
    case green:
      greenLEDOn(); // Turn on green LED
      color = teal; // Change state to teal
      break;
    case teal:
      tealLEDOn(); // Turn on teal LED
      color = blue; // Change state to blue
      break;
    case blue:
      blueLEDOn(); // Turn on blue LED
      color = purple; // Change state to purple
      break;
    case purple:
      purpleLEDOn(); // Turn on purple LED
      color = white; // Change state to white
      break;
    }
    GPTMICR |= RESET_TA; // reset timer
}
// Cycle through 7 LED colors using interrupt
void lightCycleInterrupt() {
  GPIOPortFInit(); // Initialize Port F
  GPTMInit(); // Initialize GP timer
  interruptInit(); // Initialize interrupt
  while(1) { }
}

// initialize enum for different states in FSM
enum states{go, warn, stop, off}state;
// Run traffic light FSM using timers
void timerTrafficLight() {
  LED_init(); // initialize LEDs
  switch_init(); // initialize switches
  state = off; // set initial state to be off
  int counter = 0; // Counts seconds power button is held
  int counterPed = 0; // Counts seconds pedestrian button is held
  while(1)
  {
    switch (state)
    {
    case go: // oscilate between red and green lights
      GREEN_LED_off(); // Turn off green LED
      RED_LED_on(); // Turn on red LED
      for (int i = 0; i < 5; i ++) {
        if (STARTSTOP_switch_input() != 0) { // Power button pressed
          counter ++; // Cycle counter
        } else {
          counter = 0; // Reset counter
        }
        while (!timerCycled()) {} // cycle clock
        if (counter == 2) {
          state = off; // Change state to off
          counter = 0; // Reset counter
          break;
        }
      }
      RED_LED_off(); // Turn off red LED
      GREEN_LED_on(); // Turn on green LED
      for (int i = 0; i < 5; i ++) {
        if (STARTSTOP_switch_input() != 0) { // Power button pressed
          counter ++; // Cycle counter
        } else if (PED_switch_input() != 0) { // Pedestrian button pressed
          counterPed ++; // Cycle counter
        } else {
          counter = 0; // Reset counter
          counterPed = 0; // Reset counter
        }
        while (!timerCycled()) {} // cycle clock
        if (counter == 2) {
          state = off; // Change state to off
          counter = 0; // Reset counter
          break;
        } else if (counterPed == 2) {
          state = warn; // Change state to warn
          counterPed = 0; // Reset counter
          break;
        }
      }
      break;
    case warn: // set light to yellow, delay, then set state to stop
      RED_LED_off(); // Turn off red LED
      GREEN_LED_off(); // Turn off green LED
      YELLOW_LED_on(); // Turn on yellow LED
      for (int i = 0; i < 5; i ++) {
        if (STARTSTOP_switch_input() != 0) { // Power button pressed
          counter ++; // Cycle counter
        } else if (PED_switch_input() != 0) { // Pedestrian button pressed
          counterPed ++; // Cycle counter
        } else {
          counter = 0; // Reset counter
        }
        while (!timerCycled()) {} // cycle clock
        if (counter == 2) {
          state = off; // Change state to off
          counter = 0; // Reset counter
          break;
        }
      }
      state = stop; // Change state to stop
      break;
    case stop: // set light to red, delay, then set state to go
      YELLOW_LED_off(); // Turn off yellow LED
      RED_LED_on(); // Turn on red LED
      fiveSeconds(); // cycle clock 5 times
      state = go; // Change state to go
      if (STARTSTOP_switch_input() != 0) { // Power button pressed
        counter ++; // Cycle counter
      } else {
        counter = 0; // Reset counter
      }
      while (!timerCycled()) {}
      if (counter == 2) {
        state = off; // Change state to off
        counter = 0; // Reset counter
        break;
      }
      break;
    case off: // turn off all LEDs
      GREEN_LED_off(); // Turn off green LED
      YELLOW_LED_off(); // Turn off yellow LED
      RED_LED_off(); // Turn off red LED
      if (STARTSTOP_switch_input() != 0) { // Power button pressed
        counter ++; // Cycle counter
      } else {
        counter = 0; // Reset counter
      }
      while (!timerCycled()) {} // cycle clock
      if (counter == 2) {
        state = go; // Change state to go
        counter = 0; // Reset counter
        break;
      }
      break;
    }
  }
}

// main function
int main()
{
  LED_init(); // initialize LEDs
  switch_init(); // initialize switches
  state = off; // set initial state to be off
  int counter = 0; // Counts seconds power button is held
  int counterPed = 0; // Counts seconds pedestrian button is held
  while(1)
  {
    switch (state)
    {
    case go: // oscilate between red and green lights
      GREEN_LED_off(); // Turn off green LED
      RED_LED_on(); // Turn on red LED
      for (int i = 0; i < 5; i ++) {
        if (STARTSTOP_switch_input() != 0) { // Power button pressed
          counter ++; // Cycle counter
        } else {
          counter = 0; // Reset counter
        }
        while (!timerCycled()) {} // cycle clock
        if (counter == 2) {
          state = off; // Change state to off
          counter = 0; // Reset counter
          break;
        }
      }
      RED_LED_off(); // Turn off red LED
      GREEN_LED_on(); // Turn on green LED
      for (int i = 0; i < 5; i ++) {
        if (STARTSTOP_switch_input() != 0) { // Power button pressed
          counter ++; // Cycle counter
        } else if (PED_switch_input() != 0) { // Pedestrian button pressed
          counterPed ++; // Cycle counter
        } else {
          counter = 0; // Reset counter
          counterPed = 0; // Reset counter
        }
        while (!timerCycled()) {} // cycle clock
        if (counter == 2) {
          state = off; // Change state to off
          counter = 0; // Reset counter
          break;
        } else if (counterPed == 2) {
          state = warn; // Change state to warn
          counterPed = 0; // Reset counter
          break;
        }
      }
      break;
    case warn: // set light to yellow, delay, then set state to stop
      RED_LED_off(); // Turn off red LED
      GREEN_LED_off(); // Turn off green LED
      YELLOW_LED_on(); // Turn on yellow LED
      for (int i = 0; i < 5; i ++) {
        if (STARTSTOP_switch_input() != 0) { // Power button pressed
          counter ++; // Cycle counter
        } else if (PED_switch_input() != 0) { // Pedestrian button pressed
          counterPed ++; // Cycle counter
        } else {
          counter = 0; // Reset counter
        }
        while (!timerCycled()) {} // cycle clock
        if (counter == 2) {
          state = off; // Change state to off
          counter = 0; // Reset counter
          break;
        }
      }
      state = stop; // Change state to stop
      break;
    case stop: // set light to red, delay, then set state to go
      YELLOW_LED_off(); // Turn off yellow LED
      RED_LED_on(); // Turn on red LED
      fiveSeconds(); // cycle clock 5 times
      state = go; // Change state to go
      if (STARTSTOP_switch_input() != 0) { // Power button pressed
        counter ++; // Cycle counter
      } else {
        counter = 0; // Reset counter
      }
      while (!timerCycled()) {}
      if (counter == 2) {
        state = off; // Change state to off
        counter = 0; // Reset counter
        break;
      }
      break;
    case off: // turn off all LEDs
      GREEN_LED_off(); // Turn off green LED
      YELLOW_LED_off(); // Turn off yellow LED
      RED_LED_off(); // Turn off red LED
      if (STARTSTOP_switch_input() != 0) { // Power button pressed
        counter ++; // Cycle counter
      } else {
        counter = 0; // Reset counter
      }
      while (!timerCycled()) {} // cycle clock
      if (counter == 2) {
        state = go; // Change state to go
        counter = 0; // Reset counter
        break;
      }
      break;
    }
  }
  //Uncomment a task to run it
  //lightCycle();
  //lightCycleInterrupt();
  //timerTrafficLight();
  return 0;
}