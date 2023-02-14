/*
 * retarget.c
 *
 *  Created on: May 25, 2022
 *      Author: pujak
 */

#include "serial.h"

#undef putchar
inline int putchar(int c)
{
	serial_write(&c, 1);
	return c;
}

__attribute__((used))
void nds_write(const unsigned char *buf, int size)
{
	serial_write(buf, size);
}

struct stat;
int _fstat(int fd, struct stat *buf)
{
    (void) fd;
    (void) buf;
    return 0;
}

int _write(int __fd, const void *__buf, int __nbyte)
{
	(void)__fd;

	serial_write(__buf, __nbyte);

	return __nbyte;
}

