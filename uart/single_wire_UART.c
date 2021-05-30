/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *      This is the c file of the single-wire UART driver.
 *
 *      The file contains the complete implementation for the single-wire
 *      software UART. See the header file for configuration of the UART.
 *      A timer compare interrupt and an external interrupt are used to generate
 *      interrupts needed by the driver. The SW_UART_Enable, SW_UART_Transmit
 *      and SW_UART_Receive functions and the SW_UART_status variable are used
 *      for communicating with the UART driver. \n
 *      If available it is recommended to put the status byte in a general
 *      purpose i/o register. This will decrease code size and increase the
 *      speed for the UART driver.
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
 * $Revision: 1513 $
 * $RCSfile: single_wire_UART.c,v $
 * $Date: 2007-03-29 13:17:03 +0200 (to, 29 mar 2007) $  \n
 ******************************************************************************/

#include <avr/io.h>              //Device specific register/Bit definitions.
#include <avr/interrupt.h>              //The __enable_interrupt() intrinsic.
#include "stdint.h"             //Integer types.
#include "single_wire_UART.h"   //UART settings and device spesific.

/* Counter values */
#define UART_STATE_IDLE       0
#define RECEIVE_FIRST_DATA    3
#define RECEIVE_LAST_DATA     17
#define RECEIVE_STOP_1        19
#define RECEIVE_FINISH        23
#define TRANSMIT_FIRST_DATA   2
#define TRANSMIT_LAST_DATA    16
#define TRANSMIT_STOP_1       18
#define TRANSMIT_STOP_2       20
#define TRANSMIT_FINISH       22

/* Prototypes */
void SW_UART_Complete_Transfer(void);
void SW_UART_Initiate_Transmission(void);

volatile uint8_t SW_UART_status;          //!< Byte holding status flags.
static volatile uint8_t UART_counter;     //!< Holds the counter used in the timer interrupt.

/* Communication parameters. */
static volatile uint8_t   UART_Tx_data;     //!< Byte holding data being transmitted.
static volatile uint8_t   UART_Rx_data;     //!< Byte holding data being received.
static volatile uint8_t   UART_Tx_buffer;   //!< Transmission buffer.
static volatile uint8_t   UART_Rx_buffer;   //!< Reception buffer.


/*! \brief  Enable the UART.
 *
 *  This function initialize the timer and buffers,
 *  and enables the external interrupt to sense
 *  for incoming data. It is important to run this
 *  function before calling any of the other UART
 *  functions. Received data will automatically be
 *  put in the RX buffer.
 *
 *  \note   The UART can be stopped by disabling the
 *          timer and external interrupt.
 */
void uart_enable(void)
{
  //Tri-state communication pin.
  INITIALIZE_UART_PIN();

  SW_UART_status = 0x00;
  UART_counter = UART_STATE_IDLE;
  CLEAR_UART_TIMER_ON_COMPARE_MATCH();

  //Set up interrupts
  INITIALIZE_UART_EXTERNAL_INTERRUPT(); //Set up the external interrupt to wait for incoming start bit on correct pin.
  CLEAR_UART_EXTERNAL_INTERRUPT_FLAG(); //Clear flag in case it is already set for some reason.
  ENABLE_UART_EXTERNAL_INTERRUPT();
}


/*! \brief  Disable the UART.
 *
 *  This function disables the UART by disabling the timer
 *  interrupt and the external interrupt.
 */
/*
void uart_disable()
{
  SW_UART_status = 0x00;
  DISABLE_UART_TIMER_INTERRUPT();
  DISABLE_UART_EXTERNAL_INTERRUPT();
  STOP_UART_TIMER();
}
*/


/*! \brief  Transmit one byte.
 *
 *  This function sends one byte of data by
 *  putting it in the TX data and initialize a
 *  transmission if state is IDLE (UART_counter == 0)
 *
 *  \note   The SW_UART_TX_BUFFER_FULL flag must be
 *          0 when this function is called, or data in
 *          transmit buffer will be lost.
 *
 *  \param  data  Data to be sent.
 */
