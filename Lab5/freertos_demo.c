#include "stdint.h"
#include <stdio.h> 
#include "FreeRTOS.h"  
#include "task.h"  
#include "header_HW4.h"
#include <header_Lab3.h>

// Global status flags
int pedestrian_status = 0; // Pedestrian flag
int onoff_status = 0; // On/off flag
  
// Different states in FSM  
const int OFF = 0;  
const int GO = 1;  
const int WARN = 2;  
const int STOP = 3;
const int TEMPOFF = 4;
// define FSM state  
int state = 0; // initialize state as OFF 

// Task function that checks the state of the pedestrian button.
void Pedestrian(void *p);  
  
// Task function that checks the state of the onoff button.
void StartStop(void *p);  
  
// Task function that represents your Finite State Machine.
void Control(void *p);  
  
// FSM helper method which handles the traffic light state transition.  
void FSM(void);  

// This hook is called by FreeRTOS when an stack overflow error is detected.  
void vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName) {  
  // This function can not return, so loop forever.
  while (1) {}  
}  
  
//Initializations  
// Initialize LEDs    
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
// Initialize switches    
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
// PWM initialization
void PWM_init(void){
  RCGC0 = (1<<6); // enable clock for PWM
  RCGC2_REGISTER |= 0x1; // activate clock for Port A  
  GPIO_AFSEL_PORTA &= ~PA5_TO_PA7; // regular port function for LEDs
  GPIO_PCTL_PORTA &= ~0xFFF00000; // port mux control pins 5 to 7
  RCC |= (1<<20); // use PWM divider
  RCC &= ~(7<<17); // set PWM to divide by 2
  PWM0CTL = 0x00000000; // configure PWM generator for countdown mode
  PWM0GENA = 0x0000008C;
  PWM0GENB = 0x0000080C;
  PWM0LOAD =  0x0000018F; // set 400 clock ticks per period
  PWM0CMPA = 0x0000012B; // Set the pulse width of the MnPWM0 pin for a 25% duty cycle
  PWM0CMPB = 0x00000063; // Set the pulse width of the MnPWM1 pin for a 75% duty cycle
  PWM0CTL = 0x00000001; // Start the timers in PWM generator 0
  PWMENABLE = 0x00000003; // Enable PWM outputs
}

// period is number of clock cycles in PWM period ((1/clock) units)
void PWOut_Init1(unsigned short period){
  volatile unsigned long delay;
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER0;// activate timer0
  /*SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD; // activate port D
  delay = SYSCTL_RCGC2_R;          // allow time to finish activating
  GPIO_PORTD_DEN_R |= 0x10;        // enable digital I/O on PD4
  GPIO_PORTD_AFSEL_R |= 0x10;      // enable alt funct on PD4
*/
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = TIMER_CFG_16_BIT; // configure for 16-bit timer mode
                                   // configure for alternate (PWM) mode
  TIMER0_TAMR_R = (TIMER_TAMR_TAAMS|TIMER_TAMR_TAMR_PERIOD);
  TIMER0_TAILR_R = period-1;       // timer start value
//  TIMER0_TAMATCHR_R = period/4;    // duty cycle = 75%
//  TIMER0_TAMATCHR_R = period/2;    // duty cycle = 50%
  TIMER0_TAMATCHR_R = 3*period/4;    // duty cycle = 25%
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 16-b, PWM, 50% duty
}

