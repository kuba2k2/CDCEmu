/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *      This is the header file for the single-wire software UART.
 *
 *      The file contains definitions and macros for the single-wire software
 *      UART. If a different microcontroller than the ATmega32 is used, this
 *      file needs to be changed. Port and baud rate settings may also be
 *      changed in this file.
 *
 * \par Application note:
 *      AVR274: Single-wire Software UART
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Name:  $
 * $Revision: 1364 $
 * $RCSfile: single_wire_UART.h,v $
 * $Date: 2007-02-08 14:13:02 +0100 (to, 08 feb 2007) $  \n
 ******************************************************************************/

#pragma once

#include "stdint.h"     //Integer types.

#ifndef FALSE
  #define FALSE 0
  #define TRUE !FALSE
#endif

#ifndef UART_RX_BUFFER_SIZE
#define UART_RX_BUFFER_SIZE     4
#endif
#ifndef UART_TX_BUFFER_SIZE
#define UART_TX_BUFFER_SIZE     4
#endif

// Baud rate settings (WAIT_ONE - PRESCALER):
//  Baud Rate     1MHz      2Mhz      4MHz      8MHz
//     4800     207 - 1    51 - 8   103 - 8   (207 - 8)
//     9600     103 - 1   207 - 1    51 - 8   103 - 8
//    19200       NA      103 - 1   207 - 1    51 - 8
//    28800       NA         NA     138 - 1    34 - 8
//    38400       NA         NA     103 - 1   207 - 1
// Please note that the UART consumes about all CPU resources when WAIT_ONE*PRESCALER<100.

/* Communication parameters. The WAIT_ONE definiton has to be changed according to equation 2-1 in the application note. */
#define WAIT_ONE              26      //!< Half bit period compare setting. See the application note for calculation of this value. Make sure timer prescaler is set to the intended value.
#define PRESCALER             64      //!< Prescaler setting. Must be set according to the baud rate setting.

/* Port and pin settings. */
#define SW_UART_PIN_NUMBER    4       //!< Set pin number for communication.
#define SW_UART_PORT          PORTB   //!< Set port for communication.
#define SW_UART_PIN           PINB    //!< Set pin for communication.
#define SW_UART_DDR           DDRB    //!< Data direction register. Not available for high voltage ports.

#define TRANSMIT_DELAY        70    //!< Cycles from the start bit is sent (from UART_transmit) to the timer is started plus cycles in the timer interrupt before first data bit is sent.
#define RECEIVE_DELAY         76    //!< Cycles from the start bit is detected to the timer is started plus cycles in timer interrupt before first data bit is received.

#define WAIT_ONEHALF          (WAIT_ONE + WAIT_ONE/2)

#define TIMER_PRESCALER_CONTROL_REGISTER    TCCR0B  //!< Define the timer control register according to the timer used for the UART.
#if (PRESCALER == 1)
  #define START_UART_TIMER()     (TIMER_PRESCALER_CONTROL_REGISTER |= (1<<CS00))  //Needs to be change if a different timer is used. Please refer to datasheet.
  #define STOP_UART_TIMER()      (TIMER_PRESCALER_CONTROL_REGISTER &= ~(1<<CS00))
#elif (PRESCALER == 8)
  #define START_UART_TIMER()     (TIMER_PRESCALER_CONTROL_REGISTER |= (1<<CS01))
  #define STOP_UART_TIMER()      (TIMER_PRESCALER_CONTROL_REGISTER &= ~(1<<CS01))
#elif (PRESCALER == 64)
  #define START_UART_TIMER()     (TIMER_PRESCALER_CONTROL_REGISTER |= (1<<CS01 | 1<<CS00))
  #define STOP_UART_TIMER()      (TIMER_PRESCALER_CONTROL_REGISTER &= ~(1<<CS01 | 1<<CS00))
#else
  #error PRESCALER must be set to 1 or 8
#endif

#if ( ((WAIT_ONEHALF-(RECEIVE_DELAY/PRESCALER)) > 255) || ((WAIT_ONE) > 255))
  #error WAIT_ONE is set too high. Try to increase prescaler or use a higher baud rate.
#endif

#if ( (WAIT_ONE) < (100/PRESCALER))
  #error A too high baud rate is used. Please check the PRESCALER and WAIT_ONE setting.
#endif

/* Pin macros.  */
#define INITIALIZE_UART_PIN()   ( SW_UART_PORT &= ~(1<<SW_UART_PIN_NUMBER) )    //!< Clear port.
#define READ_UART_PIN()         ( SW_UART_PIN & (1<<SW_UART_PIN_NUMBER) )