void uart_putc(const char c)
{
  SET_FLAG( SW_UART_status, SW_UART_TX_BUFFER_FULL );
  UART_Tx_buffer = c;

  //Start transmission if no ongoing communication.
  if( UART_counter == UART_STATE_IDLE )
  {
    //Copy byte from buffer and clear buffer full flag.
    UART_Tx_data = UART_Tx_buffer;
    CLEAR_FLAG( SW_UART_status, SW_UART_TX_BUFFER_FULL );

    DISABLE_UART_EXTERNAL_INTERRUPT();
    CLEAR_UART_PIN();                         //Write start bit.
    UART_counter = TRANSMIT_FIRST_DATA;       //Update counter so the 1. data bit is the next bit to be transmitted.
    CLEAR_UART_TIMER();                       //Clear timer.
    SET_UART_TIMER_COMPARE_START_TRANSMIT();  //Set timer compare value.
    CLEAR_UART_TIMER_INTERRUPT_FLAG();        //Make sure timer interrupt flag is not set.
    ENABLE_UART_TIMER_INTERRUPT();
    START_UART_TIMER();
  }
}


/*! \brief  Receive one byte.
 *
 *  This function receives one byte of data
 *  by accessing the Rx buffer.
 *
 *  \note   The SW_UART_RX_BUFFER_FULL flag
 *          must be one when this function
 *          is called.
 *
 *  \return Data received.
 */
char uart_getc(void)
{
  uint8_t data;
  data = UART_Rx_buffer;
  CLEAR_FLAG( SW_UART_status, SW_UART_RX_BUFFER_FULL );
  return data;
}


/*! \brief  External interrupt service routine
 *
 *  The falling edge in the beginning of the
 *  start bit will trigger this interrupt.
 *  Timer interrupt is enabled and counter is
 *  is set to RECEIVE_FIRST_DATA to signal that the next bit is
 *  the first data bit to be received.
 */
ISR(SW_UART_EXTERNAL_INTERRUPT_VECTOR)
{
  //Make sure bit is low.
  if(!READ_UART_PIN())
  {
    UART_counter = RECEIVE_FIRST_DATA;      //1. data bit is the next to be received
    UART_Rx_data = 0x00;
    STOP_UART_TIMER();                      //Stop timer to make sure prescaler is reset.
    CLEAR_UART_TIMER();
    SET_UART_TIMER_COMPARE_START_RECEIVE(); //Set timer compare value so the 1. data bit is sampled in the middle of the period.
    DISABLE_UART_EXTERNAL_INTERRUPT();
    CLEAR_UART_TIMER_INTERRUPT_FLAG();
    ENABLE_UART_TIMER_INTERRUPT();
    START_UART_TIMER();
  }
}


/*! \brief  Timer compare interrupt service routine
 *
 *  This interrupt is triggered when the compare
 *  register equals the timer. It increments the
 *  counter and handles the transmission and
 *  reception of each bit.
 *
 *  \note   The WAIT_ONE define has to be less than
 *          the maximum time spent in this routine
 *          so an interrupt is not missed.
 */
