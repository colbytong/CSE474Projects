#include <custom_header.h>
#include <stdint.h>

//Initializations
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

// initialize enum for different states in FSM
enum states{go, warn, stop, off}state;
int count = 0;
// Run traffic light FSM with interrupts
void Timer_Handler() {
  int counter = 0; // Counts seconds power button is held
  int counterPed = 0; // Counts seconds pedestrian button is held
  switch (state)
    {
    case go: // Oscilate between red and green lights, 5 seconds each
      if (count < 5) {
          GREEN_LED_off(); // Turn off green LED
          RED_LED_on(); // Turn on red LED
          count ++;
          if (STARTSTOP_switch_input() != 0) { // Power button pressed
            counter ++;
          } else if (PED_switch_input() != 0) { // Pedestrian button pressed
            counterPed ++;
          } else {
            counter = 0;
          }
          if (counter == 2) {
            state = off; // Change state to off
            counter = 0;
            count = 0;
            break;
          }
      } else {
          RED_LED_off(); // Turn off red LED
          GREEN_LED_on(); // Turn on green LED
          if (STARTSTOP_switch_input() != 0) { // Power button pressed
            counter ++;
          } else if (PED_switch_input() != 0) { // Pedestrian button pressed
            counterPed ++;
          } else {
            counter = 0;
            counterPed = 0;
          }
          if (counter == 2) {
            state = off; // Change state to off
            counter = 0;
            counterPed = 0;
            count = 0;
            break;
          }
          if (count == 10) {
            count = -1;
          }
          count ++;
      }
      break;
    case warn: // set light to yellow, delay 5 sec, red, delay 5 sec, then go
      count ++;
      if (count < 5) {
        RED_LED_off(); // Turn off red LED
        GREEN_LED_off(); // Turn off green LED
        YELLOW_LED_on(); // Turn on yellow LED
      } else if (count == 5) {
        YELLOW_LED_off(); // Turn off yellow LED
        RED_LED_on(); // Turn on red LED
      } else if (count == 10) {
        count = -1;
        state = go; // Change state to go
        break;
      }
      break;
     case off: // turn off all LEDs
      GREEN_LED_off(); // Turn off green LED
      YELLOW_LED_off(); // Turn off yellow LED
      RED_LED_off(); // Turn off red LED
      if (STARTSTOP_switch_input() != 0) { // Power button pressed
        counter ++;
      } else {
        counter = 0;
      }
      if (counter == 2) {
        state = go; // Change state to go
        counter = 0;
        break;
      }
      break;
    }
  GPTMICR |= RESET_TA; // Reset timer
}

// Main function
int main() {
  LED_init(); // initialize LEDs
  switch_init(); // initialize switches
  GPTMInit(); // Initialize GP timer
  interruptInit(); // Initialize interrupt
  state = off; // Change state to off
  while(1) {}
  return 0;
}