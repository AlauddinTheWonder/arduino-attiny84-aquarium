/* 
 * Alauddin Ansari
 * 2019-15-18
 * ATtiny84 Watchdog settings
 * External interrupt enabled (https://www.youtube.com/watch?v=0aAwKT0YWJU)
 */
 
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect)
{
  // Executes before resuming loop function
}

// 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms
// 6=1sec, 7=2sec, 8=4sec, 9=8sec
void setup_watchdog(int ii) 
{
  byte bb;
  
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);

  MCUSR &= ~(1<<WDRF);
  
  // start timed sequence
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  
  // set new watchdog timeout value
  WDTCSR = bb;
  WDTCSR |= _BV(WDIE);
}

// set system into the sleep state 
// system wakes up when watchdog is timed out
void system_sleep() 
{
  cbi(ADCSRA, ADEN);                    // switch Analog to Digitalconverter OFF
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  
  sei();                               // Enable the Interrupts so the wdt can wake us up

  sleep_mode();                        // System sleeps here

  sleep_disable();                     // System continues execution here when watchdog timed out 
  sbi(ADCSRA, ADEN);                    // switch Analog to Digitalconverter ON
}

/*
 * External Interrupt setting
 * 
 * PCIE0 is mapped for PCINT0 to PCINT7
 * PCIE1 is mapped for PCINT8 to PCINT11
 * 
 * PCMSK0 is mapped with PCIE0
 * PCMSK1 is mapped with PCIE1
 */
void setup_interrupt(int PCINT_PIN)
{
  if (PCINT_PIN < 8) {
    sbi(GIMSK, PCIE0);
    sbi(PCMSK0, PCINT_PIN);
  } else {
    sbi(GIMSK, PCIE1);
    sbi(PCMSK1, PCINT_PIN);
  }
}