ISR(SW_UART_TIMER_COMPARE_INTERRUPT_VECTOR)
{
  SET_UART_TIMER_COMPARE_WAIT_ONE(); //Set timer compare value to trigger the ISR once every bit period.

  //Sample bit by checking the value on the UART pin:
  uint8_t bit_in = 0x00;
  if(READ_UART_PIN())
  {
    bit_in = 0x01;
  }

  ////////////////////////////
  //Receive if counter is odd.
  ////////////////////////////
  if(UART_counter & 0x01)
  {
    //If to receive data bit -> Copy received bit into Rx_data.
    if(UART_counter <= RECEIVE_LAST_DATA)
    {
      UART_Rx_data = ( UART_Rx_data >> 1 ); //Right shift RX_data so the new bit can be masked into the Rx_data byte.
      if(bit_in)
      {
        UART_Rx_data |= 0x80;               //Set MSB of RX data if received bit == 1.
      }
    }

    //If to receive stop bit -> Copy Rx data to Rx buffer.
    else if(UART_counter == RECEIVE_STOP_1)
    {
      //Set frame error flag if a low stop bit is received.
      if(!bit_in)
      {
        SET_FLAG( SW_UART_status, SW_UART_FRAME_ERROR );
        //Disable timer and stop reception?
      }

      //Set overflow error if RX_buffer is not received before new data is ready.
      if( READ_FLAG(SW_UART_status, SW_UART_RX_BUFFER_FULL))
      {
        SET_FLAG(SW_UART_status, SW_UART_RX_BUFFER_OVERFLOW);
      }

      UART_Rx_buffer = UART_Rx_data;
      SET_FLAG( SW_UART_status, SW_UART_RX_BUFFER_FULL );
      CLEAR_UART_EXTERNAL_INTERRUPT_FLAG();
      ENABLE_UART_EXTERNAL_INTERRUPT();   //Get ready to receive new byte.
    }

    //If reception finished and no new incoming data has been detected.
    else if(UART_counter == RECEIVE_FINISH)
    {
      //Initiate transmission if there is data in TX_buffer. This is done in the
      //same way as in the UART_transmit routine.
      if( READ_FLAG(SW_UART_status, SW_UART_TX_BUFFER_FULL) )
      {
        UART_Tx_data = UART_Tx_buffer;
        CLEAR_FLAG( SW_UART_status, SW_UART_TX_BUFFER_FULL );
        DISABLE_UART_EXTERNAL_INTERRUPT();
        CLEAR_UART_PIN();                         //Write start bit.
        UART_counter = TRANSMIT_FIRST_DATA;       //Update counter so the 1. data bit is the next bit to be transmitted.
        STOP_UART_TIMER();                        //Stop timer to reset prescaler.
        CLEAR_UART_TIMER();                       //Clear timer.
        SET_UART_TIMER_COMPARE_START_TRANSMIT();  //Set timer compare value
        CLEAR_UART_TIMER_INTERRUPT_FLAG();        //Make sure timer interrupt flag is not set.
        START_UART_TIMER();
        return;                                   //Exit ISR so the counter is not updated.
      }

      //If no data is waiting to be transmitted in the Tx bufferSet to IDLE and disable interrupt if no TX data is waiting.
      else
      {
        STOP_UART_TIMER();
        DISABLE_UART_TIMER_INTERRUPT();
        UART_counter = UART_STATE_IDLE;
        return; //Exit ISR so the counter is not updated.
      }
    }
  }

  //////////////////////////////
  //Transmit if counter is even.
  //////////////////////////////
  else
  {
    uint8_t bit_out = 0x00;

    //Check if bit equals last bit sent. Set frame error flag if not. Also set UART_Tx_data to the next value.
    if(UART_counter != TRANSMIT_FIRST_DATA)
    {
      uint8_t last_bit_sent = 0x01;

      if(UART_counter <= TRANSMIT_STOP_1)
      {
        last_bit_sent = UART_Tx_data & 0x01;
        UART_Tx_data = UART_Tx_data >> 1;     //Right shift data bits. Does not have any effect for stop bits.
      }

      //if a high bit was sent, but a low received, set frame error flag.
      if( last_bit_sent != bit_in )
      {
        SET_FLAG(SW_UART_status, SW_UART_FRAME_ERROR);
        //Stop transmission ?
      }
    }

    //If to transmit data bit, set bit_out according to UART_Tx_data.
    if(UART_counter <= TRANSMIT_LAST_DATA)
    {
      if( UART_Tx_data & 0x01 )
      {
        bit_out = 0x01;
      }
    }

    //If to transmit stop bit, set the bit to be transmitted high.
    else if(UART_counter == TRANSMIT_STOP_1)
    {
      bit_out = 0x01;
    }
    else if(UART_counter == TRANSMIT_STOP_2)
    {
      CLEAR_UART_EXTERNAL_INTERRUPT_FLAG();
      ENABLE_UART_EXTERNAL_INTERRUPT();
      bit_out = 0x01;
    }

    //If transmission finished. Start a new transmission if data is present in the Tx buffer. Set the UART
    //idle if not.
    else
    {
      //Check if new data is ready to be sent. If not, set UART state to idle, disable the timer interrupt and
      //enable the external interrupt to make the UART wait for new incoming data.
      if(!READ_FLAG(SW_UART_status, SW_UART_TX_BUFFER_FULL))
      {
        STOP_UART_TIMER();
        DISABLE_UART_TIMER_INTERRUPT();
        UART_counter = UART_STATE_IDLE;
        return; //Exit ISR so the counter is not updated.
      }

      //Initiate transmission if there is data in TX_buffer.
      else
      {
        UART_Tx_data = UART_Tx_buffer;
        CLEAR_FLAG( SW_UART_status, SW_UART_TX_BUFFER_FULL );
        UART_counter = TRANSMIT_FIRST_DATA - 2; //Need to substract 2 because counter is updated at the end of the ISR.
        DISABLE_UART_EXTERNAL_INTERRUPT();
        //bit_out already set to 0x00.
      }
    }

    //Transmit bit.
    if( bit_out )
    {
      SET_UART_PIN();
    }
    else
    {
      CLEAR_UART_PIN();
    }
  }
  UART_counter = UART_counter + 2; //Update counter.
}
