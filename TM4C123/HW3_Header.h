#ifndef _HW3_HEADER_
#define _HW3_HEADER_

// UART REGISTERS
#define RCGCUART            (*(volatile uint32_t *)0x400FE618)
#define UART_CTL            (*(volatile uint32_t *)0x4000D030)
#define UART_LCRH           (*(volatile uint32_t *)0x4000D02C)
#define UART_CC             (*(volatile uint32_t *)0x4000DFC8)
#define UART_IBRD           (*(volatile uint32_t *)0x4000D024)
#define UART_FBRD           (*(volatile uint32_t *)0x4000D028)
#define UART_DR             (*(volatile uint32_t *)0x4000D000)
#define UART_FR             (*(volatile uint32_t *)0x4000D018)


// GPIO REGISTERS
#define RCGCGPIO            (*(volatile uint32_t *)0x400FE108)
#define GPIO_AFSEL_PORTB    (*(volatile uint32_t *)0x40005420)
#define GPIO_DEN_PORTB      (*(volatile uint32_t *)0x4000551C)
#define GPIO_PCTL_PORTB	    (*(volatile uint32_t *)0x4000552C)



#endif  // _HW3_HEADER_