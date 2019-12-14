#include <header_HW3.h>  
#include "stdint.h"  
#include <stdio.h>

// UART Initialization  
void UART_Init() {  
  RCGCUART |= (0x1 << 1); // Enable UART module 1
  RCGCGPIO |= 0x2; // Enable Port B clock  
  GPIOAFSEL_PORTB |= 0x3; // Enable alt functions PB0 PB1
  //Insert Digital enable for port B pins
  GPIOPCTL_PORTB |= 0x1; // Enable port control PB0  
  GPIOPCTL_PORTB |= (0x1 << 4); // Enable port control PB1 
  
  UARTCTL &= ~0x1; // Disable UART 
  UARTIBRD = 65; // Write integer part of BRD (65) to UARTIBRD  
  UARTFBRD = 51; // Write fractional part of BRD (51) to UARTFBRD  
  UARTLCRH |= (0x3 << 5); // 8-bit, no parity, 1 stop bit  
  UARTCC = 0x5; // Clock source is precision oscillator
  UARTCTL |= 0x1; // Enable UART   
}

int main()
{
  return 0;
}
