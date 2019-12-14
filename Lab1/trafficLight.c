#include <custom_header.h>
#include <stdint.h>

// initialize enum for different states in FSM
enum states{go, warn, stop, off}state;

// initialize LEDs
void LED_init(void)
{
  volatile unsigned long delay;
  RCGC2_REGISTER |= 0x01; // activate clock for Port A
  delay = RCGC2_REGISTER; // allow time for clock to start
  GPIO_PCTL_PORTA &= ~0x00000F00; // regular GPIO
  GPIO_AMSEL_PORTA &= ~PA5_TO_PA7; // disable analog function of PA5-PA7
  GPIO_DIR_PORTA |= PA5_TO_PA7; // set PA5-PA7 to output
  GPIO_AFSEL_PORTA &= ~PA5_TO_PA7; // regular port function
  GPIO_DEN_PORTA |= PA5_TO_PA7; // enable digital output on PA5-PA7
}
// turn on green LED
void GREEN_LED_on(void)
{
  GPIO_DATA_PORTA |= PA5; // set PA5 bit to 1 (on)
}
// turn on yellow LED
void YELLOW_LED_on(void)
{
  GPIO_DATA_PORTA |= PA6; // set PA6 bit to 1 (on)
}
// turn on red LED
void RED_LED_on(void)
{
  GPIO_DATA_PORTA |= PA7; // set PA7 bit to 1 (on)
}
// turn off green LED
void GREEN_LED_off(void)
{
  GPIO_DATA_PORTA &= ~PA5;  // set PA5 bit to 0 (off)
}
// turn off yellow LED
void YELLOW_LED_off(void)
{
  GPIO_DATA_PORTA &= ~PA6;  // set PA6 bit to 0 (off)
}
// turn off red LED
void RED_LED_off(void)
{
  GPIO_DATA_PORTA &= ~PA7;  // set PA7 bit to 0 (off)
}
// initialize switches
void switch_init(void)
{
  volatile unsigned long delay;
  RCGC2_REGISTER |= 0x00000001; // activate clock for Port A
  delay = RCGC2_REGISTER; // allow time for clock to start
  GPIO_AMSEL_PORTA &= ~PA2_AND_PA3; // disable analog on PA2, PA3
  GPIO_PCTL_PORTA &= ~0x0000FF00; // PCTL GPIO on PA2, PA3
  GPIO_DIR_PORTA &= ~PA2_AND_PA3; // set PA2, PA3 to input
  GPIO_AFSEL_PORTA &= ~PA2_AND_PA3; // PA2, PA3 regular port function
  GPIO_DEN_PORTA |= PA2_AND_PA3; // enable PA2, PA3 as digital port
}
// return whether the power switch is pressed or not
unsigned long STARTSTOP_switch_input(void)
{
  return (GPIO_DATA_PORTA & PA2); // 0x4 (pressed) or 0 (not pressed)
}
// return whether the pedestrian switch is pressed or not
unsigned long PED_switch_input(void)
{
  return (GPIO_DATA_PORTA & PA3); // 0x8 (pressed) or 0 (not pressed)
}
// cause a delay
void delay()
{
  for (int i = 0; i < 900000; i ++);
}
// main function
int main()
{
  LED_init(); // initialize LEDs
  switch_init(); // initialize switches
  state = off; // set initial state to be off
  while(1)
  {
    switch (state)
    {
    case go: // oscilate between red and green lights
      GREEN_LED_off();
      RED_LED_on();
      delay();
      RED_LED_off();
      GREEN_LED_on();
      delay();
      if (STARTSTOP_switch_input() != 0)
      {
        state = off; // turn system off if power button is pressed
      }
      else if (PED_switch_input() != 0)
      {
        state = warn; // set state to warn if pedestrian button is pressed
      }
      break;
    case warn: // set light to yellow, delay, then set state to stop
      RED_LED_off();
      GREEN_LED_off();
      YELLOW_LED_on();
      delay();
      delay();
      state = stop;
      delay();
      if (STARTSTOP_switch_input() != 0)
      {
        state = off; // turn system off if power button is pressed
      }
      break;
    case stop: // set light to red, delay, then set state to go
      YELLOW_LED_off();
      RED_LED_on();
      delay();
      delay();
      state = go;
      if (STARTSTOP_switch_input() != 0)
      {
        state = off; // turn system off if power button is pressed
      }
      break;
    case off: // turn off all LEDs
      GREEN_LED_off();
      YELLOW_LED_off();
      RED_LED_off();
      delay();
      if (STARTSTOP_switch_input() != 0)
      {
        state = go;// turn system on if power button is pressed
      }
      break;
    }
  }
  return 0;
}