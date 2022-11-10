/*
 * serial.c
 *
 *  Created on: May 24, 2022
 *      Author: pujak
 */

#include "serial.h"

#include <string.h>

/* private variables */
static serial_t hserial;

/* private function declarations */
static void stdout_locker(uint8_t u8_lock);
static int32_t stdin_listen(void);
static void usart_callback(uint32_t u32_event);

/* public function implementations */
void serial_init(NDS_DRIVER_USART *p_usart, uint32_t u32_baud,
		stdout_locker_t locker)
{
	hserial.p_usart = p_usart;
	hserial.locker = locker;

	// initialize the USART driver
	p_usart->Initialize(usart_callback);

	// power up the USART peripheral
	p_usart->PowerControl(NDS_POWER_FULL);

	// configure the USART control
	p_usart->Control(NDS_USART_MODE_ASYNCHRONOUS |
			   NDS_USART_DATA_BITS_8 |
			   NDS_USART_PARITY_NONE |
			   NDS_USART_STOP_BITS_1 |
			   NDS_USART_FLOW_CONTROL_NONE,
			   u32_baud);

	// enable interrupt lines
	p_usart->Control(NDS_USART_CONTROL_TX, 1);
	p_usart->Control(NDS_USART_CONTROL_RX, 1);
}

int32_t serial_start(stdin_reader_t reader, void* p_buffer, uint16_t u16_size)
{
	hserial.reader = reader;
	hserial.rx.p_buffer = p_buffer;
	hserial.rx.u16_size = u16_size;

	return (stdin_listen());
}


int32_t serial_write(const void *p_buffer, uint32_t u32_cnt)
{
	NDS_DRIVER_USART *p_usart;
	int32_t s32_rc;

	p_usart = hserial.p_usart;

	stdout_locker(1);

	s32_rc = p_usart->Send(p_buffer, u32_cnt);

	if (0 == s32_rc)
	{
		// blocking wait
		while((1 == p_usart->GetStatus().tx_busy) &&
			  (0 == p_usart->GetStatus().tx_underflow))
		{};

		if (u32_cnt != p_usart->GetTxCount())
		{
			s32_rc = -1;
		}
	}

	stdout_locker(0);

	return (s32_rc);
}


/* private function implementations */
static void stdout_locker(uint8_t u8_lock)
{
	if (NULL != hserial.locker)
	{
		hserial.locker(u8_lock);
	}
}

static int32_t stdin_listen(void)
{
	NDS_DRIVER_USART *p_usart;
	uint32_t u32_inCnt;
	int32_t s32_rc;

	p_usart = hserial.p_usart;

	/* ignore empty reader */
	if (NULL == hserial.reader)
	{
		return (-1);
	}

	/* recover when stuck */
	if (p_usart->GetStatus().rx_busy)
	{
		p_usart->Control(NDS_USART_ABORT_RECEIVE, 1);
	}

	/* copy previous data */
	u32_inCnt = p_usart->GetRxCount();
	if (0 < u32_inCnt)
	{
		hserial.reader(hserial.rx.p_buffer, u32_inCnt);
		memset(hserial.rx.p_buffer, 0x0, u32_inCnt);
	}

	/* start listening */
	s32_rc = p_usart->Receive(hserial.rx.p_buffer, hserial.rx.u16_size);

	return (s32_rc);
}

static void usart_callback(uint32_t u32_event)
{
	switch (u32_event) {
		case NDS_USART_EVENT_TRANSFER_COMPLETE:
			stdin_listen();
			break;

		case NDS_USART_EVENT_RECEIVE_COMPLETE:
		case NDS_USART_EVENT_RX_BREAK:
			stdin_listen();
			break;

		case NDS_USART_EVENT_RX_TIMEOUT:
		case NDS_USART_EVENT_RX_FRAMING_ERROR:
		case NDS_USART_EVENT_RX_PARITY_ERROR:
		case NDS_USART_EVENT_RX_OVERFLOW:
			stdin_listen();
	        break;
            
		case NDS_USART_EVENT_TX_COMPLETE:
		case NDS_USART_EVENT_SEND_COMPLETE:
			break;

		case NDS_USART_EVENT_TX_UNDERFLOW:
	        break;
	}
}
