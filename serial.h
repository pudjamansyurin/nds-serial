/*
 * serial.h
 *
 *  Created on: May 24, 2022
 *      Author: pujak
 */

#ifndef TERMINAL_SERIAL_H_
#define TERMINAL_SERIAL_H_

#include "Driver_USART.h"

typedef void (*serial_reader_t)(void* p_buffer, uint16_t u16_size);

typedef struct
{
	NDS_DRIVER_USART *p_usart;
	serial_reader_t reader;
	uint16_t u16_rxSize;
	void* p_rxBuffer;
} serial_t;

/* public function declarations */
void serial_init(NDS_DRIVER_USART *p_usart, uint32_t u32_baud);
int32_t serial_start(serial_reader_t reader, void* p_buffer, uint16_t u16_size);
int32_t serial_write(const void *p_buffer, uint32_t u32_cnt);


#endif /* TERMINAL_SERIAL_H_ */
