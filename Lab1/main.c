#include <custom_header.h>
#include <stdint.h>
void delay() // cause a delay
{
  for (int i = 0; i < 900000; i ++){}
}
// main function
int main()
{
  RCGC2_REGISTER = RCGC2_PORTF; // enable Port F GPIO
  GPIO_DEN_PORTF = RGB; // enable digital pin PF1- PF3
  GPIO_DIR_PORTF = RGB; // set PF1 - PF3 as output
  while(1)
  {
    GPIO_DATA_PORTF = RED; // set PF1 to 1 and the other port F pins to 0
    delay();
    GPIO_DATA_PORTF = PURPLE; // set PF1 and PF2 to 1 and the other port F pins to 0
    delay();
    GPIO_DATA_PORTF = BLUE; // set PF2 to 1 and the other port F pins to 0
    delay();
    GPIO_DATA_PORTF = TEAL; // set PF2 and PF3 to 1 and the other port F pins to 0
    delay();
    GPIO_DATA_PORTF = GREEN; // set PF3 to 1 and the other port F pins to 0
    delay();
    GPIO_DATA_PORTF = YELLOW; // set PF1 and PF3 to 1 and the other port F pins to 0
    delay();
    GPIO_DATA_PORTF = WHITE; // set PF1 - PF3 to 1 and the other port F pins to 0
    delay();
  }
  return 0;
}