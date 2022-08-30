/*
 * UART_circular_buffers.h
 *
 *  Created on: Aug 25, 2022
 *      Author: pmachado
 */

#ifndef INC_UART_CIRCULAR_BUFFERS_H_
#define INC_UART_CIRCULAR_BUFFERS_H_

#include <stdint.h>
#include "stm32f7xx_hal.h"
//#include "main.h"

//Change according to the used UART
extern UART_HandleTypeDef huart6;

#define BUFFER_SIZE		128				//max size for Buffer
#define UART_HNDLR		huart6			//UART handler -> change according to the UART used

//Circular Buffer Data Structure
struct CircularBuffer{
	uint8_t buffer[BUFFER_SIZE];
	uint16_t head;
	uint16_t tail;
	uint16_t nBytes;			//Nr. of Bytes loaded into the Buffer
};

enum status{
	OK,
	NOK
};

enum cond{
	TRUE,
	FALSE
};

/*
 * Reception
 */
void init_Rxbuffer();
void RxCallBackFunction();
int read_RXbuffer(uint8_t * pData, uint16_t size);
int Rx_BufferFull();
int Rx_BufferEmpty();

/*
 * Transmission
 */

void init_Txbuffer();
void TxCallBackFunction();
int write_TXbuffer(uint8_t * pData, uint16_t size);
int Tx_BufferFull();
int Tx_BufferEmpty();


#endif /* INC_UART_CIRCULAR_BUFFERS_H_ */
