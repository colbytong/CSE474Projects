//Lab 5 custom header file

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
// Port A controls
#define PA2_AND_PA3             0xC
#define PA2                     0x4
#define PA3                     0x8
#define PA5_TO_PA7              0xE0
#define PA5                     0x20
#define PA6                     0x40
#define PA7                     0x80