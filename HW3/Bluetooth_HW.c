//Colby Tong
//HW3 Assignment
#include <stdint.h>
#include <stdio.h>
#include "HW3_Header.h"

/** UART Module 1 Initialization
 *
 *  Configures necessary UART Module 1 registers
 */
void UART_Init();

/** GPIO Initialization
 *
 *  Configures necessary GPIO registers for UART 1 Module
 */
void GPIO_Init();

/** Transmit Data via UART
 *
 *  Takes in the data to be transferred
 */
void TransmitData(char* data);

/** Delay the system
 */
void delay();

int main() {
  GPIO_Init();
  UART_Init();

  while (1) {
    delay();

    // If you want to print the data in a new line,
    // put \r\n at the end of your string
    char* data = "I love 474!\r\n";
    TransmitData(data);
  }

  return 1;
}

void GPIO_Init() {
  // STEP 1:
  // Congigure the GPIO pins to enable UART function

  RCGCGPIO |= 0x2;            // enable clock to Port B
  GPIO_AFSEL_PORTB |= 0x3;    // allow alternative function for PB 0&1
  GPIO_DEN_PORTB |= 0x3;      // enable digital pins
  GPIO_PCTL_PORTB |= 0x11;    // set alternative function to UART
}

void UART_Init() {
  // STEP 2:
  // Configure the UART Module
  RCGCUART |= (0x1 << 1);     // enable clock to UART Module 1
  int32_t delay = RCGCUART;   // delay to wait for clock setup
  UART_CTL &= ~0x1;           // disable module before configuration

  // set the baud rate (integer & fraction parts)
  UART_IBRD = 8;
  UART_FBRD = 44;

  UART_LCRH |= (0x3 << 5);    // set the transmission line control
  UART_CC &= ~0xF;            // use system clock 
  UART_CTL |=0x1;             // enable UART Module
}

void delay() {
  for (int i = 0; i < 1000000; i++) {}
}

void TransmitData(char* data) {
  char* temp = data;
  
  while(*temp != '\0') {
    // STEP 3:
    // Here you check the flag to see
    // if the UART is available
    while ((UART_FR & (0x1 << 3)) != 0) {} // Wait until UART is available
    UART_DR = *temp;
    temp++;
  }
}
