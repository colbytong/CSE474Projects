// Run Mode Clock Gating Control Register
#define SYSCTL_RCGC2_R (*((volatile uint32_t *)0x400FE108))

// RCC/RCC2
#define RCC (*((volatile uint32_t *)0x400FE060))
#define RCC2 (*((volatile uint32_t *)0x400FE070))
#define RIS (*((volatile uint32_t *)0x400FE050))

// GPIO PORTF bit
#define SYSCTL_RCGC2_GPIOF 0x00000020
// GPIO PORTA bit
#define SYSCTL_RCGC2_GPIOA 0x00000001

// GPIO registers PORTF
#define GPIO_PORTF_DIR_R (*((volatile uint32_t *)0x40025400))
#define GPIO_PORTF_DEN_R (*((volatile uint32_t *)0x4002551C))
#define GPIO_PORTF_DATA_R (*((volatile uint32_t *)0x400253FC))
#define GPIO_PORTF_LOCK_R (*((volatile uint32_t *)0x40025520))
#define GPIO_PORTF_CR_R (*((volatile uint32_t *)0x40025524))
#define GPIO_PORTF_PUR_R (*((volatile uint32_t *)0x40025510))
#define GPIO_PORTF_ICR_R (*((volatile uint32_t *)0x4002541C))
#define GPIO_PORTF_IM_R (*((volatile uint32_t *)0x40025410))
#define GPIO_PORTF_MIS_R (*((volatile uint32_t *)0x40025418))
// GPIO LOCK unlock int
#define GPIO_LOCK_UNLOCK 0x4C4F434B

// GPIO registers PORTA
#define GPIO_PORTA_PCTL_R (*((volatile uint32_t *)0x4000452C))
#define GPIO_PORTA_AMSEL_R (*((volatile uint32_t *)0x40004528))
#define GPIO_PORTA_DIR_R (*((volatile uint32_t *)0x40004400))
#define GPIO_PORTA_AFSEL_R (*((volatile uint32_t *)0x40004420))
#define GPIO_PORTA_DEN_R (*((volatile uint32_t *)0x4000451C))

#define GPIO_PORTA_DATA_R (*((volatile uint32_t *)0x400043FC))
#define GPIO_PORTA_LOCK_R (*((volatile uint32_t *)0x40004520))
#define GPIO_PORTA_CR_R (*((volatile uint32_t *)0x40004524))
#define GPIO_PORTA_PUR_R (*((volatile uint32_t *)0x40004510))

#define GPIO_PORTA_DR2R_R (*((volatile uint32_t *)0x40004500))
#define GPIO_PORTA_DR4R_R (*((volatile uint32_t *)0x40004504))
#define GPIO_PORTA_DR8R_R (*((volatile uint32_t *)0x40004508))

// Timer Registers
#define SYSCTL_RCGCTIMER_R (*((volatile uint32_t *)0x400FE604))
// Timer 0
#define TIMER0_CTL_R (*((volatile uint32_t *)0x4003000C))
#define TIMER0_CFG_R (*((volatile uint32_t *)0x40030000))
#define TIMER0_TAMR_R (*((volatile uint32_t *)0x40030004))
#define TIMER0_TAILR_R (*((volatile uint32_t *)0x40030028))
#define TIMER0_RIS_R (*((volatile uint32_t *)0x4003001C))
#define TIMER0_ICR_R (*((volatile uint32_t *)0x40030024))
#define TIMER0_IMR_R (*((volatile uint32_t *)0x40030018))
// Timer 1
#define TIMER1_CTL_R (*((volatile uint32_t *)0x4003100C))
#define TIMER1_CFG_R (*((volatile uint32_t *)0x40031000))
#define TIMER1_TAMR_R (*((volatile uint32_t *)0x40031004))
#define TIMER1_TAILR_R (*((volatile uint32_t *)0x40031028))
#define TIMER1_RIS_R (*((volatile uint32_t *)0x4003101C))
#define TIMER1_ICR_R (*((volatile uint32_t *)0x40031024))
#define TIMER1_IMR_R (*((volatile uint32_t *)0x40031018))

// Enable interrupts
#define NVIC_EN0_R (*((volatile uint32_t *)0xE000E100))

// ADC Module Registers
#define SYSCTL_RCGCADC_R (*((volatile uint32_t *)0x400FE638))
// ADC 0
#define ADC0_ACTSS_R (*((volatile uint32_t *)0x40038000))
#define ADC0_EMUX_R (*((volatile uint32_t *)0x40038014))
#define ADC0_PSSI_R (*((volatile uint32_t *)0x40038028))
#define ADC0_RIS_R (*((volatile uint32_t *)0x40038004))
#define ADC0_ISC_R (*((volatile uint32_t *)0x4003800C))
#define ADC0_IM_R (*((volatile uint32_t *)0x40038008))
  // SS 3
  #define ADC0_SSCTL3_R (*((volatile uint32_t *)0x400380A4))
  #define ADC0_SSFIFO3_R (*((volatile uint32_t *)0x400380A8))

// UART Module registers
#define RCGC_UART_R (*((volatile uint32_t *)0x400FE618))
// UART 0
#define UART_CTL_R (*((volatile uint32_t *)0x4000C030))
#define UART_IBRD_R (*((volatile uint32_t *)0x4000C024))
#define UART_FBRD_R (*((volatile uint32_t *)0x4000C028))
#define UART_LCRH_R (*((volatile uint32_t *)0x4000C02C))
#define UART_DATA_R (*((volatile uint32_t *)0x4000C000))
#define UART_CC_R (*((volatile uint32_t *)0x4000CFC8))
#define UART_FR_R (*((volatile uint32_t *)0x4000C018))

// Onboard GPIO pins
// PF0 and PF4 are switches 2 and 1 respectively
// PF1, PF2, PF3 are onboard LEDs red, green, and blue respectively
#define PF0 (1 << 0)
#define PF1 (1 << 1)
#define PF2 (1 << 2)
#define PF3 (1 << 3)
#define PF4 (1 << 4)
#define PF5 (1 << 5)

// General port A pins
#define PA2 0x04
#define PA3 0x08
#define PA4 0x10
#define PA5 0x20
#define PA6 0x40