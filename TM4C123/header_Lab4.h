// Lab 4 custom header file

#define RCGCGPIO                (*((volatile uint32_t *)0x400FE608))
#define RCGCTIMER               (*((volatile uint32_t *)0x400FE604))
#define RCGC2_REGISTER          (*((volatile uint32_t *)0x400FE108)) // Run Mode Clock Gate Control 2 address
#define GPIO_UNLOCK_KEY         0x4C4F434B // Port F unlock key for writing to register
// General Purpose Timer addresses
#define GPTMCTL                 (*((volatile uint32_t *)0x4003000C))
#define GPTMCFG                 (*((volatile uint32_t *)0x40030000))
#define GPTMTAMR                (*((volatile uint32_t *)0x40030004))
#define GPTMTAILR               (*((volatile uint32_t *)0x40030028))
#define GPTMRIS                 (*((volatile uint32_t *)0x4003001C))
#define GPTMICR                 (*((volatile uint32_t *)0x40030024))
// GPIO Port A addresses
#define GPIO_PCTL_PORTA         (*((volatile uint32_t *)0x4000452C))
#define GPIO_AMSEL_PORTA        (*((volatile uint32_t *)0x40004528))
#define GPIO_DIR_PORTA          (*((volatile uint32_t *)0x40004400))
#define GPIO_AFSEL_PORTA        (*((volatile uint32_t *)0x40004420))
#define GPIO_DEN_PORTA          (*((volatile uint32_t *)0x4000451C))
#define GPIO_DATA_PORTA         (*((volatile uint32_t *)0x400043FC))
// GPIO Port F addresses
#define GPIO_DEN_PORTF          (*((volatile uint32_t *)0x4002551C))
#define GPIO_DIR_PORTF          (*((volatile uint32_t *)0x40025400))
#define GPIO_DATA_PORTF         (*((volatile uint32_t *)0x400253FC))
#define GPIO_LOCK_PORTF         (*((volatile uint32_t *)0x40025520))
#define GPIOCR_PORTF            (*((volatile uint32_t *)0x40025524))
#define GPIOPUR_PORTF           (*((volatile uint32_t *)0x40025510))
#define GPIOAFSEL               (*((volatile uint32_t *)0x40025420))
#define GPIOPCTL                (*((volatile uint32_t *)0x4002552C))
// Port A controls
#define PA2_AND_PA3             0xC
#define PA2                     0x4
#define PA3                     0x8
#define PA5_TO_PA7              0xE0
#define PA5                     0x20
#define PA6                     0x40
#define PA7                     0x80
// Port F controls
#define GPIO_UNLOCKED_PORTF     0x0
#define RCGC2_PORTF             0x20
#define PF0_AND_PF4             0x11
#define PF0_TO_PF4_DEN          0xFF
// Masks for checking Tiva board switches 1 and 2
#define SW1_MASK                0x11
#define SW2_MASK                0x01
//Tiva board LEDs
#define RGB                     0xE
#define WHITE                   0xE
#define RED                     0x2
#define BLUE                    0x4
#define GREEN                   0x8
#define PURPLE                  0x6
#define TEAL                    0xC
#define YELLOW                  0xA
// Timer configurations
#define GPTM0_16_32             0x1
#define GPTMA_ENABLE            0x1
#define GPTMA_DISABLE           0x0
#define TM_MODE_32              0x0
#define TAMR_PER_TM_MODE        0x2
#define TACDIR_COUNT_UP         0x10000
#define N4_MIL                  0x3D0900
#define N16_MIL                 0xF42400
#define N80_MIL                 0x4C4B400
#define RESET_TA                0x1
// Interrupt configuration
#define GPTMIMR_PORTF           (*((volatile uint32_t *)0x40025018))
#define GPTMIMR                 (*((volatile uint32_t *)0x40030018))
#define INTERRUPT_ENABLE        (*((volatile uint32_t *)0xE000E100))
#define GPIOICR                 (*((volatile uint32_t *)0x4000441C))
#define GPIOICR_PORTF           (*((volatile uint32_t *)0x4002541C))
#define GPIOIS_PORTF            (*((volatile uint32_t *)0x40025404))
#define GPIOIBE_PORTF           (*((volatile uint32_t *)0x40025408))
#define GPIOIEV_PORTF           (*((volatile uint32_t *)0x4002540C))
#define GPIOICR_RESET_PF0       0x1
#define GPIOICR_RESET_PF4       0x10
#define TATOIM_ENABLE           0x1
#define INT_19_ENABLE           0x80000
#define INT_30_ENABLE           0x40000000
#define INT_19_AND_30_ENABLE    0x40080000
// PLL configuration
#define RCC                     (*((volatile uint32_t *)0x400FE060))
#define RCC2                    (*((volatile uint32_t *)0x400FE070))
#define RIS                     (*((volatile uint32_t *)0x400FE050))
#define MOSCDIS                 0x1
#define XTAL_16MHZ              (0x15 << 6)
#define OSCRC_MOSC              (0x7 << 4)
#define PWRDN                   0x200
#define PWRDN2                  0x2000
#define USESYSDIV               (0x1 << 22)
#define USERCC2                 (0x1 << 31)
#define DIV400                  (0x1 << 30)
#define SYSDIV2_4MHZ            (0x19 << 23)
#define SYSDIV2_80MHZ           (0x2 << 23)
#define SYSDIV2LSB              (0x1 << 22)
#define BYPASS                  (0x1 << 11)
#define BYPASS2                 (0x1 << 11)
#define PLLLRIS_READY           (0x1 << 6)
// ADC configuration
#define RCGCADC                 (*((volatile uint32_t *)0x400FE638))
#define ADCACTSS                (*((volatile uint32_t *)0x40038000))
#define ADCEMUX                 (*((volatile uint32_t *)0x40038014))
#define ADCSSCTL3               (*((volatile uint32_t *)0x400380A4))
#define ADCIM                   (*((volatile uint32_t *)0x40038008))
#define ADCSSFIFO3              (*((volatile uint32_t *)0x400380A8))
#define ADCISC                  (*((volatile uint32_t *)0x4003800C))
// UART addresses
#define RCGCUART                (*((volatile uint32_t *)0x400FE618))
#define UARTCTL                 (*((volatile uint32_t *)0x4000C030))
#define UARTIBRD                (*((volatile uint32_t *)0x4000C024))
#define UARTFBRD                (*((volatile uint32_t *)0x4000C028))
#define UARTLCRH                (*((volatile uint32_t *)0x4000C02C))
#define UARTCC                  (*((volatile uint32_t *)0x4000CFC8))
#define UARTFR                  (*((volatile uint32_t *)0x4000C018))
#define UARTDR                  (*((volatile uint32_t *)0x4000C018))
// GPIO Port B addresses
#define GPIODEN_PORTB           (*((volatile uint32_t *)0x4000551C))
#define GPIODIR_PORTB           (*((volatile uint32_t *)0x40005400))
#define GPIOAFSEL_PORTB         (*((volatile uint32_t *)0x40005420))
