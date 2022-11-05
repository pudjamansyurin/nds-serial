# Andes serial module using USART interface

## Module description
- USART break signal should be sent explicitly as RX interrupt event.
- USART TX and RX can run in parallel.
- USART-RX line using intterupt mode (non-blocking)
- USART-TX line using polling mode (blocking)
- Optional stdout locking mechanism.

#### **`main.c`**
```c
#include "ae210p.h"
#include "usart_ae210p.h"
#include "nds-serial/serial.h"
#include <string.h>
#include <stdio.h>

/* external entity */
extern void delay(uint64_t ms);
extern NDS_DRIVER_USART Driver_USART1;

/* private variables */
static char stdin_buffer[512];

/* private function definition */
static void stdin_reader(void *p_buffer, uint16_t u16_cnt)
{
    /* do something with received data */
}

/* public function definition */
int main(void)
{
	uint8_t u8_cntr = 0;

	/* initialize terminal */
	serial_init(&Driver_USART1, 38400, NULL);
	serial_start(stdin_reader, stdin_buffer, sizeof(stdin_buffer));

	/* send directly to serial */
	char* p_text = "hello world\n";
	serial_write(p_text, strlen(p_text));

	/* infinite loop */
	while(1) {
		/* send in-direct using stdout */
		printf("counter = %d\r\n", u8_cntr++);

		delay(1);
	}

	return 0;
}

```
