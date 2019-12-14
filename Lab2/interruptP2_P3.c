#include <custom_header.h>
#include <stdint.h>

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
//BREADBOARD LEDS
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
  GPTMCTL |= GPTMA_ENABLE; // Enable Timer A
  GPIOICR_PORTF |= RESET_TA; // Reset timer Port F
}
// Initialize Interrupt
void interruptInit() {
  GPTMIMR = TATOIM_ENABLE; // Enable Timer A Time-Out Interrupt
  GPTMIMR_PORTF = 0x11; // Enable Timer A Time-Out Interrupt on Port F
  GPIOIS_PORTF &= ~PF0_AND_PF4; // Detect edge on PF0 and PF4 pins
  GPIOIBE_PORTF = IEV_CTL_PF0_AND_PF4; // Ports PF0 and PF4 detect both edges
  GPIOIEV_PORTF |= RISEDGE_PF0_AND_PF4; // Rising edge triggers interrupt
  INTERRUPT_ENABLE |= INT_19_ENABLE; // Enable interrupt 19
  INTERRUPT_ENABLE |= INT_30_ENABLE; // Enable interrupt 30
}
// Initialize user switches
void userSwitchInit() {
  GPIO_LOCK_PORTF = GPIO_UNLOCK_KEY; // unlock port F to allow writing to register
  GPIOCR_PORTF = PF0_AND_PF4; // configure register to write to GPIOPUR
  GPIOPUR_PORTF = PF0_AND_PF4; // enable pads for PF0 and PF4
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

// initialize enum for different states in FSM
enum states{go, warn, stop, off}state;

// initialize breadboard LEDs
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

void timerTrafficLight() {
  LED_init(); // initialize LEDs
  switch_init(); // initialize switches
  state = off; // set initial state to be off
  int counter = 0;
  int counterPed = 0;
  while(1)
  {
    switch (state)
    {
    case go: // oscilate between red and green lights
      GREEN_LED_off();
      RED_LED_on();
      for (int i = 0; i < 5; i ++) {
        if (STARTSTOP_switch_input() != 0) {
          counter ++;
        }
        else if (PED_switch_input() != 0) {
          counterPed ++;
        }
        else {
          counter = 0;
        }
        while (!timerCycled()) {}
        if (counter == 2) {
          state = off;
          counter = 0;
          break;
        }
      }
      RED_LED_off();
      GREEN_LED_on();
      for (int i = 0; i < 5; i ++) {
        if (STARTSTOP_switch_input() != 0) {
          counter ++;
        }
        else if (PED_switch_input() != 0) {
          counterPed ++;
        }
        else {
          counter = 0;
          counterPed = 0;
        }
        while (!timerCycled()) {}
        if (counter == 2) {
          state = off;
          counter = 0;
          break;
        }
        else if (counterPed == 2) {
          state = warn;
          counterPed = 0;
          break;
        }
      }
      break;
    case warn: // set light to yellow, delay, then set state to stop
      RED_LED_off();
      GREEN_LED_off();
      YELLOW_LED_on();
      for (int i = 0; i < 5; i ++) {
        if (STARTSTOP_switch_input() != 0) {
          counter ++;
        }
        else if (PED_switch_input() != 0) {
          counterPed ++;
        }
        else {
          counter = 0;
        }
        while (!timerCycled()) {}
        if (counter == 2) {
          state = off;
          counter = 0;
          break;
        }
      }
      state = stop;
      break;
    case stop: // set light to red, delay, then set state to go
      YELLOW_LED_off();
      RED_LED_on();
      fiveSeconds();
      state = go;
      if (STARTSTOP_switch_input() != 0) {
        counter ++;
      } else {
        counter = 0;
      }
      while (!timerCycled()) {}
      if (counter == 2) {
        state = off;
        counter = 0;
        break;
      }
      break;
    case off: // turn off all LEDs
      GREEN_LED_off();
      YELLOW_LED_off();
      RED_LED_off();
      if (STARTSTOP_switch_input() != 0) {
        counter ++;
      } else {
        counter = 0;
      }
      while (!timerCycled()) {}
      if (counter == 2) {
        state = go;
        counter = 0;
        break;
      }
      break;
    }
  }
}


// initialize enum for different states in cycle
enum colors{white, red, yellow, green, teal, blue, purple}color;

/*
void Timer_Handler(void) {
  switch (color)
    {
    case white:
      whiteLEDOn();
      color = red;
      break;
    case red:
      redLEDOn();
      color = yellow;
      break;
    case yellow:
      yellowLEDOn();
      color = green;
      break;
    case green:
      greenLEDOn();
      color = teal;
      break;
    case teal:
      tealLEDOn();
      color = blue;
      break;
    case blue:
      blueLEDOn();
      color = purple;
      break;
    case purple:
      purpleLEDOn();
      color = white;
      break;
    }
    GPTMICR |= RESET_TA; // reset timer
}
*/

// Cycle through 7 LED colors using interrupt
void lightCycleInterrupt() {
  GPIOPortFInit(); // Initialize Port F
  GPTMInit(); // Initialize GP timer
  interruptInit(); // Initialize interrupt
  while(1) { }
}

void Timer_Handler() {
  blueLEDOn();
  while (!timerCycled()) {}
  blueLEDOff();
  while (!timerCycled()) {}
  GPTMICR |= RESET_TA; // reset timer
}

void Switch_Handler(void) {
  int SW1 = GPIO_DATA_PORTF & SW1_MASK; // switch 1 status
  int SW2 = !(GPIO_DATA_PORTF & SW2_MASK); // switch 2 status
  if (SW2 == 0x01) { // switch PF0 is pressed
    GPTMCTL = 0x0; // Deactivate clock for Port A
    redLEDOn();
  }
  else if(SW1 == 0x01) { // switch PF4 is pressed
    GPTMCTL = 0x1; // Reactivate clock for Port A
  }
  GPIOICR_PORTF = 0x1;
}

void Timer_Handlers() {
  int litTime = 0;
  int counter = 0;
  int counterPed = 0;
  switch (state)
    {
    case go: // oscilate between red and green lights
      if (litTime == 5 && (GPIO_DATA_PORTA == GREEN)) {
        GREEN_LED_off();
        RED_LED_on();
        litTime = 0;
        if (STARTSTOP_switch_input() != 0) {
          counter ++;
        }
        else if (PED_switch_input() != 0) {
          counterPed ++;
        }
        else {
          counter = 0;
        }
        if (counter == 2) {
          state = off;
          counter = 0;
          break;
        }
      }
      else if (litTime == 5 && (GPIO_DATA_PORTA == RED)) {
        RED_LED_off();
        GREEN_LED_on();
        litTime = 0;
        if (STARTSTOP_switch_input() != 0) {
          counter ++;
        }
        else if (PED_switch_input() != 0) {
          counterPed ++;
        }
        else {
          counter = 0;
          counterPed = 0;
        }
        if (counter == 2) {
          state = off;
          counter = 0;
          break;
        }
        else if (counterPed == 2) {
          state = warn;
          litTime = 0;
          counterPed = 0;
          break;
        }
      }
      litTime ++;
      break;
    case warn: // set light to yellow, delay, then set state to stop
      RED_LED_off();
      GREEN_LED_off();
      YELLOW_LED_on();
      if (STARTSTOP_switch_input() != 0) {
        counter ++;
      }
      else if (PED_switch_input() != 0) {
        counterPed ++;
      }
      else {
        counter = 0;
      }
      if (counter == 2) {
        state = off;
        counter = 0;
        break;
      }
      if (litTime == 5) {
        state = stop;
        litTime = 0;
        break;
      }
      litTime ++;
      break;
    case stop: // set light to red, delay, then set state to go
      YELLOW_LED_off();
      RED_LED_on();
      fiveSeconds();
      state = go;
      if (STARTSTOP_switch_input() != 0) {
        counter ++;
      } else {
        counter = 0;
      }
      if (counter == 2) {
        state = off;
        counter = 0;
        break;
      }
      break;
    case off: // turn off all LEDs
      GREEN_LED_off();
      YELLOW_LED_off();
      RED_LED_off();
      if (STARTSTOP_switch_input() != 0) {
        counter ++;
      } else {
        counter = 0;
      }
      if (counter == 2) {
        state = go;
        litTime == 5;
        GPIO_DATA_PORTA = GREEN;
        counter = 0;
        break;
      }
      break;
    }
    GPTMICR |= RESET_TA; // reset timer
}

void interruptTrafficLight() {
  GPIOPortFInit(); // Initialize Port F
  GPTMInit(); // Initialize GP timer
  interruptInit(); // Initialize interrupt
  userSwitchInit(); // Initialize user switches
  while(1)
  {
  }
}

// main function
int main()
{
  LED_init(); // initialize LEDs
  switch_init(); // initialize switches
  state = off; // set initial state to be off
  interruptInit(); // Initialize interrupt
  while(1) {}
  return 0;
}
