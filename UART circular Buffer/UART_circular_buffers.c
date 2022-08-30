/*
 * UART_circular_buffers.c
 *
 *  Created on: Aug 25, 2022
 *      Author: pmachado
 */
#include "UART_circular_buffers.h"

/*
 * General code for UART circular buffers (RX and TX).  It was developed to
 * run on any STM32 platform. Change on "UART_circular_buffers.h" the UART
 * Handler on the UART_HNDLR definition. Both the UART and its pins configuration
 * should be done on the Application using this code.
 */


/*
 * Reception
 */
struct CircularBuffer Rx;

void init_Rxbuffer(){
	Rx.head = 0;
	Rx.tail	= 0;

	//start reception of 1 Byte into Rx_Buffer (waiting)
	HAL_UART_Receive_IT(&(UART_HNDLR),&(Rx.buffer[Rx.tail]),1);
}

//Function that is executed upon an interrupt trigger;
//Call it inside the interrupt service routine
void RxCallBackFunction(){
	//transfer data from RX data register to RX buffer
	Rx.buffer[Rx.tail] = UART_HNDLR.Instance->RDR;
	//Set UART as ready to receive another Byte
	UART_HNDLR.RxState = HAL_UART_STATE_READY;
	//Update tail index
	if(Rx.nBytes == BUFFER_SIZE){
		Rx.tail = (Rx.tail+1) % BUFFER_SIZE;							//increment tail, going around from 127 to 0 directly, if needed
		Rx.head = (Rx.head+1) % BUFFER_SIZE;							//increment head as well, discarding older Byte (let's hope not)
	}else{
		Rx.tail = (Rx.tail+1) % BUFFER_SIZE;							//just increment the tail
		Rx.nBytes++;													//Increment nBytes (no discarded Bytes)
	}

	HAL_HalfDuplex_EnableTransmitter(&(UART_HNDLR));					//Enable RX and disable TX
	HAL_UART_Receive_IT(&(UART_HNDLR),&(Rx.buffer[Rx.tail]),1);			//Wait for another Byte
}

int read_RXbuffer(uint8_t * pData, uint16_t size){
	//Check if Buffer has the expected amount of data
	if(Rx.nBytes < size){												//not enough data on Buffer
		return NOK;
	}else{
		for(int i=0; i<size; i++){
			*pData = Rx.buffer[Rx.head];								//Get #size Bytes from the RX buffer and update the head
			Rx.head = (Rx.head+1) % BUFFER_SIZE;						//Increment head
			pData++;
			Rx.nBytes--;												//Decrease nBytes available
		}
	}
	return OK;
}

int Rx_BufferFull(){													//Check if RX buffer is full
	if(Rx.nBytes == BUFFER_SIZE)
		return TRUE;
	else
		return FALSE;
}

int Rx_BufferEmpty(){													//Check if RX buffer is empty
	if(Rx.nBytes == 0)
		return TRUE;
	else
		return FALSE;
}


/*
 * Transmission
 */

struct CircularBuffer Tx;

void init_Txbuffer(){
	Tx.head = 0;
	Tx.tail	= 0;
}

//Function that is executed upon an interrupt trigger
//Call it inside the interrupt service routine
void TxCallBackFunction(){

	if(Tx.nBytes >= 1){														//If buffer has one or more Bytes to be sent
		Tx.head = (Tx.head+1) % BUFFER_SIZE;								//Increment head
		Tx.nBytes--;														//Decrement number of Bytes in the buffer
	}else{
		return;																//There are no Bytes in the buffer -> interrupt should not trigger
																			//Ignore and do nothing
	}
}

int write_TXbuffer(uint8_t * pData, uint16_t size){
	//Check if Buffer has the expected amount of data
	if(Tx.nBytes+size > BUFFER_SIZE){										//not enough space on Buffer
		return NOK;
	}else{
		for(int i=0; i<size; i++){
			Tx.buffer[Tx.tail] = *pData;									//Get #size Bytes from the TX buffer and update the tail
			Tx.tail = (Tx.tail+1) % BUFFER_SIZE;							//Increment tail
			pData++;
			Tx.nBytes++;													//Increase nBytes available
		}
	}

	//start transmission of 1 Byte into Rx_Buffer (waiting)
	HAL_HalfDuplex_EnableTransmitter(&(UART_HNDLR));						//Enable TX and disable RX
	HAL_UART_Transmit_IT(&(UART_HNDLR),&(Tx.buffer[Tx.head]),size);

	return OK;
}

int Tx_BufferFull(){														//Check if TX buffer is full
	if(Tx.nBytes == BUFFER_SIZE)
		return TRUE;
	else
		return FALSE;
}

int Tx_BufferEmpty(){														//Check if TX buffer is empty
	if(Tx.nBytes == 0)
		return TRUE;
	else
		return FALSE;
}
