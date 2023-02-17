/*
 * serial.h
 *
 *  Created on: May 24, 2022
 *      Author: pujak
 */

#ifndef TERMINAL_SERIAL_H_
#define TERMINAL_SERIAL_H_

#include "Driver_USART.h"

/* Exported typedefs -------------------------------------------------------- */
typedef void (*stdout_locker_t)(uint8_t u8_lock);
typedef void (*stdin_reader_t)(void* p_buffer, uint16_t u16_size);

typedef struct
{
    NDS_DRIVER_USART *p_usart;
    stdout_locker_t locker;
    stdin_reader_t reader;
    void* p_rxBuf;
    uint16_t u16_rxBufSz;
} serial_t;

/* Public function declarations --------------------------------------------- */
void serial_init(NDS_DRIVER_USART *p_usart, uint32_t u32_baud,
        stdout_locker_t locker);
int32_t serial_start(stdin_reader_t reader, void* p_buffer, uint16_t u16_size);
int32_t serial_write(const void *p_buffer, uint32_t u32_cnt);


#endif /* TERMINAL_SERIAL_H_ */