//Breadboard LED controls    
// turn on green LED    
void GREEN_LED_on(void) {    
  GPIO_DATA_PORTA = PA5; // set PA5 bit to 1 (on)    
}    
// turn on yellow LED    
void YELLOW_LED_on(void) {    
  GPIO_DATA_PORTA = PA6; // set PA6 bit to 1 (on)    
}    
// turn on red LED    
void RED_LED_on(void) {    
  GPIO_DATA_PORTA = PA7; // set PA7 bit to 1 (on)    
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


// Initialize FreeRTOS and start the tasks.  
int main(void) {
  // PWM_init();
  // PWOut_Init1(6000); // initialize timer0A in PWM mode (1,000 Hz)
  LED_init();  
  switch_init();  
  // xTaskCreate(Function Name,  
  //             Descriptive Task Name,  
  //             Stack Depth,  
  //             Task Function Parameter,  
  //             Priority,  
  //             Task Handle);  
  xTaskCreate(StartStop, (const char *)"StartStopButton", 1024, NULL, 2, NULL); // Highest priority  
  xTaskCreate(Pedestrian, (const char *)"PedestrianButton", 1024, NULL, 1, NULL); // Middle priority  
  xTaskCreate(Control, (const char *)"Control FSM", 1024, NULL, 0, NULL); // Lowest priority
  // Start the scheduler. This should not return.
  vTaskStartScheduler();  
  // In case the scheduler returns for some reason, loop forever.  
  while(1) {}  
}
  
// Start/Stop task
void StartStop(void *p) {
  static int curr_onoff_tick_time = 0; // current time variable
  static int prev_onoff_tick_time = 0; // previous time variable
  while (1) {  
    curr_onoff_tick_time = xTaskGetTickCount(); // set current to count of ticks since the scheduler started
  
    // Check whether the user switch is pressed.  
    //  
    // If the switch has been pressed for .5 seconds,  
    // set the global flag to 1 and set the prev_tick to be equal  
    // to curr_tick (clear the timer). Otherwise clear the global flag.  
    //  
    // If the user switch is not pressed, clear the global flag and  
    // set the prev_tick to curr_tick.  
    if ((GPIO_DATA_PORTA & PA2) != 0) { // power button is pressed  
      if (curr_onoff_tick_time - prev_onoff_tick_time >= 500) { // button pressed for .5+ seconds
        onoff_status = 1;  // Set the global flag
        prev_onoff_tick_time = curr_onoff_tick_time; // Clear the timer 
      } else {  
        onoff_status = 0; // Clear the global flag  
      }  
    } else {  
      onoff_status = 0; // Clear the global flag
      prev_onoff_tick_time = curr_onoff_tick_time; // Clear the timer
    }
    // vTaskDelay(n) will cause the task to enter Blocked state  
    // for n system clock ticks
    vTaskDelay(1);    
  }  
}  
  
// Pedestrian task
void Pedestrian(void *p) {  
  static int curr_ped_tick_time = 0; // current time variable
  static int prev_ped_tick_time = 0; // previous time variable  
  while (1) {  
    curr_ped_tick_time = xTaskGetTickCount(); // set current to count of ticks since the scheduler started  
    if ((GPIO_DATA_PORTA & PA3) != 0) { // pedestrian button is pressed  
      if (curr_ped_tick_time - prev_ped_tick_time >= 500) { // switch pressed for .5+ seconds 
        pedestrian_status = 1; // Set the global flag  
        prev_ped_tick_time = curr_ped_tick_time; // Clear the timer 
      } else {  
        pedestrian_status = 0; // Clear the global flag  
      }  
    } else {
      pedestrian_status = 0; // Clear the global flag  
      prev_ped_tick_time = curr_ped_tick_time; // Clear the timer  
    }  
    // vTaskDelay(n) will cause the task to enter Blocked state  
    // for n system clock ticks
    vTaskDelay(1);  
  }  
}  
  
// Control task
void Control(void *p) {  
  static int curr_led_tick_time = 0; // current time variable  
  static int prev_led_tick_time = 0; // previous time variable   
  while (1) {  
    curr_led_tick_time = xTaskGetTickCount(); // set current to count of ticks since the scheduler started
    // If the led has been yellow or temporarily off for .75+ seconds,  
    // or if any of the button flags have been set, switch to the  
    // corresponding next state and reset the led tick.  
    if ((curr_led_tick_time - prev_led_tick_time >= 750 && state == WARN) 
        || (curr_led_tick_time - prev_led_tick_time >= 750 && state == TEMPOFF) 
          || curr_led_tick_time - prev_led_tick_time >= 4000 || onoff_status || pedestrian_status) {  
      prev_led_tick_time = curr_led_tick_time; // Reset the led tick  
      // Switch to next state  
      if (state == GO) { // FSM is in go state  
        if (onoff_status) state = OFF; // Change state to off if onoff flag is set
        else state = WARN; // Toggle state back to warn 
      } else if (state == WARN) { // FSM is in warn state  
        if (pedestrian_status) state = STOP; // Change state to stop if pedestrian flag is set  
        else if (onoff_status) state = OFF; // Change state to off if onoff flag is set
        else state = TEMPOFF; // Change state to tempoff
      } else if (state == STOP) { // FSM is in stop state  
        if (onoff_status) state = OFF; // Change state to off if onoff flag is set
        else state = GO; // Change state to go
      } else if (state == OFF){ // FSM is in off state  
        if (onoff_status) state = WARN; // Change state to warn if onoff flag is set  
      } else if (state == TEMPOFF){ // FSM is in temporary off state  
        state = WARN; // Change state to warn
      } 
      FSM(); // Call FSM to change LEDs
    }
    // vTaskDelay(n) will cause the task to enter Blocked state  
    // for n system clock ticks
    vTaskDelay(1);  
  }  
}  
  
// FSM helper method for changing LED status
void FSM(void) {   
  if (state == OFF) { // Off state turns off all LEDs  
    GREEN_LED_off(); // Green LED off
    YELLOW_LED_off(); // Yellow LED off
    RED_LED_off(); // Red LED off 
  } else if (state == GO) { // Go state turns on green LED  
    GREEN_LED_on(); // Green LED on
  } else if (state == WARN) { // Warn state turns on yellow LED  
    YELLOW_LED_on(); // Yellow LED on
  } else if (state == STOP) { // Stop state turns on red LED  
    RED_LED_on(); // Red LED on
  } else if (state == TEMPOFF) { // Temporary off state turns off all LEDs  
    GREEN_LED_off(); // Green LED off
    YELLOW_LED_off(); // Yellow LED off
    RED_LED_off(); // Red LED off 
  }
}





/*#include <stdint.h>  
#include <stdio.h>  
#include "FreeRTOSConfig.h"  
#include "SSD2119.h"  
#include "header_HW4.h"  
#include "Lab.h"  
// Declarations  
unsigned short green = ((0x00>>3)<<11) | ((0xAA>>2)<<5) | (0x00>>3); // LCD green  
unsigned short white = ((0xAA>>3)<<11) | ((0xAA>>2)<<5) | (0xAA>>3); // LCD white  
unsigned short yellow = ((0xFF>>3)<<11) | ((0xFF>>2)<<5) | (0x55>>3); // LCD yellow  
int BLACK = 0;  
char text[10]; // Output char array  
  
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
 
int main ()  
{  
  // Initializations  
  LCD_Init(); // Initialize LCD screen  
  //int SW1 = (GPIO_DATA_PORTF & 0x11); // switch 1 status      
  //int SW2 = (GPIO_DATA_PORTF & 0x01); // switch 2 status   
  while(1) {  
    LCD_ColorFill(yellow);  
    LCD_DrawFilledCircle(160, 120, 45, BLACK); // Draw white circle  
    LCD_SetCursor(130, 115); // Move cursor to the start of the screen  
    // Send the temperate via the UART module, 1 character at a time  
    sprintf(text, "REACT NOW!");  
    LCD_PrintString(text);  
  }  
}
*/