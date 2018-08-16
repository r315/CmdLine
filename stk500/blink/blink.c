
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//#define DEBUG_ON

#ifdef DEBUG
#include "serial.h"
#include "tester.h"
#endif


#define CHG_PROTECTION CLKPR = 0x80

#define DBG_LED_PIN     (1<<3)
#define DBG_LED_CFG     DDRB |= DBG_LED_PIN
#define DBG_LED_ON      PORTB |= DBG_LED_PIN
#define DBG_LED_OFF     PORTB &= ~DBG_LED_PIN
#define DBG_LED_TOGGLE  PORTB ^= DBG_LED_PIN
                   

// =============================================================================
// main
// =============================================================================
/*!
 * The internal 8Mhz oscillator is used as main clock source
 * the system divides this clock to obtain a F_CPU of 2Mhz, this frequency 
 * gives the lowest error on 9600bps for serial ports
 *
 *  Servo pulse is generated on PA0 and use PA3 with pot for controlling pulse width
 */
// =============================================================================
int main(void)
{
   
  //CHG_PROTECTION;         // write enable for CLKPR register
  //CLKPR = (1<<CLKPS1);    // Set FCPU = (Fosc / 4)
  
  cli();			                   // Disable global interrupts
  TCCR0A  = (1<<WGM01);                // Put Timer/Counter0 in CTC (Clear Timer on Compare Match) mode
  TCCR0B  = (1<<CS02);                 // Set Timer clock to F_CPU/256
  OCR0A = 234;                         // Timer0 is reset every 52,30ms (~20Hz)
  TIMSK0 |=  (1<<OCIE0A);  			   // Enable Timer0 Match OCR0A Register interrupt 
  
  DBG_LED_CFG; 

  sei();                               //Enable global interrupts  

  while(1) 
  {    
    _delay_ms(250);
  }
}

// =============================================================================
// ISR(TIM0_COMPA_vect) 
// =============================================================================
/*
 * 
 */
// =============================================================================
ISR(TIM0_COMPA_vect)
{
    DBG_LED_TOGGLE;
}






