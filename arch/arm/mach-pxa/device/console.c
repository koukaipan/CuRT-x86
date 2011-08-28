/**
 * @file    arch/x86/ctx_sw.S
 * @author  Ying-Shiuan Pan
 * @brief   console operation functions
 * Since x86 use keyboard and screen not serial port, I have to use a wrapper to
 * isolate the arch dependent part.
 */

#include "kernel/console.h"
#include "device/serial.h"

void inline console_init(void)
{
	SerialInit();
}

void console_puts(const char *text)
{
	while (*text)
		SerialOutputByte(*text++);
}

void console_putc(char c)
{
	SerialOutputByte(c);
}

void console_clear()
{
	/* FIXME: dirty trick to clean screen */
	console_puts("\n\n\n\n\n\n\n\n\n\n"
				"\n\n\n\n\n\n\n\n\n\n"
				"\n\n\n\n\n\n\n\n\n\n"
				"\n\n\n\n\n\n\n\n\n\n"
				"\n\n\n\n\n\n\n\n\n\n"
				"\n\n\n\n\n\n\n\n\n\n"
				"\n\n\n\n\n\n\n\n\n\n");
}

int console_getc(void)
{
	while (!SerialIsReadyChar())
		/* Keep waiting */ ;
	return SerialIsGetChar() & 0xFF;
}

int console_gets(char *s)
{
	int cnt = 0;
	char  c;

	while ((c = console_getc()) != CR) {
		if (c != BS) {
			cnt++;
			*s++ = c;
			SerialOutputByte(c);	//echo
		}
		else {
			if (cnt > 0) {
				cnt--; *s-- = ' ';
				console_puts("\b \b");
			}
		}
	}
	*s = 0;
	return cnt;

}
