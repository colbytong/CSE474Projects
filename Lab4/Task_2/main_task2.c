#include <stdint.h>
#include "../SSD2119.h"
#include "../Lab.h"

// Declarations
void TIMER0_Init(void); // Initialize Timer 0
void TIMER0_start(); // Start Timer 0
void TIMER0_stop(); // Stop Timer 0
void TIMER0_reload(unsigned int val); // Reload Timer 0 with input value
void ADC0_Init(void); // Initialize ADC 0
unsigned short GREEN = ((0x00>>3)<<11) | ((0xAA>>2)<<5) | (0x00>>3); // LCD green
float temperature; // Internal temperature variable
char text[8]; // Output char array

/* Main function (Tasks 1 and 2) 
 */
int main ()
{
  // Initializations
  LCD_Init(); // Initialize LCD screen
  TIMER0_Init(); // Initialize Timer 0
  ADC0_Init(); // Initialize ADC 0
  // Fill screen with green (Task 1)
  LCD_ColorFill(GREEN);
  // load timer with initial value of 16,000,000 and start the timer
  // gives a 1 sec delay with 16Mhz clock
  TIMER0_reload(16000000);
  while(1) {}
}
// ************** TIMTER0_Init *****************************
// - Initializes the Timer to fire every second and trigger ADC readings
// *********************************************************
// Input: none
// Output: none
// *********************************************************
void TIMER0_Init(void)
{
  SYSCTL_RCGCTIMER_R |= (1 << 0);      // enable 16/32 bit timer 0
  TIMER0_CTL_R  = 0x0;                 // disable timer 0's A and B timers
  TIMER0_CFG_R  = 0x0;                 // put timer 0 into concatenated mode
  TIMER0_CTL_R  |= (1 << 5);           // enable timer to trigger ADC
  TIMER0_TAMR_R |= (0x2 << 0);         // set timer to periodic mode    (TAMR = 0x2)
  TIMER0_TAMR_R &= ~(1 << 4);          // set timer to count down mode  (TCDIR = 0)
}

/* Starts timer 0
 */
void TIMER0_start()
{
  TIMER0_ICR_R |= 0x1;     // clear the timer flag
  TIMER0_CTL_R |= 0x1;     // enable the timer
}

/* Stops timer 0
 */
void TIMER0_stop()
{
  TIMER0_CTL_R &= ~0x1;    // disable the timer
  TIMER0_ICR_R |= 0x1;     // clear the timer flag
}

/* Loads a value into timer 0 - timer should be disabled first
 *  @param - val is the initial value to load into the timer
 */
void TIMER0_reload(unsigned int val)
{
  TIMER0_CTL_R &= ~(1 << 0);   // disable the timer
  TIMER0_TAILR_R = val;        // load new initial value
  TIMER0_ICR_R |= (1 << 0);    // clear the timer flag
  TIMER0_CTL_R |= (1 << 0);    // enable the timer
}

// ************** ADC_Init *********************************
// - Initializes the ADC to use a specficed channel on SS3
// *********************************************************
// Input: none
// Output: none
// *********************************************************
void ADC0_Init(void) {
  unsigned long wait = 0;
  SYSCTL_RCGCADC_R = (1 << 0);  // enable ADC module 0
  wait++;
  wait++;
  ADC0_ACTSS_R &= ~(1 << 3);    // disable sample sequencer 3
  ADC0_EMUX_R = (0x5 << 12);    // enable timer to trigger SS3
  ADC0_SSCTL3_R |= (1 << 1);    // enable single sample sequence
  ADC0_SSCTL3_R |= (1 << 2);    // enable raw interrupt signal
  ADC0_SSCTL3_R |= (1 << 3);    // enable temp sensor selection on SS3
  ADC0_IM_R |= (1 << 3);        // forward signal to the interrupt controller
  NVIC_EN0_R |= (1 << 17);
  
  ADC0_ACTSS_R |= (1 << 3);     // enable sample sequencer 3
  ADC0_ISC_R |= (1 << 3);       // clear the raw interrupt register for SS 3
  ADC0_PSSI_R |= (1 << 3);      // initiate the sample sequence
}

// ************** ADC_Handler ******************************
// - Interrupt handler for the ADC 0
//   recalculates the temperature read by the internal sensor
// *********************************************************
// Input: none
// Output: none
// *********************************************************
void ADC0_Handler(void)
{
  temperature = 147.5 - (247.5 * ADC0_SSFIFO3_R / 4096); // evaluate temperature
  LCD_SetCursor(145, 116);         // Move cursor to the start of the screen
  // Send the temperate via the UART module, 1 character at a time
  sprintf(text, "%.2f", temperature);
  LCD_PrintString(text);           // Print char array
  ADC0_ISC_R |= (1 << 3);          // clear the raw interrupt registor for SS 3
  TIMER0_ICR_R |= (1 << 0);        // clear the timer 0 flag
}