#include <header_Lab3.h>
#include "stdint.h"
#include <stdio.h>

float temperature; // System temperature variable in C
volatile int maxVal = 0x3D0900; // Initialize max clock value to 4 million
volatile int freqDivisor = (0x63 << 22); // Initialize SYSDIV2 to 4 MHZ

//User LED controls  
// Turn on red LED  
void redLEDOn() {GPIO_DATA_PORTF = RED;}  
// Turn on yellow LED  
void yellowLEDOn() {GPIO_DATA_PORTF = YELLOW;}  
// Turn on green LED  
void greenLEDOn() {GPIO_DATA_PORTF = GREEN;}  
// Turn on teal LED  
void tealLEDOn() {GPIO_DATA_PORTF = TEAL;}  
// Turn on blue LED  
void blueLEDOn() {GPIO_DATA_PORTF = BLUE;}
// Turn on purple LED  
void purpleLEDOn() {GPIO_DATA_PORTF = PURPLE;}  
// Turn on white LED  
void whiteLEDOn() {GPIO_DATA_PORTF = WHITE;}  
// Turn off blue LED  
void blueLEDOff() {GPIO_DATA_PORTF &= ~BLUE;}  
// Turn off red LED  
void redLEDOff() {GPIO_DATA_PORTF &= ~RED;}

// Reset timer when 1 sec cycle finishes  
int timerCycled() {  
  if (GPTMRIS & 0x1 == 0x1) {  
    GPTMICR |= RESET_TA; // reset timer  
    return 1; // cycle has finished  
  } else {  
    return 0; // cycle has not finished  
  }  
}

// Configure Timer 0 with variable maximum counter value
void Timer0_Init() {
  RCGCTIMER |= GPTM0_16_32; // Enable 16/32 Timer 0  
  GPTMCTL = GPTMA_DISABLE; // Disable Timer A  
  GPTMCFG = TM_MODE_32; // Select 32-bit mode  
  GPTMTAMR |= TAMR_PER_TM_MODE; // Set periodic timer mode  
  GPTMTAMR &= ~TACDIR_COUNT_UP; // Configure TACDIR to count down  
  GPTMCTL |= (0x1 << 5); // Enable timer to trigger ADC
  GPTMTAILR = maxVal; // set max value to 4 million 
  GPTMICR |= RESET_TA; // Reset timer
  GPTMCTL |= GPTMA_ENABLE; // Enable Timer A 
}

// Configure GPIO Port F pins for user switches and LEDs
void PortF_Init(){
  // User LED initialization
  RCGC2_REGISTER = RCGC2_PORTF; // enable Port F GPIO
  GPIO_DEN_PORTF = PF0_TO_PF4_DEN; // enable digital pin PF1- PF3  
  GPIO_DIR_PORTF = RGB; // set PF1 - PF3 as output 
  // Switch initialization
  GPIO_LOCK_PORTF = GPIO_UNLOCK_KEY; // unlock port F to allow writing to register  
  GPIOCR_PORTF = PF0_AND_PF4; // configure register to write to GPIOPUR  
  GPIOPUR_PORTF = PF0_AND_PF4; // enable pads for PF0 and PF4
}

// Initialize the Phase Locked Loop
void PLL_Init(){
  RCC |= BYPASS; // Bypass PLL and clock divider
  RCC &= ~PWRDN; // Power and enable PLL
  RCC &= ~MOSCDIS; // Enable main oscillator
  RCC &= ~USESYSDIV; // System clock is used undivided
  RCC |= XTAL_16MHZ; // 16 MHZ crystal
  RCC |= (0xF << 8); // Set XTAL and bypass
  RCC2 |= (0x1 << 31); // RCC2 overrides RCC
  RCC2 |= BYPASS2; // Set BYPASS2
  RCC2 &= ~OSCRC_MOSC; // Select main oscilator
  RCC2 &= ~PWRDN2; // PLL operates normally
  RCC2 |= DIV400; // Append SYSDIV2LSB to make a 7 bit divisor
  RCC2 &= ~(0x7F << 22); // reset SYSDIV2
  RCC2 |= freqDivisor; // set SYSDIV2 to frequency divisor
  //Wait for the PLL to lock by polling the PLLLRIS bit in the Raw Interrupt Status (RIS) register.
  while(RIS & PLLLRIS_READY != PLLLRIS_READY) {}
  RCC2 &= ~BYPASS2; // Enable use of the PLL
}

// ADC Initialization
void ADC_Init() {
  RCGCADC |= 0x1; // Enable ADC clock
  for (int i = 0; i < 10000; i++){} // Small delay
  ADCACTSS &= ~(0x1 << 3); // Disable Sample Sequencer 3
  ADCEMUX &= ~(0xF000); // Clear everything
  ADCEMUX |= (0x5 << 12); // Trigger SS3 with timer
  ADCSSCTL3 |= 0xE; // Read TS, enable sample interrupt, begin sampling
  ADCIM |= (0x1 << 3); // Send raw interrupt signal from SS3
  INTERRUPT_ENABLE |= (1 << 17); // Enable interrupt from ADC
  ADCACTSS |= (0x1 << 3); // Enable Sample Sequencer 3
}

// ISR for the ADC0 module, calculate internal temperature and set LED accordingly
void ADC0_Handler(){
  temperature = 147.5 - (247.5 * (ADCSSFIFO3)) / 4096.0; // Extract temperature value
  if (temperature >= 0 && temperature <= 17) {
    redLEDOn(); // Turn on red LED
  } else if (temperature >= 17 && temperature <= 19) {
    blueLEDOn(); // Turn on blue LED
  } else if (temperature >= 19 && temperature <= 21) {
    purpleLEDOn(); // Turn on purple LED
  } else if (temperature >= 21 && temperature <= 23) {
    greenLEDOn(); // Turn on green LED
  } else if (temperature >= 23 && temperature <= 25) {
    yellowLEDOn(); // Turn on yellow LED
  } else if (temperature >= 25 && temperature <= 27) {
    tealLEDOn(); // Turn on teal LED
  } else if (temperature >= 27 && temperature <= 40) {
    whiteLEDOn(); // Turn on white LED
  }
  GPTMICR |= RESET_TA; // Reset Timer A
  ADCISC |= (0x1 << 3); // Reset ADC interrupt flag
}

// Main function
main(){
  Timer0_Init(); // Initialize Timer 0
  PortF_Init(); // Initialize GPIO Port F
  PLL_Init(); // Initialize PLL
  ADC_Init(); // Initialize ADC
  UART_Init(); // Initialize UART
  while(1){
    int SW1 = GPIO_DATA_PORTF & SW1_MASK; // switch 1 status  
    int SW2 = GPIO_DATA_PORTF & SW2_MASK; // switch 2 status  
    if(SW1 == 0x01) { // switch 1 is pressed PF4
      freqDivisor = (0x4 << 22); //Set SYSDIV2 to 80 MHZ
      PLL_Init(); // Reinitialize PLL
      maxVal = 0x4C4B400; // set max value to 80 million
      Timer0_Init(); // Reinitialize timer
    } else if(SW2 != 0x01) { // switch 2 is pressed PF0
      freqDivisor = (0x63 << 22); //Set SYSDIV2 to 4 MHZ
      PLL_Init(); // Reinitialize PLL
      maxVal = 0x3D0900; // set max value to 4 million
      Timer0_Init(); // Reinitialize timer
    }
  }
  return 0;
}