/* Macros for standard AVR ports. */
#define SET_UART_PIN()          ( SW_UART_DDR &= ~(1<<SW_UART_PIN_NUMBER) )     //!< Tri-state pin.
#define CLEAR_UART_PIN()        ( SW_UART_DDR |= (1<<SW_UART_PIN_NUMBER) )      //!< Set pin output low.
/* Macros for high voltage AVR ports. */
//#define SET_UART_PIN()          ( SW_UART_PORT &= ~(1<<SW_UART_PIN_NUMBER) )//!< Tri-state pin.
//#define CLEAR_UART_PIN()        ( SW_UART_PORT |= (1<<SW_UART_PIN_NUMBER) ) //!< Set pin output low.

/* UART interrupt vectors definitions. */
#define SW_UART_EXTERNAL_INTERRUPT_VECTOR       PCINT0_vect           //!< UART external interrupt vector. Make sure this is in accordance to the defined UART pin.
#define SW_UART_TIMER_COMPARE_INTERRUPT_VECTOR  TIMER0_COMPA_vect     //!< UART compare interrupt vector.

/* Timer macros. These are device dependent. */
#define CLEAR_UART_TIMER_ON_COMPARE_MATCH()     (TCCR0A |= (1<<WGM01))                              //!< Set timer control register to clear timer on compare match (CTC).
#define SET_UART_TIMER_COMPARE_WAIT_ONE()       (OCR0A = WAIT_ONE)                                  //!< Sets the timer compare register to one period.
#define SET_UART_TIMER_COMPARE_START_TRANSMIT() (OCR0A = WAIT_ONE - (TRANSMIT_DELAY/PRESCALER))     //!< Sets the timer compare register to the correct value when a transmission is started.
#define SET_UART_TIMER_COMPARE_START_RECEIVE()  (OCR0A = WAIT_ONEHALF - (RECEIVE_DELAY/PRESCALER))  //!< Sets the timer compare register to the correct value when a reception is started.
#define CLEAR_UART_TIMER()                      (TCNT0 = 0x00)
#define ENABLE_UART_TIMER_INTERRUPT()           (TIMSK |= (1<<OCIE0A))
#define DISABLE_UART_TIMER_INTERRUPT()          (TIMSK &= ~(1<<OCIE0A))
#define CLEAR_UART_TIMER_INTERRUPT_FLAG()       (TIFR = (1<<OCF0A))

/* External interrupt macros. These are device dependent. */
#define INITIALIZE_UART_EXTERNAL_INTERRUPT()    (GIMSK |= (1<<PCIE))    //< Enables Pin Change External Interrupts.
#define ENABLE_UART_EXTERNAL_INTERRUPT()        (PCMSK |= (1<<PCINT4))
#define DISABLE_UART_EXTERNAL_INTERRUPT()       (PCMSK &= ~(1<<PCINT4))
#define CLEAR_UART_EXTERNAL_INTERRUPT_FLAG()    (GIFR = (1<<PCIF))

/* Status register defines. */
#define SW_UART_FRAME_ERROR           7     //!< Set if a frame error has occured.

/* Flag macros. */
#define SET_FLAG(flag_register,flag_bit)    ( (flag_register) |= (1 << (flag_bit)) )  //!< Use this macro when setting a flag in a register.
#define CLEAR_FLAG(flag_register,flag_bit)  ( (flag_register) &= ~(1 << (flag_bit)) ) //!< Use this macro when clearing a flag in a register.
#define READ_FLAG(flag_register,flag_bit)   ( (flag_register) & (1 << (flag_bit)) )   //!< Use this macro when reading a flag in a register.


extern volatile uint8_t SW_UART_status;         //!< Byte holding status flags.
extern volatile uint8_t UART_Tx_buffer[];       //!< Transmission buffer.
extern volatile uint8_t UART_Tx_head;           //!< TX buffer writing head
extern volatile uint8_t UART_Tx_tail;           //!< TX buffer reading tail
extern volatile uint8_t UART_Rx_buffer[];       //!< Reception buffer.
extern volatile uint8_t UART_Rx_head;           //!< RX buffer writing head
extern volatile uint8_t UART_Rx_tail;           //!< RX buffer reading tail

//Use this to put the UART_counter in a dedicated register. Remember to comment out the previous declarations (also in the
//declaration in single_wire_UART.c) of the counter variable and to lock register 15 when this is used. This is done under
//the code section in the C/C++ compiler category in the IAR options menu.
//__no_init __regvar volatile uint8_t SW_UART_status @ 15;

//Use this to put the UART_status in a dedicated register. Remember to comment out the previous declaration of the
//counter variable and to lock register 14 when this is used. This is done under the code section in the C/C++ compiler
//category in the IAR options menu.
//__no_init __regvar volatile uint8_t UART_counter @ 14;

//Use this to put the UART_status in a general purpose io register. Check device datasheet to find adress of GPIO register if available
//(set to 0x1E here). Comment out the previos declarations (also in the declaration in single_wire_UART.c) of the status variable
//when the GPIO register is used.
//__io __no_init static volatile uint8_t SW_UART_status @ 0x1E;

void uart_transmit();
void uart_tx_buffer();
