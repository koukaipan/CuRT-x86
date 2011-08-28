
#ifndef __X86_SCREEN_H__
#define __X86_SCREEN_H__

extern void screen_init(void);
extern void screen_puts(const char *text);
extern void screen_putch(char c);
extern void screen_clear();

#endif
