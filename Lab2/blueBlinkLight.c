#include <custom_header.h>
#include <stdint.h>

//User LED controls
// Turn on red LED
void redLEDOn() {
  GPIO_DATA_PORTF = RED;
}
// Turn on blue LED
void blueLEDOn() {
  GPIO_DATA_PORTF = BLUE;
// Turn off blue LED
void blueLEDOff() {
  GPIO_DATA_PORTF &= ~BLUE;
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


int count = 0;
//Blink blue LED using interrupt
void Timer_Handler() {
  if (count == 0) {
      blueLEDOn(); // Turn on blue LED
      count ++;
  } else {
      blueLEDOff(); // Turn off blue LED
      count = 0;
  }
  GPIOICR_PORTF = 0xFF;
  GPTMICR |= RESET_TA; // Reset timer
}
//Enable/Disable Timer A and toggle red LED with user switches
void Switch_Handler(void) {
  int SW1 = GPIO_DATA_PORTF & SW1_MASK; // switch 1 status
  int SW2 = !(GPIO_DATA_PORTF & SW2_MASK); // switch 2 status
  if (SW2 == 0x01) { // switch PF0 is pressed
    GPTMCTL = GPTMA_DISABLE; // Disable Timer A
    redLEDOn(); // Turn red LED on
  } else if(SW1 == 0x01) { // switch PF4 is pressed
    GPTMCTL |= GPTMA_ENABLE; // Enable Timer A
    count = 0;
  }
  GPIOICR_PORTF = 0xFF;
  GPTMICR |= RESET_TA; // reset timer
}

// Main function
int main(){
  GPIOPortFInit(); // Initialize Port F
  GPTMInit(); // Initialize GP timer
  interruptInit(); // Initialize interrupt
  userSwitchInit(); // Initialize user switches
  while(1){}
  return 0;
}