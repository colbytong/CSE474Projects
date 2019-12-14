#include <custom_header.h>
#include <stdint.h>

// main function
int main()
{
  RCGC2_REGISTER = RCGC2_PORTF; // enable Port F GPIO
  GPIO_DEN_PORTF = PF0_TO_PF4_DEN; // enable digital pins PF0 - PF4
  GPIO_DIR_PORTF = RGB; // set PF1 - PF3 as output
  
  GPIO_LOCK_PORTF = GPIO_UNLOCK_KEY; // unlock port F to allow writing to register
  GPIOCR_PORTF = PF0_AND_PF4; // configure register to write to GPIOPUR
  GPIOPUR_PORTF = PF0_AND_PF4; // enable pads for PF0 and PF4
  
  while(1)
  {
    int SW1 = GPIO_DATA_PORTF & SW1_MASK; // switch 1 status
    int SW2 = GPIO_DATA_PORTF & SW2_MASK; // switch 2 status
    if(SW1 == 0x01) // switch 1 is pressed
    {
      GPIO_DATA_PORTF |= RED; // set PF1 to 1 and the other port F pins to 0
    }
    else if(SW2 != 0x01) // switch 2 is pressed
    {
      GPIO_DATA_PORTF |= GREEN; // set PF3 to 1 and the other port F pins to 0
    }
    else if ((SW1 == 0x01) & (SW2 != 0x01)) // both switches are pressed
    {
      GPIO_DATA_PORTF |= YELLOW; // set PF3 and PF1 to 1 and the other port F pins to 0
    }
    else
    {
      GPIO_DATA_PORTF &= !YELLOW; // turn off port F
    }
  }
  return 0;
}